#ifndef MAP_TESTS_H
#define MAP_TESTS_H

#include "map.h"

#include <stdlib.h>

typedef struct test_object_ {
	int  index;
	char character;
} * test_object;

static test_object test_object_new( int index, char character ) {
	test_object to = (test_object)malloc( sizeof(*to) );
	to->index = index;
	to->character = character;
	return to;
}

static void test_object_delete( map_obj to ) {
	free( (test_object)to );
}

static map_key test_object_key( map_obj to ) {
	return &((test_object)to)->index;
}

static int int_key_compare( map_key a, map_key b ) {
	return *(int *)a - *(int *)b;
}

static void test_object_printer( void * to, FILE * out ) {
	test_object cto = (test_object)to;
	fprintf( out, "\"%d: %c\"", cto->index, cto->character );
}

static void test_insert( map m, int index, char character ) {
	test_object value = test_object_new( index, character );
	bool success = map_insert( m, value );
	if ( !success ) {
		printf( "-- Failed to insert " );
		test_object_printer( value, stdout );
		printf( "\n : " );
		test_object_delete( value );
	} else {
		printf( "++ Successfully inserted " );
		test_object_printer( value, stdout );
		printf( "\n : " );
	}
	map_print_list( m, test_object_printer, stdout );
}

static void test_find( map m, int key ) {
	map_obj res = map_find( m, &key );
	if ( !res ) {
		printf( "-- Failed to find %d\n : ", key );
	} else {
		printf( "++ Successfully found " );
		test_object_printer( res, stdout );
		printf( "\n : " );
	}
	map_print_list( m, test_object_printer, stdout );
}

static void test_remove( map m, int key ) {
	map_obj res = map_remove( m, &key );
	if ( !res ) {
		printf( "-- Failed to remove %d\n : ", key );
	} else {
		printf( "++ Successfully removed " );
		test_object_printer( res, stdout );
		printf( "\n : " );
		test_object_delete( res );
	}
	map_print_list( m, test_object_printer, stdout );
}

int main( int argc, char * argv[] ) {
	map m = map_new( &test_object_key, &int_key_compare );

	test_insert( m, 1, 'a'+0 );
	test_insert( m, 4, 'a'+3 );
	test_insert( m, 5, 'a'+4 );
	test_insert( m, 1, 'v'   );
	test_insert( m, 3, 'a'+2 );
	test_insert( m, 6, 'a'+5 );
	test_insert( m, 2, 'a'+1 );
	test_insert( m, 3, 'w'   );

	test_find( m, 2 );
	test_find( m, 4 );
	test_find( m, 7 );

	test_remove( m, 1 );
	test_remove( m, 5 );
	test_remove( m, 6 );
	test_remove( m, 2 );
	test_remove( m, 7 );

	test_find( m, 2 );
	test_find( m, 4 );
	test_find( m, 0 );

	map_delete( m, &test_object_delete );
}

#endif
