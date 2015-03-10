#ifndef QUEUE_TEST_H
#define QUEUE_TEST_H

#include "queue.h"

#include <stdlib.h>
#include <stdio.h>

enum Op { PUSH_OP = 0, POP_OP };

typedef enum Op (* select_op_func)( int, int, int );

/* Super basic operation selection, first all push, them all pop...
 * Implementing different strategies is left as an exercise */
static enum Op select_op( int numOp, int numPushOp, int numPopOp ) {
	return numPushOp < numOp/2 ? PUSH_OP : POP_OP;
}

static void test_queue( int numOp, select_op_func selectOp ) {
	int numPushOp = 0, numPopOp = 0;

	queue q = queue_create();
	for ( int i = 0; i < numOp; ++i ) {
		//printf( "numOp: %d, numPushOp: %d, numPopOp: %d\n", numOp, numPushOp, numPopOp );
		enum Op op = selectOp( numOp, numPushOp, numPopOp );
		//printf( "op: %lf\n", op );
		if ( op == PUSH_OP ) {
			queue_push( q, NULL );
			++numPushOp;
		} else {
			queue_pop( q );
			++numPopOp;
		}
	}
	while ( !queue_empty( q ) ) {
	  queue_pop( q );
	}
	queue_destroy( q );
}

int main( int argc, char * argv[] ) {
	if ( argc > 2 ) {   printf( "Usage: %s [numOp]\n", argv[0] );   }
	int numOp = argc == 2 ? atoi( argv[1] ) : 50000;
	if ( numOp % 2 == 1 ) {   ++numOp;   }

	test_queue( numOp, select_op );

	return EXIT_SUCCESS;
}

#endif
