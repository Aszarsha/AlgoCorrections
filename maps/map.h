#ifndef MAP_H
#define MAP_H

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct map_ * map;

typedef void * map_key;
typedef void * map_obj;

typedef void (*map_del_func)( map_obj );

typedef map_key (*map_key_func)( map_obj );
typedef int     (*map_cmp_func)( map_key, map_key );

extern map  map_new   ( map_key_func keyf, map_cmp_func cmpf );
extern void map_delete( map m, map_del_func delf );

extern map_obj map_find  ( map m, map_key key );
extern map_obj map_remove( map m, map_key key );
extern bool    map_insert( map m, map_obj val );

/* debug */
typedef void (*obj_printer_func)( map_obj, FILE * out );

extern void map_print_list( map m, obj_printer_func obj_printer, FILE * out );

#endif
