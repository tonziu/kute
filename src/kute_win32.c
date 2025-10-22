#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

#define KUTE_OK 0
#define KUTE_ERROR -1

#define WIDTH 800
#define HEIGHT 600

typedef struct
{
    HINSTANCE hInst;
    HWND hWnd;
    BITMAPINFO bminfo;
    HDC hdc;
    HDC hdcMem;
    HDC hdcBuffer;        // DC per double buffering
    HBITMAP hBitmap;
    HBITMAP hBitmapBuffer; // Bitmap per double buffering
    HBITMAP hOldBitmap;
    HBITMAP hOldBitmapBuffer;
    uint32_t *pixels_premul;
    BLENDFUNCTION blend;
    bool is_running;
    int current_width;
    int current_height;
} kute_win32_platform_t;

static kute_win32_platform_t win32_platform;

static uint32_t *pixels;

static void Win32ResizeDIBSection(int width, int height)
{
    if (win32_platform.hBitmap)
    {
        SelectObject(win32_platform.hdcMem, win32_platform.hOldBitmap);
        DeleteObject(win32_platform.hBitmap);
    }

    if (win32_platform.hBitmapBuffer)
    {
        SelectObject(win32_platform.hdcBuffer, win32_platform.hOldBitmapBuffer);
        DeleteObject(win32_platform.hBitmapBuffer);
    }

    if (win32_platform.pixels_premul)
    {
        VirtualFree(win32_platform.pixels_premul, 0, MEM_RELEASE);
    }

    win32_platform.bminfo.bmiHeader.biSize = sizeof(win32_platform.bminfo.bmiHeader);
    win32_platform.bminfo.bmiHeader.biWidth = width;
    win32_platform.bminfo.bmiHeader.biHeight = -height;
    win32_platform.bminfo.bmiHeader.biPlanes = 1;
    win32_platform.bminfo.bmiHeader.biBitCount = 32;
    win32_platform.bminfo.bmiHeader.biCompression = BI_RGB;

    win32_platform.hBitmap = CreateDIBSection(
        win32_platform.hdcMem, 
        &win32_platform.bminfo, 
        DIB_RGB_COLORS, 
        (void**)&win32_platform.pixels_premul,
        NULL, 
        0
    );

    if (win32_platform.hBitmap)
    {
        win32_platform.hOldBitmap = SelectObject(win32_platform.hdcMem, win32_platform.hBitmap);
    }

    // Crea bitmap per double buffering (compatibile con lo schermo)
    win32_platform.hBitmapBuffer = CreateCompatibleBitmap(win32_platform.hdc, width, height);
    if (win32_platform.hBitmapBuffer)
    {
        win32_platform.hOldBitmapBuffer = SelectObject(win32_platform.hdcBuffer, win32_platform.hBitmapBuffer);
    }

    // Alloca buffer separato per i pixel dell'utente
    int pixelsSize = (width * height) * sizeof(uint32_t);
    pixels = VirtualAlloc(0, pixelsSize, MEM_COMMIT, PAGE_READWRITE);

    win32_platform.current_width = width;
    win32_platform.current_height = height;
}

// Copia i pixel premoltiplicando l'alpha nel buffer temporaneo
static void Win32CopyAndPremultiplyAlpha()
{
    int pixel_count = win32_platform.current_width * win32_platform.current_height;
    
    for (int i = 0; i < pixel_count; ++i)
    {
        uint32_t pixel = pixels[i];  // Leggi dal buffer utente in formato 0xAABBGGRR
        uint8_t r = (pixel >> 0) & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = (pixel >> 16) & 0xFF;
        uint8_t a = (pixel >> 24) & 0xFF;
        
        // Premoltiplica RGB per alpha
        r = (r * a) / 255;
        g = (g * a) / 255;
        b = (b * a) / 255;
        
        // Scrivi nel buffer della bitmap in formato BGRA (Windows)
        win32_platform.pixels_premul[i] = (a << 24) | (r << 16) | (g << 8) | b;
    }
}

static void Win32UpdateClientArea(HDC hdc)
{
    // Copia e premoltiplica l'alpha
    Win32CopyAndPremultiplyAlpha();
    
    // Disegna nel buffer offscreen
    // 1. Pulisci lo sfondo nero
    RECT rect = {0, 0, win32_platform.current_width, win32_platform.current_height};
    FillRect(win32_platform.hdcBuffer, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
    
    // 2. Fai il blend nel buffer
    AlphaBlend(
        win32_platform.hdcBuffer,
        0, 0, win32_platform.current_width, win32_platform.current_height,
        win32_platform.hdcMem,
        0, 0, win32_platform.current_width, win32_platform.current_height,
        win32_platform.blend
    );
    
    // 3. Copia tutto il buffer sullo schermo in una volta (no flickering)
    BitBlt(
        hdc,
        0, 0, win32_platform.current_width, win32_platform.current_height,
        win32_platform.hdcBuffer,
        0, 0,
        SRCCOPY
    );
}

static LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            if (width > 0 && height > 0)
            {
                Win32ResizeDIBSection(width, height);
            }
        } break;
        case WM_CLOSE:
        {
            win32_platform.is_running = false;
        } break;
        case WM_DESTROY:
        {
            win32_platform.is_running = false;
        } break;
        case WM_ERASEBKGND:
        {
            RECT rect;
            GetClientRect(hWnd, &rect);
            FillRect((HDC)wParam, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            return 1;
        } break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

int kute_InitWindow(int width, int height, const char* title)
{
    win32_platform.hInst = GetModuleHandle(NULL);

    WNDCLASS wndClass = {0};
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = win32_platform.hInst;
    wndClass.hIcon = NULL;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = "MainWClass";

    if (!RegisterClass(&wndClass))
    {
        return KUTE_ERROR;
    }

    RECT drawableArea = { 0, 0, width, height };
    AdjustWindowRect(&drawableArea, WS_OVERLAPPEDWINDOW, FALSE);
    width = drawableArea.right - drawableArea.left;
    height = drawableArea.bottom - drawableArea.top;
    
    // Finestra normale (NON layered) per mantenere la barra del titolo
    win32_platform.hWnd = CreateWindowExA(
                              0,  // Nessun extended style
                              "MainWClass", 
                              title, 
                              WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              width, height, 
                              NULL, NULL, 
                              win32_platform.hInst, 
                              NULL);

    if (!win32_platform.hWnd) return KUTE_ERROR;

    ShowWindow(win32_platform.hWnd, SW_SHOW);
    UpdateWindow(win32_platform.hWnd);

    win32_platform.is_running = true;
    win32_platform.hdc = GetDC(win32_platform.hWnd);
    
    HDC hdcScreen = GetDC(NULL);
    win32_platform.hdcMem = CreateCompatibleDC(hdcScreen);
    win32_platform.hdcBuffer = CreateCompatibleDC(hdcScreen);  // DC per double buffering
    ReleaseDC(NULL, hdcScreen);

    // Configurazione blend function (una volta sola)
    win32_platform.blend.BlendOp = AC_SRC_OVER;
    win32_platform.blend.BlendFlags = 0;
    win32_platform.blend.SourceConstantAlpha = 255;
    win32_platform.blend.AlphaFormat = AC_SRC_ALPHA;

    RECT clientRect;
    GetClientRect(win32_platform.hWnd, &clientRect);
    Win32ResizeDIBSection(clientRect.right, clientRect.bottom);

    return KUTE_OK;
}

bool kute_WindowIsRunning()
{
    return win32_platform.is_running;
}

void kute_HandleEvents()
{ 
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void kute_SwapBuffers()
{
    Win32UpdateClientArea(win32_platform.hdc);
}

void kute_CloseWindow()
{
    if (pixels)
    {
        VirtualFree(pixels, 0, MEM_RELEASE);
    }
    if (win32_platform.hBitmap)
    {
        SelectObject(win32_platform.hdcMem, win32_platform.hOldBitmap);
        DeleteObject(win32_platform.hBitmap);
    }
    if (win32_platform.hBitmapBuffer)
    {
        SelectObject(win32_platform.hdcBuffer, win32_platform.hOldBitmapBuffer);
        DeleteObject(win32_platform.hBitmapBuffer);
    }
    if (win32_platform.hdcMem)
    {
        DeleteDC(win32_platform.hdcMem);
    }
    if (win32_platform.hdcBuffer)
    {
        DeleteDC(win32_platform.hdcBuffer);
    }
    ReleaseDC(win32_platform.hWnd, win32_platform.hdc);
    DestroyWindow(win32_platform.hWnd);
}

// Funzione helper per creare colori in formato 0xAABBGGRR
static inline uint32_t RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (a << 24) | (b << 16) | (g << 8) | r;
}

int main(void)
{
    if (kute_InitWindow(WIDTH, HEIGHT, "Kute (win32) - Alpha Blending") == KUTE_ERROR)
        return -1;

    while (kute_WindowIsRunning())
    {
        kute_HandleEvents();
        
        kute_SwapBuffers();
        Sleep(16);
    }

    kute_CloseWindow();

    return 0;
}