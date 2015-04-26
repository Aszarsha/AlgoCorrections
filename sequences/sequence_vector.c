#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests
exit
*/
#include "sequence.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#if !defined(BASE_SIZE) || BASE_SIZE < 2
#define BASE_SIZE 10
#endif

#if !defined(GROWTH_FACTOR) || GROWTH_FACTOR < 1
#define GROWTH_FACTOR 2
#endif

struct sequence_ {
	sequence_obj * vector;
	size_t size;
	size_t capacity;
};

extern sequence sequence_new( void ) {
	sequence s = (sequence)malloc( sizeof(*s) );
	s->vector = (sequence_obj *)malloc( BASE_SIZE*sizeof(*s->vector) );
	s->capacity = BASE_SIZE;
	s->size = 0;
	return s;
}

extern void sequence_delete( sequence s, obj_del_func delf ) {
	assert( s );

	for ( size_t i = 0; i < s->size; ++i ) {
		delf( s->vector[i] );
	}
	free( s->vector );
	free( s );
}

extern size_t sequence_length( sequence s ) {
	assert( s );

	return s->size;
}

extern sequence_obj sequence_get( sequence s, size_t pos ) {
	assert( s );
	assert( pos >= 0 && pos < s->size && "index out of bounds" );

	return s->vector[pos];
}

extern void sequence_remove( sequence s, size_t pos ) {
	assert( s );
	assert( pos >= 0 && pos < s->size && "index out of bounds" );

	memmove( s->vector+pos, s->vector+pos+1, sizeof(*s->vector)*(s->size-pos-1) );
	--s->size;
}

static inline void sequence_extend_buffer( sequence s ) {
	s->capacity *= GROWTH_FACTOR;
	s->vector = (sequence_obj *)realloc( s->vector, s->capacity*sizeof(*s->vector) );
}

extern void sequence_insert( sequence s, size_t pos, sequence_obj object ) {
	assert( s );
	assert( pos >= 0 && pos <= s->size && "index out of bounds" );

	++s->size;
	if ( s->size == s->capacity ) {
		sequence_extend_buffer( s );
	}
	memmove( s->vector+pos+1, s->vector+pos, sizeof(*s->vector)*(s->size-pos-1) );
	s->vector[pos] = object;
}

#ifdef WITH_TESTS
#include "sequence_tests.h"
#endif
