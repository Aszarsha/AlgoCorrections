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

#if !defined(BASE_SIZE) || BASE_SIZE < 2
#define BASE_SIZE 10
#endif

#if !defined(GROWTH_FACTOR) || GROWTH_FACTOR < 1
#define GROWTH_FACTOR 2
#endif

struct stack_ {
	stack_obj * vector;
	ptrdiff_t index;
	size_t capacity;
};

extern stack stack_new( void ) {
	stack s = (stack)malloc( sizeof(*s) );
	s->vector = (stack_obj *)malloc( BASE_SIZE*sizeof(*s->vector) );
	s->capacity = BASE_SIZE;
	s->index = 0;
	return s;
}

extern void stack_delete( stack s, obj_del_func delf ) {
	assert( s );

	for ( int i = 0; i < s->index; ++i ) {
		delf( s->vector[i] );
	}
	free( s->vector );
	free( s );
}

extern int stack_empty( stack s ) {
	assert( s );

	return !s->index;
}

extern stack_obj stack_top( stack s ) {
	assert( s );
	assert( s->index && "must not be empty" );

	return s->vector[s->index-1];
}

extern void stack_pop( stack s ) {
	assert( s );
	assert( s->index && "must not be empty" );

	--s->index;
}

extern void stack_push( stack s, stack_obj object ) {
	assert( s );

	if ( s->index == s->capacity ) {
		s->capacity *= GROWTH_FACTOR;
		s->vector = (stack_obj *)realloc( s->vector, s->capacity*sizeof(*s->vector) );
	}
	s->vector[s->index] = object;
	++s->index;
}

#ifdef WITH_TESTS
#include "stack_tests.h"
#endif
