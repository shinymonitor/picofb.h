//Define screen dimensions
#define WIDTH 640
#define HEIGHT 360

#include "picofb.h"

int main(){
    //Allocate window
    PICOFB_Window picofb_window={0};
    uint32_t fb[WIDTH * HEIGHT]={0};

    //Initialize the window
    if (!PICOFB_init("EXAMPLE", WIDTH, HEIGHT, fb, &picofb_window)) {fprintf(stderr, "init failed\n"); return 1;}

    //modify frame buffer before main loop (gradient)
    for (size_t y = 0; y < HEIGHT; ++y) for (size_t x = 0; x < WIDTH; ++x) PICOFB_set_pixel(&picofb_window, x, y, PICOFB_color_rgb((x * 255) / WIDTH, (y * 255) / HEIGHT, ((x + y) * 255) / (WIDTH + HEIGHT)));

    //loop until quit
    while (!picofb_window.quit) {
        //modify frame buffer during main loop
        //set a pixel to a rgb color
        PICOFB_set_pixel(&picofb_window, WIDTH/2, HEIGHT/2, PICOFB_color_rgb(255, 255, 255));

        //update the window
        PICOFB_update(&picofb_window);

        // mouse input example
        printf("Mouse: %zu, %zu %d ", picofb_window.mouse_x, picofb_window.mouse_y, picofb_window.scroll_delta);
        if (picofb_window.mouse_left) printf("Left ");
        if (picofb_window.mouse_middle) printf("Middle ");
        if (picofb_window.mouse_right) printf("Right ");

        //multikey input
        if (PICOFB_is_input(&picofb_window, PICOFB_Key_w)) printf("w");
        if (PICOFB_is_input(&picofb_window, PICOFB_Key_s)) printf("s");
        if (PICOFB_is_input(&picofb_window, PICOFB_Key_a)) printf("a");
        if (PICOFB_is_input(&picofb_window, PICOFB_Key_d)) printf("d");
        printf("\n");
        if (PICOFB_is_input(&picofb_window, PICOFB_Key_q) || PICOFB_is_input(&picofb_window, PICOFB_Key_ESC)) break;
    }

    //save last frame to ppm
    PICOFB_save_ppm(&picofb_window, "example.ppm");

    //cleanup window
    PICOFB_cleanup(&picofb_window);
    
    return 0;
}