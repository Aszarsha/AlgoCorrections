#ifndef STACK_H
#define STACK_H

typedef struct stack_ * stack;

extern stack stack_create ( void );
extern void  stack_destroy( stack s ); //< The stack must be empty (pop all elements before if necessary)

extern int    stack_empty( stack s );
extern void * stack_top  ( stack s );  //< The stack must not be empty
extern void   stack_pop  ( stack s );  //< The stack must not be empty
extern void   stack_push ( stack s, void * object );

#endif
