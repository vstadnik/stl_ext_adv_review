/////////////////////////////////////////////////////////////////
//
//          Copyright Vadim Stadnik 2011-2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
/////////////////////////////////////////////////////////////////
//
//  See folder "../doc" for documentation
//
/////////////////////////////////////////////////////////////////


#ifndef _PERFORM_TESTS_HPP
#define _PERFORM_TESTS_HPP

//#pragma once


//  standard containers
#include <vector> 
#include <deque>
#include <list> 
#include <set> 

//  boost containers
#include <boost/container/vector.hpp>
#include <boost/container/deque.hpp>
#include <boost/container/list.hpp>
#include <boost/container/set.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/stable_vector.hpp>

//  do not link serialization library 
#define BOOST_MULTI_INDEX_DISABLE_SERIALIZATION
//
//  "node compression" improves locality of reference and hence performance 
//#define BOOST_MULTI_INDEX_DISABLE_COMPRESSED_ORDERED_INDEX_NODES
//
#include <boost/multi_index_container.hpp> 
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/random_access_index.hpp>

//  std_ext_adv containers
#include "bp_tree_array.hpp"
#include "bp_tree_array_acc.hpp"
#include "bpt_sequence.hpp"
#include "bpt_set.hpp"
#include "bpt_map.hpp"
//
//  dynamically allocated augmented B+ tree from project:
//  https://github.com/vstadnik/stl_ext_adv
//
//  can be used to compare performance and effect of 
//  locality of reference with array based B+ trees ; 
//  
//#include "bp_tree_idx.hpp" 

#include "test_helpers.hpp"
#include "timer_chrono.hpp" 


//
//  group of simple tests for single operations of 
//  container that support STL interfaces ; 
//  
//  example how to use the tests:
//
//      std::vector<size_t> vector_szt ;
//      std::string         test_result ; 
//      const size_t        sz_test = 1000 ;   
//      test_performance::test_push_back ( sz_test , vector_szt , test_result ) ;
//      std::cout << "tests results for size = " << sz_test << " ;" <<std::endl ;
//      std::cout << test_result << std::endl ; 
//
namespace test_performance
{

    void AddTestResult 
        (
            TimerChrono const &     timer , 
            std::string const &     info  ,
            std::string &           result 
        ) 
    {
        std::string		str_timer ; 
        timer . TimeToString ( info , str_timer ) ; 
        result += str_timer ; 
    } 


    template < class _Ty_Seqce >
    void test_push_back_fill
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , 1 , 1 ) ;

        seqce_test . clear ( ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        for (    ; it_cur != it_end ; ++it_cur )
        {
            seqce_test . push_back ( *it_cur ) ;
        }

        timer . Stop ( ) ;
        AddTestResult ( timer , "push_back_fill" , test_res ) ; 
    }


    template < class _Ty_Seqce >
    void test_insert_fill
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , 1 , 1 ) ;

        seqce_test . clear ( ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;

        timer . Stop ( ) ;
        AddTestResult ( timer , "insert_fill" , test_res ) ; 
    }


    template < class _Ty_Seqce >
    void test_clear 
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , 1 , 1 ) ;

        seqce_test . clear ( ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        seqce_test . clear ( ) ;

        timer . Stop ( ) ;
        AddTestResult ( timer , "test clear" , test_res ) ; 
    }


    template < class _Ty_Seqce >
    void test_sort
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , 1 , 1 ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        seqce_test . clear ( ) ;
        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        seqce_test . sort( ) ;
        //  for array based B+ tree class bp_tree_array only:
    //  seqce_test . sort_tree( ) ;
    //  seqce_test . sort_vector( ) ;

        timer . Stop ( ) ;
        AddTestResult ( timer , "sort" , test_res ) ; 
    }


    template < class _Ty_Seqce >
    void test_unique
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        //  every second element will be removed by unique()
        const size_t            n_dupl  = 2 ;
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , n_dupl , 1 ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        seqce_test . clear ( ) ;
        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;
        seqce_test . sort ( ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        seqce_test . unique ( ) ;

        timer . Stop ( ) ;
        AddTestResult ( timer , "unique" , test_res ) ; 
    }


    template < class _Ty_Seqce >
    void test_reverse
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        //  every second element will be removed by unique()
        const size_t            n_dupl  = 1 ;
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , n_dupl , 1 ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        seqce_test . clear ( ) ;
        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;
        seqce_test . sort ( ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        seqce_test . reverse ( ) ;

        timer . Stop ( ) ;
        AddTestResult ( timer , "reverse" , test_res ) ; 
    }


    template < class _Ty_Seqce >
    void test_remove_if
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        //  every second element will be removed by unique()
        const size_t            n_dupl  = 1 ;
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , n_dupl , 1 ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        seqce_test . clear ( ) ;
        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;
        seqce_test . sort ( ) ;

        test_std_ext_adv::Even<size_t>
                                pr_even ;
        TimerChrono             timer ; 
        timer . Start ( ) ;

        seqce_test . remove_if ( pr_even ) ;

        timer . Stop ( ) ;
        AddTestResult ( timer , "remove if" , test_res ) ; 
    }


    template < class _Ty_Seqce >
    void test_push_back
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        seqce_test . clear ( ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        for ( size_t  i = 0 ; i < sz_test ; ++i )
        {
            seqce_test . push_back ( i ) ;
        }

        timer . Stop ( ) ;
        AddTestResult ( timer , "push_back" , test_res ) ; 
    }


    template < class _Ty_Seqce >
    void test_insert_mid
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test  ,
            std::string &       test_res 
        )
    {
        seqce_test . clear ( ) ;

        typename 
        _Ty_Seqce::iterator     it_mid = seqce_test . begin ( ) ;
        size_t                  count  = 0 ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        for ( size_t  i = 0 ; i < sz_test ; ++i )
        {
            if ( i % 2 )
                ++it_mid ;

            it_mid = seqce_test . insert ( it_mid , i ) ;
        }

        timer . Stop ( ) ;
        AddTestResult ( timer , "insert mid" , test_res ) ; 
    }


    template < class _Ty_M_Set >
    void test_insert_m_set
        (
            const size_t        sz_test     ,
            const bool          fill_rand   ,
            _Ty_M_Set &         m_set_test  ,
            std::string &       test_res 
        )
    {
        m_set_test . clear ( ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        for ( size_t  i = 0 ; i < sz_test ; ++i )
        {
            if ( fill_rand )
                m_set_test . insert ( rand() ) ;
            else
                m_set_test . insert ( i ) ;
        }

        timer . Stop ( ) ;
        std::string     comment = (fill_rand) ? 
                                  "insert multiset random"  :
                                  "insert multiset ordered" ;  
        AddTestResult ( timer , comment , test_res ) ; 
    }


    //  test of insert before a position near
    //  the middle of the input container ;
    //  make sure that input container has element of
    //  signed integer type, such as std::multiset<int>
    template < class _Ty_M_Set >
    void test_insert_mid_m_set
        (
            const size_t        sz_test     ,
            _Ty_M_Set &         m_set_test  ,
            std::string &       test_res 
        )
    {
        m_set_test . clear ( ) ;

        int         sz = int ( sz_test ) ;
        TimerChrono             timer ; 
        timer . Start ( ) ;

        for ( int   i = sz ; i > 0 ; --i )
        {
            //  i is decreasing ==> position is in the middle of container
            if ( i % 2 )
                m_set_test . insert ( i ) ;
            else
                m_set_test . insert ( -i ) ;
        }

        timer . Stop ( ) ;
        AddTestResult ( timer , "insert multiset mid posn" , test_res ) ; 
    }


    template < class _Ty_Contr >
    size_t  test_accumulate_std 
        ( 
            _Ty_Contr const &   contr_test  ,
            std::string &       test_res             
        )
    {
        size_t                  res      = 0  ;
        size_t                  n_runs   = 10 ;
        size_t                  sz_contr = contr_test . size ( ) ;
        if ( sz_contr < 100 )
        {
            std::cout << "   " << std::endl ;
            return res ;
        }

        TimerChrono             timer ; 
        typename 
        _Ty_Contr::const_iterator
                                it_cur = contr_test . begin ( ) ,
                                it_end = contr_test . begin ( ) ;

        std::advance ( it_cur , int ( sz_contr / 4 ) ) ;
        std::advance ( it_end , int ( sz_contr / 4 + sz_contr / 2 ) ) ;


        timer . Start ( ) ;

        for ( size_t    i = 0 ; i < n_runs ; ++i )
        {
            res += std::accumulate ( it_cur , it_end , 0 ) ;
        }

        timer . Stop ( ) ;
        AddTestResult ( timer , "std::accumulate" , test_res ) ; 

        return res ;
    }


    size_t  test_accumulate_fast 
        ( 
            const size_t        sz_test     ,
            std::string &       test_res 
        )
    {
        size_t                  res    = 0    ;
        size_t                  n_runs = 1000 ;

        typedef int                 _T  ;
        typedef std::allocator<_T>  _AT ;
        typedef std_ext_adv::sequence<_T, _AT, std_ext_adv::bp_tree_array_acc>
                                _SEQCE_ACC ;
        _SEQCE_ACC              seqce_test ;

        test_push_back ( sz_test , seqce_test , test_res ) ;

        _SEQCE_ACC::const_iterator
                                iter_a = seqce_test . begin ( ) ,
                                iter_b = seqce_test . begin ( ) ;
        iter_a += int ( sz_test / 4 ) ;
        iter_b += int ( sz_test / 4 + sz_test / 2 ) ;

        TimerChrono             timer ; 
        timer . Start ( ) ;

        for ( size_t    i = 0 ; i < n_runs ; ++i )
        {
            res += seqce_test . accumulate ( iter_a , iter_b , 0 ) ;
        }

        timer . Stop ( ) ;
        AddTestResult ( timer , "fast accumulate" , test_res ) ; 

        return res ;
    }

}   //  namespace test_performance ;



//
//  methods for measurements of performance of containers 
//  that support interfaces of C++ STL containers:
//  - standard containers ;
//  - containers using augmented B+ trees ;
//  - Boost containers ;
//  - Boost.MultiIndex containers ;
//
namespace test_performance
{

    //  this test implements an algorithm that moves an iterator 
    //  to a specified position and then insert a new element 
    //  into a containers ; 
    //  
    //  for a practical application of similar algorithms, 
    //  see the classical Josephus problem 
    //      http://en.wikipedia.org/wiki/Josephus_problem ;
    //  
    //  the running time of this test 
    //  for all the standard and Boost containers     is O ( N*N ) ;
    //  for containers using augmented data stuctures is O ( N*logN ) ; 
    //  
    template < class _Ty_Contr_Test , class _Ty_Vect_Values >
    void advance_insert
        (
            _Ty_Vect_Values const &         vec_value  , 
            std::vector<size_t> const &     vec_dist   , 
            _Ty_Contr_Test &                contr_test ,
            std::string &                   message_res
        )
    {
        typedef typename _Ty_Contr_Test::value_type _Ty_Val   ;
        typedef typename _Ty_Contr_Test::iterator   _Iter     ;
        typedef typename _Iter::difference_type     _DistType ;

        const size_t    sz_test = vec_value . size ( ) ; 

        //  insert sz_test/4 elements 
        {
            typedef typename _Ty_Vect_Values::const_iterator    _Iter_Value ; 
            typedef typename _Ty_Vect_Values::difference_type   _Dist_Value ; 
            _Iter_Value     iter_4 = vec_value.begin() + static_cast<_Dist_Value>(sz_test/4) ; 
            _Ty_Contr_Test  contr_temp ( vec_value.begin() , iter_4 ) ; 
            contr_test = contr_temp ; 
        }


        _Ty_Val         value = _Ty_Val() ; 
        _DistType       dist  = 0 ;
        _Iter           iter ; 

        TimerChrono     timer ;
        timer . Start ( ) ;

        for ( size_t    i = sz_test/4 ; i < sz_test ; ++i )
        {
            //  in this test ( i == contr_test.size() ) ; 
            dist  = static_cast<_DistType>( vec_dist[i] % i ) ;
            value = vec_value [ i ] ; 
            iter  = contr_test . begin ( ) ; 
            std::advance ( iter , dist ) ; 
            contr_test . insert ( iter , value ) ; 
        } 

        timer . Stop ( ) ;

        std::string     comment ( "advance_insert();" ) ; 
        std::string     str_timer ; 
        timer . TimeToString ( comment , str_timer ) ; 
        message_res += str_timer ;
    }


    //  the test of sequential processing, 
    //  its running time depends on locality 
    //  of reference of input container 
    template < class _Ty_Contr , class _Ty_Val >
    void std_accumulate 
        (
            _Ty_Contr const &   contr_test  , 
            _Ty_Val &           accum_res   ,
            std::string &       message_res        
        )
    {
        typename
        _Ty_Contr::const_iterator   
                        it_cur = contr_test . begin ( ) ,
                        it_end = contr_test . end   ( ) ;
        _Ty_Val         res    = _Ty_Val() ;
        size_t          n_runs = 10 ;


        TimerChrono     timer ;
        timer . Start ( ) ;

        for ( size_t    i = 0 ; i < n_runs ; ++i )
        {
            res += std::accumulate ( it_cur , it_end , res ) ;
        }

        timer . Stop ( ) ;

        std::string     comment ( "accumulate();" ) ; 
        std::string     str_timer ; 
        timer . TimeToString ( comment , str_timer ) ; 
        message_res += str_timer ;

        //  to compare results for different containers and 
        //  to avoid compiler optimization effect 
        accum_res += res ; 
    }


    template < class _Ty_Contr_Test , class _Ty_Vect_Values >
    void run_tests 
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            _Ty_Contr_Test &                contr_test ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Contr_Test::value_type 
                                            _Ty_Elem ;
        _Ty_Elem                            accum_res = _Ty_Elem() ;

        advance_insert ( vec_values , vec_dist , contr_test , message_res ) ; 
        std_accumulate ( contr_test , accum_res , message_res ) ; 
    } 


    //
    //  standard containers
    //

    template < class _Ty_Vect_Values >
    void std_vector
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type 
                                            _Ty_Elem ;
        std::vector<_Ty_Elem>               contr_x ; 

        message_res += ("std::vector" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void std_list
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type 
                                            _Ty_Elem ;
        std::list<_Ty_Elem>                 contr_x ; 

        message_res += ("std::list" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void std_deque
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type 
                                            _Ty_Elem ;
        std::deque<_Ty_Elem>                contr_x ; 

        message_res += ("std::deque" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void std_multiset
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type 
                                            _Ty_Elem ;
        std::multiset<_Ty_Elem>             contr_x ; 

        message_res += ("std::multiset" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }


    //
    //  containers using augmented B+ trees 
    //

    template < class _Ty_Vect_Values >
    void bpt_sequence_array
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type _Ty_Elem ;
        std_ext_adv::sequence<_Ty_Elem>              contr_x  ; 

        message_res += ("std_ext_adv::sequence_bptree_array" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }


    template < class _Ty_Vect_Values >
    void bpt_multiset_array
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type _Ty_Elem ;
        std_ext_adv::multiset<_Ty_Elem>              contr_x  ; 

        message_res += ("std_ext_adv::multiset_bptree_array" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    //  test of dynamically allocated B+ tree "bp_tree_idx.hpp"
//  template < class _Ty_Vect_Values >
//  void bpt_sequence_list
//      (
//          _Ty_Vect_Values const &         vec_values , 
//          std::vector<size_t> const &     vec_dist   , 
//          std::string const &             elem_type  ,
//          std::string &                   message_res 
//      )
//  {
//      typedef typename _Ty_Vect_Values::value_type 
//                                          _Ty_Elem ;
//      std_ext_adv::sequence<_Ty_Elem, std::allocator<_Ty_Elem>, 
//                                      std_ext_adv::bp_tree_idx>   
//                                          contr_x ; 
//
//      message_res += ("std_ext_adv::sequence_bptree_list" + elem_type + ":\n") ; 
//      run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
//  }


    //
    //  Boost containers 
    //

    template < class _Ty_Vect_Values >
    void boost_vector
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type    _Ty_Elem ;
        boost::container::vector<_Ty_Elem>              contr_x ; 

        message_res += ("boost::vector" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void boost_list
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type    _Ty_Elem ;
        boost::container::list<_Ty_Elem>                contr_x ; 

        message_res += ("boost::list" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void boost_deque
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type    _Ty_Elem ;
        boost::container::deque<_Ty_Elem>               contr_x ; 

        message_res += ("boost::deque" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void boost_multiset
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type    _Ty_Elem ;
        boost::container::multiset<_Ty_Elem>            contr_x  ; 

        message_res += ("boost::multiset" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void boost_stable_vector
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type    _Ty_Elem ;
        boost::container::stable_vector<_Ty_Elem>       contr_x  ; 

        message_res += ("boost::stable_vector" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void boost_flat_multiset
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type    _Ty_Elem ;
        boost::container::flat_multiset<_Ty_Elem>       contr_x  ; 

        message_res += ("boost::flat_multiset" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }


    //
    //  Boost.MultiIndex containers 
    //

    using namespace boost ; 
    using namespace boost::multi_index ;

    template < class _Ty_Vect_Values >
    void boost_m_idx_multiset 
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type 
                                            _Ty_Elem ;
        multi_index_container<_Ty_Elem, indexed_by<ordered_non_unique<identity<_Ty_Elem> > > > 
                                            contr_x ;  

        message_res += ("boost::m_idx::multiset" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    //  class adaptor to emulate std::list, 
    //  for more details see Boost.MultiIndex documentation
    template < typename T > 
    struct mutable_value 
    {
        mutable_value ( const T &  _t ) : t(_t) {  } 
        operator T&() const { return t; }
    private:
        mutable T   t ; 
    } ;

    template < class _Ty_Vect_Values >
    void boost_m_idx_list 
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type 
                                            _Ty_Elem ;
    //  this container can be tested too
        multi_index_container<_Ty_Elem, indexed_by<sequenced< > > >
    //  multi_index_container<mutable_value<_Ty_Elem>, indexed_by<sequenced< > > >
                                            contr_x ;  

        message_res += ("boost::m_idx::list" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

    template < class _Ty_Vect_Values >
    void boost_m_idx_random_access 
        (
            _Ty_Vect_Values const &         vec_values , 
            std::vector<size_t> const &     vec_dist   , 
            std::string const &             elem_type  ,
            std::string &                   message_res 
        )
    {
        typedef typename _Ty_Vect_Values::value_type 
                                            _Ty_Elem ;
    //  this container can be tested too
        multi_index_container<_Ty_Elem, indexed_by<random_access< > > >
    //  multi_index_container<mutable_value<_Ty_Elem>, indexed_by<random_access< > > >
                                            contr_x ;  

        message_res += ("boost::m_idx::random_access" + elem_type + ":\n") ; 
        run_tests ( vec_values , vec_dist , contr_x , message_res ) ;
    }

}   //  namespace test_performance ;



namespace test_performance 
{

    template < class _Ty_Elem > 
    class TestContainers
    {
    public:

        TestContainers ( std::string const &  el_type ) : 
            elem_type(el_type) , values() , dist_data() {  } 

        void InitializeData ( const size_t  sz_test ) 
        {
            //  test data are updated only when user changes 
            //  the number of elements in a container to test
            if ( sz_test != values.size() ) 
            {
                values . clear ( ) ; 
                for ( size_t  i = 0 ; i < sz_test ; ++i )
                    values . push_back ( static_cast<_Ty_Elem>(i) ) ;
                std::random_shuffle ( values.begin() , values.end() ) ;

                dist_data . clear ( ) ; 
                test_std_ext_adv::fill_rand ( dist_data , sz_test , 1 , 1 ) ;
            }
        } 

        //  standard containers
        void std_vector          ( std::string &  test_res ) { test_performance::std_vector          ( values , dist_data , elem_type , test_res ) ; } 
        void std_list            ( std::string &  test_res ) { test_performance::std_list            ( values , dist_data , elem_type , test_res ) ; } 
        void std_deque           ( std::string &  test_res ) { test_performance::std_deque           ( values , dist_data , elem_type , test_res ) ; } 
        void std_multiset        ( std::string &  test_res ) { test_performance::std_multiset        ( values , dist_data , elem_type , test_res ) ; } 

        //  containers using augmented B+ trees 
        void bpt_sequence_array  ( std::string &  test_res ) { test_performance::bpt_sequence_array  ( values , dist_data , elem_type , test_res ) ; }
        void bpt_multiset_array  ( std::string &  test_res ) { test_performance::bpt_multiset_array  ( values , dist_data , elem_type , test_res ) ; }
    //  void bpt_sequence_list   ( std::string &  test_res ) { test_performance::bpt_sequence_list   ( values , dist_data , elem_type , test_res ) ; } 

        //  Boost containers  
        void boost_vector        ( std::string &  test_res ) { test_performance::boost_vector        ( values , dist_data , elem_type , test_res ) ; }
        void boost_list          ( std::string &  test_res ) { test_performance::boost_list          ( values , dist_data , elem_type , test_res ) ; }
        void boost_deque         ( std::string &  test_res ) { test_performance::boost_deque         ( values , dist_data , elem_type , test_res ) ; }
        void boost_multiset      ( std::string &  test_res ) { test_performance::boost_multiset      ( values , dist_data , elem_type , test_res ) ; }
        void boost_stable_vector ( std::string &  test_res ) { test_performance::boost_stable_vector ( values , dist_data , elem_type , test_res ) ; }
        void boost_flat_multiset ( std::string &  test_res ) { test_performance::boost_flat_multiset ( values , dist_data , elem_type , test_res ) ; }

        //  Boost.MultiIndex containers 
        void boost_m_idx_multiset     ( std::string &  test_res ) { test_performance::boost_m_idx_multiset     ( values , dist_data , elem_type , test_res ) ; }
        void boost_m_idx_list         ( std::string &  test_res ) { test_performance::boost_m_idx_list         ( values , dist_data , elem_type , test_res ) ; }
        void boost_m_idx_random_access( std::string &  test_res ) { test_performance::boost_m_idx_random_access( values , dist_data , elem_type , test_res ) ; }

    private: 
        std::string             elem_type ; 
        std::vector<_Ty_Elem>   values    ;
        std::vector<size_t>     dist_data ;
    } ; 


    //  this function can be used to test 
    //  performance of a specific container 
    //  that support STL interfaces
    template < class _Ty_Elem > 
    void TestSelectedContainerType
        ( 
            std::string const & elem_type   ,  
            const size_t        sel_contr   ,
            const size_t        sz_test     ,  
            std::string &       str_test_res 
        )
    {
        static TestContainers<_Ty_Elem>     test ( elem_type ) ; 

        //  the test data are updated only when user changes 
        //  the number of elements to test
        test . InitializeData ( sz_test ) ; 

        //  standard containers
        if ( sel_contr== 0 ) test.std_vector          ( str_test_res ) ;
        if ( sel_contr== 1 ) test.std_list            ( str_test_res ) ;
        if ( sel_contr== 2 ) test.std_deque           ( str_test_res ) ;
        if ( sel_contr== 3 ) test.std_multiset        ( str_test_res ) ;
        //  containers using augmented B+ trees 
        if ( sel_contr== 4 ) test.bpt_sequence_array  ( str_test_res ) ;
        if ( sel_contr== 5 ) test.bpt_multiset_array  ( str_test_res ) ;
    //  if ( sel_contr== 6 ) test.bpt_sequence_list   ( str_test_res ) ;
        //  Boost containers 
        if ( sel_contr== 7 ) test.boost_vector        ( str_test_res ) ;
        if ( sel_contr== 8 ) test.boost_list          ( str_test_res ) ;
        if ( sel_contr== 9 ) test.boost_deque         ( str_test_res ) ;
        if ( sel_contr==10 ) test.boost_multiset      ( str_test_res ) ;
        if ( sel_contr==11 ) test.boost_stable_vector ( str_test_res ) ;
        if ( sel_contr==12 ) test.boost_flat_multiset ( str_test_res ) ;
    }
//  
//  example code using function TestSelectedContainerType() 
//  in C++ project
//  
//  int  main ( )
//  {
//      std::string         str_test ; 
//      const size_t        selContr = 0 ; 
//      const size_t        selSize  = 1000 ; 
//  
//      std::cout << "performance test for N=" << selSize 
//                << " elements;" << std::endl << std::endl ; 
//  
//      test_performance::TestSelectedContainerType<size_t> ( "<size_t>" , selContr , selSize , str_test ) ; 
//      test_performance::TestSelectedContainerType<double> ( "<double>" , selContr , selSize , str_test ) ; 
//      std::cout << str_test << std::endl ; 
//  
//      return 0 ; 
//  }
//  
//  =====================================================================
//  
//  example code using function TestSelectedContainerType() 
//  in Android NDK project 
//
//  JNIEXPORT jstring JNICALL Java_com_example_performancendk_PerformanceApp_TestContainer 
//    (JNIEnv*   env, jobject  thiz, jint  selContr, jint selSize, jint selElemType) 
//  {
//      std::string     str_test ;
//
//      if      ( selElemType == 0 ) { test_performance::TestSelectedContainerType<size_t> ( "<size_t>" , selContr , selSize , str_test ) ; }
//      else if ( selElemType == 1 ) { test_performance::TestSelectedContainerType<double> ( "<double>" , selContr , selSize , str_test ) ; }
//      else if ( selElemType == 2 ) { test_performance::TestSelectedContainerType<float>  ( "<float>"  , selContr , selSize , str_test ) ; }
//      else if ( selElemType == 3 ) { test_performance::TestSelectedContainerType<long>   ( "<long>"   , selContr , selSize , str_test ) ; }
//      else
//      {
//          str_test = "error: invalid type of element !" ;
//      } 
//  
//      return env->NewStringUTF( str_test.c_str( ) ) ;
//  } 
//  
//  =====================================================================
//  


}   //  namespace test_performance ;


#endif  //  _PERFORM_TESTS_HPP
