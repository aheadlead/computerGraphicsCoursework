#include <stdio.h>  /* printf */
#include <stdlib.h>  /* malloc free */
#include <stddef.h>  /* size_t */

#include <GLUT/glut.h>

#include "point.h"
#include "input.h"

/* Though the OpenGL APIs used in this program is abandoned and outdated, I have 
 * to use it still because I don't have plenty of time to improve my coursework and 
 * use new version APIs.
 */
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


/* private functions */
static void 
__flush() {
    glClear(GL_COLOR_BUFFER_BIT);

    struct bg_point p;

    glBegin(GL_POINTS);
    for (p.x=0; p.x<400; ++p.x) {
        for (p.y=0; p.y<400; ++p.y) {
            glVertex2i(p.x, p.y);
        }
    }
    glEnd();

    glutSwapBuffers();

    return;
}


/* public functions */

/* Initializing.
 *
 * It must be called before using the library. */
void 
bg_init() {
    /* simplified initialization */
    int fake_argc=1;
    char * fake_arg="";
    char ** fake_argv=&fake_arg;
    glutInit(&fake_argc, fake_argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutWindowSize(400, 400);
    glutCreateWindow("Computer graphics coursework");

    /* TODO may could be removed */
    glutDisplayFunc(__flush);

    glutIdleFunc(__flush);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0, 400, 0, 400);

    /* initialize menu */
    glutCreateMenu(bg_menu_callback);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    /* initialize mouse */
    glutMouseFunc(bg_mouse_plot_callback);
    glutMotionFunc(bg_mouse_move_callback);

    /* initialize keyboard */
    glutKeyboardFunc(bg_keyboard_callback);
    
    return;
}

void
bg_mainloop()
{
    glutMainLoop();
    return;
}

/*
void bg_selection_move(
        bg_point * LU_p,
        bg_point * RD_p,
        bg_point * delta);

void bg_selection_rotate(
        bg_point * LU_p,
        bg_point * RD_p,
        double radian);
*/
