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

static inline int max( int a, int b ) {
  return a < b ? b : a;
}

//===== AVL Node =====//
typedef struct avl_node_ {
	struct avl_node_ * left;
	struct avl_node_ * right;
	int    height;
	map_obj value;
} * avl_node;

static inline avl_node avl_node_new( map_obj value ) {
	avl_node n = (avl_node)malloc( sizeof(*n) );
	n->left   = NULL;
	n->right  = NULL;
	n->height = 0;
	n->value  = value;
	return n;
}

static void avl_node_delete( avl_node node, map_del_func delf ) {
	if ( node ) {
		avl_node_delete( node->left , delf );
		delf( node->value );
		avl_node right = node->right;
		free( node );
		avl_node_delete( right, delf );   // terminal
	}
}

static avl_node avl_node_find( avl_node node
                             , map_key_func keyf
                             , map_cmp_func cmpf
                             , map_key key
                             ) {
	if ( !node ) {   return NULL;   }
	int cmp = cmpf( key, keyf( node->value ) );
	if      ( cmp > 0 ) {   return avl_node_find( node->right, keyf, cmpf, key );   }
	else if ( cmp < 0 ) {   return avl_node_find( node->left , keyf, cmpf, key );   }
	return node;
}

static inline int avl_node_height( avl_node node ) {
	return node ? node->height : -1;
}

static inline avl_node avl_node_left_rotate( avl_node node ) {
	avl_node right = node->right;

	node->right = right->left;
	right->left = node;

	node->height  -= 1;
	right->height += 1;

	return right;
}

static inline avl_node avl_node_right_rotate( avl_node node ) {
	avl_node left = node->left;

	node->left = left->right;
	left->right = node;

	node->height -= 1;
	left->height += 1;

	return left;
}

static inline int avl_node_balance( astroyvl_node node ) {
	int height_left  = avl_node_height( node->left  );
	int height_right = avl_node_height( node->right );
	return height_left - height_right;
}

static avl_node avl_node_rebalance( avl_node node ) {
	int balance = avl_node_balance( node );
	avl_node result_node = node;
	if ( balance >= 2 ) {
		if ( avl_node_balance( node->left  ) <= -1 ) {
			node->left = avl_node_left_rotate( node->left );
		}
		result_node = avl_node_right_rotate( node );
	} else if ( balance <= -2 ) {
		if ( avl_node_balance( node->right ) >=  1 ) {
			node->right = avl_node_right_rotate( node->right );
		}
		result_node = avl_node_left_rotate( node );
	}

	result_node->height = 1 + max( avl_node_height( result_node->left  )
	                             , avl_node_height( result_node->right )
	                             );
	return result_node;
}

static avl_node avl_node_insert( avl_node node
                               , map_key_func keyf
                               , map_cmp_func cmpf
                               , map_obj value
                               ) {
	if ( !node ) {   return avl_node_new( value );   }
	int cmp = cmpf( keyf( value ), keyf( node->value ) );
	if ( cmp > 0 ) {
		avl_node new_right = avl_node_insert( node->right, keyf, cmpf, value );
		if ( new_right != NULL ) {
			node->right = new_right;
			return avl_node_rebalance( node );
		}
	} else if ( cmp < 0 ) {
		avl_node new_left  = avl_node_insert( node->left , keyf, cmpf, value );
		if ( new_left != NULL ) {
			node->left = new_left;
			return avl_node_rebalance( node );
		}
	}
	return NULL;
}

typedef struct remove_return_ {
	avl_node new_root;
	map_obj removed_object;
} remove_return;

static remove_return avl_node_remove_successor( avl_node current, avl_node root ) {
	remove_return ret;
	if ( !current->left ) {
		ret.removed_object = root->value;
		root->value  = current->value;
		ret.new_root = current->right;
		free( current );
		return ret;
	}

	ret = avl_node_remove_successor( current->left, root );
	current->left = ret.new_root;
	current->height = 1 + max( avl_node_height( current->left  )
	                         , avl_node_height( current->right )
	                         );
	ret.new_root = avl_node_rebalance( current );
	return ret;
}

static remove_return avl_node_remove_predecessor( avl_node current, avl_node root ) {
	remove_return ret;
	if ( !current->right ) {
		ret.removed_object = root->value;
		root->value = current->value;
		ret.new_root = current->left;
		free( current );
		return ret;
	}

	ret = avl_node_remove_predecessor( current->right, root );
	current->right = ret.new_root;
	current->height = 1 + max( avl_node_height( current->left  )
	                         , avl_node_height( current->right )
	                         );
	ret.new_root = avl_node_rebalance( current );
	return ret;
}

static remove_return avl_node_remove( avl_node node
                                    , map_key_func keyf
                                    , map_cmp_func cmpf
                                    , map_key key
                                    ) {
	remove_return ret = { NULL, NULL };
	if ( !node ) {   return ret;   }
	int cmp = cmpf( key, keyf( node->value ) );
	if ( cmp > 0 ) {
		ret = avl_node_remove( node->right, keyf, cmpf, key );
		node->right = ret.new_root;
	} else if ( cmp < 0 ) {
		ret = avl_node_remove( node->left, keyf, cmpf, key );
		node->left = ret.new_root;
	} else {
		ret.removed_object = node->value;
		avl_node left  = node->left;
		avl_node right = node->right;
		if ( !left || !right ) {
			free( node );
			ret.new_root = left ? left : right ? right : NULL;
			return ret;
		} else {
			if ( avl_node_balance( node ) < 0 ) {
				ret = avl_node_remove_successor  ( right, node );
				node->right = ret.new_root;
			} else {
				ret = avl_node_remove_predecessor( left , node );
				node->left  = ret.new_root;
			}
		}
	}
	node->height = 1 + max( avl_node_height( node->left  )
	                      , avl_node_height( node->right )
	                      );
	ret.new_root = avl_node_rebalance( node );
	return ret;
}

static void avl_node_print_list( avl_node node, obj_printer_func obj_printer, FILE * out ) {
	if ( node ) {
		fprintf( out, "(" );
		avl_node_print_list( node->left, obj_printer, out );
		fprintf( out, " " );
		obj_printer( node->value, out );
		fprintf( out, " " );
		avl_node_print_list( node->right, obj_printer, out );
		fprintf( out, ")" );
	}
}

//===== AVL Map =====//
struct map_ {
	avl_node     root;
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

	avl_node_delete( m->root, delf );
	free( m );
}

map_obj map_find( map m, map_key key ) {
	assert( m );

	avl_node node = avl_node_find( m->root, m->keyf, m->cmpf, key );
	return node ? node->value : NULL;
}

map_obj map_remove( map m, map_key key ) {
	assert( m );

	remove_return ret = avl_node_remove( m->root, m->keyf, m->cmpf, key );
	m->root = ret.new_root;
	return ret.removed_object;
}

bool map_insert( map m, map_obj obj ) {
	assert( m );

	avl_node node = avl_node_insert( m->root, m->keyf, m->cmpf, obj );
	return node ? (m->root = node, true) : false;
}

void map_print_list( map m, obj_printer_func obj_printer, FILE * out ) {
	avl_node_print_list( m->root, obj_printer, out );
	fprintf( out, "\n" );
}

#ifdef WITH_TESTS
#include "map_tests.h"
#endif
