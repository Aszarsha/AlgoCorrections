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
	queue_obj objects[UNROLLING_FACTOR];
} * node;

static node node_new( node next ) {
	node n = (node)malloc( sizeof(*n) );
	n->next = next;
	return n;
}

static void node_delete( node n, size_t start, size_t stop, obj_del_func delf ) {
	for ( size_t i = start; i < stop; ++i ) {
		delf( n->objects[i] );
	}
	free( n );
}

struct queue_ {
	node last; //< Circular list!
	ptrdiff_t readIndex, writeIndex;
};

extern queue queue_new( void ) {
	queue q = (queue)malloc( sizeof(*q) );
	q->last = NULL;
	q->readIndex  = 0;
	q->writeIndex = 0;
	return q;
}

extern void queue_delete( queue q, obj_del_func delf ) {
	assert( q );

	if ( q->last ) {
		node tmp = q->last->next;
		q->last->next = NULL;   // break cycle
		node it = tmp->next;
		node_delete( tmp, q->readIndex, UNROLLING_FACTOR, delf );   // treat first node differently
		while ( it && it->next ) {   // check of it if only one node
			node tmp = it;
			it = it->next;
			node_delete( tmp, 0, UNROLLING_FACTOR, delf );
		}
		if ( it ) {
			node_delete( it, 0, q->writeIndex, delf );               // treat last node differently
		}
	}
	free( q );
}

extern int queue_empty( queue q ) {
	assert( q );

	return !q->last;
}

extern queue_obj queue_front( queue q ) {
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

extern void queue_push( queue q, queue_obj object ) {
	assert( q );

	if ( !q->last ) {
		q->last = node_new( NULL );
		q->last->next = q->last;
	} else if ( q->writeIndex == UNROLLING_FACTOR ) {
		q->writeIndex = 0;
		q->last->next = node_new( q->last->next );
		q->last = q->last->next;
	}
	q->last->objects[q->writeIndex] = object;
	++q->writeIndex;
}

#ifdef WITH_TESTS
#include "queue_tests.h"
#endif
