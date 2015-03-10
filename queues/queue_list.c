#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests
exit
*/
#include "queue.h"

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

struct queue_ {
	node last; //< Circular list!
};

extern queue queue_create( void ) {
	queue q = (queue)malloc( sizeof(*q) );
	q->last = NULL;
	return q;
}

extern void queue_destroy( queue q ) {
	assert( q );
	assert( !q->last && "must be empty" );

	free( q );
}

extern int queue_empty( queue q ) {
	assert( q );

	return !q->last;
}

extern void * queue_front( queue q ) {
	assert( q );
	assert( q->last && "must not be empty" );

	return q->last->next->object;
}

extern void queue_pop( queue q ) {
	assert( q );
	assert( q->last && "must not be empty" );

	node tmp = q->last->next;
	q->last->next = tmp->next;
	if ( q->last->next == tmp ) {
		q->last = NULL;
	}
	free( tmp );
}

extern void queue_push( queue q, void * object ) {
	assert( q );

	if ( !q->last ) {
		q->last = alloc_node( object, NULL );
		q->last->next = q->last;
	} else {
		q->last->next = alloc_node( object, q->last->next );
		q->last = q->last->next;
	}
}

#ifdef WITH_TESTS
#include "queue_tests.h"
#endif
