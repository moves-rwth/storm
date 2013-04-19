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

#include "harness.h"
#include "tbb/flow_graph.h"
#include "tbb/atomic.h"
#include "tbb/task_scheduler_init.h"

const int L = 10;
const int N = 1000;

template< typename T >
struct serial_receiver : public tbb::flow::receiver<T> {
   T next_value;

   serial_receiver() : next_value(T(0)) {}

   /* override */ tbb::task *try_put_task( const T &v ) {
       ASSERT( next_value++  == v, NULL );
       return const_cast<tbb::task *>(tbb::flow::interface6::SUCCESSFULLY_ENQUEUED);
   }

   /*override*/void reset_receiver() {next_value = T(0);}
};

template< typename T >
struct parallel_receiver : public tbb::flow::receiver<T> {

   tbb::atomic<int> my_count;

   parallel_receiver() { my_count = 0; }

   /* override */ tbb::task *try_put_task( const T &/*v*/ ) {
       ++my_count;
       return const_cast<tbb::task *>(tbb::flow::interface6::SUCCESSFULLY_ENQUEUED);
   }

   /*override*/void reset_receiver() {my_count = 0;}
};

template< typename T >
struct empty_sender : public tbb::flow::sender<T> {
        /* override */ bool register_successor( tbb::flow::receiver<T> & ) { return false; }
        /* override */ bool remove_successor( tbb::flow::receiver<T> & ) { return false; }
};


template< typename T >
struct put_body : NoAssign {

    tbb::flow::limiter_node<T> &my_lim;
    tbb::atomic<int> &my_accept_count;

    put_body( tbb::flow::limiter_node<T> &lim, tbb::atomic<int> &accept_count ) : 
        my_lim(lim), my_accept_count(accept_count) {}

    void operator()( int ) const {
        for ( int i = 0; i < L; ++i ) {
            bool msg = my_lim.try_put( T(i) );
            if ( msg == true )
               ++my_accept_count; 
        }
    }
};

template< typename T >
struct put_dec_body : NoAssign {

    tbb::flow::limiter_node<T> &my_lim;
    tbb::atomic<int> &my_accept_count;

    put_dec_body( tbb::flow::limiter_node<T> &lim, tbb::atomic<int> &accept_count ) : 
        my_lim(lim), my_accept_count(accept_count) {}

    void operator()( int ) const {
        int local_accept_count = 0;
        while ( local_accept_count < N ) {
            bool msg = my_lim.try_put( T(local_accept_count) );
            if ( msg == true ) {
                ++local_accept_count;
                ++my_accept_count;
                my_lim.decrement.try_put( tbb::flow::continue_msg() );
            } 
        }
    }

};

template< typename T >
void test_puts_with_decrements( int num_threads, tbb::flow::limiter_node< T >& lim ) {
    parallel_receiver<T> r;
    empty_sender< tbb::flow::continue_msg > s;
    tbb::atomic<int> accept_count;
    accept_count = 0;
    tbb::flow::make_edge( lim, r );
    lim.decrement.register_predecessor( s );
    // test puts with decrements
    NativeParallelFor( num_threads, put_dec_body<T>(lim, accept_count) );
    int c = accept_count;
    ASSERT( c == N*num_threads, NULL );
    ASSERT( r.my_count == N*num_threads, NULL );
}

//
// Tests
//
// limiter only forwards below the limit, multiple parallel senders / single receiver
// mutiple parallel senders that put to decrement at each accept, limiter accepts new messages
// 
// 
template< typename T >
int test_parallel(int num_threads) {
 
   // test puts with no decrements
   for ( int i = 0; i < L; ++i ) {
       tbb::flow::graph g;
       tbb::flow::limiter_node< T > lim(g, i);
       parallel_receiver<T> r;
       tbb::atomic<int> accept_count;
       accept_count = 0;
       tbb::flow::make_edge( lim, r );
       // test puts with no decrements
       NativeParallelFor( num_threads, put_body<T>(lim, accept_count) );
       g.wait_for_all();
       int c = accept_count;
       ASSERT( c == i, NULL );
   }

   // test puts with decrements
   for ( int i = 1; i < L; ++i ) {
       tbb::flow::graph g;
       tbb::flow::limiter_node< T > lim(g, i);
       test_puts_with_decrements(num_threads, lim);
       tbb::flow::limiter_node< T > lim_copy( lim );
       test_puts_with_decrements(num_threads, lim_copy);
   }

   return 0;
}

//
// Tests
//
// limiter only forwards below the limit, single sender / single receiver
// at reject, a put to decrement, will cause next message to be accepted
// 
template< typename T >
int test_serial() {
 
   // test puts with no decrements
   for ( int i = 0; i < L; ++i ) {
       tbb::flow::graph g;
       tbb::flow::limiter_node< T > lim(g, i);
       serial_receiver<T> r;
       tbb::flow::make_edge( lim, r );
       for ( int j = 0; j < L; ++j ) {
           bool msg = lim.try_put( T(j) );
           ASSERT( ( j < i && msg == true ) || ( j >= i && msg == false ), NULL );
       }
       g.wait_for_all();
   }

   // test puts with decrements
   for ( int i = 1; i < L; ++i ) {
       tbb::flow::graph g;
       tbb::flow::limiter_node< T > lim(g, i);
       serial_receiver<T> r;
       empty_sender< tbb::flow::continue_msg > s;
       tbb::flow::make_edge( lim, r );
       lim.decrement.register_predecessor( s );
       for ( int j = 0; j < N; ++j ) {
           bool msg = lim.try_put( T(j) );
           ASSERT( ( j < i && msg == true ) || ( j >= i && msg == false ), NULL );
           if ( msg == false ) {
               lim.decrement.try_put( tbb::flow::continue_msg() );
               msg = lim.try_put( T(j) );
               ASSERT( msg == true, NULL );
           }
       }
   }
   return 0;
}

int TestMain() { 
    for (int i = 1; i <= 8; ++i) {
        tbb::task_scheduler_init init(i);
        test_serial<int>();
        test_parallel<int>(i);
    }
   return Harness::Done;
}
