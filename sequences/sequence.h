#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <stddef.h>

typedef struct sequence_ * sequence;

typedef void * sequence_obj;

typedef void (*obj_del_func)( sequence_obj );

extern sequence sequence_new   ( void );
extern void     sequence_delete( sequence s, obj_del_func delf );

extern size_t   sequence_length( sequence s );
extern void   * sequence_get   ( sequence s, size_t pos ); //< An item a position `pos` must exist
extern void     sequence_remove( sequence s, size_t pos ); //< An item a position `pos` must exist
extern void     sequence_insert( sequence s, size_t pos, sequence_obj object ); //< The sequence must be at least `pos-1` long

#endif
