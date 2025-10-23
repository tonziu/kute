all: kute wasm windows

kute: lib/kute.lib lib/libkute.a

wasm: web/module.wasm

windows: windows/kute_win32.exe

bin/kute.o : src/kute.c
	mkdir -p bin
	clang -c src/kute.c -o bin/kute.o -Iinclude
	
bin/kute_wasm.o : src/kute.c
	mkdir -p bin
	clang -c src/kute.c -o bin/kute_wasm.o -Iinclude --target=wasm32

lib/kute.lib : bin/kute.o
	mkdir -p lib
	llvm-ar rcs lib/kute.lib bin/kute.o

lib/libkute.a : bin/kute_wasm.o
	mkdir -p lib
	llvm-ar rcs lib/libkute.a bin/kute_wasm.o

bin/kute_win32.o : examples/kute_win32.c
	clang -c examples/kute_win32.c -o bin/kute_win32.o -Iinclude

windows/kute_win32.exe : bin/kute_win32.o lib/kute.lib
	mkdir -p windows
	clang bin/kute_win32.o -o windows/kute_win32.exe -luser32 -lgdi32 -lmsimg32 -Llib -lkute

bin/module.o: examples/kute_wasm32.c
	mkdir -p bin
	clang -c examples/kute_wasm32.c -o bin/module.o --target=wasm32 -nostdlib -Iinclude

web/module.wasm: bin/module.o lib/libkute.a
	mkdir -p web
	wasm-ld bin/module.o -o web/module.wasm --no-entry  --export-all -Llib -lkute

clean:
	rm -rf lib
	rm -rf bin
	rm web/*.wasm