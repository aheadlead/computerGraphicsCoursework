/* Base graphics library for coursework 
 *
 * Undo
 */

#ifndef UNDO_H
#define UNDO_H

/* Take the snapshot from the frame buffer (global variable "current").
 * 
 * Call this function to make current screen as a rollback point when undo.
 */
void bg_undo_commit();

/* Restore the latest snapshot to the current frame buffer. */
void bg_undo_restore();

/* Delete the latest snapshot. */
void bg_undo_pop();

/* Delete all snapshots. */
void bg_undo_clear();

#endif  /* UNDO_H */
