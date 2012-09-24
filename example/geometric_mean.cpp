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
#include <stdexcept>
#include "bp_tree_array_acc.hpp"
#include "bpt_sequence.hpp"


namespace BPTreeDemo
{
    using namespace std_ext_adv ;

    //  class double_mul has been developed to demonstrate
    //  how to use fast algorithm bp_tree_..._acc::accumulate(),
    //  which has logarithmic running time,
    //  in order to calculate the geometric mean value
    //  of any number of consecutive elements ;
    //  arithmetic operators of this class implement
    //  multiplication and division operations ;
    class double_mul
    {
    public:
        explicit
        double_mul ( double d=1.0 ) : value(d) { assert ( value!=0.0 ) ; }

        operator double( ) const { return value ; }

        double_mul &  operator += ( const double_mul  x )
        {
            value *= x.value ;
            return *this ;
        }

        double_mul &  operator -= ( const double_mul  x )
        {
            value /= x.value ;
            return *this ;
        }

        double_mul    operator +  ( const double_mul  x )
        {
            double_mul   res(*this) ;
            res += x ;
            return res ;
        }

        double_mul    operator -  ( const double_mul  x )
        {
            double_mul   res(*this) ;
            res -= x ;
            return res ;
        }

    private:
        double  value ;
    } ;

    typedef std::vector<double>        VectorDouble    ;
    typedef std::vector<double_mul>    VectorDoubleMul ;
    //  sequence with fast algorithm accumulate()
    typedef sequence<double_mul,
                std::allocator<double_mul>,
                bp_tree_array_acc>     SequenceDoubleMul ;


    void GeometricMean ( )
    {
        VectorDouble        vect_dbl   ;
        VectorDoubleMul     vect_dbl_m ;

        //  add data for a very simple test of correctness
        //  using a small value of size
        //
        //    const int           sz = 9 ;
        //    {
        //        for ( int       i = 0 ; i < sz ; ++i )
        //        {
        //            double      val = pow ( 2.0 , i ) ;
        //            double_mul  val_m ( val ) ;
        //            vect_dbl   . push_back ( val   ) ;
        //            vect_dbl_m . push_back ( val_m ) ;
        //        }
        //    }
        //  ==============================================

        //  add data for thorough test ,
        //  this limit is defined by the precision of type double,
        //  too big value of sz can cause overflow of product values
        const int           sz = 10000  ;
        {
            //  insert values in the range [ mean-devn , mean+devn ]
            //  using the sine function
            const double    mean = 1.15 ;
            const double    devn = 0.25 ;
            const double    pi   = 3.1415926535897931 ;
            for ( int       i = 0 ; i < sz ; ++i )
            {
                double      x   = double(i) / double(sz) ;
                double      val = mean - devn*sin( 2.0*pi*x ) ;
                double_mul  val_m ( val ) ;

                vect_dbl   . push_back ( val   ) ;
                vect_dbl_m . push_back ( val_m ) ;
            }
        }


        //  result values are calculated using 3 methods:
        //  direct multiplication,
        //  algorithm std::accumulate() and
        //  fast algorithm bp_tree_..._acc::accumulate()
        double              res_exact = 1.0 ;
        double              res_std   = 1.0 ;
        double              res_fast  = 1.0 ;

        const double_mul    product_m_init ;
        double_mul          product_m_std  ;
        double_mul          product_m_fast ;

        const int           dist_a = sz/4 ;
        const int           dist_b = sz - sz/4 ;

        //  calculate value of res_exact
        //  using direct multiplication
        {
            for ( int   i = dist_a ; i < dist_b ; ++i )
            {
                double      x = vect_dbl [ i ] ;
                res_exact *= x ;
            }
        }

        //  calculate value of res_std
        //  using linear algorithm std::accumulate()
        {
            VectorDoubleMul::const_iterator     pos_a = vect_dbl_m.begin() + dist_a ;
            VectorDoubleMul::const_iterator     pos_b = vect_dbl_m.begin() + dist_b ;

            //  note that 4th parameter of a binary operation is not required,
            //  since class double_mul overloads arithmetic operators,
            //  including the required operator+
            product_m_std = std::accumulate ( pos_a , pos_b , product_m_init ) ;
            res_std       = product_m_std ;
        }

        //  calculate value of res_fast
        //  using fast algorithm bp_tree_..._acc::accumulate()
        {
            //  construct sequence with fast algorithm accumulate()
            SequenceDoubleMul                   seqce_dlb_m ( vect_dbl_m.begin() , vect_dbl_m.end() ) ;
            SequenceDoubleMul::const_iterator   pos_a = seqce_dlb_m.begin() + dist_a ;
            SequenceDoubleMul::const_iterator   pos_b = seqce_dlb_m.begin() + dist_b ;

            product_m_fast = seqce_dlb_m . accumulate ( pos_a , pos_b , product_m_init ) ;
            res_fast       = product_m_fast ;
            assert ( ( dist_b-dist_a ) == ( pos_b-pos_a ) ) ;
        }

        double      res_error = fabs( res_fast - res_exact ) / res_exact ;
        double      power = 1.0 / ( dist_b-dist_a ) ;
        double      mean_geom = pow ( res_fast , power ) ;

        std::cout.precision(15) ;
        std::cout << "example of calculation of geometric mean value:" << std::endl
                  << "    res_exact = " << res_exact << " ; " << std::endl
                  << "    res_std   = " << res_std   << " ; " << std::endl
                  << "    res_fast  = " << res_fast  << " ; " << std::endl
                  << "    res_error = " << res_error << " ; " << std::endl
                  << "    mean_geom = " << mean_geom << " ; " << std::endl ;
    }
}


int main ( )
{
    BPTreeDemo::GeometricMean ( ) ;
    return 0 ;
}










