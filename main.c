#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>  /* exit */
#include <time.h>  /* time difftime */
#ifndef NDEBUG
#include <signal.h>  /* signal */
#include <execinfo.h>  /* backtrace */
#include <unistd.h>  /* STDERR_FILENO */
void debug_handler(int sig) {
    void *array[10];
    size_t size=backtrace(array, 10);
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}
#endif  /* NDEBUG */


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
#define STATE_FILL_WAITING_CLICK 4
#define STATE_RECTANGLE_WAITING_CLICK_FIRST_POINT 5
#define STATE_RECTANGLE_WAITING_RELEASE_FIRST_POINT 6
#define STATE_RECTANGLE_WAITING_CLICK_SECOND_POINT 7
#define STATE_RECTANGLE_WAITING_RELEASE_SECOND_POINT 8
#define STATE_POLYLINE_WAITING_CLICK_FIRST_POINT 9
#define STATE_POLYLINE_WAITING_RELEASE_FIRST_POINT 10
#define STATE_POLYLINE_WAITING_CLICK_SUBSEQUENT_POINT 11
#define STATE_POLYLINE_WAITING_RELEASE_SUBSEQUENT_POINT 12

static int state=STATE_LINE_WAITING_CLICK_FIRST_POINT;

static struct bg_point from;
static struct bg_point_list plist;
static time_t last_press=0;
static struct bg_point last_press_point;
static const double double_click_threshold=0.1;

void on_press(struct bg_point * pos_p) {
#ifndef NDEBUG
    printf("on_press() %d %d %d\n", pos_p->x, pos_p->y, state);
#endif

    /* time difference for double-click support */
    time_t now;
    time(&now);
    double timediff_since_last_press=now-last_press;
    last_press = now;

    switch (state) {
        case STATE_LINE_WAITING_CLICK_FIRST_POINT:
            from = *pos_p;
            state = STATE_LINE_WAITING_RELEASE_FIRST_POINT;
            break;
        case STATE_LINE_WAITING_CLICK_SECOND_POINT:
            state = STATE_LINE_WAITING_RELEASE_SECOND_POINT;
            break;

        case STATE_FILL_WAITING_CLICK:
            bg_draw_fill(pos_p);
            bg_undo_commit();
            bg_draw_flush();
            state = STATE_FILL_WAITING_CLICK;
            break;

        case STATE_RECTANGLE_WAITING_CLICK_FIRST_POINT:
            from = *pos_p;
            state = STATE_RECTANGLE_WAITING_RELEASE_FIRST_POINT;
            /* no break */
        case STATE_RECTANGLE_WAITING_CLICK_SECOND_POINT:
            state = STATE_RECTANGLE_WAITING_RELEASE_SECOND_POINT;
            break;

        case STATE_POLYLINE_WAITING_CLICK_FIRST_POINT:
            bg_point_list_clear(&plist);
            bg_point_list_append(&plist, pos_p);
            state = STATE_POLYLINE_WAITING_RELEASE_FIRST_POINT;
            break;
        case STATE_POLYLINE_WAITING_CLICK_SUBSEQUENT_POINT:
            if (timediff_since_last_press < double_click_threshold &&
                    (pos_p->x == last_press_point.x && 
                     pos_p->y == last_press_point.y)) {
                bg_point_list_removeback(&plist);
                bg_undo_restore();
                bg_draw_polyline(&plist, NULL);
                bg_undo_commit();
                bg_draw_flush();
                state = STATE_POLYLINE_WAITING_CLICK_FIRST_POINT;
            } else {
                bg_point_list_append(&plist, pos_p);
                state = STATE_POLYLINE_WAITING_RELEASE_SUBSEQUENT_POINT;
            }
    }

    last_press_point = *pos_p;
    return;
}

void on_release(struct bg_point * pos_p) {
#ifndef NDEBUG
    printf("on_release() %d %d %d\n", pos_p->x, pos_p->y, state);
#endif  /* NDEBUG */

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

        case STATE_RECTANGLE_WAITING_RELEASE_FIRST_POINT:
            state = STATE_RECTANGLE_WAITING_CLICK_SECOND_POINT;
            break;
        case STATE_RECTANGLE_WAITING_RELEASE_SECOND_POINT:
            bg_undo_restore();
            bg_draw_rectangle(&from, pos_p);
            bg_undo_commit();
            bg_draw_flush();
            state = STATE_RECTANGLE_WAITING_CLICK_FIRST_POINT;
            break;

        case STATE_POLYLINE_WAITING_RELEASE_FIRST_POINT:
            state = STATE_POLYLINE_WAITING_CLICK_SUBSEQUENT_POINT;
            break;
        case STATE_POLYLINE_WAITING_RELEASE_SUBSEQUENT_POINT:
            bg_undo_restore();
            bg_draw_polyline(&plist, pos_p);
            bg_draw_flush();
            state = STATE_POLYLINE_WAITING_CLICK_SUBSEQUENT_POINT;
            break;
    }
    return;
}

void on_move(struct bg_point * pos_p) {
#ifndef NDEBUG
    printf("on_move() %d %d %d\n", pos_p->x, pos_p->y, state);
#endif 
    switch (state) {
        case STATE_LINE_WAITING_CLICK_SECOND_POINT:
            bg_undo_restore();
            bg_draw_line(&from, pos_p);
            bg_draw_flush();
            state = STATE_LINE_WAITING_CLICK_SECOND_POINT;
            break;

        case STATE_RECTANGLE_WAITING_CLICK_SECOND_POINT:
            bg_undo_restore();
            bg_draw_rectangle(&from, pos_p);
            bg_draw_flush();
            state = STATE_RECTANGLE_WAITING_CLICK_SECOND_POINT;

        case STATE_POLYLINE_WAITING_CLICK_SUBSEQUENT_POINT:
            bg_undo_restore();
            bg_draw_polyline(&plist, pos_p);
            bg_draw_flush();
            state = STATE_POLYLINE_WAITING_CLICK_SUBSEQUENT_POINT;
            break;
    }
    return;
}

void on_drag(struct bg_point * pos_p) {
#ifndef NDEBUG
    printf("on_drag() %d %d %d\n", pos_p->x, pos_p->y, state);
#endif 
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

        case STATE_RECTANGLE_WAITING_RELEASE_FIRST_POINT:
        case STATE_RECTANGLE_WAITING_RELEASE_SECOND_POINT:
            bg_undo_restore();
            bg_draw_rectangle(&from, pos_p);
            bg_draw_flush();
            state = STATE_RECTANGLE_WAITING_RELEASE_SECOND_POINT;
            break;
        case STATE_RECTANGLE_WAITING_CLICK_SECOND_POINT:
            bg_undo_restore();
            bg_draw_rectangle(&from, pos_p);
            bg_draw_flush();
            state = STATE_RECTANGLE_WAITING_RELEASE_SECOND_POINT;
            break;
    }
    return;
}

void menu_line() {
    state = STATE_LINE_WAITING_CLICK_FIRST_POINT;
    return;
}

void menu_rectangle() {
    state = STATE_RECTANGLE_WAITING_CLICK_FIRST_POINT;
    return;
}

void menu_fill() {
    state = STATE_FILL_WAITING_CLICK;
    return;
}

void menu_polyline() {
    state = STATE_POLYLINE_WAITING_CLICK_FIRST_POINT;
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
    return;
}

int main(int argc, char ** argv) {
#ifndef NDEBUG
    signal(SIGSEGV, debug_handler);
    signal(SIGABRT, debug_handler);
#endif  /* NDEBUG */

    bg_init();
    bg_mouse_press_bind(on_press);
    bg_mouse_release_bind(on_release);
    bg_mouse_move_bind(on_move);
    bg_mouse_drag_bind(on_drag);

    bg_menu_bind("直线", menu_line);
    bg_menu_bind("折线", menu_polyline);
    bg_menu_bind("矩形", menu_rectangle);
    bg_menu_bind("填充", menu_fill);
    bg_menu_bind("清空画布", menu_clean);
    bg_menu_bind("撤销", menu_undo);
    bg_menu_bind("清空历史记录", menu_undo_clear);
    bg_menu_bind("退出", exit);

    bg_mainloop();

    return 0;
}

