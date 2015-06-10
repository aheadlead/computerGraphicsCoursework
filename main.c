#include <stddef.h>
#include <stdio.h>

#include "base_graphics.h"
#include "input.h"
#include "point.h"
#include "draw.h"
#include "undo.h"

int state=0;
struct bg_point from;

void on_press(struct bg_point * pos_p) {
    printf("on_press() %d %d\n", pos_p->x, pos_p->y);
    return;
}

void on_release(struct bg_point * pos_p) {
    printf("on_release() %d %d\n", pos_p->x, pos_p->y);

    bg_undo_restore();
    bg_draw_line(&from, pos_p);
    bg_undo_commit();
    bg_draw_flush();

    state = 0;
    return;
}

void on_drag(struct bg_point * pos_p) {
    printf("on_drag() %d %d\n", pos_p->x, pos_p->y);

    if (state == 0)
    {
        state = 1;
        from = *pos_p;
    }

    bg_undo_restore();

    bg_draw_line(&from, pos_p);
    bg_draw_flush();
    return;
}


int main() {
    bg_init();
    bg_mouse_press_bind(on_press);
    bg_mouse_release_bind(on_release);
    bg_mouse_drag_bind(on_drag);

    bg_mainloop();

    return 0;
}

