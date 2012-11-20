/////////////////////////////////////////////////////////////////
//
//          Copyright Vadim Stadnik 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
/////////////////////////////////////////////////////////////////
//
//  See folder "../doc" for documentation
//
/////////////////////////////////////////////////////////////////


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

//  std_ext_adv containers
#include "bpt_sequence.hpp"
#include "bpt_set.hpp"
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


//  methods for measurements of performance of containers 
//  that support interfaces of C++ STL containers 
namespace test_performance
{

    std::vector<size_t>     vec_value ;
    std::vector<size_t>     vec_dist  ;


    void initialize_test_data ( const size_t  sz_test ) 
    {
        vec_value . clear ( ) ; 
        vec_dist  . clear ( ) ; 
        test_std_ext_adv::fill_rand ( vec_value , sz_test , 1 , 1 ) ;
        test_std_ext_adv::fill_rand ( vec_dist  , sz_test , 1 , 1 ) ;
    }


    template < class _Ty_Contr >
    void advance_insert
        (
            const size_t            sz_test   ,
            std::string const &     comment   , 
            _Ty_Contr &             contr_test
        )
    {
        typedef typename _Ty_Contr::iterator      _Iter     ;
        typedef typename _Iter::difference_type   _DistType ;

        //  insert sz_test/4 elements 
        {
            std::vector<size_t>::iterator   
                        iter_4 = vec_value.begin() + static_cast<_DistType>(sz_test/4) ; 
            _Ty_Contr   contr_temp ( vec_value.begin() , iter_4 ) ; 
            contr_test = contr_temp ; 
        }

        size_t          value = 0 ; 
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
        timer . PrintTime ( comment ) ;
    }


    //  the test of sequential processing, 
    //  its running time depends on locality 
    //  of reference of input container 
    template < class _Ty_Contr >
    size_t  std_accumulate ( _Ty_Contr const &  contr_test )
    {
        typename
        _Ty_Contr::const_iterator   
                        it_cur = contr_test . begin ( ) ,
                        it_end = contr_test . end   ( ) ;
        size_t          res    = 0  ;
        size_t          n_runs = 10 ;
        std::string     comment ( "accumulate" ) ; 

        TimerChrono     timer ;
        timer . Start ( ) ;

        for ( size_t    i = 0 ; i < n_runs ; ++i )
        {
            res += std::accumulate ( it_cur , it_end , 0 ) ;
        }

        timer . Stop ( ) ;
        timer . PrintTime ( comment ) ;

        //  to compare results for different containers and 
        //  to avoid compiler optimization effect 
        std::cout << res << std::endl << std::endl ; 
        return res ;
    }


    void bpt_sequence_array ( const size_t  sz_test ) 
    {
        std_ext_adv::sequence<size_t>   bpt_seqce_array ; 
        advance_insert ( sz_test , "std_ext_adv::sequence_array" , bpt_seqce_array ) ; 
        std_accumulate ( bpt_seqce_array ) ; 
    }

    void bpt_multiset_array ( const size_t  sz_test ) 
    {
        std_ext_adv::multiset<size_t>   bpt_m_set_array ; 
        advance_insert ( sz_test , "std_ext_adv::multiset_array" , bpt_m_set_array ) ; 
        std_accumulate ( bpt_m_set_array ) ; 
    } 

    //  test of dynamically allocated B+ tree "bp_tree_idx.hpp"
//  void bpt_sequence_list ( const size_t  sz_test ) 
//  {
//      std_ext_adv::sequence<size_t, std::allocator<size_t>,
//                                    std_ext_adv::bp_tree_idx>   
//                                    bpt_seqce_list ; 
//      advance_insert ( sz_test , "std_ext_adv::sequence_list" , bpt_seqce_list ) ; 
//      std_accumulate ( bpt_seqce_list ) ; 
//  }

    void std_vector ( const size_t  sz_test ) 
    {
        std::vector<size_t>     std_vector ; 
        advance_insert ( sz_test , "std::vector" , std_vector ) ; 
        std_accumulate ( std_vector ) ; 
    }

    void std_deque ( const size_t  sz_test ) 
    {
        std::deque<size_t>      std_deque ; 
        advance_insert ( sz_test , "std::deque" , std_deque ) ; 
        std_accumulate ( std_deque ) ; 
    }

    void std_list ( const size_t  sz_test ) 
    {
        std::list<size_t>       std_list ; 
        advance_insert ( sz_test , "std::list" , std_list ) ; 
        std_accumulate ( std_list ) ; 
    }

    void std_multiset ( const size_t  sz_test ) 
    {
        std::multiset<size_t>   std_m_set ; 
        advance_insert ( sz_test , "std::multiset" , std_m_set ) ; 
        std_accumulate ( std_m_set ) ; 
    }

    void boost_vector ( const size_t  sz_test ) 
    {
        boost::container::vector<size_t>    b_vector ;
        advance_insert ( sz_test , "boost::vector" , b_vector ) ; 
        std_accumulate ( b_vector ) ; 
    }

    void boost_deque ( const size_t  sz_test ) 
    {
        boost::container::deque<size_t>     b_deque ;
        advance_insert ( sz_test , "boost::deque" , b_deque ) ; 
        std_accumulate ( b_deque ) ; 
    }

    void boost_list ( const size_t  sz_test ) 
    {
        boost::container::list<size_t>      b_list ;
        advance_insert ( sz_test , "boost::list" , b_list ) ; 
        std_accumulate ( b_list ) ; 
    }

    void boost_multiset ( const size_t  sz_test ) 
    {
        boost::container::multiset<size_t>  b_m_set ; 
        advance_insert ( sz_test , "boost::multiset" , b_m_set ) ; 
        std_accumulate ( b_m_set ) ; 
    }

    void boost_flat_multiset ( const size_t  sz_test ) 
    {
        boost::container::flat_multiset<size_t>  b_fm_set ; 
        advance_insert ( sz_test , "boost::flat_multiset" , b_fm_set ) ; 
        std_accumulate ( b_fm_set ) ; 
    }

    void boost_stable_vector ( const size_t  sz_test ) 
    {
        boost::container::stable_vector<size_t>  b_stable_vector ; 
        advance_insert ( sz_test , "boost::stable_vector" , b_stable_vector ) ; 
        std_accumulate ( b_stable_vector ) ; 
    }

}   //  namespace test_performance ;


int  main ( int  argc , char*  argv[] )
{
    const size_t    n_test = 1000 ; 

    std::cout << "performance test for N=" << n_test 
              << " elements;" << std::endl << std::endl ; 


    //  use the same set of test data for every container
    test_performance::initialize_test_data ( n_test ) ; 

    test_performance::bpt_sequence_array ( n_test ) ; 
    test_performance::bpt_multiset_array ( n_test ) ;
//  test_performance::bpt_sequence_list  ( n_test ) ; 

    test_performance::std_vector   ( n_test ) ; 
    test_performance::std_deque    ( n_test ) ; 
    test_performance::std_list     ( n_test ) ; 
    test_performance::std_multiset ( n_test ) ; 

    test_performance::boost_vector ( n_test ) ; 
    test_performance::boost_deque  ( n_test ) ; 
    test_performance::boost_list   ( n_test ) ; 
    test_performance::boost_multiset      ( n_test ) ; 
    test_performance::boost_stable_vector ( n_test ) ; 
    test_performance::boost_flat_multiset ( n_test ) ; 

    return 0 ; 
}

