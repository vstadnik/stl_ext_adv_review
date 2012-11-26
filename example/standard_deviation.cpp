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


#include <assert.h>
#include <math.h>
#include <vector>
#include <numeric>
#include <iostream>
#include "bp_tree_array_acc.hpp"
#include "bpt_sequence.hpp"


namespace BPTreeDemo
{
    using namespace std_ext_adv ;

    //  class ElementValues has been developed to demonstrate
    //  how to use fast algorithm bp_tree_..._acc::accumulate(),
    //  which has logarithmic running time,
    //  in order to calculate the mean value and standard deviation
    //  of consecutive elements ;
    //
    //  class ElementValues stores both a value
    //  of an element and its squared value ;
    //  the squared value is calculated and updated automatically ;
    class ElementValues
    {
    public:
        explicit
        ElementValues ( int v=0 ) : value(v) , value_squared(v*v) {  }

        int     Value ( )        const { return value ;         }
        int     ValueSquared ( ) const { return value_squared ; }

        ElementValues &  operator += ( const ElementValues &  x )
        {
            value         += x.value ;
            value_squared += x.value_squared ;
            return *this ;
        }

        ElementValues &  operator -= ( const ElementValues &  x )
        {
            value         -= x.value ;
            value_squared -= x.value_squared ;
            return *this ;
        }

        ElementValues    operator +  ( const ElementValues &  x )
        {
            ElementValues   res(*this) ;
            res += x ;
            return res ;
        }

        ElementValues    operator -  ( const ElementValues &  x )
        {
            ElementValues   res(*this) ;
            res -= x ;
            return res ;
        }

    private:
        int     value ;
        int     value_squared ;
    } ;


    typedef std::vector<ElementValues>      VectorElemValues ;
    //  sequence with fast algorithm accumulate()
    typedef sequence<ElementValues,
                std::allocator<ElementValues>,
                bp_tree_array_acc>          SequenceElemValues ;


    void TestDeviation ( )
    {
        //  for 32-bit integer safe size to run
        //  this test without overflow errors <= 1,000,000 ;
        const int           sz = 100000 ;
        VectorElemValues    vect_values ;
        {
            //  insert random values in the range [ mean-devn , mean+devn ]
            const int   mean = 20 ;
            const int   devn = 10 ;
            for ( int   i = 0 ; i < sz ; ++i )
            {
                int             val = mean - devn + rand()%(2*devn+1) ;
                ElementValues   el_val ( val ) ;
                vect_values . push_back ( el_val ) ;
            }
        }

        //  exact values of sums will be compared with results of two
        //  algorithms: std::accumulate() and bp_tree_..._acc::accumulate()
        int                 sum_x_exact   = 0 ;
        int                 sum_x_2_exact = 0 ;
        int                 sum_x   = 0 ;
        int                 sum_x_2 = 0 ;
        const ElementValues sum_elem_values_0 ;
        ElementValues       sum_elem_values   ;
        const int           dist_a = sz/4 ;
        const int           dist_b = sz/4 + sz/2 ;

        //  calculate exact values of sums
        {
            for ( int   i = dist_a ; i < dist_b ; ++i )
            {
                ElementValues const &
                        elem_values = vect_values [ i ] ;
                int     x   = elem_values.Value() ;
                int     x_2 = x*x ;

                sum_x_exact   += x ;
                sum_x_2_exact += x_2 ;
            }
        }

        //  compare exact values of sums with results
        //  of method std::accumulate() applied to data
        //  stored in std::vector<ElementValues>
        {
            VectorElemValues::const_iterator    pos_a = vect_values.begin() + dist_a ;
            VectorElemValues::const_iterator    pos_b = vect_values.begin() + dist_b ;

            sum_elem_values = std::accumulate ( pos_a , pos_b , sum_elem_values_0 ) ;
            sum_x           = sum_elem_values . Value ( ) ;
            sum_x_2         = sum_elem_values . ValueSquared ( ) ;
            assert ( ( sum_x  ==sum_x_exact   ) &&
                     ( sum_x_2==sum_x_2_exact )    ) ;
            assert ( ( dist_b-dist_a ) == ( pos_b-pos_a ) ) ;
        }

        //  construct sequence with fast algorithm accumulate()
        //  and compare exact values of sums with results
        //  obtained using method std::accumulate()
        //  and fast algorithm bp_tree_..._acc::accumulate()
        SequenceElemValues  seqce_values ( vect_values.begin() , vect_values.end() ) ;
        {
            SequenceElemValues::const_iterator  pos_a = seqce_values.begin() + dist_a ;
            SequenceElemValues::const_iterator  pos_b = seqce_values.begin() + dist_b ;

            sum_elem_values = std::accumulate ( pos_a , pos_b , sum_elem_values_0 ) ;
            sum_x           = sum_elem_values . Value ( ) ;
            sum_x_2         = sum_elem_values . ValueSquared ( ) ;
            assert ( ( sum_x  ==sum_x_exact   ) &&
                     ( sum_x_2==sum_x_2_exact )    ) ;

            sum_elem_values = seqce_values . accumulate ( pos_a , pos_b , sum_elem_values_0 ) ;
            sum_x           = sum_elem_values . Value ( ) ;
            sum_x_2         = sum_elem_values . ValueSquared ( ) ;
            assert ( ( sum_x  ==sum_x_exact   ) &&
                     ( sum_x_2==sum_x_2_exact )    ) ;
            assert ( ( dist_b-dist_a ) == ( pos_b-pos_a ) ) ;
        }

        //  calculate mean value and standard deviation
        double      n     = static_cast<double>( dist_b - dist_a ) ;
        double      sum   = static_cast<double>( sum_x ) ;
        double      sum_2 = static_cast<double>( sum_x_2 ) ;
        double      mean  = sum / n ;
        double      variance  = sum_2/n - mean*mean ;
        // unbiased_variance  = variance*n/(n-1) ;
        double      deviation = sqrt ( variance ) ;

        std::cout << "example of calculation of mean and standard deviation:" << std::endl
                  << "    n         = " << n         << " ; " << std::endl
                  << "    mean      = " << mean      << " ; " << std::endl
                  << "    deviation = " << deviation << " ; " << std::endl ;
    }

}


int main ( )
{
    BPTreeDemo::TestDeviation ( ) ;
    return 0 ;
}











