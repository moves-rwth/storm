/*
    Copyright 2005-2013 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

#include "harness_graph.h"
#include "tbb/task.h"

#include "tbb/task_scheduler_init.h"

const int N = 1000;

template< typename T >
class test_push_receiver : public tbb::flow::receiver<T> {

    tbb::atomic<int> my_counters[N];

public:

    test_push_receiver() {  
        for (int i = 0; i < N; ++i ) 
            my_counters[i] = 0;
    }

    int get_count( int i ) {
       int v = my_counters[i]; 
       return v;
    }

    tbb::task *try_put_task( const T &v ) {
       int i = (int)v;
       ++my_counters[i];
       return const_cast<tbb::task *>(tbb::flow::interface6::SUCCESSFULLY_ENQUEUED);
    }

    /*override*/void reset_receiver() {}
};

template< typename T >
class source_body {

   tbb::atomic<int> my_count;
 
public:

   source_body() { my_count = 0; }

   bool operator()( T &v ) {
      v = (T)my_count.fetch_and_increment();
      if ( (int)v < N )
         return true;
      else
         return false;
   } 

};

template< typename T >
class function_body {

    tbb::atomic<int> *my_counters;

public:

    function_body( tbb::atomic<int> *counters ) : my_counters(counters) {  
        for (int i = 0; i < N; ++i ) 
            my_counters[i] = 0;
    }

    bool operator()( T v ) {
        ++my_counters[(int)v];
        return true;
    } 

};

template< typename T >
void test_single_dest() {

   // push only
   tbb::flow::graph g;
   tbb::flow::source_node<T> src(g, source_body<T>() );
   test_push_receiver<T> dest;
   tbb::flow::make_edge( src, dest );
   g.wait_for_all();
   for (int i = 0; i < N; ++i ) {
       ASSERT( dest.get_count(i) == 1, NULL ); 
   }

   // push only
   tbb::atomic<int> counters3[N];
   tbb::flow::source_node<T> src3(g, source_body<T>() );
   function_body<T> b3( counters3 );
   tbb::flow::function_node<T,bool> dest3(g, tbb::flow::unlimited, b3 );
   tbb::flow::make_edge( src3, dest3 );
   g.wait_for_all();
   for (int i = 0; i < N; ++i ) {
       int v = counters3[i];
       ASSERT( v == 1, NULL ); 
   }

   // push & pull 
   tbb::flow::source_node<T> src2(g, source_body<T>() );
   tbb::atomic<int> counters2[N];
   function_body<T> b2( counters2 );
   tbb::flow::function_node<T,bool> dest2(g, tbb::flow::serial, b2 );
   tbb::flow::make_edge( src2, dest2 );
   g.wait_for_all();
   for (int i = 0; i < N; ++i ) {
       int v = counters2[i];
       ASSERT( v == 1, NULL ); 
   }

   // test copy constructor
   tbb::flow::source_node<T> src_copy(src);
   test_push_receiver<T> dest_c;
   ASSERT( src_copy.register_successor(dest_c), NULL );
   g.wait_for_all();
   for (int i = 0; i < N; ++i ) {
       ASSERT( dest_c.get_count(i) == 1, NULL ); 
   }
}

int TestMain() { 
    if( MinThread<1 ) {
        REPORT("number of threads must be positive\n");
        exit(1);
    }
    for ( int p = MinThread; p < MaxThread; ++p ) {
        tbb::task_scheduler_init init(p);
        test_single_dest<int>();
        test_single_dest<float>();
    }
    return Harness::Done;
}

