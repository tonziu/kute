kute: build/kute.o

tests: web_test

examples: wasm_fill_example wasm_line_example wasm_triangle3d_example

wasm_fill_example: kute build/wasm_fill.wasm
wasm_line_example: kute build/wasm_line.wasm
wasm_triangle3d_example: kute build/wasm_triangle3d.wasm

web_test: kute build/wasm_test.wasm

build/wasm_test.wasm: kute build/wasm_test.o
	wasm-ld build/kute.o build/wasm_test.o -o build/wasm_test.wasm --no-entry --export-all --allow-undefined
	cp build/wasm_test.wasm test/web/wasm_test.wasm

build/wasm_fill.wasm: kute build/wasm_fill.o
	wasm-ld build/kute.o build/wasm_fill.o -o build/wasm_fill.wasm --no-entry --export-all --allow-undefined
	# cp build/wasm_fill.wasm ../website/wasm/wasm_fill.wasm

build/wasm_line.wasm: kute build/wasm_line.o
	wasm-ld build/kute.o build/wasm_line.o -o build/wasm_line.wasm --no-entry --export-all --allow-undefined
	# cp build/wasm_line.wasm ../website/wasm/wasm_line.wasm

build/wasm_triangle3d.wasm: kute build/wasm_triangle3d.o
	wasm-ld build/kute.o build/wasm_triangle3d.o -o build/wasm_triangle3d.wasm --no-entry --export-all --allow-undefined
	# cp build/wasm_triangle3d.wasm ../website/wasm/wasm_triangle3d.wasm

build/kute.o: src/kute.c
	clang -c src/kute.c -o build/kute.o -Iinclude --target=wasm32

build/wasm_test.o: test/web/wasm_test.c
	clang -c test/web/wasm_test.c -o build/wasm_test.o --target=wasm32 -Iinclude

build/wasm_fill.o: examples/wasm_fill.c
	clang -c examples/wasm_fill.c -o build/wasm_fill.o --target=wasm32 -Iinclude

build/wasm_line.o: examples/wasm_line.c
	clang -c examples/wasm_line.c -o build/wasm_line.o --target=wasm32 -Iinclude

build/wasm_triangle3d.o: examples/wasm_triangle3d.c
	clang -c examples/wasm_triangle3d.c -o build/wasm_triangle3d.o --target=wasm32 -Iinclude

clean:
	rm build/*