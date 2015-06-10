/* Base graphics library for coursework 
 *
 * Point Queue
 */

#ifndef POINT_QUEUE
#define POINT_QUEUE

#include "point.h"

struct bg_point_queue_element {
    struct bg_point * point;
    struct bg_point_queue_element * next;
};

struct bg_point_queue {
    struct bg_point_element * head;
    struct bg_point_element * tail;
};

int 
bg_point_queue_empty(
        const struct bg_point_queue * q) {
    return q->head == NULL;
}

void
bg_point_queue_push(
        struct bg_point_queue * q,
        const struct bg_point * p) {
    if ( 
}

#endif  /* POINT_QUEUE */

