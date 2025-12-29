//================================================================
// DOCS
//================================================================
// Guidelines
// - Define PICOFB_WIDTH and PICOFB_HEIGHT before including
// - Allocate a zero initialized PICOFB_Window to store state and pass to API
// - use PICOFB_Window.quit to loop
// - See PICOFB_Key for the keyboard input enum
// - Define PICOFB_BACKEND_OVERRIDE and then PICOFB_<X11/WIN32/WAYLAND/SDL>_BACKEND to override backend select
//
// API:
// - bool PICOFB_init(const char* window_title, PICOFB_Window* picofb_window)                - initialize and present a window with a window title string
// - void PICOFB_set_pixel(PICOFB_Window* picofb_window, size_t x, size_t y, uint32_t color) - set the frame buffer pixel at x, y to a color
// - uint32_t PICOFB_color_rgb(uint8_t r, uint8_t g, uint8_t b)                              - returns a cross-platform color value to be used in set pixel function
// - void PICOFB_update(PICOFB_Window* picofb_window)                                        - updates the window to display the frame buffer and gets user input
// - bool PICOFB_is_input(PICOFB_Window* picofb_window, PICOFB_Key key)                      - check if the key was pressed last frame
// - void PICOFB_cleanup(PICOFB_Window* picofb_window)                                       - cleanup the window
// - void PICOFB_save_ppm(PICOFB_Window* picofb_window, const char *path)                    - save the frame buffer to a ppm file
// 
// User visible fields of PICOFB_Window (cross-platform):
//     uint32_t frame_buffer[PICOFB_HEIGHT][PICOFB_WIDTH];
//     bool key_states[PICOFB_Key_COUNT];
//     bool quit;
//

//================================================================
// PICOFB
//================================================================

#ifndef _PICOFB_H
#define _PICOFB_H

#ifndef PICOFB_WIDTH
#define PICOFB_WIDTH 640
#endif
#ifndef PICOFB_HEIGHT
#define PICOFB_HEIGHT 360
#endif

//================================================================
// UNIVERSAL INCLUDES
//================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

//================================================================
// PICOFB_Key
//================================================================

typedef enum {
    PICOFB_Key_UNKNOWN = 0,
    // Letters
    PICOFB_Key_a, PICOFB_Key_b, PICOFB_Key_c, PICOFB_Key_d,
    PICOFB_Key_e, PICOFB_Key_f, PICOFB_Key_g, PICOFB_Key_h,
    PICOFB_Key_i, PICOFB_Key_j, PICOFB_Key_k, PICOFB_Key_l,
    PICOFB_Key_m, PICOFB_Key_n, PICOFB_Key_o, PICOFB_Key_p,
    PICOFB_Key_q, PICOFB_Key_r, PICOFB_Key_s, PICOFB_Key_t,
    PICOFB_Key_u, PICOFB_Key_v, PICOFB_Key_w, PICOFB_Key_x,
    PICOFB_Key_y, PICOFB_Key_z,
    // Numbers
    PICOFB_Key_0, PICOFB_Key_1, PICOFB_Key_2, PICOFB_Key_3,
    PICOFB_Key_4, PICOFB_Key_5, PICOFB_Key_6, PICOFB_Key_7,
    PICOFB_Key_8, PICOFB_Key_9,
    // Function keys
    PICOFB_Key_F1, PICOFB_Key_F2, PICOFB_Key_F3, PICOFB_Key_F4,
    PICOFB_Key_F5, PICOFB_Key_F6, PICOFB_Key_F7, PICOFB_Key_F8,
    PICOFB_Key_F9, PICOFB_Key_F10, PICOFB_Key_F11, PICOFB_Key_F12,
    // Arrow keys
    PICOFB_Key_UP, PICOFB_Key_DOWN, PICOFB_Key_LEFT, PICOFB_Key_RIGHT,
    // Control keys
    PICOFB_Key_ESC, PICOFB_Key_SPACE, PICOFB_Key_ENTER, PICOFB_Key_TAB,
    PICOFB_Key_BACKSPACE, PICOFB_Key_DELETE, PICOFB_Key_INSERT,
    PICOFB_Key_HOME, PICOFB_Key_END, PICOFB_Key_PAGEUP, PICOFB_Key_PAGEDOWN,
    // Modifiers
    PICOFB_Key_LSHIFT, PICOFB_Key_RSHIFT, PICOFB_Key_LCTRL, PICOFB_Key_RCTRL,
    PICOFB_Key_LALT, PICOFB_Key_RALT, PICOFB_Key_LSUPER, PICOFB_Key_RSUPER,
    // Punctuation/Symbols
    PICOFB_Key_MINUS, PICOFB_Key_EQUALS, PICOFB_Key_LBRACKET, PICOFB_Key_RBRACKET,
    PICOFB_Key_BACKSLASH, PICOFB_Key_SEMICOLON, PICOFB_Key_APOSTROPHE,
    PICOFB_Key_COMMA, PICOFB_Key_PERIOD, PICOFB_Key_SLASH, PICOFB_Key_GRAVE,
    // Numpad
    PICOFB_Key_KP_0, PICOFB_Key_KP_1, PICOFB_Key_KP_2, PICOFB_Key_KP_3,
    PICOFB_Key_KP_4, PICOFB_Key_KP_5, PICOFB_Key_KP_6, PICOFB_Key_KP_7,
    PICOFB_Key_KP_8, PICOFB_Key_KP_9,
    PICOFB_Key_KP_MULTIPLY, PICOFB_Key_KP_PLUS, PICOFB_Key_KP_MINUS,
    PICOFB_Key_KP_DIVIDE, PICOFB_Key_KP_ENTER, PICOFB_Key_KP_PERIOD,
    // Lock keys
    PICOFB_Key_CAPSLOCK, PICOFB_Key_NUMLOCK, PICOFB_Key_SCROLLLOCK,
    PICOFB_Key_COUNT
} PICOFB_Key;

//================================================================
// BACKEND SELECT
//================================================================

#ifndef PICOFB_BACKEND_OVERRIDE

#ifdef _WIN32
#define PICOFB_WIN32_BACKEND
#else

#ifdef __linux__
#ifdef PICOFB_WAYLAND
#define PICOFB_WAYLAND_BACKEND
#else
#define PICOFB_X11_BACKEND
#endif

#else
#define PICOFB_SDL_BACKEND
#endif

#endif

#endif // !(PICOFB_BACKEND_OVERRIDE)

//================================================================
// BACKEND IMPLEMENTATIONS
//================================================================

#ifdef PICOFB_X11_BACKEND

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
    uint32_t frame_buffer[PICOFB_HEIGHT][PICOFB_WIDTH];
    bool key_states[PICOFB_Key_COUNT];
    bool quit;

    Display* display;
    Window window;
    XWindowAttributes wa;
    XImage* image;
    GC gc;
    Atom wm_delete_window;
    XEvent event;
} PICOFB_Window;

static inline PICOFB_Key PICOFB_from_x11_keysym(KeySym key){
    switch(key) {
        // Letters
        case XK_a: return PICOFB_Key_a;
        case XK_b: return PICOFB_Key_b;
        case XK_c: return PICOFB_Key_c;
        case XK_d: return PICOFB_Key_d;
        case XK_e: return PICOFB_Key_e;
        case XK_f: return PICOFB_Key_f;
        case XK_g: return PICOFB_Key_g;
        case XK_h: return PICOFB_Key_h;
        case XK_i: return PICOFB_Key_i;
        case XK_j: return PICOFB_Key_j;
        case XK_k: return PICOFB_Key_k;
        case XK_l: return PICOFB_Key_l;
        case XK_m: return PICOFB_Key_m;
        case XK_n: return PICOFB_Key_n;
        case XK_o: return PICOFB_Key_o;
        case XK_p: return PICOFB_Key_p;
        case XK_q: return PICOFB_Key_q;
        case XK_r: return PICOFB_Key_r;
        case XK_s: return PICOFB_Key_s;
        case XK_t: return PICOFB_Key_t;
        case XK_u: return PICOFB_Key_u;
        case XK_v: return PICOFB_Key_v;
        case XK_w: return PICOFB_Key_w;
        case XK_x: return PICOFB_Key_x;
        case XK_y: return PICOFB_Key_y;
        case XK_z: return PICOFB_Key_z;
        // Numbers
        case XK_0: return PICOFB_Key_0;
        case XK_1: return PICOFB_Key_1;
        case XK_2: return PICOFB_Key_2;
        case XK_3: return PICOFB_Key_3;
        case XK_4: return PICOFB_Key_4;
        case XK_5: return PICOFB_Key_5;
        case XK_6: return PICOFB_Key_6;
        case XK_7: return PICOFB_Key_7;
        case XK_8: return PICOFB_Key_8;
        case XK_9: return PICOFB_Key_9;
        // Function keys
        case XK_F1: return PICOFB_Key_F1;
        case XK_F2: return PICOFB_Key_F2;
        case XK_F3: return PICOFB_Key_F3;
        case XK_F4: return PICOFB_Key_F4;
        case XK_F5: return PICOFB_Key_F5;
        case XK_F6: return PICOFB_Key_F6;
        case XK_F7: return PICOFB_Key_F7;
        case XK_F8: return PICOFB_Key_F8;
        case XK_F9: return PICOFB_Key_F9;
        case XK_F10: return PICOFB_Key_F10;
        case XK_F11: return PICOFB_Key_F11;
        case XK_F12: return PICOFB_Key_F12;
        // Arrow keys
        case XK_Up: return PICOFB_Key_UP;
        case XK_Down: return PICOFB_Key_DOWN;
        case XK_Left: return PICOFB_Key_LEFT;
        case XK_Right: return PICOFB_Key_RIGHT;
        // Control keys
        case XK_Escape: return PICOFB_Key_ESC;
        case XK_space: return PICOFB_Key_SPACE;
        case XK_Return: return PICOFB_Key_ENTER;
        case XK_Tab: return PICOFB_Key_TAB;
        case XK_BackSpace: return PICOFB_Key_BACKSPACE;
        case XK_Delete: return PICOFB_Key_DELETE;
        case XK_Insert: return PICOFB_Key_INSERT;
        case XK_Home: return PICOFB_Key_HOME;
        case XK_End: return PICOFB_Key_END;
        case XK_Page_Up: return PICOFB_Key_PAGEUP;
        case XK_Page_Down: return PICOFB_Key_PAGEDOWN;
        // Modifiers
        case XK_Shift_L: return PICOFB_Key_LSHIFT;
        case XK_Shift_R: return PICOFB_Key_RSHIFT;
        case XK_Control_L: return PICOFB_Key_LCTRL;
        case XK_Control_R: return PICOFB_Key_RCTRL;
        case XK_Alt_L: return PICOFB_Key_LALT;
        case XK_Alt_R: return PICOFB_Key_RALT;
        case XK_Super_L: return PICOFB_Key_LSUPER;
        case XK_Super_R: return PICOFB_Key_RSUPER;
        // Punctuation
        case XK_minus: return PICOFB_Key_MINUS;
        case XK_equal: return PICOFB_Key_EQUALS;
        case XK_bracketleft: return PICOFB_Key_LBRACKET;
        case XK_bracketright: return PICOFB_Key_RBRACKET;
        case XK_backslash: return PICOFB_Key_BACKSLASH;
        case XK_semicolon: return PICOFB_Key_SEMICOLON;
        case XK_apostrophe: return PICOFB_Key_APOSTROPHE;
        case XK_comma: return PICOFB_Key_COMMA;
        case XK_period: return PICOFB_Key_PERIOD;
        case XK_slash: return PICOFB_Key_SLASH;
        case XK_grave: return PICOFB_Key_GRAVE;
        // Numpad
        case XK_KP_0: return PICOFB_Key_KP_0;
        case XK_KP_1: return PICOFB_Key_KP_1;
        case XK_KP_2: return PICOFB_Key_KP_2;
        case XK_KP_3: return PICOFB_Key_KP_3;
        case XK_KP_4: return PICOFB_Key_KP_4;
        case XK_KP_5: return PICOFB_Key_KP_5;
        case XK_KP_6: return PICOFB_Key_KP_6;
        case XK_KP_7: return PICOFB_Key_KP_7;
        case XK_KP_8: return PICOFB_Key_KP_8;
        case XK_KP_9: return PICOFB_Key_KP_9;
        case XK_KP_Multiply: return PICOFB_Key_KP_MULTIPLY;
        case XK_KP_Add: return PICOFB_Key_KP_PLUS;
        case XK_KP_Subtract: return PICOFB_Key_KP_MINUS;
        case XK_KP_Divide: return PICOFB_Key_KP_DIVIDE;
        case XK_KP_Enter: return PICOFB_Key_KP_ENTER;
        case XK_KP_Decimal: return PICOFB_Key_KP_PERIOD;
        // Lock keys
        case XK_Caps_Lock: return PICOFB_Key_CAPSLOCK;
        case XK_Num_Lock: return PICOFB_Key_NUMLOCK;
        case XK_Scroll_Lock: return PICOFB_Key_SCROLLLOCK;
        
        default: return PICOFB_Key_UNKNOWN;
    }
}

static int PICOFB_destroy_image(XImage *img) {(void)img; return 0;}
static inline bool PICOFB_init(const char* window_title, PICOFB_Window* picofb_window) {
    if (!picofb_window) return false;
    picofb_window->display = XOpenDisplay(NULL);
    if (picofb_window->display == NULL) return false;
    picofb_window->window = XCreateSimpleWindow(picofb_window->display, XDefaultRootWindow(picofb_window->display), 0, 0, PICOFB_WIDTH, PICOFB_HEIGHT, 0, 0, 0);
    if (!XGetWindowAttributes(picofb_window->display, picofb_window->window, &picofb_window->wa)) {
        XDestroyWindow(picofb_window->display, picofb_window->window);
        XCloseDisplay(picofb_window->display);
        return false;
    }
    picofb_window->quit = false;
    picofb_window->image = XCreateImage(picofb_window->display, picofb_window->wa.visual, picofb_window->wa.depth, ZPixmap, 0, (char*) picofb_window->frame_buffer, PICOFB_WIDTH, PICOFB_HEIGHT, 32, PICOFB_WIDTH * sizeof(uint32_t));
    if (!picofb_window->image) {
        XDestroyWindow(picofb_window->display, picofb_window->window);
        XCloseDisplay(picofb_window->display);
        return false;
    }
    picofb_window->image->f.destroy_image = PICOFB_destroy_image;
    picofb_window->gc = XCreateGC(picofb_window->display, picofb_window->window, 0, NULL);
    if (!picofb_window->gc) {
        XDestroyImage(picofb_window->image);
        XDestroyWindow(picofb_window->display, picofb_window->window);
        XCloseDisplay(picofb_window->display);
        return false;
    }
    picofb_window->wm_delete_window = XInternAtom(picofb_window->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(picofb_window->display, picofb_window->window, &picofb_window->wm_delete_window, 1);
    XSelectInput(picofb_window->display, picofb_window->window, KeyPressMask | KeyReleaseMask | ExposureMask | StructureNotifyMask);
    XStoreName(picofb_window->display, picofb_window->window, window_title ? window_title : "PICOFB");
    XMapWindow(picofb_window->display, picofb_window->window);
    XSizeHints hints = {0};
    hints.flags = PMinSize | PMaxSize;
    hints.min_width = hints.max_width = PICOFB_WIDTH;
    hints.min_height = hints.max_height = PICOFB_HEIGHT;
    XSetWMNormalHints(picofb_window->display, picofb_window->window, &hints);
    XFlush(picofb_window->display);
    return true;
}

static inline uint32_t PICOFB_color_rgb(uint8_t r, uint8_t g, uint8_t b) {return (0xFFu << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;}
static inline void PICOFB_set_pixel(PICOFB_Window* picofb_window, size_t x, size_t y, uint32_t color){picofb_window->frame_buffer[y][x] = color;}

static inline void PICOFB_update(PICOFB_Window* picofb_window) {
    while (XPending(picofb_window->display) > 0) {
        XNextEvent(picofb_window->display, &picofb_window->event);
        switch(picofb_window->event.type) {
            case KeyPress: {
                PICOFB_Key key = PICOFB_from_x11_keysym(XLookupKeysym(&picofb_window->event.xkey, 0));
                if (key != PICOFB_Key_UNKNOWN) picofb_window->key_states[key] = true;
                break;
            }
            case KeyRelease: {
                PICOFB_Key key = PICOFB_from_x11_keysym(XLookupKeysym(&picofb_window->event.xkey, 0));
                if (key != PICOFB_Key_UNKNOWN) picofb_window->key_states[key] = false;
                break;
            }
            case ClientMessage:
                if ((Atom) picofb_window->event.xclient.data.l[0] == picofb_window->wm_delete_window) picofb_window->quit = true;
            break;
        }
    }
    XPutImage(picofb_window->display, picofb_window->window, picofb_window->gc, picofb_window->image, 0, 0, 0, 0, PICOFB_WIDTH, PICOFB_HEIGHT);
    XFlush(picofb_window->display);
}

static inline bool PICOFB_is_input(PICOFB_Window* picofb_window, PICOFB_Key key) {
    if (key < 0 || key >= PICOFB_Key_COUNT) return false;
    return picofb_window->key_states[key];
}

static inline void PICOFB_cleanup(PICOFB_Window* picofb_window) {
    if (picofb_window->image) XDestroyImage(picofb_window->image);
    if (picofb_window->gc) XFreeGC(picofb_window->display, picofb_window->gc);
    if (picofb_window->window) XDestroyWindow(picofb_window->display, picofb_window->window);
    if (picofb_window->display) XCloseDisplay(picofb_window->display);
}

static inline void PICOFB_save_ppm(PICOFB_Window* picofb_window, const char *path){
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6 %d %d 255\n", PICOFB_WIDTH, PICOFB_HEIGHT);
    for (int y = 0; y < PICOFB_HEIGHT; y++){
        for (int x = 0; x < PICOFB_WIDTH; x++){
            uint32_t px = picofb_window->frame_buffer[y][x];
            unsigned char rgb[3] = { (px >> 16) & 0xFF, (px >> 8) & 0xFF, px & 0xFF };
            fwrite(rgb, 1, 3, f);
        }
    }
    fclose(f);
}

#endif // PICOFB_X11_BACKEND

#ifdef PICOFB_WIN32_BACKEND

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string.h>

typedef struct {
    uint32_t frame_buffer[PICOFB_HEIGHT][PICOFB_WIDTH];
    bool key_states[PICOFB_Key_COUNT];
    bool quit;

    HWND hwnd;
    HDC hdc;
    HDC hdc_mem;
    HBITMAP hbitmap;
    HBITMAP hbitmap_old;
    WNDCLASSEX wc;
    char class_name_storage[64];
    void* dib_bits;
} PICOFB_Window;

static inline PICOFB_Key PICOFB_from_win32_vk(WPARAM vk, LPARAM lparam) {
    bool extended = (lparam & 0x01000000) != 0;
    switch(vk) {
        // Letters
        case 'A': return PICOFB_Key_a;
        case 'B': return PICOFB_Key_b;
        case 'C': return PICOFB_Key_c;
        case 'D': return PICOFB_Key_d;
        case 'E': return PICOFB_Key_e;
        case 'F': return PICOFB_Key_f;
        case 'G': return PICOFB_Key_g;
        case 'H': return PICOFB_Key_h;
        case 'I': return PICOFB_Key_i;
        case 'J': return PICOFB_Key_j;
        case 'K': return PICOFB_Key_k;
        case 'L': return PICOFB_Key_l;
        case 'M': return PICOFB_Key_m;
        case 'N': return PICOFB_Key_n;
        case 'O': return PICOFB_Key_o;
        case 'P': return PICOFB_Key_p;
        case 'Q': return PICOFB_Key_q;
        case 'R': return PICOFB_Key_r;
        case 'S': return PICOFB_Key_s;
        case 'T': return PICOFB_Key_t;
        case 'U': return PICOFB_Key_u;
        case 'V': return PICOFB_Key_v;
        case 'W': return PICOFB_Key_w;
        case 'X': return PICOFB_Key_x;
        case 'Y': return PICOFB_Key_y;
        case 'Z': return PICOFB_Key_z;
        // Numbers
        case '0': return PICOFB_Key_0;
        case '1': return PICOFB_Key_1;
        case '2': return PICOFB_Key_2;
        case '3': return PICOFB_Key_3;
        case '4': return PICOFB_Key_4;
        case '5': return PICOFB_Key_5;
        case '6': return PICOFB_Key_6;
        case '7': return PICOFB_Key_7;
        case '8': return PICOFB_Key_8;
        case '9': return PICOFB_Key_9;
        // Function keys
        case VK_F1: return PICOFB_Key_F1;
        case VK_F2: return PICOFB_Key_F2;
        case VK_F3: return PICOFB_Key_F3;
        case VK_F4: return PICOFB_Key_F4;
        case VK_F5: return PICOFB_Key_F5;
        case VK_F6: return PICOFB_Key_F6;
        case VK_F7: return PICOFB_Key_F7;
        case VK_F8: return PICOFB_Key_F8;
        case VK_F9: return PICOFB_Key_F9;
        case VK_F10: return PICOFB_Key_F10;
        case VK_F11: return PICOFB_Key_F11;
        case VK_F12: return PICOFB_Key_F12;
        // Arrow keys
        case VK_UP: return PICOFB_Key_UP;
        case VK_DOWN: return PICOFB_Key_DOWN;
        case VK_LEFT: return PICOFB_Key_LEFT;
        case VK_RIGHT: return PICOFB_Key_RIGHT;
        // Control keys
        case VK_ESCAPE: return PICOFB_Key_ESC;
        case VK_SPACE: return PICOFB_Key_SPACE;
        case VK_RETURN: return extended ? PICOFB_Key_KP_ENTER : PICOFB_Key_ENTER;
        case VK_TAB: return PICOFB_Key_TAB;
        case VK_BACK: return PICOFB_Key_BACKSPACE;
        case VK_DELETE: return PICOFB_Key_DELETE;
        case VK_INSERT: return PICOFB_Key_INSERT;
        case VK_HOME: return PICOFB_Key_HOME;
        case VK_END: return PICOFB_Key_END;
        case VK_PRIOR: return PICOFB_Key_PAGEUP;
        case VK_NEXT: return PICOFB_Key_PAGEDOWN;
        // Modifiers
        case VK_SHIFT: return MapVirtualKey(VK_RSHIFT, MAPVK_VK_TO_VSC) == ((lparam >> 16) & 0xFF) ? PICOFB_Key_RSHIFT : PICOFB_Key_LSHIFT;
        case VK_CONTROL: return extended ? PICOFB_Key_RCTRL : PICOFB_Key_LCTRL;
        case VK_MENU: return extended ? PICOFB_Key_RALT : PICOFB_Key_LALT;
        case VK_LWIN: return PICOFB_Key_LSUPER;
        case VK_RWIN: return PICOFB_Key_RSUPER;
        // Punctuation
        case VK_OEM_MINUS: return PICOFB_Key_MINUS;
        case VK_OEM_PLUS: return PICOFB_Key_EQUALS;
        case VK_OEM_4: return PICOFB_Key_LBRACKET;
        case VK_OEM_6: return PICOFB_Key_RBRACKET;
        case VK_OEM_5: return PICOFB_Key_BACKSLASH;
        case VK_OEM_1: return PICOFB_Key_SEMICOLON;
        case VK_OEM_7: return PICOFB_Key_APOSTROPHE;
        case VK_OEM_COMMA: return PICOFB_Key_COMMA;
        case VK_OEM_PERIOD: return PICOFB_Key_PERIOD;
        case VK_OEM_2: return PICOFB_Key_SLASH;
        case VK_OEM_3: return PICOFB_Key_GRAVE;
        // Numpad
        case VK_NUMPAD0: return PICOFB_Key_KP_0;
        case VK_NUMPAD1: return PICOFB_Key_KP_1;
        case VK_NUMPAD2: return PICOFB_Key_KP_2;
        case VK_NUMPAD3: return PICOFB_Key_KP_3;
        case VK_NUMPAD4: return PICOFB_Key_KP_4;
        case VK_NUMPAD5: return PICOFB_Key_KP_5;
        case VK_NUMPAD6: return PICOFB_Key_KP_6;
        case VK_NUMPAD7: return PICOFB_Key_KP_7;
        case VK_NUMPAD8: return PICOFB_Key_KP_8;
        case VK_NUMPAD9: return PICOFB_Key_KP_9;
        case VK_MULTIPLY: return PICOFB_Key_KP_MULTIPLY;
        case VK_ADD: return PICOFB_Key_KP_PLUS;
        case VK_SUBTRACT: return PICOFB_Key_KP_MINUS;
        case VK_DIVIDE: return PICOFB_Key_KP_DIVIDE;
        case VK_DECIMAL: return PICOFB_Key_KP_PERIOD;
        // Lock keys
        case VK_CAPITAL: return PICOFB_Key_CAPSLOCK;
        case VK_NUMLOCK: return PICOFB_Key_NUMLOCK;
        case VK_SCROLL: return PICOFB_Key_SCROLLLOCK;
        
        default: return PICOFB_Key_UNKNOWN;
    }
}

static LRESULT CALLBACK PICOFB_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    PICOFB_Window* window = (PICOFB_Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    switch (msg) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN: {
            if (window) {
                PICOFB_Key key = PICOFB_from_win32_vk(wparam, lparam);
                if (key != PICOFB_Key_UNKNOWN && !(lparam & 0x40000000)) window->key_states[key] = true;
            }
            return 0;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            if (window) {
                PICOFB_Key key = PICOFB_from_win32_vk(wparam, lparam);
                if (key != PICOFB_Key_UNKNOWN) window->key_states[key] = false;
            }
            return 0;
        }
        case WM_CLOSE: if (window) window->quit = true; return 0;
        case WM_DESTROY: PostQuitMessage(0); return 0;
        default: return DefWindowProc(hwnd, msg, wparam, lparam);
    }
}
static inline bool PICOFB_init(const char* window_title, PICOFB_Window* picofb_window) {
    if (!picofb_window) return false;
    snprintf(picofb_window->class_name_storage, sizeof(picofb_window->class_name_storage), "PICOFB_%p", (void*)picofb_window);
    picofb_window->wc.cbSize = sizeof(WNDCLASSEX);
    picofb_window->wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    picofb_window->wc.lpfnWndProc = PICOFB_window_proc;
    picofb_window->wc.cbClsExtra = 0;
    picofb_window->wc.cbWndExtra = 0;
    picofb_window->wc.hInstance = GetModuleHandle(NULL);
    picofb_window->wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    picofb_window->wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    picofb_window->wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    picofb_window->wc.lpszMenuName = NULL;
    picofb_window->wc.lpszClassName = picofb_window->class_name_storage;
    picofb_window->wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassEx(&picofb_window->wc)) return false;
    RECT rect = {0, 0, PICOFB_WIDTH, PICOFB_HEIGHT};
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    AdjustWindowRect(&rect, style, FALSE);
    picofb_window->hwnd = CreateWindowEx(0, picofb_window->class_name_storage, window_title ? window_title : "PICOFB", style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, GetModuleHandle(NULL), NULL);
    if (!picofb_window->hwnd) {UnregisterClass(picofb_window->class_name_storage, GetModuleHandle(NULL)); return false;}
    SetWindowLongPtr(picofb_window->hwnd, GWLP_USERDATA, (LONG_PTR)picofb_window);
    picofb_window->hdc = GetDC(picofb_window->hwnd);
    if (!picofb_window->hdc) {
        DestroyWindow(picofb_window->hwnd);
        UnregisterClass(picofb_window->class_name_storage, GetModuleHandle(NULL));
        return false;
    }
    picofb_window->hdc_mem = CreateCompatibleDC(picofb_window->hdc);
    if (!picofb_window->hdc_mem) {
        ReleaseDC(picofb_window->hwnd, picofb_window->hdc);
        DestroyWindow(picofb_window->hwnd);
        UnregisterClass(picofb_window->class_name_storage, GetModuleHandle(NULL));
        return false;
    }
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = PICOFB_WIDTH;
    bmi.bmiHeader.biHeight = -PICOFB_HEIGHT;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    picofb_window->hbitmap = CreateDIBSection(picofb_window->hdc_mem, &bmi, DIB_RGB_COLORS, &picofb_window->dib_bits, NULL, 0);
    if (!picofb_window->hbitmap) {
        DeleteDC(picofb_window->hdc_mem);
        ReleaseDC(picofb_window->hwnd, picofb_window->hdc);
        DestroyWindow(picofb_window->hwnd);
        UnregisterClass(picofb_window->class_name_storage, GetModuleHandle(NULL));
        return false;
    }
    picofb_window->hbitmap_old = (HBITMAP)SelectObject(picofb_window->hdc_mem, picofb_window->hbitmap);
    ShowWindow(picofb_window->hwnd, SW_SHOW);
    UpdateWindow(picofb_window->hwnd);
    return true;
}

static inline uint32_t PICOFB_color_rgb(uint8_t r, uint8_t g, uint8_t b) {return (0xFFu << 24) | ((uint32_t)b << 16) | ((uint32_t)g << 8) | (uint32_t)r;}
static inline void PICOFB_set_pixel(PICOFB_Window* picofb_window, size_t x, size_t y, uint32_t color){picofb_window->frame_buffer[y][x] = color;}

static inline void PICOFB_update(PICOFB_Window* picofb_window) {
    MSG msg;
    while (PeekMessage(&msg, picofb_window->hwnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) picofb_window->quit = true;
    }
    memcpy(picofb_window->dib_bits, picofb_window->frame_buffer, PICOFB_WIDTH * PICOFB_HEIGHT * sizeof(uint32_t));
    BitBlt(picofb_window->hdc, 0, 0, PICOFB_WIDTH, PICOFB_HEIGHT, picofb_window->hdc_mem, 0, 0, SRCCOPY);
}

static inline bool PICOFB_is_input(PICOFB_Window* picofb_window, PICOFB_Key key) {
    if (key < 0 || key >= PICOFB_Key_COUNT) return false;
    return picofb_window->key_states[key];
}

static inline void PICOFB_cleanup(PICOFB_Window* picofb_window) {
    if (picofb_window->hbitmap) {
        if (picofb_window->hdc_mem && picofb_window->hbitmap_old) SelectObject(picofb_window->hdc_mem, picofb_window->hbitmap_old);
        DeleteObject(picofb_window->hbitmap);
    }
    if (picofb_window->hdc_mem) DeleteDC(picofb_window->hdc_mem);
    if (picofb_window->hdc && picofb_window->hwnd) ReleaseDC(picofb_window->hwnd, picofb_window->hdc);
    if (picofb_window->hwnd) DestroyWindow(picofb_window->hwnd);
    if (picofb_window->class_name_storage) UnregisterClass(picofb_window->class_name_storage, GetModuleHandle(NULL));
}

static inline void PICOFB_save_ppm(PICOFB_Window* picofb_window, const char *path) {
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6 %d %d 255\n", PICOFB_WIDTH, PICOFB_HEIGHT);
    for (int y = 0; y < PICOFB_HEIGHT; y++) {
        for (int x = 0; x < PICOFB_WIDTH; x++) {
            uint32_t px = picofb_window->frame_buffer[y][x];
            unsigned char rgb[3] = { px & 0xFF, (px >> 8) & 0xFF, (px >> 16) & 0xFF };
            fwrite(rgb, 1, 3, f);
        }
    }
    fclose(f);
}

#endif // PICOFB_WIN32_BACKEND

#ifdef PICOFB_WAYLAND_BACKEND

// ///////////////////////////////////WIP///////////////////////////////////////////

#include <wayland-client.h>

typedef struct {
    uint32_t frame_buffer[PICOFB_HEIGHT][PICOFB_WIDTH];
    bool key_states[PICOFB_Key_COUNT];
    bool quit;

    struct wl_display *display;
} PICOFB_Window;

//static inline PICOFB_Key PICOFB_from_sdl_scancode(SDL_Scancode sc);

static inline bool PICOFB_init(const char* window_title, PICOFB_Window* picofb_window){
    if (!picofb_window) return false;
    picofb_window->display = wl_display_connect(NULL);
    if (!picofb_window->display) return false;
    return true;
}

static inline uint32_t PICOFB_color_rgb(uint8_t r, uint8_t g, uint8_t b){return (0xFFu << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;}
static inline void PICOFB_set_pixel(PICOFB_Window* picofb_window, size_t x, size_t y, uint32_t color){picofb_window->frame_buffer[y][x] = color;}

static inline void PICOFB_update(PICOFB_Window* picofb_window){
    picofb_window->quit = wl_display_dispatch(display)==-1;
}
static inline bool PICOFB_is_input(PICOFB_Window* picofb_window, PICOFB_Key key){
    return true;
}
static inline void PICOFB_cleanup(PICOFB_Window* picofb_window){
    wl_display_disconnect(picofb_window->display);
}
static inline void PICOFB_save_ppm(PICOFB_Window* picofb_window, const char *path){

}

#endif // PICOFB_WAYLAND_BACKEND

#ifdef PICOFB_SDL_BACKEND

#include <SDL2/SDL.h>

typedef struct {
    uint32_t frame_buffer[PICOFB_HEIGHT][PICOFB_WIDTH];
    bool key_states[PICOFB_Key_COUNT];
    bool quit;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Event event;
} PICOFB_Window;

static inline PICOFB_Key PICOFB_from_sdl_scancode(SDL_Scancode sc) {
    switch(sc) {
        // Letters
        case SDL_SCANCODE_A: return PICOFB_Key_a;
        case SDL_SCANCODE_B: return PICOFB_Key_b;
        case SDL_SCANCODE_C: return PICOFB_Key_c;
        case SDL_SCANCODE_D: return PICOFB_Key_d;
        case SDL_SCANCODE_E: return PICOFB_Key_e;
        case SDL_SCANCODE_F: return PICOFB_Key_f;
        case SDL_SCANCODE_G: return PICOFB_Key_g;
        case SDL_SCANCODE_H: return PICOFB_Key_h;
        case SDL_SCANCODE_I: return PICOFB_Key_i;
        case SDL_SCANCODE_J: return PICOFB_Key_j;
        case SDL_SCANCODE_K: return PICOFB_Key_k;
        case SDL_SCANCODE_L: return PICOFB_Key_l;
        case SDL_SCANCODE_M: return PICOFB_Key_m;
        case SDL_SCANCODE_N: return PICOFB_Key_n;
        case SDL_SCANCODE_O: return PICOFB_Key_o;
        case SDL_SCANCODE_P: return PICOFB_Key_p;
        case SDL_SCANCODE_Q: return PICOFB_Key_q;
        case SDL_SCANCODE_R: return PICOFB_Key_r;
        case SDL_SCANCODE_S: return PICOFB_Key_s;
        case SDL_SCANCODE_T: return PICOFB_Key_t;
        case SDL_SCANCODE_U: return PICOFB_Key_u;
        case SDL_SCANCODE_V: return PICOFB_Key_v;
        case SDL_SCANCODE_W: return PICOFB_Key_w;
        case SDL_SCANCODE_X: return PICOFB_Key_x;
        case SDL_SCANCODE_Y: return PICOFB_Key_y;
        case SDL_SCANCODE_Z: return PICOFB_Key_z;
        // Numbers
        case SDL_SCANCODE_0: return PICOFB_Key_0;
        case SDL_SCANCODE_1: return PICOFB_Key_1;
        case SDL_SCANCODE_2: return PICOFB_Key_2;
        case SDL_SCANCODE_3: return PICOFB_Key_3;
        case SDL_SCANCODE_4: return PICOFB_Key_4;
        case SDL_SCANCODE_5: return PICOFB_Key_5;
        case SDL_SCANCODE_6: return PICOFB_Key_6;
        case SDL_SCANCODE_7: return PICOFB_Key_7;
        case SDL_SCANCODE_8: return PICOFB_Key_8;
        case SDL_SCANCODE_9: return PICOFB_Key_9;
        // Function keys
        case SDL_SCANCODE_F1: return PICOFB_Key_F1;
        case SDL_SCANCODE_F2: return PICOFB_Key_F2;
        case SDL_SCANCODE_F3: return PICOFB_Key_F3;
        case SDL_SCANCODE_F4: return PICOFB_Key_F4;
        case SDL_SCANCODE_F5: return PICOFB_Key_F5;
        case SDL_SCANCODE_F6: return PICOFB_Key_F6;
        case SDL_SCANCODE_F7: return PICOFB_Key_F7;
        case SDL_SCANCODE_F8: return PICOFB_Key_F8;
        case SDL_SCANCODE_F9: return PICOFB_Key_F9;
        case SDL_SCANCODE_F10: return PICOFB_Key_F10;
        case SDL_SCANCODE_F11: return PICOFB_Key_F11;
        case SDL_SCANCODE_F12: return PICOFB_Key_F12;
        // Arrow keys
        case SDL_SCANCODE_UP: return PICOFB_Key_UP;
        case SDL_SCANCODE_DOWN: return PICOFB_Key_DOWN;
        case SDL_SCANCODE_LEFT: return PICOFB_Key_LEFT;
        case SDL_SCANCODE_RIGHT: return PICOFB_Key_RIGHT;
        // Control keys
        case SDL_SCANCODE_ESCAPE: return PICOFB_Key_ESC;
        case SDL_SCANCODE_SPACE: return PICOFB_Key_SPACE;
        case SDL_SCANCODE_RETURN: return PICOFB_Key_ENTER;
        case SDL_SCANCODE_TAB: return PICOFB_Key_TAB;
        case SDL_SCANCODE_BACKSPACE: return PICOFB_Key_BACKSPACE;
        case SDL_SCANCODE_DELETE: return PICOFB_Key_DELETE;
        case SDL_SCANCODE_INSERT: return PICOFB_Key_INSERT;
        case SDL_SCANCODE_HOME: return PICOFB_Key_HOME;
        case SDL_SCANCODE_END: return PICOFB_Key_END;
        case SDL_SCANCODE_PAGEUP: return PICOFB_Key_PAGEUP;
        case SDL_SCANCODE_PAGEDOWN: return PICOFB_Key_PAGEDOWN;
        // Modifiers
        case SDL_SCANCODE_LSHIFT: return PICOFB_Key_LSHIFT;
        case SDL_SCANCODE_RSHIFT: return PICOFB_Key_RSHIFT;
        case SDL_SCANCODE_LCTRL: return PICOFB_Key_LCTRL;
        case SDL_SCANCODE_RCTRL: return PICOFB_Key_RCTRL;
        case SDL_SCANCODE_LALT: return PICOFB_Key_LALT;
        case SDL_SCANCODE_RALT: return PICOFB_Key_RALT;
        case SDL_SCANCODE_LGUI: return PICOFB_Key_LSUPER;
        case SDL_SCANCODE_RGUI: return PICOFB_Key_RSUPER;
        // Punctuation
        case SDL_SCANCODE_MINUS: return PICOFB_Key_MINUS;
        case SDL_SCANCODE_EQUALS: return PICOFB_Key_EQUALS;
        case SDL_SCANCODE_LEFTBRACKET: return PICOFB_Key_LBRACKET;
        case SDL_SCANCODE_RIGHTBRACKET: return PICOFB_Key_RBRACKET;
        case SDL_SCANCODE_BACKSLASH: return PICOFB_Key_BACKSLASH;
        case SDL_SCANCODE_SEMICOLON: return PICOFB_Key_SEMICOLON;
        case SDL_SCANCODE_APOSTROPHE: return PICOFB_Key_APOSTROPHE;
        case SDL_SCANCODE_COMMA: return PICOFB_Key_COMMA;
        case SDL_SCANCODE_PERIOD: return PICOFB_Key_PERIOD;
        case SDL_SCANCODE_SLASH: return PICOFB_Key_SLASH;
        case SDL_SCANCODE_GRAVE: return PICOFB_Key_GRAVE;
        // Numpad
        case SDL_SCANCODE_KP_0: return PICOFB_Key_KP_0;
        case SDL_SCANCODE_KP_1: return PICOFB_Key_KP_1;
        case SDL_SCANCODE_KP_2: return PICOFB_Key_KP_2;
        case SDL_SCANCODE_KP_3: return PICOFB_Key_KP_3;
        case SDL_SCANCODE_KP_4: return PICOFB_Key_KP_4;
        case SDL_SCANCODE_KP_5: return PICOFB_Key_KP_5;
        case SDL_SCANCODE_KP_6: return PICOFB_Key_KP_6;
        case SDL_SCANCODE_KP_7: return PICOFB_Key_KP_7;
        case SDL_SCANCODE_KP_8: return PICOFB_Key_KP_8;
        case SDL_SCANCODE_KP_9: return PICOFB_Key_KP_9;
        case SDL_SCANCODE_KP_MULTIPLY: return PICOFB_Key_KP_MULTIPLY;
        case SDL_SCANCODE_KP_PLUS: return PICOFB_Key_KP_PLUS;
        case SDL_SCANCODE_KP_MINUS: return PICOFB_Key_KP_MINUS;
        case SDL_SCANCODE_KP_DIVIDE: return PICOFB_Key_KP_DIVIDE;
        case SDL_SCANCODE_KP_ENTER: return PICOFB_Key_KP_ENTER;
        case SDL_SCANCODE_KP_PERIOD: return PICOFB_Key_KP_PERIOD;
        // Lock keys
        case SDL_SCANCODE_CAPSLOCK: return PICOFB_Key_CAPSLOCK;
        case SDL_SCANCODE_NUMLOCKCLEAR: return PICOFB_Key_NUMLOCK;
        case SDL_SCANCODE_SCROLLLOCK: return PICOFB_Key_SCROLLLOCK;
        
        default: return PICOFB_Key_UNKNOWN;
    }
}

static inline bool PICOFB_init(const char* window_title, PICOFB_Window* picofb_window) {
    if (!picofb_window) return false;
    SDL_Init(SDL_INIT_VIDEO);
    picofb_window->window = SDL_CreateWindow(window_title ? window_title : "PICOFB", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, PICOFB_WIDTH, PICOFB_HEIGHT, 0);
    picofb_window->renderer = SDL_CreateRenderer(picofb_window->window, -1, SDL_RENDERER_ACCELERATED);
    picofb_window->texture = SDL_CreateTexture(picofb_window->renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, PICOFB_WIDTH, PICOFB_HEIGHT);
    if (!(picofb_window->window && picofb_window->renderer && picofb_window->texture)) return false;
    return true;
}

static inline uint32_t PICOFB_color_rgb(uint8_t r, uint8_t g, uint8_t b){return (0xFFu << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b;}
static inline void PICOFB_set_pixel(PICOFB_Window* picofb_window, size_t x, size_t y, uint32_t color){picofb_window->frame_buffer[y][x] = color;}

static inline void PICOFB_update(PICOFB_Window* picofb_window) {
    SDL_UpdateTexture(picofb_window->texture, NULL, picofb_window->frame_buffer, PICOFB_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(picofb_window->renderer);
    SDL_RenderCopy(picofb_window->renderer, picofb_window->texture, NULL, NULL);
    SDL_RenderPresent(picofb_window->renderer);
    while (SDL_PollEvent(&picofb_window->event)) {
        switch(picofb_window->event.type) {
            case SDL_KEYDOWN: {
                PICOFB_Key key = PICOFB_from_sdl_scancode(picofb_window->event.key.keysym.scancode);
                if (key != PICOFB_Key_UNKNOWN) picofb_window->key_states[key] = true;
                break;
            }
            case SDL_KEYUP: {
                PICOFB_Key key = PICOFB_from_sdl_scancode(picofb_window->event.key.keysym.scancode);
                if (key != PICOFB_Key_UNKNOWN) picofb_window->key_states[key] = false;
                break;
            }
            case SDL_QUIT:
                picofb_window->quit = true;
                break;
        }
    }
}

static inline bool PICOFB_is_input(PICOFB_Window* picofb_window, PICOFB_Key key) {
    if (key < 0 || key >= PICOFB_Key_COUNT) return false;
    return picofb_window->key_states[key];
}

static inline void PICOFB_cleanup(PICOFB_Window* picofb_window) {
    SDL_DestroyTexture(picofb_window->texture);
    SDL_DestroyRenderer(picofb_window->renderer);
    SDL_DestroyWindow(picofb_window->window);
    SDL_Quit();
}

static inline void PICOFB_save_ppm(PICOFB_Window* picofb_window, const char *path){
    FILE *f = fopen(path, "wb");
    if (!f) return;
    fprintf(f, "P6 %d %d 255\n", PICOFB_WIDTH, PICOFB_HEIGHT);
    for (int y = 0; y < PICOFB_HEIGHT; y++){
        for (int x = 0; x < PICOFB_WIDTH; x++){
            uint32_t px = picofb_window->frame_buffer[y][x];
            unsigned char rgb[3] = { (px >> 16) & 0xFF, (px >> 8) & 0xFF, px & 0xFF };
            fwrite(rgb, 1, 3, f);
        }
    }
    fclose(f);
}

#endif // PICOFB_SDL_BACKEND

#endif // _PICOFB_H