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

#if !defined(GAP_LENGTH) || GAP_LENGTH < 1
#define GAP_LENGTH 8
#endif

struct sequence_ {
	sequence_obj * vector;
	size_t size;
	size_t capacity;
	size_t gapSize;
	ptrdiff_t gapOffset;
};

extern sequence sequence_new( void ) {
	sequence s = (sequence)malloc( sizeof(*s) );
	s->vector = (sequence_obj *)malloc( BASE_SIZE*sizeof(*s->vector) );
	s->capacity = s->gapSize = BASE_SIZE;
	s->size = s->gapOffset = 0;
	return s;
}

extern void sequence_delete( sequence s, obj_del_func delf ) {
	assert( s );

	for ( size_t i = 0; i < s->gapOffset; ++i ) {
		delf( s->vector[i] );
	}
	for ( size_t i = s->gapOffset; i < s->size; ++i ) {
		delf( s->vector[i + s->gapSize] );
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

	return pos >= s->gapOffset ? s->vector[pos + s->gapSize] : s->vector[pos];
}

static inline void sequence_extend_buffer( sequence s ) {
	s->capacity *= GROWTH_FACTOR;
	s->vector = (sequence_obj *)realloc( s->vector, s->capacity*sizeof(*s->vector) );
}

static inline void sequence_move_extend_gap( sequence s, size_t pos ) {
	if ( s->size + GAP_LENGTH >= s->capacity ) {
		sequence_extend_buffer( s );
	}

	sequence_obj * vectorPos = s->vector + pos;
	sequence_obj * vectorGap = s->vector + s->gapOffset;
	if ( pos <= s->gapOffset ) {
		size_t offset = s->gapOffset - pos;
		memmove( vectorPos + GAP_LENGTH + offset
		       , vectorGap + s->gapSize
		       , sizeof(*s->vector)*(s->size - s->gapOffset)
		       );
		memmove( vectorPos + GAP_LENGTH
		       , vectorPos
		       , sizeof(*s->vector)*offset
		       );
	} else if ( pos > s->gapOffset ) {
		memmove( vectorGap
		       , vectorGap + s->gapSize
		       , sizeof(*s->vector)*(pos - s->gapOffset + s->gapSize)
		       );
		memmove( vectorPos + GAP_LENGTH
		       , vectorPos
		       , sizeof(*s->vector)*(s->size - pos)
		       );
	}
	s->gapSize = GAP_LENGTH;
	s->gapOffset = pos;
}

extern void sequence_remove( sequence s, size_t pos ) {
	assert( s );
	assert( pos >= 0 && pos < s->size && "index out of bounds" );

	if ( pos != s->gapOffset || s->gapSize == 0 ) {
		sequence_move_extend_gap( s, pos );
	}
	++s->gapSize;
	--s->size;
}

extern void sequence_insert( sequence s, size_t pos, sequence_obj object ) {
	assert( s );
	assert( pos >= 0 && pos <= s->size && "index out of bounds" );

	if ( pos != s->gapOffset || s->gapSize == 0 ) {
		sequence_move_extend_gap( s, pos );
	}
	s->vector[s->gapOffset] = object;
	++s->gapOffset;
	--s->gapSize;
	++s->size;
}

#ifdef WITH_TESTS
#include "sequence_tests.h"
#endif
