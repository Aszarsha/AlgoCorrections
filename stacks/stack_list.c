#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests
exit
*/
#include "stack.h"

#include <assert.h>
#include <stdlib.h>

typedef struct node_ {
	struct node_ * next;
	void * object;
} * node;

static node alloc_node( void * object, node next ) {
	node n = (node)malloc( sizeof(*n) );
	n->object = object;
	n->next = next;
	return n;
}

struct stack_ {
	node head;
};

extern stack stack_create( void ) {
	stack s = (stack)malloc( sizeof(*s) );
	s->head = NULL;
	return s;
}

extern void stack_destroy( stack s ) {
	assert( s );
	assert( !s->head && "must be empty" );

	free( s );
}

extern int stack_empty( stack s ) {
	assert( s );

	return !s->head;
}

extern void * stack_top( stack s ) {
	assert( s );
	assert( s->head && "must not be empty" );

	return s->head->object;
}

extern void stack_pop( stack s ) {
	assert( s );
	assert( s->head && "must not be empty" );

	node tmp = s->head;
	s->head = s->head->next;
	free( tmp );
}

extern void stack_push( stack s, void * object ) {
	assert( s );

	s->head = alloc_node( object, s->head );
}

#ifdef WITH_TESTS
#include "stack_tests.h"
#endif
