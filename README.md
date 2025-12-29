# picofb

A minimal single-header cross-platform library for displaying a framebuffer on screen with multi input handling

## Features

- Simple API: init, set pixels, update, cleanup
- Many implementations (Auto backend select):
  - **X11**: Linux/X11 only
  - **WIN32**: Windows only
  - **SDL2**: Cross-platform (Linux/Windows/Mac), Slower
- Save framebuffer to PPM image format
- Configurable resolution via compile-time defines

## Use cases
- Software rendered applications (UI, Games)
- Easy render testing

## Usage
See `example.c` and docs section in `picofb.h`

## Compilation

Link with:
- Linux/X11: -lX11
- Windows: None
- Others: -SDL2

## TODO

- Wayland