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


#include <vector>
#include <iostream>
#include <assert.h>
#include "bpt_set.hpp"


namespace BPTreeDemo
{
    //  class Interval represents a semi-open interval:
    //  [ a , b ) , with a <= b ;
    //
    //  in order to simplify demo code
    //  endpoints are of type int ;
    class Interval
    {
    public:
        explicit
        Interval ( int  _a=0 , int  _b=0 ) : a ( _a ) , b ( _b )
        {
            if ( b < a )
                std::swap ( a , b ) ;
        }

        int     A ( ) const { return a ; }
        int     B ( ) const { return b ; }

    private:
        int     a ;
        int     b ;
    } ;


    //  this function detects intersection of two intervals,
    //  it returns true in the following cases:
    //  - both intervals are non-empty and their intersection
    //    is a non-empty interval ;
    //  - an empty interval is in the interior of the other
    //    non-empty interval ;
    //  in all other cases this functions returns false ;
    bool Intersect ( Interval const &  x , Interval const &  y )
    {
        return ( x.A() < y.B()  ) && ( y.A() < x.B() ) ;
    }


    //  type VectInterval represents a collection of intervals;
    //  in the general case a collection, such as VectInterval,
    //  can store either unordered or ordered objects of any size
    //  and any type provided that each object represents an interval ;
    typedef std::vector<Interval>       VectInterval ;
    //
    //  type MultiSetInt represents a collection of intervals
    //  ordered by the lower or upper endpoint of an interval ;
    typedef std_ext_adv::multiset<int>  MultiSetInt  ;


    void FillOrdered
        (
            VectInterval &      vect_fill   ,
            const int           sz_fill     ,
            const int           n_dupl
        )
    {
        vect_fill . clear ( ) ;

        for ( int   i = 0 ; i < sz_fill ; ++i )
        {
            int         a = i*10 ;
            int         b = (i+1)*10 ;
            Interval    interval ( a , b ) ;

            for ( int   j = 0 ; j < n_dupl ; ++j )
                vect_fill . push_back ( interval ) ;
        }
    }

    void FillRandom
        (
            VectInterval &      vect_fill   ,
            const int           sz_fill     ,
            const int           n_dupl
        )
    {
        vect_fill . clear ( ) ;

        for ( int   i = 0 ; i < sz_fill ; ++i )
        {
            int         a = rand ( ) ;
            int         b = rand ( ) ;
            Interval    interval ( a , b ) ;

            for ( int   j = 0 ; j < n_dupl ; ++j )
                vect_fill . push_back ( interval ) ;
        }
    }


    void InitMultiSets
        (
            VectInterval const &    vect_interval ,
            MultiSetInt &           mset_a ,
            MultiSetInt &           mset_b
        )
    {
        mset_a . clear ( ) ;
        mset_b . clear ( ) ;

        const int   sz = vect_interval . size ( ) ;
        for ( int   i  = 0 ; i < sz ; ++i )
        {
            Interval const &    interval = vect_interval . at ( i ) ;
            mset_a . insert ( interval . A ( ) ) ;
            mset_b . insert ( interval . B ( ) ) ;
        }
    }


    //  this function implements linear counting of intersections
    int  CountIntersections
        (
            Interval const &        interval_x  ,
            VectInterval const &    vect_interval
        )
    {
        int         count = 0 ;
        const int   sz = vect_interval . size ( ) ;

        for ( int   i = 0 ; i < sz ; ++i )
        {
            Interval const &
                    interval_i = vect_interval . at ( i ) ;

            if ( Intersect ( interval_x , interval_i ) )
                ++count ;
        }

        return count ;
    }


    //  this function implements efficient counting of intersections
    //  with logarithmic running time in the worst case;
    //  for implementation details of this method see documentation;
    int  CountIntersections
        (
            Interval const &        interval_x  ,
            MultiSetInt const &     mset_a      ,
            MultiSetInt const &     mset_b
        )
    {
        assert ( mset_a.size()==mset_b.size() ) ;

        int     n     = mset_a . size ( ) ;
        int     n_a   = mset_b . upper_bound ( interval_x.A() ) -
                        mset_b . begin ( ) ;
        int     n_b   = mset_a . end ( ) -
                        mset_a . lower_bound ( interval_x.B() ) ;
        int     count = n - n_a - n_b ;
        return  count ;
    }


    void TestRandom
        (
            const int       sz_fill ,
            const int       n_dupl
        )
    {
        VectInterval        vect_interval ;
        MultiSetInt         mset_a ;
        MultiSetInt         mset_b ;
        Interval            int_test ;
        int                 n_vect  = 0 ;
        int                 n_msets = 0 ;

        FillRandom    ( vect_interval , sz_fill , n_dupl ) ;
        InitMultiSets ( vect_interval , mset_a  , mset_b ) ;

        std::cout << "sequence of random intervals:" << std::endl ;

        //  generate new interval with random endpoints
        int_test = Interval ( rand() , rand() ) ;
        n_vect   = CountIntersections ( int_test , vect_interval ) ;
        n_msets  = CountIntersections ( int_test , mset_a , mset_b ) ;
        //  check correctness of calculations of the number of intersecting intervals
        assert ( n_vect==n_msets ) ;

        std::cout << "    number of intersections with new interval:" << std::endl
                  << "    n_inters = " << n_msets << " ; " << std::endl ;

        //  use for testing an interval stored in the containers
        int_test = vect_interval . at ( sz_fill*n_dupl / 2 ) ;
        n_vect   = CountIntersections ( int_test , vect_interval ) ;
        n_msets  = CountIntersections ( int_test , mset_a , mset_b ) ;
        //  check correctness of calculations of the number of intersecting intervals
        assert ( n_vect==n_msets ) ;

        std::cout << "    number of intersections with copy interval selected from the sequence:" << std::endl
                  << "    n_inters = " << n_msets << " ; " << std::endl ;
        std::cout << std::endl ;
    }


    void TestOrdered
        (
            const int       sz_fill ,
            const int       n_dupl
        )
    {
        VectInterval        vect_interval ;
        MultiSetInt         mset_a ;
        MultiSetInt         mset_b ;
        Interval            int_test ;
        int                 n_vect  = 0 ;
        int                 n_msets = 0 ;

        FillOrdered   ( vect_interval , sz_fill , n_dupl ) ;
        InitMultiSets ( vect_interval , mset_a  , mset_b ) ;

        std::cout << "sequence of ordered intervals:" << std::endl ;

        //  generate new interval with random endpoints
        int_test = Interval ( rand() , rand() ) ;
        n_vect   = CountIntersections ( int_test , vect_interval ) ;
        n_msets  = CountIntersections ( int_test , mset_a , mset_b ) ;
        //  check correctness of calculations of the number of intersecting intervals
        assert ( n_vect==n_msets ) ;

        std::cout << "    number of intersections with new interval:" << std::endl
                  << "    n_inters = " << n_msets << " ; " << std::endl ;

        //  use for testing an interval stored in the containers
        int_test = vect_interval . at ( sz_fill*n_dupl / 2 ) ;
        n_vect   = CountIntersections ( int_test , vect_interval ) ;
        n_msets  = CountIntersections ( int_test , mset_a , mset_b ) ;
        //  check correctness of calculations of the number of intersecting intervals
        assert ( n_vect==n_msets ) ;
        assert ( n_dupl==n_msets ) ;

        std::cout << "    number of intersections with copy interval selected from the sequence:" << std::endl
                  << "    n_inters = " << n_msets << " ; " << std::endl ;
        std::cout << std::endl ;
    }

}


int main ( )
{
    std::cout << "example of fast counting of intersections between" << std::endl
              << "one interval and a sequence of intervals:"         << std::endl << std::endl ;

    int   sz_fill = 1000 ;
    int   n_dupl  = 10   ;
    BPTreeDemo::TestRandom  ( sz_fill , n_dupl ) ;

    sz_fill = 10   ;
    n_dupl  = 1000 ;
    BPTreeDemo::TestOrdered ( sz_fill , n_dupl ) ;

    return 0 ;
}










