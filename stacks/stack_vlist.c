#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests
exit
*/
#include "stack.h"

#include <assert.h>
#include <stdlib.h>

#if !defined(BASE_SIZE) || BASE_SIZE < 1
#define BASE_SIZE 1
#endif

#if !defined(GROWTH_FACTOR) || GROWTH_FACTOR < 1
#define GROWTH_FACTOR 2
#endif

typedef struct node_ {
	struct node_ * next;
	void * objects[];
} * node;

static node alloc_node( size_t length, node next ) {
	node n = (node)malloc( sizeof(*n) + length*sizeof(*n->objects) );
	n->next = next;
	return n;
}

struct stack_ {
	node head;
	size_t index, capacity;
};

extern stack stack_create( void ) {
	stack s = (stack)malloc( sizeof(*s) );
	s->head = NULL;
	s->index = 0;
	s->capacity = 1;
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
		s->capacity /= GROWTH_FACTOR;
		s->index = s->capacity;
		node tmp = s->head;
		s->head = s->head->next;
		free( tmp );
	}
}

extern void stack_push( stack s, void * object ) {
	assert( s );

	if ( !s->head || s->index == s->capacity ) {
		s->capacity *= GROWTH_FACTOR;
		s->head = alloc_node( s->capacity, s->head );
		s->index = 0;
	}
	s->head->objects[s->index] = object;
	++s->index;
}

#ifdef WITH_TESTS
#include "stack_tests.h"
#endif
