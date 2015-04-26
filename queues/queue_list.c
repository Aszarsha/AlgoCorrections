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
	queue_obj object;
} * node;

static node node_new( queue_obj object, node next ) {
	node n = (node)malloc( sizeof(*n) );
	n->object = object;
	n->next = next;
	return n;
}

static void node_delete( node n, obj_del_func delf ) {
	delf( n->object );
	free( n );
}

struct queue_ {
	node last; //< Circular list!
};

extern queue queue_new( void ) {
	queue q = (queue)malloc( sizeof(*q) );
	q->last = NULL;
	return q;
}

extern void queue_delete( queue q, obj_del_func delf ) {
	assert( q );

	if ( q->last ) {
		node it = q->last->next;
		q->last->next = NULL;   // break cycle
		while ( it ) {
			node tmp = it;
			it = it->next;
			node_delete( tmp, delf );
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

extern void queue_push( queue q, queue_obj object ) {
	assert( q );

	if ( !q->last ) {
		q->last = node_new( object, NULL );
		q->last->next = q->last;
	} else {
		q->last->next = node_new( object, q->last->next );
		q->last = q->last->next;
	}
}

#ifdef WITH_TESTS
#include "queue_tests.h"
#endif
