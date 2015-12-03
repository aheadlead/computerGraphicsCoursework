/* Base graphics library for coursework *
 * INPUT
 */

#ifndef INPUT_H
#define INPUT_H

#include <GLUT/glut.h>

#include "point.h"

/* User-defined callback functions should NOT free the pointer pos_p. 
 * The pointer variable pos_p is the comunicator between the two layer of
 * the software, so I decided to free the pointer at the lower-level caller 
 * function to keep the simplicity in higher-level code.
*/
void bg_menu_callback(int value);
void bg_menu_bind(
        char * title,
        void (*callback)());

void bg_mouse_plot_callback(
        GLint button,
        GLint action,
        GLint xMouse,
        GLint yMouse);
void bg_mouse_press_bind(void (*callback)(struct bg_point *));
void bg_mouse_release_bind(void (*callback)(struct bg_point *));


/* "Move" means moving cursor without left-click. 
 * "Drag" means moving cursor with left-click.
 */
void bg_mouse_move_callback(
        GLint xMouse,
        GLint yMouse);
void bg_mouse_drag_callback(
        GLint xMouse,
        GLint yMouse);
void bg_mouse_move_bind(void (*callback)(struct bg_point *));
void bg_mouse_drag_bind(void (*callback)(struct bg_point *));

void bg_keyboard_callback(
        unsigned char key,
        int x,  
        int y);
/* The best is to use characters, digits or space as a key. */
void bg_keyboard_bind(
        unsigned char key, 
        void (*callback)());

#endif  /* INPUT_H */
