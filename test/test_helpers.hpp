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

#ifndef _TEST_HELPERS_HPP
#define _TEST_HELPERS_HPP

#include <vector>
#include <algorithm>
#include <numeric>


namespace test_std_ext_adv
{

    template < class _Iter >
    void move_forw ( _Iter &  iter_res , const size_t  n_steps )
    {
        typedef
        typename _Iter::difference_type
                        DistType ;
        DistType        dist = static_cast<DistType>( n_steps ) ;
        std::advance ( iter_res , dist ) ;
    }


    template < class _Ty >
    struct Identity
    {
        const _Ty &
        operator ( ) ( const _Ty &  val_x ) const
        {
            return val_x ;
        }
    } ;


    template < class _TyPair , class _Ty1st >
    struct Get1st
    {
        const _Ty1st &
        operator ( ) ( const _TyPair &  val_x ) const
        {
            return val_x.first ;
        }
    } ;


    template < class _TyPair , class _Ty2nd >
    struct Get2nd
    {
        const _Ty2nd &
        operator ( ) ( const _TyPair &  val_x ) const
        {
            return val_x.second ;
        }
    } ;


    template < class _Ty >
    struct Even
    {
        bool operator ( ) ( const _Ty &  x ) const
        {
            return ( 2*(x/2 ) == x ) ;
        }
    } ;


    void fill_rand ( std::vector<size_t> &  vec_res ,
                     const size_t           n_fill  ,
                     const size_t           n_dupl  ,
                     const size_t           factor  )
    {
        vec_res . clear ( ) ;

        for (     size_t  i = 0 ; i < n_fill ; ++i )
        {   for ( size_t  k = 0 ; k < n_dupl ; ++k )
                vec_res . push_back ( i*factor ) ;
        }

        std::random_shuffle ( vec_res.begin() , vec_res.end() ) ;
    }


    template < class _Contr >
    void fill_even_rand ( _Contr &  contr , size_t  n_fill , size_t  n_dupl )
    {
        std::vector<size_t>     vec_rnd ;
        fill_rand ( vec_rnd , n_fill , n_dupl , 2 ) ;

        //  use constructor common to all STL containers
        contr = _Contr ( vec_rnd.begin() , vec_rnd.end() ) ;
    }


    template < class _Contr >
    void fill_even_rand_map ( _Contr &  contr, size_t  n_fill, size_t  n_dupl )
    {
        std::vector<size_t>     vec_rnd ;
        fill_rand ( vec_rnd , n_fill , n_dupl , 1 ) ;

        std::vector<std::pair<size_t, size_t> >  vec_map ;
        {
            const size_t  sz = vec_rnd . size ( ) ;
            for ( size_t  j  = 0 ; j < sz ; ++j )
            {
                size_t    x  = vec_rnd . at ( j ) ;
                vec_map.push_back(std::pair<size_t,size_t>( 2*x, 5*x ));
            }
        }

        //  use constructor common to all STL containers
        contr = _Contr ( vec_map.begin() , vec_map.end() ) ;
    }


    size_t  size_test_min ( ) { return size_t(8) ; }
}


#endif  //  _TEST_HELPERS_HPP
