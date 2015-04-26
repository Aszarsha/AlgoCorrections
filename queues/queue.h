#ifndef QUEUE_H
#define QUEUE_H

typedef struct queue_ * queue;

typedef void * queue_obj;

typedef void (*obj_del_func)( queue_obj );

extern queue queue_new   ( void );
extern void  queue_delete( queue q, obj_del_func delf );

extern int       queue_empty( queue q );
extern queue_obj queue_front( queue q );  //< The queue must not be empty
extern void      queue_pop  ( queue q );  //< The queue must not be empty
extern void      queue_push ( queue q, queue_obj object );

#endif
