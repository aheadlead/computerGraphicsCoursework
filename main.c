#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>  /* exit */

#include "base_graphics.h"
#include "input.h"
#include "point.h"
#include "draw.h"
#include "undo.h"

#include "frame_cache.h"
extern bg_frame current;

#define STATE_LINE_WAITING_CLICK_FIRST_POINT 0
#define STATE_LINE_WAITING_RELEASE_FIRST_POINT 1
#define STATE_LINE_WAITING_CLICK_SECOND_POINT 2
#define STATE_LINE_WAITING_RELEASE_SECOND_POINT 3

static int state=STATE_LINE_WAITING_CLICK_FIRST_POINT;

struct bg_point from;

void on_press(struct bg_point * pos_p) {
    printf("on_press() %d %d %d\n", pos_p->x, pos_p->y, state);
    switch (state) {
        case STATE_LINE_WAITING_CLICK_FIRST_POINT:
            from = *pos_p;
            state = STATE_LINE_WAITING_RELEASE_FIRST_POINT;
            break;
        case STATE_LINE_WAITING_CLICK_SECOND_POINT:
            state = STATE_LINE_WAITING_RELEASE_SECOND_POINT;
            break;
    }
    return;
}

void on_release(struct bg_point * pos_p) {
    printf("on_release() %d %d %d\n", pos_p->x, pos_p->y, state);
    switch (state) {
        case STATE_LINE_WAITING_RELEASE_FIRST_POINT:
            state = STATE_LINE_WAITING_CLICK_SECOND_POINT;
            break;
        case STATE_LINE_WAITING_RELEASE_SECOND_POINT:
            bg_undo_restore();
            bg_draw_line(&from, pos_p);
            bg_undo_commit();
            bg_draw_flush();
            state = STATE_LINE_WAITING_CLICK_FIRST_POINT;
            break;
    }
    return;
}

void on_move(struct bg_point * pos_p) {
    printf("on_move() %d %d %d\n", pos_p->x, pos_p->y, state);
    switch (state) {
        case STATE_LINE_WAITING_CLICK_SECOND_POINT:
            bg_undo_restore();
            bg_draw_line(&from, pos_p);
            bg_draw_flush();
            state = STATE_LINE_WAITING_CLICK_SECOND_POINT;
            break;
    }
    return;
}

void on_drag(struct bg_point * pos_p) {
    printf("on_drag() %d %d %d\n", pos_p->x, pos_p->y, state);
    switch (state) {
        case STATE_LINE_WAITING_RELEASE_FIRST_POINT:
        case STATE_LINE_WAITING_RELEASE_SECOND_POINT:
            bg_undo_restore();
            bg_draw_line(&from, pos_p);
            bg_draw_flush();
            state = STATE_LINE_WAITING_RELEASE_SECOND_POINT;
            break;
        case STATE_LINE_WAITING_CLICK_SECOND_POINT:
            bg_undo_restore();
            bg_draw_line(&from, pos_p);
            bg_draw_flush();
            state = STATE_LINE_WAITING_RELEASE_SECOND_POINT;
            break;
    }
    return;
}

void menu_line() {
    state = STATE_LINE_WAITING_CLICK_FIRST_POINT;
    return;
}

void menu_rectangle() {
    return;
}

void menu_fill() {
    return;
}

void menu_clean() {
    bg_clean();
    bg_draw_flush();
    return;
}

void menu_undo() {
    bg_undo_pop();
    bg_undo_restore();
    bg_draw_flush();
    fprintf(stderr, "caonima\n");
    return;
}

void menu_undo_clear() {
    bg_undo_clear();
    bg_undo_commit();
    return;
}

int main() {
    bg_init();
    bg_mouse_press_bind(on_press);
    bg_mouse_release_bind(on_release);
    bg_mouse_move_bind(on_move);
    bg_mouse_drag_bind(on_drag);

    bg_menu_bind("直线", menu_line);
    /*
     *bg_menu_bind("矩形", menu_rectangle);
     *bg_menu_bind("填充", menu_fill);
     */
    bg_menu_bind("清空画布", menu_clean);
    bg_menu_bind("撤销", menu_undo);
    bg_menu_bind("清空历史记录", menu_undo_clear);
    bg_menu_bind("退出", exit);

    bg_mainloop();

    return 0;
}

