/* Base graphics library for coursework
 *
 * Undo
 */

#include <stdlib.h>  /* malloc */
#include <string.h>  /* memcpy */

#include "undo.h"
#include "frame_cache.h"

extern bg_frame current;

/* Here is a stack to store each frame of all stages for undo function.
 * The stack is no limited, so it may require many memory. 
 *
 * A frame would be pushed in the stack when it is committed. If the operation by
 * users have not been decided yet (e.g. previewing by rubber band method), the
 * frame would not be push in this stack instantly.
 *
 * The stack is like to a linked list. The end of the stack is null pointer.
 *
 * Typical situation:
 *  undo_list_head--
 *                 |
 *                 V
 *          [undo_list_element(now)]->[undo_list_element(old 1)]->...->NULL
 */
struct undo_list_item {
    bg_frame frame;
    struct undo_list_item * next_p;
};
static struct undo_list_item * undo_list_head_p;

void bg_undo_commit() {
    /* allocate spaces */
    struct undo_list_item * new=(struct undo_list_item *)malloc(sizeof(struct undo_list_item));

    /* copy */
    memcpy(&new->frame, &current, sizeof(current));

    /* pointer change */
    new->next_p = undo_list_head_p;
    undo_list_head_p = new;

    return;
}

void bg_undo_restore() {
    if (NULL != undo_list_head_p) {
        /* restore */
        memcpy(&current, &undo_list_head_p->frame, sizeof(current));
    }
    return;
}

/* pop the topest element without any checks */
void __bg_undo_pop_force() {
    /* pointer change */
    struct undo_list_item * tmp=undo_list_head_p;
    undo_list_head_p = undo_list_head_p->next_p;

    /* free */
    free(tmp);
    tmp = NULL;
    return;
}

/* pop the topest element if not the last element */
void bg_undo_pop() {
    if (NULL != undo_list_head_p->next_p) {
        __bg_undo_pop_force();
    }
    return;
}

/* pop all elements and push the current frame */
void bg_undo_clear() {
    while (NULL != undo_list_head_p)
        __bg_undo_pop_force();
    bg_undo_commit();
    return;
}

