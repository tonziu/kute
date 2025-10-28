(function (global) {
    class Kute {
        constructor(wasmPath) {
            this.wasmPath = wasmPath || 'kute.wasm';
            this.instance = null;
            this.exports = null;
            this.pixels = null;
            this.width = 0;
            this.height = 0;
            this.canvas = null;
        }

        async _init() {
            // trova automaticamente il primo canvas
            this.canvas = document.querySelector('canvas');
            if (!this.canvas) {
                console.error('No canvas found on the page!');
                return;
            }

            const response = await fetch(this.wasmPath);
            const bytes = await response.arrayBuffer();

            const { instance } = await WebAssembly.instantiate(bytes, { env: {} });
            this.instance = instance;
            this.exports = instance.exports;

            this.width = this.canvas.width;
            this.height = this.canvas.height;

            // inizializza il buffer nel wasm
            this.exports.kute_init(this.width, this.height);

            // crea un array JS che punta al buffer wasm
            this.pixels = new Uint32Array(
                this.exports.memory.buffer,
                this.exports.kute_get_pixels(),
                this.width * this.height
            );
        }

        // chiamata generica a qualsiasi funzione wasm
        call(funcName, ...args) {
            if (!this.instance) return;
            if (!(funcName in this.exports)) {
                console.error(`Function ${funcName} not found in WASM`);
                return;
            }
            return this.exports[funcName](...args);
        }

        clear(color) {
            this.call('kute_clear', color);
        }

        rect(x, y, w, h, color) {
            this.call('kute_rect', x, y, w, h, color);
        }

        circle(cx, cy, radius, color) {
            this.call('kute_circle', cx, cy, radius, color);
        }

        line(x0, y0, x1, y1, color) {
            this.call('kute_line', x0, y0, x1, y1, color);
        }

        async updateCanvas() {
            if (!this.canvas || !this.width || !this.height) return;

            const ctx = this.canvas.getContext('2d');
            const imageData = new ImageData(
                new Uint8ClampedArray(this.pixels.buffer, this.pixels.byteOffset, this.pixels.byteLength),
                this.width,
                this.height
            );
            ctx.putImageData(imageData, 0, 0);
        }

        // loop di animazione semplice
        async loop(callback) {
                while (!this.pixels) {
                    await new Promise(r => setTimeout(r, 10));
                }

                const render = () => {
                    callback();
                    this.updateCanvas();
                    requestAnimationFrame(render);
                };
                render();
            }
        }

        // inizializzazione automatica
        const kuteInstance = new Kute();
    global.kute = kuteInstance;
    window.addEventListener('DOMContentLoaded', async () => {
        await kuteInstance._init();
    });
})(window);
