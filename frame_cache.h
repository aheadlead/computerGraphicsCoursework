/* Base graphics library for coursework 
 *
 * Frame Cache
 *
 * Header file only.
 */

#ifndef BG_FRAMECACHE
#define BG_FRAMECACHE

/* This is a temporary frame buffer implementation for canvas editing in this
 * project. I had tried to learn related knowledge for canvas editing in OpenGL,
 * but OpenGL is hard to get started in a short time for me. 
 *
 * There is no doubt that my own coined frame buffer causes performance problem
 * more or less.
 *
 * The two-dimensional array presents the canvas. For each pixel, zero present
 * white pixel and non-zero value is black.
 *
 * TODO Fix the potential performance problem here, using the substitute from
 * OpenGL.
 */
typedef unsigned int bg_frame[400][400];
bg_frame current;

#endif  /* BG_FRAMECACHE */
