#include <stddef.h>
#include <stdio.h>

#include "base_graphics.h"
#include "input.h"
#include "point.h"
#include "draw.h"
#include "undo.h"

#define FUNC_LINE 0
#define FUNC_RECTANGLE 1
#define FUNC_FILL 2

#define MOUSE_UP 0
#define MOUSE_DOWN 1

int func=FUNC_LINE;
int mouse=MOUSE_DOWN;

struct bg_point from;

void on_press(struct bg_point * pos_p) {
    printf("on_press() %d %d\n", pos_p->x, pos_p->y);
    
    switch (func) {
        case FUNC_FILL:
            bg_draw_fill(pos_p);
            bg_undo_commit();
            break;
    }

    bg_draw_flush();

    return;
}

void on_release(struct bg_point * pos_p) {
    printf("on_release() %d %d\n", pos_p->x, pos_p->y);

    bg_undo_restore();

    switch (func) {
        case FUNC_LINE:
            bg_draw_line(&from, pos_p);
            break;
        case FUNC_RECTANGLE:
            bg_draw_rectangle(&from, pos_p);
            break;
    }

    bg_undo_commit();
    bg_draw_flush();

    mouse = MOUSE_UP;
    return;
}

void on_drag(struct bg_point * pos_p) {
    printf("on_drag() %d %d\n", pos_p->x, pos_p->y);

    if (mouse == MOUSE_UP)
    {
        mouse = MOUSE_DOWN;
        from = *pos_p;
    }

    bg_undo_restore();

    switch (func) {
        case FUNC_LINE:
            bg_draw_line(&from, pos_p);
            break;
        case FUNC_RECTANGLE:
            bg_draw_rectangle(&from, pos_p);
            break;
    }

    bg_draw_flush();
    return;
}

void menu_line() {
    fprintf(stderr, "asdfjaslkdfjasldkfja");
    func = FUNC_LINE;
    return;
}

void menu_rectangle() {
    func = FUNC_RECTANGLE;
    return;
}

void menu_fill() {
    func = FUNC_FILL;
    return;
}

void menu_clean() {
    bg_clean();
    bg_draw_flush();
    return;
}

int main() {
    bg_init();
    bg_mouse_press_bind(on_press);
    bg_mouse_release_bind(on_release);
    bg_mouse_drag_bind(on_drag);

    bg_menu_bind("直线", menu_line);
    bg_menu_bind("矩形", menu_rectangle);
    bg_menu_bind("填充", menu_fill);
    bg_menu_bind("清空画布", menu_clean);

    bg_mainloop();

    return 0;
}

