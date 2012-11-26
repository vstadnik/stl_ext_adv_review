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


    //  class WeightedValue has been developed to demonstrate
    //  how to use fast algorithm bp_tree_..._acc::accumulate(),
    //  which has logarithmic running time,
    //  in order to calculate weighted mean value ;
    //
    //  class WeightedValue contains three data members ;
    //  value of an element and its weight must be provided
    //  by a user algorithm, contribution of the value
    //  is calculated automatically using both the value and its weight ;
    //
    class WeightedValue
    {
    public:
        explicit
        WeightedValue ( int v=0 , int w=0 ) :
            value(v), weight(w), contribution(v*w) {  }

        int     Value  ( ) const      { return value  ; }
        int     Weight ( ) const      { return weight ; }
        int     Contribution( ) const { return contribution ; }

        WeightedValue &  operator += ( const WeightedValue &  x )
        {
            value        += x.value ;
            weight       += x.weight ;
            contribution += x.contribution ;
            return *this ;
        }

        WeightedValue &  operator -= ( const WeightedValue &  x )
        {
            value        -= x.value ;
            weight       -= x.weight ;
            contribution -= x.contribution ;
            return *this ;
        }

        WeightedValue    operator +  ( const WeightedValue &  x )
        {
            WeightedValue   res(*this) ;
            res += x ;
            return res ;
        }

        WeightedValue    operator -  ( const WeightedValue &  x )
        {
            WeightedValue   res(*this) ;
            res -= x ;
            return res ;
        }

    private:
        int     value  ;
        int     weight ;
        //  contribution of value = value*weight ;
        int     contribution ;
    } ;


    typedef std::vector<WeightedValue>      VectorWValue ;
    //  sequence with fast algorithm accumulate()
    typedef sequence<WeightedValue,
                std::allocator<WeightedValue>,
                bp_tree_array_acc>          SequenceWValue ;


    void TestWeightedMean ( )
    {
        //  for 32-bit integer safe size to run
        //  this test without overflow errors <= 1,000,000 ;
        const int           sz = 100000 ;
        VectorWValue        vect_w_value ;
        {
            //  insert random values in the range [ mean-devn , mean+devn ] ,
            //  weight varies in the range [ 0 , w_max )
            const int   mean  = 20 ;
            const int   devn  = 10 ;
            const int   w_max = 10 ;
            for ( int   i = 0 ; i < sz ; ++i )
            {
                int             val = mean - devn + rand()%(2*devn+1) ;
                int             wgt = rand() % w_max ;
                WeightedValue   w_val ( val , wgt ) ;
                vect_w_value . push_back ( w_val ) ;
            }
        }

        //  exact values of sums will be compared with results of two
        //  algorithms: std::accumulate() and bp_tree_..._acc::accumulate()
        int                 sum_value_exact  = 0 ;
        int                 sum_weight_exact = 0 ;
        int                 sum_contribution_exact = 0 ;
        int                 sum_value  = 0 ;
        int                 sum_weight = 0 ;
        int                 sum_contribution = 0 ;
        const WeightedValue sum_w_value_0 ;
        WeightedValue       sum_w_value   ;
        const int           dist_a = sz/4 ;
        const int           dist_b = sz/4 + sz/2 ;

        //  calculate exact values of sums
        {
            for ( int   i = dist_a ; i < dist_b ; ++i )
            {
                WeightedValue const &
                        w_val = vect_w_value [ i ] ;
                int     val   = w_val.Value()  ;
                int     wgt   = w_val.Weight() ;
                int     cbn   = w_val.Contribution() ;
                assert ( cbn == val*wgt ) ;

                sum_value_exact  += val ;
                sum_weight_exact += wgt ;
                sum_contribution_exact += cbn ;
            }
        }

        //  compare exact values of sums with results
        //  of method std::accumulate() applied to data
        //  stored in std::vector<WeightedValue>
        {
            VectorWValue::const_iterator    pos_a = vect_w_value.begin() + dist_a ;
            VectorWValue::const_iterator    pos_b = vect_w_value.begin() + dist_b ;

            sum_w_value      = std::accumulate ( pos_a , pos_b , sum_w_value_0 ) ;
            sum_value        = sum_w_value . Value ( ) ;
            sum_weight       = sum_w_value . Weight( ) ;
            sum_contribution = sum_w_value . Contribution ( ) ;
            assert ( ( sum_value ==sum_value_exact  ) &&
                     ( sum_weight==sum_weight_exact ) &&
                     ( sum_contribution==sum_contribution_exact ) ) ;
            assert ( ( dist_b-dist_a ) == ( pos_b-pos_a ) ) ;
        }

        //  construct sequence with fast algorithm accumulate()
        //  and compare exact values of sums with results
        //  obtained using method std::accumulate()
        //  and fast algorithm bp_tree_..._acc::accumulate()
        SequenceWValue      seqce_w_value ( vect_w_value.begin() , vect_w_value.end() ) ;
        {
            SequenceWValue::const_iterator  pos_a = seqce_w_value.begin() + dist_a ;
            SequenceWValue::const_iterator  pos_b = seqce_w_value.begin() + dist_b ;

            sum_w_value      = std::accumulate ( pos_a , pos_b , sum_w_value_0 ) ;
            sum_value        = sum_w_value . Value ( ) ;
            sum_weight       = sum_w_value . Weight( ) ;
            sum_contribution = sum_w_value . Contribution ( ) ;
            assert ( ( sum_value ==sum_value_exact  ) &&
                     ( sum_weight==sum_weight_exact ) &&
                     ( sum_contribution==sum_contribution_exact ) ) ;

            sum_w_value      = seqce_w_value . accumulate ( pos_a , pos_b , sum_w_value_0 ) ;
            sum_value        = sum_w_value . Value ( ) ;
            sum_weight       = sum_w_value . Weight( ) ;
            sum_contribution = sum_w_value . Contribution ( ) ;
            assert ( ( sum_value ==sum_value_exact  ) &&
                     ( sum_weight==sum_weight_exact ) &&
                     ( sum_contribution==sum_contribution_exact ) ) ;
            assert ( ( dist_b-dist_a ) == ( pos_b-pos_a ) ) ;
        }

        //  calculate arithmetic and weighted mean
        double      n               = static_cast<double>( dist_b-dist_a ) ;
        double      mean_arithmetic = static_cast<double>( sum_value ) / n ;
        double      mean_weighted   = static_cast<double>( sum_contribution ) /
                                      static_cast<double>( sum_weight ) ;

        std::cout << "example of calculation of weighted mean:" << std::endl
                  << "    n               = " << n               << " ; " << std::endl
                  << "    mean_arithmetic = " << mean_arithmetic << " ; " << std::endl
                  << "    mean_weighted   = " << mean_weighted   << " ; " << std::endl ;
    }

}


int main ( )
{
    BPTreeDemo::TestWeightedMean ( ) ;
    return 0 ;
}










