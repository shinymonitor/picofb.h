<div align="center">
    <img src="assets/LOGO.png", width="200"/>
    <h1>picofb.h</h1>
</div>

A minimal single-header cross-platform windowing library for displaying a framebuffer on screen with multi input and mouse handling

## Features

- Single header and simple API
- Multi-key and mouse input
- Many implementations (Auto backend select):
  - **X11**: Linux/X11 only
  - **WIN32**: Windows only
  - **SDL2**: Cross-platform, Slower, Fallback

## Use cases
- Software rendered applications (UI, Games)
- Prototyping and easy render testing
- Educational

## Usage
See `example.c` and docs section in `picofb.h`

## Compilation

Link with:
- Linux/X11: -lX11
- Windows: -lgdi32 -luser32
- Others: -lSDL2
