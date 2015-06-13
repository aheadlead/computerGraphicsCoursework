/* Base graphics library for coursework
 *
 * Input
 */

#include <stdio.h>  /* fprintf stderr */
#include <stdlib.h>  /* malloc */
#include <stddef.h>  /* NULL */

#include <GLUT/glut.h>

/* Though the OpenGL APIs used in this program is abandoned and outdated, I have 
 * to use it still because I don't have plenty of time to improve my coursework and 
 * use new version APIs.
 */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "point.h"

static int
last_menu_value=0;

struct menu_item {
    int value;
    void (*callback)();
    struct menu_item * next;
};
static struct menu_item * menu_item_list_head=NULL;

/* add menu item to menu item list for searching when click menu */
static void 
menu_item_list_push(
        int value, 
        void (*callback)()) {
    struct menu_item * ret=(struct menu_item *)malloc(sizeof(struct menu_item));
    ret->value = value;
    ret->callback = callback;
    ret->next = menu_item_list_head;
    menu_item_list_head = ret;
    return;
}

/* Call the callback by value */
static void
menu_item_list_callback_by_value(int value) {
    struct menu_item * p=menu_item_list_head;
    while (NULL != p) {
        fprintf(stderr, "p.callback=%p next=%p\n", p->callback, p->next);
        if (p->value != value) {
            p = p->next;
        }
        else {
            (p->callback)();
            break;
        }
    }
    return;
}

/* mouse events bindings */
void (*press_callback)(struct bg_point *),
     (*release_callback)(struct bg_point *),
     (*move_callback)(struct bg_point *),
     (*drag_callback)(struct bg_point *);

/* keyboard events bindings */
void (*kb_callback[256])()={NULL};

/* public functions */

void bg_menu_callback(int value) {
    menu_item_list_callback_by_value(value);
}

void bg_menu_bind(
        char * title,
        void (*callback)()) {
    glutAddMenuEntry(title, last_menu_value);
    menu_item_list_push(last_menu_value, callback);
    last_menu_value += 1;
    return;
}

void bg_mouse_plot_callback(
        GLint button,
        GLint action,
        GLint xMouse,
        GLint yMouse) {
    if (button == GLUT_LEFT_BUTTON) {
        if (action == GLUT_DOWN) {
            if (NULL != press_callback) {
                struct bg_point * tmp_point=
                    bg_point_make((unsigned int)xMouse, 400-(unsigned int)yMouse);
                press_callback(tmp_point);
                free(tmp_point);
            }
        }
        else if (action == GLUT_UP) {
            if (NULL != release_callback) {
                struct bg_point * tmp_point=
                    bg_point_make((unsigned int)xMouse, 400-(unsigned int)yMouse);
                release_callback(tmp_point);
                free(tmp_point);
            }
        }
    }
    return;
}

void bg_mouse_move_callback(
        GLint xMouse,
        GLint yMouse) {
    if (NULL != move_callback) {
        struct bg_point * tmp_point=
            bg_point_make((unsigned int)xMouse, 400-(unsigned int)yMouse);
        move_callback(tmp_point);
        free(tmp_point);
    }
}

void bg_mouse_drag_callback(
        GLint xMouse,
        GLint yMouse) {
    if (NULL != drag_callback) {
        struct bg_point * tmp_point=
            bg_point_make((unsigned int)xMouse, 400-(unsigned int)yMouse);
        drag_callback(tmp_point);
        free(tmp_point);
    }
}

void bg_mouse_press_bind(void (*callback)(struct bg_point *)) {
    press_callback = callback;
    return;
}

void bg_mouse_release_bind(void (*callback)(struct bg_point *)) {
    release_callback = callback;
    return;
}

void bg_mouse_move_bind(void (*callback)(struct bg_point *)) {
    move_callback = callback;
    return;
}

void bg_mouse_drag_bind(void (*callback)(struct bg_point *)) {
    drag_callback = callback;
    return;
}

void bg_keyboard_callback(unsigned char key, int x, int y) {
    if (NULL != kb_callback[key]) {
        kb_callback[key]();
    }
    return;
}

void bg_keyboard_bind(
        unsigned char key, 
        void (*callback)()) {
    kb_callback[key] = callback;
    return;
}

