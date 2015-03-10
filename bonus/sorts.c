#define COMPILE_TESTS /*
cc -g -Wall -pedantic -std=c11 -DWITH_TESTS -lm -o ${0%.*}_tests ${0}
echo Running tests for ${0}
valgrind --tool=memcheck --leak-check=full ./${0%.*}_tests $1
exit
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <math.h>

static inline double * copy_array( double * array, size_t size ) {
  double * copy = (double *)malloc( sizeof(*copy)*size );
  memcpy( copy, array, size*sizeof(*array) );
  return copy;
}

static inline void swap( double * a, double * b ) {
  double tmp = *a;
  *a = *b;
  *b = tmp;
}

void shuffle( double * array, size_t size ) {
  for ( size_t i = 0; i != size; ++i ) {
    int randindex = (int)((size-i)*(rand()/nextafter( RAND_MAX, INFINITY )));
    swap( array+i, array+randindex+i );
  }
}

void selection_sort( double * array, size_t size ) {
  for ( size_t i = 0; i < size; ++i ) {
    size_t min = i;
    for ( size_t j = i; j < size; ++j ) {
      if ( array[j] < array[min] ) {
        min = j;
      }
    }
    swap( array+i, array+min );
  }
}

void bubble_sort( double * array, size_t size ) {
  for ( size_t i = 0; i < size; ++i ) {
    for ( size_t j = 0; j < size-i-1; ++j ) {
      if( array[j] > array[j+1] ) {
        swap( array+j, array+j+1 );
      }
    }
  }
}

void insertion_sort( double * array, size_t size ) {
  for ( size_t i = 1; i < size; ++i ) {
    double x = array[i];
    size_t j = i;
    for ( ; j > 0 && array[j-1] > x; --j ) {
      array[j] = array[j-1];
    }
    array[j] = x;
  }
}

static double * merge( double * t0, size_t s0, double * t1, size_t s1 ) {
  double * res = (double *)malloc( sizeof(*res)*(s0+s1) );
  size_t i = 0, j = 0;
  while( i + j != s0 + s1 ) {
    if ( i == s0 || (j != s1 && t0[i] > t1[j]) ) {
      res[i+j] = t1[j];
      j++;
    } else {
      res[i+j] = t0[i];
      i++;
    }
  }
  return res;
}

static double * rec_merge_sort( double * array, size_t size ) {
  if( size == 1 ) {
    return copy_array( array, 1 );
  } else{
    size_t size2 = size/2;
    double * s0 = rec_merge_sort( array, size2 );
    double * s1 = rec_merge_sort( array+size2, size - size2 );
    double * res = merge( s0, size2, s1, size-size2 );
    free( s0 );
    free( s1 );
    return res;
  }
}

void merge_sort( double * array, size_t size ) {
  double * tmp = rec_merge_sort( array, size );
  memcpy( array, tmp, sizeof(*array)*size );
  free( tmp );
}

static void rec_quick_sort( double * array, size_t size ) {
  if ( size <= 1 ) {
    return;
  }
  double p = array[0];
  size_t i = 0, j = 1, k = size-1;
  while ( j <= k ) {
    if ( array[j] > p ) {
      swap( array+j, array+k );
      --k;
    } else if ( array[j] < p ) {
      swap( array+i, array+j );
      ++i;   ++j;
    } else {
      ++j;
    }
  }
  rec_quick_sort( array, j-1 );
  rec_quick_sort( array+j, size-j );
}

void quick_sort( double * array, size_t size ) {
  shuffle( array, size );
  rec_quick_sort( array, size );
}

static void rec_three_way_qsort( double * array, size_t size ) {
  if ( size <= 1 ) {
    return;
  }
  double p = array[0];
  size_t i = 0, j = 0, k = 1, l = size-1;
  while ( k <= l ) {
    if ( array[k] > p ) {
      swap( array+k, array+l );
      --l;
    } else if ( array[k] < p ) {
      swap( array+i, array+k );
      ++i;   ++j;   ++k;
    } else {
      ++k;
    }
  }
  rec_three_way_qsort( array, j );
  rec_three_way_qsort( array+k, size-k );
}

void three_way_qsort( double * array, size_t size ) {
  shuffle( array, size );
  rec_three_way_qsort( array, size );
}

#ifdef WITH_TESTS

typedef void (*sort_func)( double *, size_t );

static double * init_array( size_t size ) {
  double * arr = (double *)malloc( size*sizeof(*arr) );
  assert( arr );
  for ( size_t i = 0; i < size; ++i ) {
    arr[i] = rand()/(double)RAND_MAX;
  }
  return arr;
}

static void print_array( double * array, size_t size ) {
  printf( "[" );
  for ( size_t i = 0; i < size; ++i ) {
    printf( "%lf, ", array[i] );
  }
  printf( "\b\b]\n" );
}

static void test_sort( double * array, size_t size
                     , sort_func func, char const * header
                     ) {
  double * copy = copy_array( array, size );
  printf( "%s", header );
  func( copy, size );
  print_array( copy, size );
  free( copy );
}

int main( int argc, char * argv[] ) {
  if( argc > 2 ) {
    fprintf( stderr, "Usage: %s size\n", argv[0] );
    exit( EXIT_FAILURE );
  }
  srand( time( NULL ) );
  int size = argv == 2 ? atoi( argv[1] ) : 10;
  double * array = init_array( size );
  printf( "Base array     : " );
  print_array( array, size );

  test_sort( array, size, &shuffle        , "Shuffle        : " );
  test_sort( array, size, &selection_sort , "Selection sort : " );
  test_sort( array, size, &bubble_sort    , "Bubble sort    : " );
  test_sort( array, size, &insertion_sort , "Insertion sort : " );
  test_sort( array, size, &merge_sort     , "Merge sort     : " );
  test_sort( array, size, &quick_sort     , "Quick sort     : " );
  test_sort( array, size, &three_way_qsort, "Three Way Qsort: " );

  free( array );

  return EXIT_SUCCESS;
}

#endif
