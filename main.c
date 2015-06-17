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

    /* additional instructions */
    if (sig == SIGILL){
        fprintf(stderr, "This error may raised when integer overflowing.\n");
    }

    exit(1);
}
#endif  /* NDEBUG */


#include "base_graphics.h"
#include "input.h"
#include "point.h"
#include "point_list.h"
#include "draw.h"
#include "undo.h"
#include "math_extension.h"

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
#define STATE_BEZIERCURVE_WAITING_CLICK_FIRST_POINT 13
#define STATE_BEZIERCURVE_WAITING_RELEASE_FIRST_POINT 14
#define STATE_BEZIERCURVE_WAITING_CLICK_SUBSEQUENT_POINT 15
#define STATE_BEZIERCURVE_WAITING_RELEASE_SUBSEQUENT_POINT 16
#define STATE_CIRCLE_WAITING_CLICK_CENTER_POINT 17
#define STATE_CIRCLE_WAITING_RELEASE_CENTER_POINT 18
#define STATE_CIRCLE_WAITING_CLICK_POINT_ON_CIRCLE 19
#define STATE_CIRCLE_WAITING_RELEASE_POINT_ON_CIRCLE 20
#define STATE_SELECTION_WAITING_CLICK_POINT 21
#define STATE_SELECTION_WAITING_RELEASE_POINT 22
#define STATE_ROTATE_WAITING_PRESS 23
#define STATE_ROTATE_WAITING_RELEASE 24

static int state=STATE_LINE_WAITING_CLICK_FIRST_POINT;

static struct bg_point from;
static struct bg_point_list plist;
static time_t last_press=0;
static struct bg_point last_press_point;
static struct bg_point center_point;
static const double double_click_threshold=0.1;  /* 双击判定阈值 0.1秒 */

static struct bg_point selection_from, selection_to;

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
            break;

        case STATE_BEZIERCURVE_WAITING_CLICK_FIRST_POINT:
            bg_point_list_clear(&plist);  
            bg_point_list_append(&plist, pos_p);  /* 记录第一个点的位置，加入点列表 */
            state = STATE_BEZIERCURVE_WAITING_RELEASE_FIRST_POINT;
            break;
        case STATE_BEZIERCURVE_WAITING_CLICK_SUBSEQUENT_POINT:
            if (timediff_since_last_press < double_click_threshold &&
                    (pos_p->x == last_press_point.x && 
                     pos_p->y == last_press_point.y)) {  /* 双击 */
                bg_point_list_removeback(&plist);
                bg_undo_restore();
                bg_draw_beziercurve(&plist, NULL);
                bg_undo_commit();
                bg_draw_flush();
                state = STATE_BEZIERCURVE_WAITING_CLICK_FIRST_POINT;
            } else {  /* 单击 */
                bg_point_list_append(&plist, pos_p);  /* 当前点加入点列表 */
                state = STATE_BEZIERCURVE_WAITING_RELEASE_SUBSEQUENT_POINT;
            }
            break;

        case STATE_CIRCLE_WAITING_CLICK_CENTER_POINT:
            state = STATE_CIRCLE_WAITING_RELEASE_CENTER_POINT;
            break;
        case STATE_CIRCLE_WAITING_CLICK_POINT_ON_CIRCLE:
            state = STATE_CIRCLE_WAITING_RELEASE_POINT_ON_CIRCLE;
            break;

        case STATE_SELECTION_WAITING_CLICK_POINT:
            from = *pos_p;  /* 记录选区的第一个点的位置 */
            state = STATE_SELECTION_WAITING_RELEASE_POINT;
            break;

        case STATE_ROTATE_WAITING_PRESS:
            from = *pos_p; 
            state = STATE_ROTATE_WAITING_RELEASE;
            break;
    }

    last_press_point = *pos_p;
    return;
}

void on_release(struct bg_point * pos_p) {
#ifndef NDEBUG
    printf("on_release() %d %d %d\n", pos_p->x, pos_p->y, state);
#endif  /* NDEBUG */

    /* for SELECTION in switch */
    int minX, maxX;
    int minY, maxY;

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

        case STATE_BEZIERCURVE_WAITING_RELEASE_FIRST_POINT:
            state = STATE_BEZIERCURVE_WAITING_CLICK_SUBSEQUENT_POINT;
            break;
        case STATE_BEZIERCURVE_WAITING_RELEASE_SUBSEQUENT_POINT:
            bg_undo_restore();
            bg_draw_line_set_pattern(BG_LINE_PATTERN_3);  /* 切换到虚线 1/8 */
            bg_draw_polyline(&plist, pos_p);  /* 画控制线 */
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_beziercurve(&plist, pos_p);
            bg_draw_flush();
            state = STATE_BEZIERCURVE_WAITING_CLICK_SUBSEQUENT_POINT;
            break;

        case STATE_CIRCLE_WAITING_RELEASE_POINT_ON_CIRCLE:
            bg_undo_restore();
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_circle(  /* 画圆 */
                    &center_point, 
                    euclidian_distance(center_point, *pos_p));
            bg_undo_commit();  /* 提交更改 */
            bg_draw_flush();
            state = STATE_CIRCLE_WAITING_CLICK_CENTER_POINT;
            break;
        case STATE_CIRCLE_WAITING_RELEASE_CENTER_POINT:
            center_point = *pos_p;
            state = STATE_CIRCLE_WAITING_CLICK_POINT_ON_CIRCLE;
            break;

        case STATE_SELECTION_WAITING_RELEASE_POINT:
#define MIN(a, b) ((a)>(b)?(b):(a))
#define MAX(a, b) ((a)>(b)?(a):(b))
            selection_from = from;
            selection_to = *pos_p;
            int minX = MIN(selection_from.x, selection_to.x);
            int maxX = MAX(selection_from.x, selection_to.x);
            int minY = MIN(selection_from.y, selection_to.y);
            int maxY = MAX(selection_from.y, selection_to.y);
            selection_from.x = minX;
            selection_from.y = minY;
            selection_to.x = maxX;
            selection_to.y = maxY;
#undef MIN
#undef MAX
            state = STATE_SELECTION_WAITING_CLICK_POINT;
            break;

        case STATE_ROTATE_WAITING_RELEASE:
            bg_undo_restore();
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_rotate(
                    &selection_from, &selection_to,
                    &from, pos_p);
            bg_undo_commit();
            bg_draw_flush();
            selection_from.x = selection_from.y = 400;
            selection_to.x = selection_to.y = 400;
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

        case STATE_BEZIERCURVE_WAITING_CLICK_SUBSEQUENT_POINT:
            bg_undo_restore();
            bg_draw_line_set_pattern(BG_LINE_PATTERN_3);  /* 切换到虚线 1/8 */
            bg_draw_polyline(&plist, pos_p);  /* 画控制线 */
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_beziercurve(&plist, pos_p);
            bg_draw_flush();
            state = STATE_BEZIERCURVE_WAITING_CLICK_SUBSEQUENT_POINT; /* 指向自己的状态转移 */
            break;

        case STATE_CIRCLE_WAITING_CLICK_POINT_ON_CIRCLE:
            bg_undo_restore();
            bg_draw_circle(  /* 画圆 */
                    &center_point, 
                    euclidian_distance(center_point, *pos_p));
            bg_draw_line_set_pattern(BG_LINE_PATTERN_3);  /* 切换到虚线 1/8 */
            bg_draw_line(&center_point, pos_p);
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_flush();
            /* state 不变 */
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

        case STATE_CIRCLE_WAITING_RELEASE_CENTER_POINT:
            center_point = *pos_p;
            state = STATE_CIRCLE_WAITING_RELEASE_POINT_ON_CIRCLE;
            break;
        case STATE_CIRCLE_WAITING_RELEASE_POINT_ON_CIRCLE:
            bg_undo_restore();
            bg_draw_circle(  /* 画圆 */
                    &center_point, 
                    euclidian_distance(center_point, *pos_p));
            bg_draw_line_set_pattern(BG_LINE_PATTERN_3);  /* 切换到虚线 1/8 */
            bg_draw_line(&center_point, pos_p);  /* 辅助线 */
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_flush();
            /* state 不变 */
            break;

        case STATE_SELECTION_WAITING_RELEASE_POINT:
            bg_undo_restore();
            bg_draw_line_set_pattern(BG_LINE_PATTERN_2);  /* 切换到虚线 1/4 */
            bg_draw_rectangle(&from, pos_p);
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_flush();
            /* state 不变 */
            break;

        case STATE_ROTATE_WAITING_RELEASE:
            bg_undo_restore();
            /* 为了避免辅助线被“刷白”的选区遮挡，要后画辅助线 */
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_rotate(
                    &selection_from, &selection_to,
                    &from, pos_p);
            bg_draw_line_set_pattern(BG_LINE_PATTERN_3);  /* 切换到虚线 1/4 */
            bg_draw_line(&from, pos_p);  /* 辅助线 */
            bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
            bg_draw_flush();
            /* state 不变 */
            return;
    }
    return;
}

void menu_line() {
    bg_undo_restore();
    bg_draw_flush();
    state = STATE_LINE_WAITING_CLICK_FIRST_POINT;
    return;
}

void menu_rectangle() {
    bg_undo_restore();
    bg_draw_flush();
    state = STATE_RECTANGLE_WAITING_CLICK_FIRST_POINT;
    return;
}

void menu_fill() {
    bg_undo_restore();
    bg_draw_flush();
    state = STATE_FILL_WAITING_CLICK;
    return;
}

void menu_polyline() {
    bg_undo_restore();
    bg_draw_flush();
    state = STATE_POLYLINE_WAITING_CLICK_FIRST_POINT;
    return;
}

void menu_beziercurve() {
    bg_undo_restore();
    bg_draw_flush();
    state = STATE_BEZIERCURVE_WAITING_CLICK_FIRST_POINT;
    return;
}

void menu_circle() {
    bg_undo_restore();
    bg_draw_flush();
    state = STATE_CIRCLE_WAITING_CLICK_CENTER_POINT;
    return;
}

void menu_clean() {
    bg_clean();
    bg_draw_flush();
    return;
}

void menu_undo() {
    bg_undo_pop();  /* 删除在回退堆栈的当前帧 */
    bg_undo_restore();  /* 回退上一帧的画面 */
    bg_draw_flush();
    return;
}

void menu_undo_clear() {
    bg_undo_clear();
    return;
}

void menu_dummy() {
    /* for separator */
    fprintf(stderr, "我tm一个分隔符点你妹啊\n");
    return;
}

void menu_select_selection() {
    bg_undo_restore();
    /* 之前可能有过选区，把选区重新画上去 */
    bg_draw_line_set_pattern(BG_LINE_PATTERN_2);  /* 切换到虚线 1/4 */
    bg_draw_rectangle(&selection_from, &selection_to);
    bg_draw_line_set_pattern(BG_LINE_PATTERN_0);  /* 切换到实线 */
    bg_draw_flush();
    state = STATE_SELECTION_WAITING_CLICK_POINT;
    return;
}

void menu_clean_selection() {
    selection_from.x = 400;  /* out of the screen */
    selection_from.y = 400;
    selection_to.x = 400;
    selection_to.y = 400;
    bg_undo_restore();
    bg_draw_flush();
    return;
}

void menu_rotate() {
    bg_undo_restore();
    bg_draw_flush();
    state = STATE_ROTATE_WAITING_PRESS;
    return;
}   

int main(int argc, char ** argv) {
#ifndef NDEBUG
    signal(SIGSEGV, debug_handler);
    signal(SIGABRT, debug_handler);
    signal(SIGILL, debug_handler);
#endif  /* NDEBUG */

    bg_init();
    bg_mouse_press_bind(on_press);
    bg_mouse_release_bind(on_release);
    bg_mouse_move_bind(on_move);
    bg_mouse_drag_bind(on_drag);

    bg_menu_bind("直线", menu_line);
    bg_menu_bind("折线", menu_polyline);
    bg_menu_bind("矩形", menu_rectangle);
    bg_menu_bind("--------", menu_dummy);

    bg_menu_bind("圆", menu_circle);
    bg_menu_bind("贝塞尔曲线", menu_beziercurve);
    bg_menu_bind("--------", menu_dummy);

    bg_menu_bind("填充", menu_fill);
    bg_menu_bind("--------", menu_dummy);
    
    bg_menu_bind("选择选区", menu_select_selection);
    bg_menu_bind("清除选区", menu_clean_selection);
    bg_menu_bind("旋转", menu_rotate);
    /*bg_menu_bind("平移", menu_transform);*/
    bg_menu_bind("--------", menu_dummy);

    bg_menu_bind("清空画布", menu_clean);
    bg_menu_bind("撤销", menu_undo);
    bg_menu_bind("清空历史记录", menu_undo_clear);
    bg_menu_bind("退出", exit);

    bg_mainloop();

    return 0;
}

