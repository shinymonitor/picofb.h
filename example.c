#include "picofb.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 360
uint32_t frame_buffer[SCREEN_WIDTH * SCREEN_HEIGHT]={0};

int main(){
    //Allocate window
    PICOFB_Window picofb_window={0};

    //Initialize the window
    if (!PICOFB_init("EXAMPLE", SCREEN_WIDTH, SCREEN_HEIGHT, frame_buffer, &picofb_window)) {fprintf(stderr, "init failed\n"); return 1;}

    //loop until quit
    while (!picofb_window.quit) {
        //modify frame buffer during main loop
        for (size_t y = 0; y < SCREEN_HEIGHT; ++y) {
            for (size_t x = 0; x < SCREEN_WIDTH; ++x) {
                frame_buffer[y*SCREEN_WIDTH+x] = PICOFB_color_argb(0xFF, (x * 255) / SCREEN_WIDTH, (y * 255) / SCREEN_HEIGHT, ((x + y) * 255) / (SCREEN_WIDTH + SCREEN_HEIGHT));
            }
        }

        // mouse input
        printf("Mouse: %d, %d %d ", picofb_window.mouse.x, picofb_window.mouse.y, picofb_window.mouse.scroll_delta);
        if (picofb_window.mouse.left) printf("Left ");
        if (picofb_window.mouse.middle) printf("Middle ");
        if (picofb_window.mouse.right) printf("Right ");

        // keyboard input (key_pressed() or .keyboard)
        if (PICOFB_key_pressed(&picofb_window, PICOFB_Key_w)) printf("w");
        if (PICOFB_key_pressed(&picofb_window, PICOFB_Key_s)) printf("s");
        if (picofb_window.keyboard[PICOFB_Key_a]) printf("a");
        if (picofb_window.keyboard[PICOFB_Key_d]) printf("d");
        printf("\n");
        if (PICOFB_key_pressed(&picofb_window, PICOFB_Key_q) || PICOFB_key_pressed(&picofb_window, PICOFB_Key_ESC)) break;

        //update the window
        PICOFB_update(&picofb_window);
    }

    //save last frame to ppm
    PICOFB_save_ppm(&picofb_window, "example.ppm");

    //cleanup window
    PICOFB_cleanup(&picofb_window);
    
    return 0;
}