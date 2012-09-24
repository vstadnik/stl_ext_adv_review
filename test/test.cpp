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


#include <boost/test/minimal.hpp>
#include "test_containers.hpp"


//  for documentation see "Boost Test Library",
//  part III. "The minimal testing facility"
int test_main ( int , char * [ ] )
{
    const size_t    sz_test  = 128 ; // >= 8
    const size_t    num_dupl = 2   ; // >= 2

    test_std_ext_adv::adapters_array    ( sz_test , num_dupl ) ;
    test_std_ext_adv::adapters_array_acc( sz_test , num_dupl ) ;

    return 0 ;
}
