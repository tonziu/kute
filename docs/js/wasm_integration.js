async function InitCanvas(canvasId, wasmPath) {
    const response = await fetch(wasmPath);
    const bytes = await response.arrayBuffer();

    const { instance } = await WebAssembly.instantiate(bytes, {
        env: {
            cosf: x => Math.cos(x),
            sinf: x => Math.sin(x),
            tanf: x => Math.tan(x),
            sqrtf: x => Math.sqrt(x),
        }
    });

    let canvas = document.getElementById(canvasId);
    canvas.width = instance.exports.wasm_get_width();
    canvas.height = instance.exports.wasm_get_height();

    const ptr = instance.exports.wasm_get_pixels();
    const pixels = new Uint32Array(
        instance.exports.memory.buffer,
        ptr,
        canvas.width * canvas.height
    );

    const ctx = canvas.getContext('2d');

    function loop() {
        instance.exports.wasm_loop();

        const imageData = new ImageData(
            new Uint8ClampedArray(pixels.buffer, pixels.byteOffset, pixels.byteLength),
            canvas.width,
            canvas.height
        );
        ctx.putImageData(imageData, 0, 0);
        requestAnimationFrame(loop);
    }

    loop();
}