#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests
exit
*/
#include "queue.h"

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

struct queue_ {
	node last; //< Circular list!
	ptrdiff_t readIndex, writeIndex;
};

extern queue queue_create( void ) {
	queue q = (queue)malloc( sizeof(*q) );
	q->last = NULL;
	q->readIndex  = 0;
	q->writeIndex = 0;
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

	return q->last->next->objects[q->readIndex];
}

extern void queue_pop( queue q ) {
	assert( q );
	assert( q->last && "must not be empty" );

	++q->readIndex;
	if ( q->readIndex == UNROLLING_FACTOR ) {
		q->readIndex = 0;
		node tmp = q->last->next;
		q->last->next = tmp->next;
		if ( q->last->next == tmp ) {
			q->last = NULL;
		}
		free( tmp );
	}
}

extern void queue_push( queue q, void * object ) {
	assert( q );

	if ( !q->last ) {
		q->last = alloc_node( NULL );
		q->last->next = q->last;
	} else if ( q->writeIndex == UNROLLING_FACTOR ) {
		q->writeIndex = 0;
		q->last->next = alloc_node( q->last->next );
		q->last = q->last->next;
	}
	q->last->objects[q->writeIndex] = object;
	++q->writeIndex;
}

#ifdef WITH_TESTS
#include "queue_tests.h"
#endif
