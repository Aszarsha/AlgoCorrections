#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests
exit
*/
#include "stack.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#if !defined(UNROLLING_FACTOR) || UNROLLING_FACTOR < 2
#define UNROLLING_FACTOR 10
#endif

typedef struct node_ {
	struct node_ * next;
	void * objects[UNROLLING_FACTOR];
} * node;

static node alloc_node( node next ) {
	node n = (node)malloc( sizeof(*n) );
	n->next = next;
	return n;
}

struct stack_ {
	node head;
	ptrdiff_t index;
};

extern stack stack_create( void ) {
	stack s = (stack)malloc( sizeof(*s) );
	s->head = NULL;
	s->index = 0;
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

	return s->head->objects[s->index-1];
}

extern void stack_pop( stack s ) {
	assert( s );
	assert( s->head && "must not be empty" );

	--s->index;
	if ( s->index == 0 ) {
		node tmp = s->head;
		s->head = s->head->next;
		free( tmp );
		s->index = UNROLLING_FACTOR;
	}
}

extern void stack_push( stack s, void * object ) {
	assert( s );

	if ( !s->head || s->index == UNROLLING_FACTOR ) {
		s->head = alloc_node( s->head );
		s->index = 0;
	}
	s->head->objects[s->index] = object;
	++s->index;
}

#ifdef WITH_TESTS
#include "stack_tests.h"
#endif
