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
	stack_obj object;
} * node;

static node node_new( stack_obj object, node next ) {
	node n = (node)malloc( sizeof(*n) );
	n->object = object;
	n->next = next;
	return n;
}

static void node_delete( node n, obj_del_func delf ) {
	delf( n->object );
	free( n );
}

struct stack_ {
	node head;
};

extern stack stack_new( void ) {
	stack s = (stack)malloc( sizeof(*s) );
	s->head = NULL;
	return s;
}

extern void stack_delete( stack s, obj_del_func delf ) {
	assert( s );

	node it = s->head;
	while ( it ) {
		node tmp = it;
		it = it->next;
		node_delete( tmp, delf );
	}
	free( s );
}

extern int stack_empty( stack s ) {
	assert( s );

	return !s->head;
}

extern stack_obj stack_top( stack s ) {
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

extern void stack_push( stack s, stack_obj object ) {
	assert( s );

	s->head = node_new( object, s->head );
}

#ifdef WITH_TESTS
#include "stack_tests.h"
#endif
