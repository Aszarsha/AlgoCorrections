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

/* /!\ Number of consecutive operations withous reallocation limited to 2^(8*sizeof(size_t)) /!\
   Correcting this is left as a supplementary exercise */

#if !defined(BASE_SIZE) || BASE_SIZE < 2
#define BASE_SIZE 10
#endif

#if !defined(GROWTH_FACTOR) || GROWTH_FACTOR < 1
#define GROWTH_FACTOR 2
#endif

struct queue_ {
	queue_obj * vector;
	size_t pushCount, popCount, capacity;
};

extern queue queue_new( void ) {
	queue q = (queue)malloc( sizeof(*q) );
	q->vector = (queue_obj *)malloc( BASE_SIZE*sizeof(*q->vector) );
	q->capacity = BASE_SIZE;
	q->pushCount = 0;
	q->popCount  = 0;
	return q;
}

extern void queue_delete( queue q, obj_del_func delf ) {
	assert( q );

	for ( size_t i = q->popCount; i != q->pushCount; i = (i+1)%q->capacity ) {
		delf( q->vector[i] );
	}
	free( q->vector );
	free( q );
}

extern int queue_empty( queue q ) {
	assert( q );

	return q->pushCount == q->popCount;
}

extern queue_obj queue_front( queue q ) {
	assert( q );
	assert( q->pushCount != q->popCount && "must not be empty" );

	return q->vector[q->popCount % q->capacity];
}

extern void queue_pop( queue q ) {
	assert( q );
	assert( q->pushCount != q->popCount && "must not be empty" );

	++q->popCount;
}

extern void queue_push( queue q, queue_obj object ) {
	assert( q );

	if ( q->pushCount - q->popCount >= q->capacity ) {
		// Manually copy the vector to 'decircularize' the data that still needs to be read
		queue_obj * tmp = (queue_obj *)malloc( q->capacity*GROWTH_FACTOR*sizeof(*q->vector) );
		ptrdiff_t readOffset = q->popCount % q->capacity;
		for ( int i = 0; i < q->pushCount - q->popCount; ++i ) {
				tmp[i] = q->vector[i + readOffset];
		}
		free( q->vector );
		q->vector = tmp;
		q->pushCount = q->pushCount - q->popCount;
		q->popCount = 0;
		q->capacity *= GROWTH_FACTOR;
	}
	q->vector[q->pushCount] = object;
	++q->pushCount;
}

#ifdef WITH_TESTS
#include "queue_tests.h"
#endif
