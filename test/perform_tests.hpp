/////////////////////////////////////////////////////////////////
//
//          Copyright Vadim Stadnik 2011-2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
/////////////////////////////////////////////////////////////////
//
//  See folder "../doc" for documentation
//
/////////////////////////////////////////////////////////////////


//#pragma once

#include <vector>
#include <list>
#include <set>
#include <numeric>

#include "timers.hpp"
#include "test_helpers.hpp"
#include "bp_tree_array.hpp"
#include "bp_tree_array_acc.hpp"
#include "bpt_sequence.hpp"
#include "bpt_set.hpp"
#include "bpt_map.hpp"


//  example how to use the tests:
//
//      int                 sz_test = 100000 ;
//      std::vector<int>    vector_test ;
//      test_performance::test_push_back ( sz_test , vector_test ) ;
//
namespace test_performance
{

    template < class _Ty_Seqce >
    void test_push_back_fill
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
        )
    {
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , 1 , 1 ) ;

        seqce_test . clear ( ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        for (    ; it_cur != it_end ; ++it_cur )
        {
            seqce_test . push_back ( *it_cur ) ;
        }

        timer . Stop ( ) ;
        timer . PrintTime ( "push_back_fill" ) ;
    }


    template < class _Ty_Seqce >
    void test_insert_fill
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
        )
    {
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , 1 , 1 ) ;

        seqce_test . clear ( ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;

        timer . Stop ( ) ;
        timer . PrintTime ( "insert_fill" ) ;
    }


    template < class _Ty_Seqce >
    void test_clear 
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
        )
    {
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , 1 , 1 ) ;

        seqce_test . clear ( ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        seqce_test . clear ( ) ;

        timer . Stop ( ) ;
        timer . PrintTime ( "test clear" ) ;
    }


    template < class _Ty_Seqce >
    void test_sort
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
        )
    {
        std::vector<size_t>     vec_szt ;
        test_std_ext_adv::fill_rand ( vec_szt , sz_test , 1 , 1 ) ;

        std::vector<size_t>::const_iterator     it_cur = vec_szt . begin ( ) ;
        std::vector<size_t>::const_iterator     it_end = vec_szt . end   ( ) ;

        seqce_test . clear ( ) ;
        seqce_test . insert ( seqce_test.end() , it_cur , it_end ) ;

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        seqce_test . sort( ) ;
        //  for array based B+ tree class bp_tree_array only:
    //  seqce_test . sort_tree( ) ;
    //  seqce_test . sort_vector( ) ;

        timer . Stop ( ) ;
        timer . PrintTime ( "sort" ) ;
    }


    template < class _Ty_Seqce >
    void test_unique
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
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

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        seqce_test . unique ( ) ;

        timer . Stop ( ) ;
        timer . PrintTime ( "unique" ) ;
    }


    template < class _Ty_Seqce >
    void test_reverse
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
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

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        seqce_test . reverse ( ) ;

        timer . Stop ( ) ;
        timer . PrintTime ( "reverse" ) ;
    }


    template < class _Ty_Seqce >
    void test_remove_if
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
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
        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        seqce_test . remove_if ( pr_even ) ;

        timer . Stop ( ) ;
        timer . PrintTime ( "remove_if" ) ;
    }


    template < class _Ty_Seqce >
    void test_push_back
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
        )
    {
        seqce_test . clear ( ) ;

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        for ( size_t  i = 0 ; i < sz_test ; ++i )
        {
            seqce_test . push_back ( i ) ;
        }

        timer . Stop ( ) ;
        timer . PrintTime ( "push_back" ) ;
    }


    template < class _Ty_Seqce >
    void test_insert_mid
        (
            const size_t        sz_test     ,
            _Ty_Seqce &         seqce_test
        )
    {
        seqce_test . clear ( ) ;

        _Ty_Seqce::iterator     it_mid = seqce_test . begin ( ) ;
        size_t                  count  = 0 ;

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        for ( size_t  i = 0 ; i < sz_test ; ++i )
        {
            if ( i % 2 )
                ++it_mid ;

            it_mid = seqce_test . insert ( it_mid , i ) ;
        }

        timer . Stop ( ) ;
        timer . PrintTime ( "insert mid" ) ;
    }


    template < class _Ty_M_Set >
    void test_insert_m_set
        (
            const size_t        sz_test     ,
            const bool          fill_rand   ,
            _Ty_M_Set &         m_set_test
        )
    {
        m_set_test . clear ( ) ;

        Testing::TimerHighRes   timer  ;
        timer . Start ( ) ;

        for ( size_t  i = 0 ; i < sz_test ; ++i )
        {
            if ( fill_rand )
                m_set_test . insert ( rand() ) ;
            else
                m_set_test . insert ( i ) ;
        }

        timer . Stop ( ) ;
        if ( fill_rand )
            timer . PrintTime ( "insert multiset random" ) ;
        else
            timer . PrintTime ( "insert multiset ordered" ) ;
    }


    //  test of insert before a position near
    //  the middle of the input container ;
    //  make sure that input container has element of
    //  signed integer type, such as std::multiset<int>
    template < class _Ty_M_Set >
    void test_insert_mid_m_set
        (
            const size_t        sz_test     ,
            _Ty_M_Set &         m_set_test
        )
    {
        m_set_test . clear ( ) ;

        int         sz = int ( sz_test ) ;
        Testing::TimerHighRes   timer  ;
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
        timer . PrintTime ( "insert multiset mid posn" ) ;
    }


    template < class _Ty_Contr >
    size_t  test_accumulate_std ( _Ty_Contr const &  contr_test )
    {
        size_t                  res      = 0  ;
        size_t                  n_runs   = 10 ;
        size_t                  sz_contr = contr_test . size ( ) ;
        if ( sz_contr < 100 )
        {
            std::cout << "   " << std::endl ;
            return res ;
        }

        Testing::TimerHighRes   timer  ;
        _Ty_Contr::const_iterator
                                it_cur = contr_test . begin ( ) ,
                                it_end = contr_test . begin ( )   ;

        std::advance ( it_cur , int ( sz_contr / 4 ) ) ;
        std::advance ( it_end , int ( sz_contr / 4 + sz_contr / 2 ) ) ;


        timer . Start ( ) ;

        for ( size_t    i = 0 ; i < n_runs ; ++i )
        {
            res += std::accumulate ( it_cur , it_end , 0 ) ;
        }

        timer . Stop ( ) ;
        timer . PrintTime ( "accumulate std" ) ;

        return res ;
    }


    size_t  test_accumulate_fast ( const size_t  sz_test )
    {
        size_t                  res    = 0    ;
        size_t                  n_runs = 1000 ;

        typedef int                 _T  ;
        typedef std::allocator<_T>  _AT ;
        typedef std_ext_adv::sequence<_T, _AT, std_ext_adv::bp_tree_array_acc>
                                _SEQCE_ACC ;
        _SEQCE_ACC              seqce_test ;

        test_push_back ( sz_test , seqce_test ) ;

        _SEQCE_ACC::const_iterator
                                iter_a = seqce_test . begin ( ) ,
                                iter_b = seqce_test . begin ( ) ;
        iter_a += int ( sz_test / 4 ) ;
        iter_b += int ( sz_test / 4 + sz_test / 2 ) ;

        Testing::TimerHighRes   timer  ;

        timer . Start ( ) ;

        for ( size_t    i = 0 ; i < n_runs ; ++i )
        {
            res += seqce_test . accumulate ( iter_a , iter_b , 0 ) ;
        }

        timer . Stop ( ) ;
        timer . PrintTime ( "accumulate std" ) ;

        return res ;
    }

}   //  namespace test_performance ;











