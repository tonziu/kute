web: web/kute.wasm

web/kute.wasm : bin/kute.o
	wasm-ld bin/kute.o -o web/kute.wasm --no-entry --export-all

bin/kute.o : src/kute.c
	clang -c src/kute.c -o bin/kute.o -Iinclude --target=wasm32 -nostdlib 

clean:
	rm bin/*.o
	rm web/*.wasm