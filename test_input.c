#include <stdio.h>

#include "base_graphics.h"
#include "input.h"

void on_press() {
    printf("你是个傻");
    fflush(stdout);
    return;
}

void on_relese() {
    printf("\n");
    fflush(stdout);
    return;
}

void on_drag() {
    printf("逼");
    fflush(stdout);
    return;
}


int main() {
    bg_init();
    bg_mouse_press_bind(on_press);
    bg_mouse_release_bind(on_relese);
    bg_mouse_drag_bind(on_drag);

    bg_mainloop();

    return 0;
}

