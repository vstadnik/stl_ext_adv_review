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

#ifndef _TEST_COMMON_HPP
#define _TEST_COMMON_HPP

#include "test_helpers.hpp"


//  methods common to all STL variants of containers
namespace test_std_ext_adv
{

    template < class _Contr >
    void size ( _Contr const &  contr , size_t  sz_test , size_t  n_dupl_test )
    {
        if ( contr.size() != sz_test*n_dupl_test )
            BOOST_ERROR ( "\n  !: ERROR size_multi ;\n" ) ;
    }


    template < class _Contr >
    void max_size ( _Contr const &  contr )
    {
        if ( contr.get_allocator().max_size() !=
             contr.max_size() )
            BOOST_ERROR ( "\n  !: ERROR max_size ;\n" ) ;
    }


    template < class _Contr >
    void clear_copy ( _Contr &  contr )
    {
        const _Contr    ctr_copy ( contr ) ;
        if ( ctr_copy != contr )
            BOOST_ERROR ( "\n  !: ERROR copy ;\n" ) ;

        contr . clear ( ) ;
        if ( ! contr.empty() )
            BOOST_ERROR ( "\n  !: ERROR clear ;\n" ) ;

        contr = ctr_copy ;
        if ( ctr_copy != contr )
            BOOST_ERROR ( "\n  !: ERROR operator= ;\n" ) ;
    }


    template < class _Contr >
    void begin_end ( _Contr &   contr )
    {
    typedef
    typename std::iterator_traits<typename _Contr::iterator>::difference_type
                            DiffType ;
        DiffType            dist = 0 ;
        DiffType            size = static_cast<DiffType>(contr.size())  ;

        typename
        _Contr::iterator    it_beg  = contr.begin() ,
                            it_end  = contr.end()   ;
        typename
        _Contr::const_iterator
                            cit_beg = contr.begin() ,
                            cit_end = contr.end()   ;

        dist = std::distance ( it_beg , it_end ) ;
        if ( size != dist )
            BOOST_ERROR ( "\n  !: ERROR begin-end ;\n" ) ;

        dist = std::distance ( cit_beg , cit_end ) ;
        if ( size != dist )
            BOOST_ERROR ( "\n  !: ERROR const begin-end ;\n" ) ;

        typename
        _Contr::reverse_iterator
                            rit_beg  = contr.rbegin() ,
                            rit_end  = contr.rend()   ;
        typename
        _Contr::const_reverse_iterator
                            crit_beg = contr.rbegin() ,
                            crit_end = contr.rend()   ;

        dist = std::distance ( rit_beg , rit_end ) ;
        if ( size != dist )
            BOOST_ERROR ( "\n  !: ERROR reverse begin-end ;\n" ) ;

        dist = std::distance ( crit_beg , crit_end ) ;
        if ( size != dist )
            BOOST_ERROR ( "\n  !: ERROR const_reverse begin-end ;\n" ) ;
    }


    template < class _Contr >
    void swap ( _Contr & contr )
    {
        if ( contr . empty ( ) )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        typename _Contr::size_type
                    sz_orig = contr . size ( ) ;
        _Contr      other ( contr ) ;
        other . erase ( other.begin() ) ;

        contr . swap ( other ) ;
        if ( contr.size() == sz_orig )
            BOOST_ERROR ( "\n  !: ERROR swap ;\n" ) ;

        swap ( contr , other ) ;
        if ( contr.size() != sz_orig )
            BOOST_ERROR ( "\n  !: ERROR swap ;\n" ) ;
    }


    template < class _Contr >
    void split ( _Contr & contr )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;
        _Contr          other ;

        typename _Contr::iterator
                        it_from = contr.begin() ,
                        it_to   = contr.begin() ;
        size_t          sz      = contr.size()  ;
        move_forw ( it_from , sz/4 ) ;
        move_forw ( it_to   , sz/4 + sz/2 ) ;

        contr . split ( it_from , it_to , other ) ;
        if ( (contr.size() + other.size()) != sz )
            BOOST_ERROR ( "\n  !: ERROR split ;\n" ) ;
        if ( other.size() != sz/2 )
            BOOST_ERROR ( "\n  !: ERROR split ;\n" ) ;


        other . clear ( ) ;
        contr = copy ;
        it_from = contr . begin ( ) ;
        move_forw ( it_from , sz/2 ) ;

        contr . split ( it_from , other ) ;
        if ( (contr.size() + other.size()) != sz )
            BOOST_ERROR ( "\n  !: ERROR split ;\n" ) ;
        if ( contr.size() != sz/2 )
            BOOST_ERROR ( "\n  !: ERROR split ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr >
    void erase ( _Contr & contr )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;

        typename _Contr::iterator
                        it_from = contr.begin() ,
                        it_to   = contr.begin() ;
        size_t          sz      = contr.size()  ;
        move_forw ( it_from , sz/4 ) ;
        move_forw ( it_to   , sz/4 + sz/2 ) ;


        contr . erase ( it_from , it_to ) ;
        if ( sz - contr.size() != sz/2 )
            BOOST_ERROR ( "\n  !: ERROR erase ;\n" ) ;

        sz      = contr . size  ( ) ;
        it_from = contr . begin ( ) ;
        move_forw ( it_from , sz/2 ) ;
        contr . erase ( it_from ) ;
        if ( sz - contr.size() != 1 )
            BOOST_ERROR ( "\n  !: ERROR erase ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr , class _Ty2 , class _Get2nd >
    void accumulate ( const _Contr &  contr    ,
                      const _Ty2      , // val_in
                      const _Get2nd   f_get2nd )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        const _Ty2      sum_0     = _Ty2() ;
        _Ty2            sum_contr = _Ty2() ;
        _Ty2            sum_std   = _Ty2() ;
        const size_t    sz        = contr.size() ;
        const size_t    pos_from  = sz/4 ;
        const size_t    pos_to    = pos_from + sz/2 ;

        //  get sum_ty2 using contr.accumulate
        {
            typename _Contr::const_iterator
                        it_from = contr.begin() ,
                        it_to   = contr.begin() ;
            move_forw ( it_from , pos_from ) ;
            move_forw ( it_to   , pos_to   ) ;

            sum_contr = contr . accumulate ( it_from , it_to , sum_0 ) ;
        }

        //  get sum using std::accumulate()
        {
            std::vector<_Ty2>   vect_ty2 ;
            {
                typename _Contr::const_iterator
                            iter   = contr.begin() ,
                            it_end = contr.end()   ;
                for (  ; iter!=it_end ; ++iter )
                {
                    _Ty2    val_2 = f_get2nd ( *iter ) ;
                    vect_ty2 . push_back ( val_2 ) ;
                }
            }

            typename std::vector<_Ty2>::const_iterator
                        it_from = vect_ty2.begin() ,
                        it_to   = vect_ty2.begin() ;
            move_forw ( it_from , pos_from ) ;
            move_forw ( it_to   , pos_to   ) ;

            sum_std = std::accumulate ( it_from , it_to , sum_0 ) ;
        }

        if ( sum_contr != sum_std )
            BOOST_ERROR ( "\n  !: ERROR accumulate ;\n" ) ;
    }


    template < class _Contr >
    void rel_operators ( const _Contr &  contr )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr      other ( contr ) ;

        if (   other!=contr  )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if ( !(other==contr) )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if (   other< contr  )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if ( !(other<=contr) )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if (   other> contr  )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if ( !(other>=contr) )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;


        typename _Contr::iterator
                    iter = other . end( ) ;
        --iter ;
        other . erase ( iter ) ;

        if (   other==contr  )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if ( !(other!=contr) )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if (   contr< other  )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if (   contr<=other  )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if (   other> contr  )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
        if (   other>=contr  )
            BOOST_ERROR ( "\n  !: ERROR rel oper ;\n" ) ;
    }


    template < class _Contr >
    void test_common ( _Contr &     contr      ,
                       size_t       sz_test    ,
                       size_t       n_dupl_add ,
                       bool         is_unique  )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        const size_t    n_dupl_test = is_unique ? 1 : n_dupl_add ;

        size         ( contr , sz_test , n_dupl_test ) ;
        max_size     ( contr ) ;
        clear_copy   ( contr ) ;
        begin_end    ( contr ) ;
        erase        ( contr ) ;
        swap         ( contr ) ;
        split        ( contr ) ;
        rel_operators( contr ) ;
    }

}


#endif  //  _TEST_COMMON_HPP
