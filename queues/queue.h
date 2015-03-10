#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_ * queue;

extern queue queue_create ( void );
extern void  queue_destroy( queue q ); //< The queue must be empty (pop all elements before if necessary)

extern int    queue_empty( queue q );
extern void * queue_front( queue q );  //< The queue must not be empty
extern void   queue_pop  ( queue q );  //< The queue must not be empty
extern void   queue_push ( queue q, void * object );

#endif
