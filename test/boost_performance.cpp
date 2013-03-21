/////////////////////////////////////////////////////////////////
//
//          Copyright Vadim Stadnik 2012-2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
/////////////////////////////////////////////////////////////////
//
//  See folder "../doc" for documentation
//
/////////////////////////////////////////////////////////////////


#include "perform_tests.hpp" 


int  main ( )
{
    const size_t    n_test = 1000 ; 
    std::string     str_test ; 

    std::cout << "performance test for N=" << n_test 
              << " elements;" << std::endl << std::endl ; 


    test_performance::TestContainers<size_t>     test ( "<size_t>" ) ; 

    //  the same set of data is used to test every container
    test . InitializeData ( n_test ) ; 

    //  standard containers
    test . std_vector   ( str_test ) ; 
    test . std_list     ( str_test ) ; 
    test . std_deque    ( str_test ) ; 
    test . std_multiset ( str_test ) ; 

    //  containers using augmented B+ trees 
    test . bpt_sequence_array ( str_test ) ; 
    test . bpt_multiset_array ( str_test ) ;
//  test . bpt_sequence_list  ( str_test ) ; 

    //  Boost containers 
    test . boost_vector        ( str_test ) ; 
    test . boost_list          ( str_test ) ; 
    test . boost_deque         ( str_test ) ; 
    test . boost_multiset      ( str_test ) ; 
    test . boost_stable_vector ( str_test ) ; 
    test . boost_flat_multiset ( str_test ) ; 

    //  Boost.MultiIndex containers 
    test . boost_m_idx_multiset     ( str_test ) ; 
    test . boost_m_idx_list         ( str_test ) ; 
    test . boost_m_idx_random_access( str_test ) ; 

    std::cout << str_test << std::endl ; 
    return 0 ; 
}
