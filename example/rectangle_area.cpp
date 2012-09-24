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


    //  class RectArea has been developed to demonstrate
    //  how to use fast algorithm bp_tree_..._acc::accumulate(),
    //  which has logarithmic running time,
    //  in order to calculate sums and mean values of
    //  parameters of a sequence of rectangles:
    //  width, length, perimeter and area ;
    //
    //  values of the width and length must be
    //  provided by a user algorithm, the area of
    //  the rectangle is calculated automatically ;
    class RectArea
    {
    public:
        explicit
        RectArea ( int w=0 , int l=0 ) : width(w), length(l), area(w*l) {  }

        int     Width  ( ) const { return width  ; }
        int     Length ( ) const { return length ; }
        int     Area   ( ) const { return area   ; }

        RectArea &  operator += ( const RectArea &  x )
        {
            width  += x.width  ;
            length += x.length ;
            area   += x.area   ;
            return *this ;
        }

        RectArea &  operator -= ( const RectArea &  x )
        {
            width  -= x.width  ;
            length -= x.length ;
            area   -= x.area   ;
            return *this ;
        }

        RectArea    operator +  ( const RectArea &  x )
        {
            RectArea    res(*this) ;
            res += x ;
            return res ;
        }

        RectArea    operator -  ( const RectArea &  x )
        {
            RectArea    res(*this) ;
            res -= x ;
            return res ;
        }

    private:
        int     width  ;
        int     length ;
        int     area   ;
    } ;


    typedef std::vector<RectArea>       VectorRectArea ;
    //  sequence with fast algorithm accumulate()
    typedef sequence<RectArea,
                std::allocator<RectArea>,
                bp_tree_array_acc>      SequenceRectArea ;


    void TestRectArea ( )
    {
        //  for 32-bit integer safe size to run
        //  this test without overflow errors <= 1,000,000 ;
        const int           sz = 100000 ;
        VectorRectArea      vect_rect_area ;
        {
            //  insert random values of parameters of rectangles
            //  in the range [ mean-devn , mean+devn ]
            const int   mean_wid = 10 ;
            const int   mean_len = 20 ;
            const int   devn     = 10 ;
            for ( int   i = 0 ; i < sz ; ++i )
            {
                int     width  = mean_wid - devn + rand()%(2*devn+1) ;
                int     length = mean_len - devn + rand()%(2*devn+1) ;

                RectArea    rect_area ( width , length ) ;
                vect_rect_area . push_back ( rect_area ) ;
            }
        }


        //  exact values of sums will be compared with results of two
        //  algorithms: std::accumulate() and bp_tree_..._acc::accumulate()
        int                 sum_width_exact  = 0 ;
        int                 sum_length_exact = 0 ;
        int                 sum_area_exact   = 0 ;
        int                 sum_width        = 0 ;
        int                 sum_length       = 0 ;
        int                 sum_area         = 0 ;
        const RectArea      sum_rect_area_0  ;
        RectArea            sum_rect_area    ;
        const int           dist_a = sz/4 ;
        const int           dist_b = sz/4 + sz/2 ;

        //  calculate exact values of sums
        {
            for ( int   i = dist_a ; i < dist_b ; ++i )
            {
                RectArea const &
                        rect_area = vect_rect_area [ i ] ;
                int     width     = rect_area . Width ( ) ;
                int     length    = rect_area . Length( ) ;
                int     area      = rect_area . Area  ( ) ;
                assert ( area == width*length ) ;

                sum_width_exact  += width  ;
                sum_length_exact += length ;
                sum_area_exact   += area   ;
            }
        }

        //  compare exact values of sums with results
        //  of method std::accumulate() applied to data
        //  stored in std::vector<RectArea>
        {
            VectorRectArea::const_iterator    pos_a = vect_rect_area.begin() + dist_a ;
            VectorRectArea::const_iterator    pos_b = vect_rect_area.begin() + dist_b ;

            sum_rect_area = std::accumulate ( pos_a , pos_b , sum_rect_area_0 ) ;
            sum_width     = sum_rect_area . Width ( ) ;
            sum_length    = sum_rect_area . Length( ) ;
            sum_area      = sum_rect_area . Area  ( ) ;
            assert ( ( sum_width ==sum_width_exact  ) &&
                     ( sum_length==sum_length_exact ) &&
                     ( sum_area  ==sum_area_exact   )    ) ;
            assert ( ( dist_b-dist_a ) == ( pos_b-pos_a ) ) ;
        }

        //  construct sequence with fast algorithm accumulate()
        //  and compare exact values of sums with results
        //  obtained using method std::accumulate()
        //  and fast algorithm bp_tree_..._acc::accumulate()
        SequenceRectArea    seqce_rect_area ( vect_rect_area.begin() , vect_rect_area.end() ) ;
        {
            SequenceRectArea::const_iterator  pos_a = seqce_rect_area.begin() + dist_a ;
            SequenceRectArea::const_iterator  pos_b = seqce_rect_area.begin() + dist_b ;

            sum_rect_area = std::accumulate ( pos_a , pos_b , sum_rect_area_0 ) ;
            sum_width     = sum_rect_area . Width ( ) ;
            sum_length    = sum_rect_area . Length( ) ;
            sum_area      = sum_rect_area . Area  ( ) ;
            assert ( ( sum_width ==sum_width_exact  ) &&
                     ( sum_length==sum_length_exact ) &&
                     ( sum_area  ==sum_area_exact   )    ) ;

            sum_rect_area = seqce_rect_area . accumulate ( pos_a , pos_b , sum_rect_area_0 ) ;
            sum_width     = sum_rect_area . Width ( ) ;
            sum_length    = sum_rect_area . Length( ) ;
            sum_area      = sum_rect_area . Area  ( ) ;
            assert ( ( sum_width ==sum_width_exact  ) &&
                     ( sum_length==sum_length_exact ) &&
                     ( sum_area  ==sum_area_exact   )    ) ;
            assert ( ( dist_b-dist_a ) == ( pos_b-pos_a ) ) ;
        }

        //  calculate mean values
        double      n              = static_cast<double>( dist_b - dist_a ) ;
        double      mean_width     = static_cast<double>( sum_width  ) / n ;
        double      mean_length    = static_cast<double>( sum_length ) / n ;
        double      mean_area      = static_cast<double>( sum_area   ) / n ;
        double      mean_perimeter = 2.0 * ( mean_width + mean_length ) ;

        std::cout << "example of calculation of mean values of parameters of rectangles:" << std::endl
                  << "    n              = " << n              << " ; " << std::endl
                  << "    mean_width     = " << mean_width     << " ; " << std::endl
                  << "    mean_length    = " << mean_length    << " ; " << std::endl
                  << "    mean_perimeter = " << mean_perimeter << " ; " << std::endl
                  << "    mean_area      = " << mean_area      << " ; " << std::endl ;
    }

}


int main ( )
{
    BPTreeDemo::TestRectArea ( ) ;
    return 0 ;
}










