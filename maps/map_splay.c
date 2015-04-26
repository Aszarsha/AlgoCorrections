#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests
exit
*/
#include "map.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

//===== Splay Node =====//
typedef struct splay_node_t {
  struct splay_node_t * left;
  struct splay_node_t * right;
  map_obj value;
} * splay_node;

static inline splay_node splay_node_new( splay_node left, splay_node right, map_obj value ) {
	splay_node node = (splay_node)malloc( sizeof(*node) );
	node->left  = left;
	node->right = right;
	node->value = value;
	return node;
}

static void splay_node_delete( splay_node node, map_del_func delf ) {
	if ( node ) {
		splay_node_delete( node->left , delf );
		delf( node->value );
		splay_node right = node->right;
		free( node );
		splay_node_delete( right, delf );   // terminal
	}
}

static inline splay_node splay_node_left_rotate( splay_node node ) {
	splay_node right = node->right;

	node->right = right->left;
	right->left = node;

	return right;
}

static inline splay_node splay_node_right_rotate( splay_node node ) {
	splay_node left = node->left;

	node->left  = left->right;
	left->right = node;

	return left;
}

static splay_node splay_node_splay( splay_node node
                                  , map_key_func keyf
                                  , map_cmp_func cmpf
                                  , map_key key
                                  ) {
	map_key nkey = keyf( node->value );
	int ncmp = cmpf( key, nkey );
	splay_node child;
	if ( ncmp >= 1 && (child = node->right) ) {          // right-?
		map_key ckey = keyf( child->value );
		int ccmp = cmpf( key, ckey );
		splay_node grandChild;
		if ( ccmp >= 1 && (grandChild = child->right) ) {          // right-right
			grandChild = child->right = splay_node_splay( grandChild, keyf, cmpf, key );

			//splay_node alpha = n->left;
			splay_node beta  = child->left;
			splay_node gamma = grandChild->left;
			//splay_node theta = grandChild->right;

			//grandChild->right = theta;
			grandChild->left  = child;
			child->right  = gamma;
			child->left   = node;
			node->right  = beta;
			//n->left   = alpha;

			return grandChild;
		} else if ( ccmp <= -1 && (grandChild = child->left) ) {   // right-left
			grandChild = child->left = splay_node_splay( grandChild, keyf, cmpf, key );

			//splay_node theta = child->right;
			splay_node gamma = grandChild->right;
			splay_node beta  = grandChild->left;
			//splay_node alpha = n->left;

			grandChild->left  = node;
			grandChild->right = child;
			//child->right  = theta;
			child->left   = gamma;
			node->right  = beta;
			//n->left   = alpha;

			return grandChild;
		} else {   // odd height to node || no grand-child -> closest key found
			splay_node new_root = splay_node_left_rotate( node );
			return new_root;
		}
	} else if ( ncmp <= -1 && (child = node->left) ) {   // left-?
		map_key ckey = keyf( child->value );
		int ccmp = cmpf( key, ckey );
		splay_node grandChild;
		if ( ccmp >= 1 && (grandChild = child->right) ) {          // left-right
			grandChild = child->right = splay_node_splay( grandChild, keyf, cmpf, key );

			//splay_node alpha = c->left;
			splay_node beta  = grandChild->left;
			splay_node gamma = grandChild->right;
			//splay_node theta = n->right;

			grandChild->left  = child;
			grandChild->right = node;
			//c->left   = alpha;
			child->right  = beta;
			node->left   = gamma;
			//n->right  = theta;

			return grandChild;
		} else if ( ccmp <= -1 && (grandChild = child->left) ) {   // left-left
			grandChild = child->left = splay_node_splay( grandChild, keyf, cmpf, key );

			//splay_node theta = n->right;
			splay_node gamma = child->right;
			splay_node beta  = grandChild->right;
			//splay_node alpha = grandChild->left;

			//grandChild->left  = alpha;
			grandChild->right = child;
			child->left   = beta;
			child->right  = node;
			node->left   = gamma;
			//n->right  = theta;

			return grandChild;
		} else {   // odd height to node || no grand-child -> closest key found
			splay_node new_root = splay_node_right_rotate( node );
			return new_root;
		}
	}   // else, no child ==> closest key
	return node;
}

/* = Stupid splaying strategy often encountered for fast implementation and tests

static splay_node splay_node_cheap_splay( splay_node node
                                        , map_key_func keyf
                                        , map_cmp_func cmpf
                                        , map_key key
                                        ) {
	map_key localkey = keyf( node->value );
	int cmp = cmpf( key, localkey );
	if ( cmp > 0 ) {
		node->right = splay_node_cheap_splay( node->right, keyf, cmpf, key );
		return splay_node_left_rotate( node );
	} else if ( cmp < 0 ) {
		node->left = splay_node_cheap_splay( node->left, keyf, cmpf, key );
		return splay_node_right_rotate( node );
	}
	return node;
}
*/

static void splay_node_print_list( splay_node node, obj_printer_func obj_printer, FILE * out ) {
	if ( node ) {
		fprintf( out, "(" );
		splay_node_print_list( node->left, obj_printer, out );
		fprintf( out, " " );
		obj_printer( node->value, out );
		fprintf( out, " " );
		splay_node_print_list( node->right, obj_printer, out );
		fprintf( out, ")" );
	}
}

//===== Splay Map =====//
struct map_ {
	splay_node   root;
	map_key_func keyf;
	map_cmp_func cmpf;
};

map map_new( map_key_func keyf, map_cmp_func cmpf ) {
	map m = (map)malloc( sizeof(*m) );
	m->root = NULL;
	m->keyf = keyf;
	m->cmpf = cmpf;
	return m;
}

void map_delete( map m, map_del_func delf ) {
	assert( m );

	splay_node_delete( m->root, delf );
	free( m );
}

map_obj map_find( map m, map_key key ) {
	assert( m );

	if ( !m->root ) {   return NULL;   }
	m->root = splay_node_splay( m->root, m->keyf, m->cmpf, key );
	if ( m->cmpf( m->keyf( m->root->value ), key ) == 0 ) {
		return m->root->value;
	}
	return NULL;
}

static splay_node node_find_predecessor( splay_node node ) {
	return !node->right ? node : node_find_predecessor( node->right );
}

map_obj map_remove( map m, map_key key ) {
	assert( m );

	if ( !m->root ) {   return NULL;   }
	m->root = splay_node_splay( m->root, m->keyf, m->cmpf, key );
	if ( m->cmpf( key, m->keyf( m->root->value ) ) != 0 ) {
		return NULL;
	}
	splay_node root  = m->root;
	splay_node left  = root->left;
	splay_node right = root->right;

	map_obj value = m->root->value;
	if ( !left ) {
		if ( !right ) {
			free( m->root );
			m->root = NULL;
		} else {
			m->root = right;
			free( root );
		}
	} else {
		splay_node pred = node_find_predecessor( left );
		m->root = left;
		map_key predkey = m->keyf( pred->value );
		m->root = splay_node_splay( m->root, m->keyf, m->cmpf, predkey );
		m->root->right = right;
		free( root );
	}
	return value;
}

bool map_insert( map m, map_obj object ) {
	assert( m );

	if ( !m->root ) {
		m->root = splay_node_new( NULL, NULL, object );
		return true;
	}

	map_key objectkey = m->keyf( object );
	m->root = splay_node_splay( m->root, m->keyf, m->cmpf, objectkey );
	int cmp = m->cmpf( objectkey, m->keyf( m->root->value ) );
	if ( cmp > 0 ) {
		splay_node node = splay_node_new( m->root, m->root->right, object );
		m->root->right = NULL;
		m->root = node;
		return true;
	} else if ( cmp < 0 ) {
		splay_node node = splay_node_new( m->root->left, m->root, object );
		m->root->left = NULL;
		m->root = node;
		return true;
	} else {
		return false;
	}
}

/* = Following function are left for reference since they are often used with splay trees
 * = though they are not par of the map interface
 */
map splay_map_coupe( map m, map_key key ) {
	assert( m != NULL );
	if ( !m->root ) {
		return map_new( m->keyf, m->cmpf );
	}
	m->root = splay_node_splay( m->root, m->keyf, m->cmpf, key );
	map_key rootkey = m->keyf( m->root->value );

	map res = map_new( m->keyf, m->cmpf );
	if ( m->cmpf( key, rootkey ) >= 0 ) {
		res->root  = m->root->right;
		m->root->right = NULL;
	} else {
		res->root  = m->root;
		m->root = m->root->left;
		res->root->left   = NULL;
	}
	return res;
}

static int splay_max_func( map_key u, map_key v ) {   return  1;   }
static int splay_min_func( map_key u, map_key v ) {   return -1;   }

map splay_map_union( map A, map B ) {
	assert( A != NULL && B != NULL );
	A->root = splay_node_splay( A->root, A->keyf, splay_max_func, NULL );
	B->root = splay_node_splay( B->root, B->keyf, splay_min_func, NULL );
	map res = map_new( A->keyf, A->cmpf );
	if ( !A->root ) {
		res->root = B->root;
	} else {
		A->root->right = B->root;
		res->root = A->root;
	}
	return res;
}

void map_print_list( map m, obj_printer_func obj_printer, FILE * out ) {
	splay_node_print_list( m->root, obj_printer, out );
	fprintf( out, "\n" );
}

#ifdef WITH_TESTS
#include "map_tests.h"
#endif
