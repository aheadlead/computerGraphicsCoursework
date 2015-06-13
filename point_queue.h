/* Base graphics library for coursework 
 *
 * Point Queue
 */

#ifndef POINT_QUEUE
#define POINT_QUEUE

#include "point.h"
#include "linux/list.h"

struct bg_point_queue_node {
    struct bg_point * point;
    struct list_head list;
};

typedef struct list_head bg_point_queue;

int 
bg_point_queue_empty(
        const bg_point_queue * head) {
    return list_empty(head);
}

void
bg_point_queue_pushback(
        const bg_point_queue * head,
        const struct bg_point * p) {
    list_add_tail(&(p->list), head);
    return;
}

struct bg_point * 
bg_point_queue_popfront(
        const bg_point_queue * head) {
    struct bg_point_queue_node ret;
    list_first_entry(ret, 
        

#endif  /* POINT_QUEUE */

