(async () => {
    const canvas = document.getElementById('canvas');
    const ctx = canvas.getContext('2d');
    const width = canvas.width, height = canvas.height;

    const response = await fetch('module.wasm');
    const bytes = await response.arrayBuffer();
    const wasmModule = await WebAssembly.instantiate(bytes, {
        env: {
            sinf: Math.sin,
            cosf: Math.cos,
        },
    });
    const { wasm_update, wasm_get_pixels, memory } = wasmModule.instance.exports;

    const ptr = wasm_get_pixels();
    const pixels32 = new Uint32Array(memory.buffer, ptr, width*height);
    const pixels8 = new Uint8ClampedArray(pixels32.buffer, pixels32.byteOffset, width*height*4);
    const imageData = new ImageData(pixels8, width, height);

    function render(t) {
        wasm_update(t);
        ctx.putImageData(imageData, 0, 0);
        requestAnimationFrame(() => render(t + 1));
    }

    render(0);
})();