/* Base graphics library for coursework
 *
 * INPUT
 */

#ifndef INPUT_H
#define INPUT_H

void bg_menu_callback(int value);
void bg_menu_bind(
        char * title,
        void (*callback)());

void bg_mouse_plot_callback(
        GLint button,
        GLint action,
        GLint xMouse,
        GLint yMouse);
void bg_mouse_press_bind(void (*callback)());
void bg_mouse_release_bind(void (*callback)());

void bg_mouse_move_callback(
        GLint xMouse,
        GLint yMouse);
void bg_mouse_drag_bind(void (*callback)());

void bg_keyboard_callback(
        unsigned char key,
        int x,  
        int y);
/* The best is to use characters, digits or space as a key. */
void bg_keyboard_bind(
        unsigned char key, 
        void (*callback)());

#endif  /* INPUT_H */
