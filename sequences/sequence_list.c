#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests
exit
*/
#include "sequence.h"

#include <assert.h>
#include <stdlib.h>

typedef struct node_ {
	struct node_ * prev;
	struct node_ * next;
	sequence_obj object;
} * node;

static node node_new( sequence_obj object, node prev, node next ) {
	node n = (node)malloc( sizeof(*n) );
	n->object = object;
	n->prev = prev;
	n->next = next;
	return n;
}

static void node_delete( node n, obj_del_func delf ) {
	delf( n->object );
	free( n );
}

struct sequence_ {
	node head; //< Circular list!
	size_t size;
};

extern sequence sequence_new( void ) {
	sequence s = (sequence)malloc( sizeof(*s) );
	s->head = NULL;
	s->size = 0;
	return s;
}

extern void sequence_delete( sequence s, obj_del_func delf ) {
	assert( s );

	node it = s->head;
	s->head->prev->next = NULL;
	while ( it ) {
		node tmp = it;
		it = it->next;
		node_delete( tmp, delf );
	}
	free( s );
}

extern size_t sequence_length( sequence s ) {
	assert( s );

	return s->size;
}

static node get_node( sequence s, size_t pos ) {
	node it = s->head;
	if ( pos <= s->size/2 ) {
		for ( size_t i = 0; i < pos; ++i, it = it->next ) {   ;   }
	} else {
		for ( size_t i = s->size; i > pos; --i, it = it->prev ) {   ;   }
	}
	return it;
}

extern sequence_obj sequence_get( sequence s, size_t pos ) {
	assert( s );
	assert( s->head && "must not be empty" );
	assert( pos >= 0 && pos < s->size && "index out of bounds" );

	return get_node( s, pos )->object;
}

extern void sequence_remove( sequence s, size_t pos ) {
	assert( s );
	assert( s->head && "must not be empty" );
	assert( pos >= 0 && pos < s->size && "index out of bounds" );

	node it = get_node( s, pos );
	it->prev->next = it->next;
	it->next->prev = it->prev;
	free( it );
	if ( !(--s->size) ) {
		s->head = NULL;
	}
}

extern void sequence_insert( sequence s, size_t pos, sequence_obj object ) {
	assert( s );
	assert( pos >= 0 && pos <= s->size && "index out of bounds" );

	node n;
	if ( !s->size ) {
		n = node_new( object, NULL, NULL );
		s->head = n->next = n->prev = n;
	} else {
		node it = get_node( s, pos );
		n = node_new( object, it->prev, it );
		it->prev = it->prev->next = n;
	}
	++s->size;
	if ( pos == 0 ) {
		s->head = n;
	}
}

#ifdef WITH_TESTS
#include "sequence_tests.h"
#endif
