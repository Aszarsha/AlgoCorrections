#ifndef STACK_H
#define STACK_H

typedef struct stack_ * stack;

typedef void * stack_obj;

typedef void (*obj_del_func)( stack_obj );

extern stack stack_new   ( void );
extern void  stack_delete( stack s, obj_del_func delf );

extern int       stack_empty( stack s );
extern stack_obj stack_top  ( stack s );  //< The stack must not be empty
extern void      stack_pop  ( stack s );  //< The stack must not be empty
extern void      stack_push ( stack s, stack_obj object );

#endif
