wasm: web/module.wasm

windows: windows/kute_win32.exe

bin/kute_win32.o : src/kute_win32.c
	clang -c src/kute_win32.c -o bin/kute_win32.o

windows/kute_win32.exe : bin/kute_win32.o
	mkdir -p windows
	clang bin/kute_win32.o -o windows/kute_win32.exe -luser32 -lgdi32 -lmsimg32

bin/module.o: src/kute_wasm32.c
	mkdir -p bin
	clang -c src/kute_wasm32.c -o bin/module.o --target=wasm32 -nostdlib

web/module.wasm: bin/module.o
	mkdir -p web
	wasm-ld bin/module.o -o web/module.wasm --no-entry  --export-all

clean:
	rm -rf bin
	rm web/*.wasm