/* Base graphics library for coursework
 *
 * Input
 */

#include <stdlib.h>  /* malloc */
#include <stddef.h>  /* NULL */

#include <GLUT/glut.h>

/* Though the OpenGL APIs used in this program is abandoned and outdated, I have 
 * to use it still because I don't have plenty of time to improve my coursework and 
 * use new version APIs.
 */
/* TODO test for without this pragma */ 
/*#pragma GCC diagnostic ignored "-Wdeprecated-declarations"*/

static int
last_menu_value=0;

struct menu_item {
    int value;
    void (*callback)();
    struct menu_item * next;
};
static struct menu_item * menu_item_list_head;

/* add menu item to menu item list for search when click menu */
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
menu_item_list_callback_by_value(
        int value) {
    struct menu_item * p=menu_item_list_head;
    while (NULL != p) {
        if (p->value != value) {
            p += 1;
        }
        else {
            p->callback();
        }
    }
    return;
}

/* mouse events bindings */
void (*press_callback)(),
     (*release_callback)(),
     (*move_callback)(int, int);

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
                press_callback();
            }
        }
        else if (action = GLUT_UP) {
            if (NULL != release_callback) {
                release_callback();
            }
        }
    }
    return;
}

void bg_mouse_move_callback(
        GLint xMouse,
        GLint yMouse) {
    if (NULL != move_callback) {
        move_callback(xMouse, yMouse);
    }
}

void bg_mouse_press_bind(void (*callback)()) {
    press_callback = callback;
    return;
}

void bg_mouse_release_bind(void (*callback)()) {
    release_callback = callback;
    return;
}

void bg_mouse_drag_bind(void (*callback)()) {
    move_callback = callback;
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

