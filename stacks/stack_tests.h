#ifndef STACK_TESTS_H
#define STACK_TESTS_H

#include "stack.h"

#include <stdlib.h>
#include <stdio.h>

enum Op { PUSH_OP = 0, POP_OP };

typedef enum Op (* select_op_func)( int, int, int );

/* Super basic operation selection, first all push, them all pop...
 * Implementing different strategies is left as an exercise */
static enum Op select_op( int numOp, int numPushOp, int numPopOp ) {
	return numPushOp < numOp/2 ? PUSH_OP : PUSH_OP;
}

static void nop_delf( stack_obj o ) {   ;   }

static void test_stack( int numOp, select_op_func selectOp ) {
	int numPushOp = 0, numPopOp = 0;

	stack s = stack_new();
	for ( int i = 0; i < numOp; ++i ) {
		enum Op op = selectOp( numOp, numPushOp, numPopOp );
		if ( op == PUSH_OP ) {
			stack_push( s, NULL );
			++numPushOp;
		} else {
			stack_pop( s );
			++numPopOp;
		}
	}
	stack_delete( s, nop_delf );
}

int main( int argc, char * argv[] ) {
	if ( argc > 2 ) {   printf( "Usage: %s [numOp]\n", argv[0] );   }
	int numOp = argc == 2 ? atoi( argv[1] ) : 50000;
	if ( numOp % 2 == 1 ) {   ++numOp;   }

	test_stack( numOp, select_op );

	return EXIT_SUCCESS;
}

#endif
