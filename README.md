# Kute

Kute is a tiny, cross-platform and CPU-only graphics library in C. I use it to learn the very fundamentals of graphics, at a pixel level.
The goal is to have something very simple, easy to understand, and that can be improved. Any suggestion is vastly appreciated.

## Web Integration

Use the **Makefile** to build Kute against a web target:
```
  make web
```
This will generate a *wasm* module and a *kute.js* module in a **web** folder, and that's all you need to start playing with the library. In your HTML, you just need to import **kute.js** and then define what should happen in the kute.loop() function:
```
  <body>
    <canvas width="800" height="600"></canvas>
    <script src="kute.js"></script>
    kute.loop(() => {
        kute.clear(0xFF222222);
        kute.line(200, 100, 600, 200, 0xFFFF0000);
        kute.line(200, 100, 350, 450, 0xFF00FF00);
        kute.line(600, 200, 350, 450, 0xFF0000FF);
  
    });
  </body>
```

The library provides a more readable api within the javascript module, otherwise you can just run *kute.call('function_name', ...args)* to explicitly use the *kute.h* signatures. For example, the script above is equivalent to this one:
```
  <canvas width="800" height="600"></canvas>
  <script src="kute.js"></script>
  kute.loop(() => {
      kute.clear(0xFF222222);
      kute.call('kute_line', 200, 100, 600, 200, 0xFFFF0000);
      kute.call('kute_line', 200, 100, 350, 450, 0xFF00FF00);
      kute.call('kute_line', 600, 200, 350, 450, 0xFF0000FF);

  });

```
Be aware that you need to define a **canvas** element before importing **kute.js**, so that you can specify the amount of pixels to use.

## References

It is greatly inspired by the amazing work done in **olive.c** (check it out at https://github.com/tsoding/olive.c).
