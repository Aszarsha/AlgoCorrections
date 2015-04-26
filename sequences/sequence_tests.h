#ifndef SEQUENCE_TESTS_H
#define SEQUENCE_TESTS_H

#include "sequence.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (* print_elem_func)( sequence_obj e );

static void nop_delf( sequence_obj o ) {   ;   }

static void print_char_elem( sequence_obj e ) {
	printf( "%c", (char)e );
}

static void sequence_dump( sequence s, print_elem_func print_elem ) {
	int len = sequence_length( s );
	if ( len ) {
		print_elem( sequence_get( s, 0 ) );
		for ( int i = 1; i < len; ++i ) {
			printf( ", " );
			print_elem( sequence_get( s, i ) );
		}
	}
}

static void test_insert( sequence s, sequence_obj e, int pos ) {
	printf( "Inserting " );
	print_char_elem( e );
	printf( " at position %d:\t", pos );
	sequence_insert( s, pos, e );
	sequence_dump( s, print_char_elem );
	printf( "\n" );
}

static void test_remove( sequence s, int pos ) {
	printf( "Removing at position %d:\t", pos );
	sequence_remove( s, pos );
	sequence_dump( s, print_char_elem );
	printf( "\n" );
}

int main( int argc, char * argv[] ) {
	sequence s = sequence_new();

	for ( int i = 0; i < 10; ++i ) {
		test_insert( s, (sequence_obj)('a'+i), sequence_length( s ) );
	}

	for ( int i = 0; i < 10; ++i ) {
		test_insert( s, (sequence_obj)('0'+i), 5+i );
	}

	for ( int i = 0; i < 10; ++i ) {
		test_remove( s, 10 );
	}

	for ( int i = 0; i < 5; ++i ) {
		test_insert( s, (sequence_obj)('z'-i), 0 );
	}

	for ( int i = 0; i < 5; ++i ) {
		test_remove( s, 10 );
	}

	sequence_delete( s, nop_delf );
}

#endif
