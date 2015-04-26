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

//===== Binary Search Tree (BST) Node =====//
typedef struct bst_node_ {
	struct bst_node_ * left;
	struct bst_node_ * right;
	map_obj value;
} * bst_node;

static inline bst_node bst_node_new( map_obj value ) {
	bst_node n = (bst_node)malloc( sizeof(*n) );
	n->left  = NULL;
	n->right = NULL;
	n->value = value;
	return n;
}

static void bst_node_delete( bst_node node, map_del_func delf ) {
	if ( node ) {
		bst_node_delete( node->left , delf );
		delf( node->value );
		bst_node right = node->right;
		free( node );
		bst_node_delete( right, delf );   // terminal
	}
}

static bst_node bst_node_find( bst_node node
                             , map_key_func keyf
                             , map_cmp_func cmpf
                             , map_key key
                             ) {
	if ( !node ) {   return NULL;   }
	int cmp = cmpf( key, keyf( node->value ) );
	if      ( cmp > 0 ) {   return bst_node_find( node->right, keyf, cmpf, key );   }
	else if ( cmp < 0 ) {   return bst_node_find( node->left , keyf, cmpf, key );   }
	return node;
}

static bst_node bst_node_insert( bst_node node
                               , map_key_func keyf
                               , map_cmp_func cmpf
                               , map_obj value
                               ) {
	if ( !node ) {   return bst_node_new( value );   }
	int cmp = cmpf( keyf( value ), keyf( node->value ) );
	if ( cmp > 0 ) {
		bst_node new_right = bst_node_insert( node->right, keyf, cmpf, value );
		if ( new_right != NULL ) {
			node->right = new_right;
			return node;
		}
	} else if ( cmp < 0 ) {
		bst_node new_left  = bst_node_insert( node->left , keyf, cmpf, value );
		if ( new_left != NULL ) {
			node->left = new_left;
			return node;
		}
	}
	return NULL;
}

typedef struct remove_return_ {
	bst_node new_root;
	map_obj removed_object;
} remove_return;

static remove_return bst_node_remove_successor( bst_node current, bst_node root ) {
	remove_return ret;
	if ( !current->left ) {
		ret.removed_object = root->value;
		root->value  = current->value;
		ret.new_root = current->right;
		free( current );
		return ret;
	}

	ret = bst_node_remove_successor( current->left, root );
	current->left = ret.new_root;
	ret.new_root = current;
	return ret;
}

static remove_return bst_node_remove( bst_node node
                                    , map_key_func keyf
                                    , map_cmp_func cmpf
                                    , map_key key
                                    ) {
	remove_return ret = { NULL, NULL };
	if ( !node ) {   return ret;   }
	int cmp = cmpf( key, keyf( node->value ) );
	if ( cmp > 0 ) {
		ret = bst_node_remove( node->right, keyf, cmpf, key );
		node->right = ret.new_root;
	} else if ( cmp < 0 ) {
		ret = bst_node_remove( node->left , keyf, cmpf, key );
		node->left = ret.new_root;
	} else {
		ret.removed_object = node->value;
		bst_node left  = node->left;
		bst_node right = node->right;
		if ( !left || !right ) {
			free( node );
			ret.new_root = left ? left : right ? right : NULL;
			return ret;
		} else {
			ret = bst_node_remove_successor( right, node );
			node->right = ret.new_root;
		}
	}
	ret.new_root = node;
	return ret;
}

static void bst_node_print_list( bst_node node, obj_printer_func obj_printer, FILE * out ) {
	if ( node ) {
		fprintf( out, "(" );
		bst_node_print_list( node->left, obj_printer, out );
		fprintf( out, " " );
		obj_printer( node->value, out );
		fprintf( out, " " );
		bst_node_print_list( node->right, obj_printer, out );
		fprintf( out, ")" );
	}
}

//===== Binary Search Tree (BST) Map =====//
struct map_ {
	bst_node     root;
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

	bst_node_delete( m->root, delf );
	free( m );
}

map_obj map_find( map m, map_key key ) {
	assert( m );

	bst_node node = bst_node_find( m->root, m->keyf, m->cmpf, key );
	return node ? node->value : NULL;
}

map_obj map_remove( map m, map_key key ) {
	assert( m );

	remove_return ret = bst_node_remove( m->root, m->keyf, m->cmpf, key );
	m->root = ret.new_root;
	return ret.removed_object;
}

bool map_insert( map m, map_obj obj ) {
	assert( m );

	bst_node node = bst_node_insert( m->root, m->keyf, m->cmpf, obj );
	return node ? (m->root = node, true) : false;
}

void map_print_list( map m, obj_printer_func obj_printer, FILE * out ) {
	bst_node_print_list( m->root, obj_printer, out );
	fprintf( out, "\n" );
}

#ifdef WITH_TESTS
#include "map_tests.h"
#endif
