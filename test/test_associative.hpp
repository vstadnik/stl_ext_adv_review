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

#ifndef _TEST_ASSOCIATIVE_HPP
#define _TEST_ASSOCIATIVE_HPP


#include "test_helpers.hpp"
#include "test_common.hpp"


//  methods to test associative containers:
//  set, multiset, map, multimap ;
namespace test_std_ext_adv
{

    template < class _Contr >
    void constr_associative ( const _Contr &  contr )
    {
        if ( contr.size ( ) < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr      contr_0 ;
        if ( contr_0.size() != 0 )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        contr_0 = contr ;
        if ( contr_0.size() != contr.size() )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        _Contr      ctr_copy ( contr ) ;
        if ( ctr_copy.size() != contr.size() )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;


        typename _Contr::allocator_type  alr  = contr.get_allocator() ;
        typename _Contr::key_compare     pred = contr.key_comp() ;

        _Contr      contr_pred ( pred ) ;
        if ( contr_pred.size() != 0 )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        _Contr      contr_pred_alr ( pred , alr ) ;
        if ( contr_pred_alr.size() != 0 )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        _Contr      contr_it_pred ( contr.begin() , contr.end() , pred ) ;
        if ( contr_it_pred.size() != contr.size() )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        _Contr      contr_it_pred_alr
                    ( contr.begin() ,
                      contr.end()   ,
                      pred          ,
                      alr           ) ;
        if ( contr_it_pred_alr.size() != contr.size() )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;
    }


    template < class _Contr , class _Get1st >
    void erase_key ( _Contr &       contr       ,
                     const size_t   n_dupl_test ,
                     const _Get1st  get1st      )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;

        typename _Contr::const_iterator
                        iter = contr . begin( ) ;
        size_t          sz   = contr . size ( ) ;
        move_forw ( iter , sz/2 ) ;

        typename _Contr::key_type
                        key  = get1st ( *iter ) ;

        //  erase existing key
        size_t          n_ers= contr . erase ( key ) ;
        if ( n_ers != n_dupl_test )
            BOOST_ERROR ( "\n  !: ERROR erase key ;\n" ) ;

        //  erase non-existing key
        ++key ;
        n_ers = contr . erase ( key ) ;
        if ( n_ers != 0 )
            BOOST_ERROR ( "\n  !: ERROR erase key ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr , class _Iter , class _Get1st >
    void find_key ( _Contr &        contr       ,
                    const _Iter &   , // iter_in
                    const size_t    n_dupl_test ,
                    const _Get1st   get1st      )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Iter       it_end = contr . end ( ) ;
        _Iter       iter   = contr . begin( ) ;
        size_t      sz     = contr . size ( ) ;
        move_forw ( iter , sz/2 ) ;

        typename _Contr::key_type
                    key    = get1st ( *iter ) ;
        _Iter       it_low = contr . lower_bound ( key ) ;
        _Iter       it_upp = contr . upper_bound ( key ) ;
        std::pair<_Iter,_Iter>
                    range  = contr . equal_range ( key ) ;
        _Iter       it_find= contr . find  ( key ) ;
        size_t      count  = contr . count ( key ) ;

        if ( ( it_low == it_end ) || ( get1st(*iter) < get1st(*it_low) ) )
            BOOST_ERROR ( "\n  !: ERROR lower_bound ;\n" ) ;
        if ( ( it_upp == it_end ) || ( get1st(*it_upp) < get1st(*iter) ) )
            BOOST_ERROR ( "\n  !: ERROR upper_bound ;\n" ) ;
        if ( ( range.first != it_low ) || ( range.second != it_upp ) )
            BOOST_ERROR ( "\n  !: ERROR equal_range ;\n" ) ;
        if ( ( it_find== it_end ) || ( get1st(*it_find) != get1st(*iter) ) )
            BOOST_ERROR ( "\n  !: ERROR find ;\n" ) ;
        if ( count != n_dupl_test )
            BOOST_ERROR ( "\n  !: ERROR count ;\n" ) ;

        //  key does not exist:  key_min < key < key_max
        //  range [it_low,it_upp) must be empty
        ++key ;
        count = contr . count ( key ) ;
        if ( count != 0 )
            BOOST_ERROR ( "\n  !: ERROR count ;\n" ) ;

        //  key does not exist:  k_max < key
        _Iter       it_last = contr.end() ;
        --it_last ;
        key    = get1st ( *it_last ) ;
        ++key ;
        it_low = contr . lower_bound ( key ) ;
        it_upp = contr . upper_bound ( key ) ;
        range  = contr . equal_range ( key ) ;
        it_find= contr . find  ( key ) ;
        count  = contr . count ( key ) ;

        if ( it_low != it_end )
            BOOST_ERROR ( "\n  !: ERROR lower_bound ;\n" ) ;
        if ( it_upp != it_end )
            BOOST_ERROR ( "\n  !: ERROR upper_bound ;\n" ) ;
        if ( ( range.first != it_end ) || ( range.second != it_end ) )
            BOOST_ERROR ( "\n  !: ERROR equal_range ;\n" ) ;
        if ( it_find!= it_end )
            BOOST_ERROR ( "\n  !: ERROR find ;\n" ) ;
        if ( count != 0 )
            BOOST_ERROR ( "\n  !: ERROR count ;\n" ) ;
    }


    template < class _Contr , class _Get1st >
    void find_key_ct ( const _Contr &  contr       ,
                       const size_t    n_dupl_test ,
                       const _Get1st   get1st      )
    {
        typename _Contr::const_iterator
                    it_end = contr . end ( ) ;
        find_key ( contr , it_end , n_dupl_test , get1st ) ;
    }


    template < class _Contr , class _Get1st >
    void find_key  ( _Contr &        contr       ,
                     const size_t    n_dupl_test ,
                     const _Get1st   get1st      )
    {
        typename _Contr::iterator
                    it_end = contr . end ( ) ;
        find_key ( contr , it_end , n_dupl_test , get1st ) ;
    }


    //  only for map
    template < class _Contr >
    void map_oper_key ( _Contr &  contr )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;

        typedef
        typename _Contr::mapped_type    MapType ;

        typename _Contr::const_iterator
                        iter = contr . begin( ) ;
        size_t          sz   = contr . size ( ) ;
        move_forw ( iter , sz/2 ) ;

        typename _Contr::key_type
                        key = iter->first ;
        MapType         val = contr [ key ] ;

        if ( contr.size() != sz )
            BOOST_ERROR ( "\n  !: ERROR map oper_key ;\n" ) ;

        //  non-existing key with new value
        ++key ;
        ++val ;

        contr[key] = val ;
        if ( contr.size() != (sz+1) )
            BOOST_ERROR ( "\n  !: ERROR map oper_key ;\n" ) ;
        if ( val != contr[key] )
            BOOST_ERROR ( "\n  !: ERROR map oper_key ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr , class _Get1st >
    void key_val_compare ( const _Contr &   contr  ,
                           const _Get1st &  get1st )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        typedef typename _Contr::key_type       KeyType ;
        typedef typename _Contr::value_type     ValType ;

        typename _Contr::key_compare    key_comp = contr.key_comp()   ;
        typename _Contr::value_compare  val_comp = contr.value_comp() ;
        typename _Contr::const_iterator iter     = contr.begin() ;

        KeyType     key_0 = get1st(*iter) ;
        ValType     val_0 = *iter ;
        ++iter ;
        KeyType     key_1 = get1st(*iter) ;
        ValType     val_1 = *iter ;

        if ( key_comp ( key_1 , key_0 ) )
            BOOST_ERROR ( "\n  !: ERROR key_compare ;\n" ) ;
        if ( val_comp ( val_1 , val_0 ) )
            BOOST_ERROR ( "\n  !: ERROR value_compare ;\n" ) ;
    }


    //  for set and map only
    template < class _Contr >
    void insert_unique ( const _Contr &  contr )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        typename _Contr::const_iterator
                    it_from = contr.begin() ,
                    it_to   = contr.begin() ;
        size_t      sz      = contr.size() ;
        move_forw ( it_from , sz/4 ) ;
        move_forw ( it_to   , sz/4 + sz/2 ) ;

        //  insert existing elements
        _Contr      other ( contr ) ;
        other . insert ( *it_from ) ;
        if ( other.size() != sz )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;

        typename _Contr::iterator
                    it_beg = other.begin() ;
        other . insert ( it_beg , *it_from ) ;
        if ( other.size() != sz )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;

        other . insert ( it_from , it_to ) ;
        if ( other.size() != sz )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;


        //  insert new elements
        other . clear ( ) ;
        other . insert ( *it_from ) ;
        if ( other.size() != 1 )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;

        it_beg = other.begin() ;
        other . insert ( it_beg , *it_to ) ;
        if ( other.size() != 2 )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;

        other . clear ( ) ;
        other . insert ( it_from , it_to ) ;
        if ( other.size() != sz/2 )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;
    }


    //  for multiset and multimap only
    template < class _Contr >
    void insert_multi ( const _Contr &  contr )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        typename _Contr::const_iterator
                    it_from = contr.begin() ,
                    it_to   = contr.begin() ;
        size_t      sz      = contr.size() ;
        move_forw ( it_from , sz/4 ) ;
        move_forw ( it_to   , sz/4 + sz/2 ) ;

        _Contr      other ( contr ) ;

        other . insert ( *it_from ) ;
        if ( other.size() != ( sz + 1 ) )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;

        typename _Contr::iterator
        it_beg = other.begin() ;
        other . insert ( it_beg , *it_to ) ;
        if ( other.size() != ( sz + 2 ) )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;

        other = contr ;
        other . insert ( it_from , it_to ) ;
        if ( other.size() != ( sz + sz/2 ) )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;
    }


    template < class _Contr , class _Ty2 , class _Get2nd >
    void update_value_set ( _Contr &       contr    ,
                            const _Ty2     , // val_in
                            const _Get2nd  f_get2nd )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;

        typedef typename _Contr::iterator   Iter ;

        std::pair<Iter, bool>
                        iter_bool ;
        Iter            iter = contr . begin( ) ;
        const size_t    sz   = contr . size ( ) ;
        move_forw ( iter , sz/4 ) ;
        _Ty2            val_x= f_get2nd ( *iter ) ;

        //  update using existing value
        //  at the same position
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr != copy )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;
        if ( iter_bool.second != false )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;
        if ( iter_bool.first != iter )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        //  at some other position
        iter = contr . begin ( ) ;
        move_forw ( iter , sz/2 ) ;
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr != copy )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;
        if ( iter_bool.second != false )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        //  update using new value
        ++val_x ;
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr == copy )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;
        if ( iter_bool.second != true )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        if ( contr.size()!= sz )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr , class _Ty2 , class _Get2nd >
    void update_value_map ( _Contr &       contr    ,
                            const _Ty2     , // val_in
                            const _Get2nd  f_get2nd )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;

        typedef typename _Contr::iterator   Iter ;

        std::pair<Iter, bool>
                        iter_bool ;
        Iter            iter = contr . begin( ) ;
        const size_t    sz   = contr . size ( ) ;
        move_forw ( iter , sz/4 ) ;
        _Ty2            val_x= f_get2nd ( *iter ) ;

        //  update using existing value
        //  at the same position
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr != copy )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;
        if ( iter_bool.second != true )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;


        //  at some other position
        iter = contr . begin ( ) ;
        move_forw ( iter , sz/2 ) ;
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr == copy )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;
        if ( iter_bool.second != true )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        //  update using new value
        ++val_x ;
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr == copy )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;
        if ( iter_bool.second != true )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        if ( contr.size()!= sz )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr , class _Ty2 , class _Get2nd >
    void update_value_multi ( _Contr &        contr    ,
                              const _Ty2      , // val_in
                              const _Get2nd   f_get2nd )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;

        typedef typename _Contr::iterator   Iter ;

        std::pair<Iter, bool>
                        iter_bool ;
        Iter            iter = contr . begin( ) ;
        const size_t    sz   = contr . size ( ) ;
        move_forw ( iter , sz/4 ) ;
        _Ty2            val_x= f_get2nd ( *iter ) ;

        //  existing value
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr != copy )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        //  new value
        ++val_x ;
        iter = contr . begin ( ) ;
        move_forw ( iter , sz/2 ) ;
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr == copy )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        if ( contr.size()!= sz )
            BOOST_ERROR ( "\n  !: ERROR write associative ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr >
    void merge_set ( _Contr &  contr , const bool  unique )
    {
        if ( contr . empty ( ) )
            BOOST_ERROR ( "\n  !: ERROR invalid input container ;\n" ) ;

        _Contr              orig ( contr ) ;
        {
            _Contr              ctr_a ( orig ) ;
            _Contr              ctr_b ( orig ) ;
            std::vector<size_t> vec_a ( ctr_a.begin() , ctr_a.end() ) ;
            std::vector<size_t> vec_b ( ctr_b.begin() , ctr_b.end() ) ;

            ctr_a . merge ( ctr_b ) ;

            std::vector<size_t> vec_std_ext ( ctr_a.begin() , ctr_a.end() ) ;
            std::vector<size_t> vec_std     ( vec_a.size() + vec_b.size() ) ;
            std::merge( vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), vec_std.begin() ) ;
            if ( unique )
            {
                std::vector<size_t>::iterator   res_end ;
                res_end = std::unique ( vec_std.begin(), vec_std.end() ) ;
                if ( res_end != vec_std.end() )
                    vec_std . erase ( res_end , vec_std.end() ) ;
            }

            if ( vec_std_ext != vec_std )
                BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
        }

        contr = orig ;
        {
            _Contr              ctr_a ( orig ) ;
            _Contr              ctr_b ;
            fill_even_rand ( ctr_b , 2*ctr_a.size() , 2 ) ;

            std::vector<size_t> vec_a ( ctr_a.begin() , ctr_a.end() ) ;
            std::vector<size_t> vec_b ( ctr_b.begin() , ctr_b.end() ) ;

            ctr_a . merge ( ctr_b ) ;

            std::vector<size_t> vec_std_ext ( ctr_a.begin() , ctr_a.end() ) ;
            std::vector<size_t> vec_std     ( vec_a.size() + vec_b.size() ) ;
            std::merge( vec_a.begin(), vec_a.end(), vec_b.begin(), vec_b.end(), vec_std.begin() ) ;
            if ( unique )
            {
                std::vector<size_t>::iterator   res_end ;
                res_end = std::unique ( vec_std.begin(), vec_std.end() ) ;
                if ( res_end != vec_std.end() )
                    vec_std . erase ( res_end , vec_std.end() ) ;
            }

            if ( vec_std_ext != vec_std )
                BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
        }

        contr = orig ;
    }


    template < class _Contr , class _Get1st >
    void merge_map ( _Contr &  contr , const _Get1st &  get1st , const bool  unique )
    {
        if ( contr . empty ( ) )
            BOOST_ERROR ( "\n  !: ERROR invalid input container ;\n" ) ;

        typedef typename _Contr::key_type   KeyType ;
        typename _Contr::key_compare        key_comp = contr.key_comp()   ;

        _Contr              orig ( contr ) ;
        {
            _Contr              ctr_a ( orig ) ;
            _Contr              ctr_b ( orig ) ;

            ctr_a . merge ( ctr_b ) ;

            if ( !ctr_b.empty() )
                BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
            if ( unique )
            {
                if ( ctr_a.size() != orig.size() )
                    BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
            }
            else
            {
                if ( ctr_a.size() != 2*orig.size() )
                    BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
            }

            typename _Contr::const_iterator
                        it_curr = contr . begin ( ) ,
                        it_next = contr . begin ( ) ;
            ++it_next ;
            for (  ; it_next != contr.end() ; ++it_curr , ++it_next )
            {
                KeyType     k_curr = get1st(*it_curr) ;
                KeyType     k_next = get1st(*it_next) ;
                if ( key_comp ( k_next , k_curr ) )
                    BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
            }
        }

        contr = orig ;
        {
            _Contr              ctr_a ( orig ) ;
            _Contr              ctr_b ;
            const size_t        k_sz   = 2 ;
            const size_t        n_dupl = 2 ;
            fill_even_rand_map ( ctr_b , k_sz*ctr_a.size() , n_dupl ) ;

            ctr_a . merge ( ctr_b ) ;

            if ( !ctr_b.empty() )
                BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
            if ( unique )
            {
                if ( ctr_a.size() != k_sz*orig.size() )
                    BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
            }
            else
            {
                if ( ctr_a.size() != (1 + n_dupl*k_sz)*orig.size() )
                    BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
            }

            typename _Contr::const_iterator
                        it_curr = contr . begin ( ) ,
                        it_next = contr . begin ( ) ;
            ++it_next ;
            for (  ; it_next != contr.end() ; ++it_curr , ++it_next )
            {
                KeyType     k_curr = get1st(*it_curr) ;
                KeyType     k_next = get1st(*it_next) ;
                if ( key_comp ( k_next , k_curr ) )
                    BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
            }
        }

        contr = orig ;
    }



    template < class _Contr , class _Get1st >
    void common_assoc ( _Contr &         contr    ,
                        const size_t     n_dupl   ,
                        const _Get1st &  f_get1st )
    {
        constr_associative
                       ( contr ) ;
        find_key_ct    ( contr , n_dupl , f_get1st ) ;
        find_key       ( contr , n_dupl , f_get1st ) ;
        erase_key      ( contr , n_dupl , f_get1st ) ;
        key_val_compare( contr , f_get1st ) ;
    }


    template < class _Contr >
    void test_set ( _Contr &  contr , size_t  sz_test , size_t  n_dupl )
    {
        bool                is_unique = true ;
        Identity<size_t>    f_iden ;
        size_t              val = 0 ;

        fill_even_rand
                     ( contr , sz_test , n_dupl ) ;
        test_common  ( contr , sz_test , n_dupl , is_unique ) ;

        common_assoc ( contr , 1 , f_iden ) ;
        insert_unique( contr ) ;
        update_value_set
                     ( contr , val , f_iden ) ;
        accumulate   ( contr , val , f_iden ) ;
        merge_set    ( contr , is_unique ) ;
    }


    template < class _Contr >
    void test_multi_set ( _Contr &  contr , size_t  sz_test , size_t  n_dupl )
    {
        bool                is_unique = false ;
        Identity<size_t>    f_iden ;
        size_t              val = 0 ;

        fill_even_rand
                    ( contr , sz_test , n_dupl ) ;
        test_common ( contr , sz_test , n_dupl , is_unique ) ;

        common_assoc( contr , n_dupl , f_iden ) ;
        insert_multi( contr ) ;
        update_value_multi
                    ( contr , val , f_iden ) ;
        accumulate  ( contr , val , f_iden ) ;
        merge_set   ( contr , is_unique ) ;
    }


    template < class _Contr >
    void test_map ( _Contr &  contr , size_t  sz_test , size_t  n_dupl )
    {
        bool        is_unique = true ;
        size_t      val = 0 ;
        Get1st<std::pair<const size_t,size_t> , const size_t>  f_get1st ;
        Get2nd<std::pair<const size_t,size_t> ,       size_t>  f_get2nd ;

        fill_even_rand_map
                     ( contr , sz_test , n_dupl ) ;
        test_common  ( contr , sz_test , n_dupl , is_unique ) ;

        common_assoc ( contr , 1 , f_get1st ) ;
        insert_unique( contr ) ;
        map_oper_key ( contr ) ;
        update_value_map
                     ( contr , val , f_get2nd ) ;
        accumulate   ( contr , val , f_get2nd ) ;
        merge_map    ( contr , f_get1st , is_unique ) ;
    }


    template < class _Contr >
    void test_multi_map ( _Contr &  contr , size_t  sz_test , size_t  n_dupl )
    {
        bool        is_unique = false ;
        size_t      val = 0 ;
        Get1st<std::pair<const size_t,size_t> , const size_t>  f_get1st ;
        Get2nd<std::pair<const size_t,size_t> ,       size_t>  f_get2nd ;

        fill_even_rand_map
                     ( contr , sz_test , n_dupl ) ;
        test_common  ( contr , sz_test , n_dupl , is_unique ) ;

        common_assoc ( contr , n_dupl , f_get1st ) ;
        insert_multi ( contr ) ;
        update_value_multi
                     ( contr , val , f_get2nd ) ;
        accumulate   ( contr , val , f_get2nd ) ;
        merge_map    ( contr , f_get1st , is_unique ) ;
    }

}


#endif  //  _TEST_ASSOCIATIVE_HPP
