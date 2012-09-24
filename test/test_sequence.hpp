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

#ifndef _TEST_SEQUENCE_HPP
#define _TEST_SEQUENCE_HPP

#include "test_helpers.hpp"
#include "test_common.hpp"


//  methods to test sequence containers
namespace test_std_ext_adv
{

    template < class _Contr >
    void constr_seqce ( const _Contr &  contr )
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


        typename _Contr::allocator_type  alr = contr.get_allocator() ;
        typename _Contr::value_type      val = *(contr.begin()) ;

        _Contr      contr_alr ( alr ) ;
        if ( contr_alr.size() != 0 )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        _Contr      contr_cnt ( contr.size() ) ;
        if ( contr_cnt.size() != contr.size() )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        _Contr      contr_cnt_val ( contr.size() , val ) ;
        if ( contr_cnt_val.size() != contr.size() )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        _Contr      contr_cnt_val_alr ( contr.size() , val , alr ) ;
        if ( contr_cnt_val_alr.size() != contr.size() )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;

        _Contr      contr_it_alr ( contr.begin() , contr.end() , alr ) ;
        if ( contr_it_alr.size() != contr.size() )
            BOOST_ERROR ( "\n  !: ERROR constructor ;\n" ) ;
    }


    template < class _Contr >
    void assign ( const _Contr &  contr )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr      other ( contr ) ;

        typename _Contr::const_iterator
                    it_from = contr.begin() ,
                    it_to   = contr.begin() ;
        size_t      sz      = contr.size()  ;

        move_forw ( it_from , sz/4 ) ;
        move_forw ( it_to   , sz/4 + sz/2 ) ;

        other . assign ( it_from , it_to ) ;
        if ( other.size() != sz/2 )
            BOOST_ERROR ( "\n  !: ERROR assign ;\n" ) ;

        other . assign ( sz , *it_from ) ;
        if ( other.size() != sz )
            BOOST_ERROR ( "\n  !: ERROR assign ;\n" ) ;
    }


    template < class _Contr >
    void insert_seqce ( const _Contr &  contr )
    {
        if ( contr.size() < size_test_min() )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        typename _Contr::const_iterator
                    it_from = contr.begin() ,
                    it_to   = contr.begin() ;
        size_t      sz      = contr.size()  ;
        move_forw ( it_from , sz/4 ) ;
        move_forw ( it_to   , sz/4 + sz/2 ) ;

        _Contr      other ;
        typename _Contr::iterator
                    iter = other.end() ;
        other . insert ( iter , it_from , it_to ) ;
        if ( other.size() != sz/2 )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;

        iter = other.begin() ;
        other . insert( iter , ( sz-sz/2) , *it_from ) ;
        if ( other.size() != sz )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;

        iter = other.begin() ;
        ++iter ;
        other . insert ( iter , *it_to ) ;
        if ( other.size() != ( sz+1 ) )
            BOOST_ERROR ( "\n  !: ERROR insert ;\n" ) ;
    }


    template < class _Contr >
    void front_back ( _Contr &  contr )
    {
        typedef
        typename _Contr::value_type     ValType ;

        const ValType   v_front = contr . front ( ) ;
        const ValType   v_back  = contr . back  ( ) ;

        contr . push_back ( v_front ) ;
        if ( v_front != contr.back() )
            BOOST_ERROR ( "\n  !: ERROR push_back ;\n" ) ;

        contr . pop_back ( ) ;
        if ( v_back != contr.back() )
            BOOST_ERROR ( "\n  !: ERROR pop_back ;\n" ) ;
    }


    template < class _Contr >
    void resize ( _Contr &  contr )
    {
        typedef
        typename _Contr::size_type  SizeType ;
        const SizeType  sz = contr . size ( ) ;

        contr . resize ( sz + sz/2 ) ;
        if ( contr.size() != ( sz + sz/2 ) )
            BOOST_ERROR ( "\n  !: ERROR size ;\n" ) ;

        contr . resize ( sz ) ;
        if ( contr.size() != sz )
            BOOST_ERROR ( "\n  !: ERROR size ;\n" ) ;
    }


    template < class _Contr >
    void at_oper_idx ( _Contr & contr )
    {
        typedef
        typename _Contr::value_type     ValType ;

        typename
        _Contr::const_iterator
                        iter  = contr.begin() ;
        size_t          i_mid = contr.size() / 2 ;
        move_forw ( iter , i_mid ) ;

        const ValType   v_mid = *iter ;

        if ( v_mid != contr[i_mid] )
            BOOST_ERROR ( "\n  !: ERROR oper_index ;\n" ) ;

        if ( v_mid != contr.at(i_mid) )
            BOOST_ERROR ( "\n  !: ERROR at(idx) ;\n" ) ;
    }


    //  testing capacity() , reserve() ;
    //  these methods have different behaviour,
    //  since for dynamically allocated data structures
    //  'capacity' is resposibility of allocators
    //  rather than containers ;
    template < class _Contr >
    void capacity ( _Contr & contr )
    {
        typedef
        typename _Contr::size_type  SizeType ;

        SizeType    sz = contr . size ( ) ;
        SizeType    cap_orig = contr . capacity ( ) ;
        contr . reserve ( sz + sz ) ;
        SizeType    cap_new  = contr . capacity ( ) ;

        if ( cap_new < cap_orig )
            BOOST_ERROR ( "\n  !: ERROR capacity or reserve ;\n" ) ;
    }


    template < class _Contr >
    void reverse ( _Contr &  contr )
    {
        _Contr      contr_orig( contr ) ;
        std::vector<size_t>
                    vect ( contr.begin() , contr.end() ) ;

        contr . reverse( ) ;
        std::reverse ( vect.begin() , vect.end() ) ;

        std::vector<size_t>
                    vec_res ( contr.begin() , contr.end() ) ;
        if ( vec_res != vect )
            BOOST_ERROR ( "\n  !: ERROR reverse method ;\n" ) ;

        contr . reverse( ) ;
        if ( contr != contr_orig )
            BOOST_ERROR ( "\n  !: ERROR reverse method ;\n" ) ;
    }


    template < class _Contr >
    void splice
        (
            const _Contr &  contr_in
        )
    {
        size_t      sz_test = 1024 ;
        size_t      n_dupl  = 1    ;
        _Contr      contr_a ;
        _Contr      contr_b ;
        if ( contr_in.size() > sz_test )
        {
            sz_test = 4*(contr_in.size()/4) ;
        }

        fill_even_rand ( contr_a , sz_test , n_dupl ) ;
        fill_even_rand ( contr_b , sz_test , n_dupl ) ;

        typename _Contr::iterator
                    it_a0 = contr_a.begin() ,
                    it_b1 = contr_b.begin() ,
                    it_b2 = contr_b.begin() ;

        move_forw ( it_a0 , sz_test/4 ) ;
        move_forw ( it_b1 , sz_test/4 ) ;
        move_forw ( it_b2 , sz_test/4 + sz_test/2 ) ;

        typename _Contr::iterator
                    it_a1 ;

        it_a1 = contr_a . splice ( it_a0 , contr_b , it_b1 , it_b2 ) ;

        if ( contr_a.size() != ( sz_test + sz_test/2 ) )
            BOOST_ERROR ( "\n  !: ERROR splice method ;\n" ) ;
        if ( contr_b.size() != ( sz_test - sz_test/2 ) )
            BOOST_ERROR ( "\n  !: ERROR splice method ;\n" ) ;
        if ( size_t ( it_a1 - contr_a.begin() ) != ( sz_test/4 + sz_test/2 ) )
            BOOST_ERROR ( "\n  !: ERROR iterator state ;\n" ) ;
    }


    template < class _Contr >
    void sort ( _Contr &  contr )
    {
        std::vector<size_t>     vec_orig ( contr.begin() , contr.end() ) ;

        contr.sort( ) ;
        std::sort ( vec_orig.begin() , vec_orig.end() ) ;

        std::vector<size_t>     vec_res  ( contr.begin() , contr.end() ) ;
        if ( vec_res != vec_orig )
            BOOST_ERROR ( "\n  !: ERROR sort method ;\n" ) ;
    }


    template < class _Contr , class _Pred >
    void sort_pred ( _Contr &  contr , _Pred const &  pred )
    {
        std::vector<size_t>     vec_orig ( contr.begin() , contr.end() ) ;

        contr.sort( pred ) ;
        std::sort ( vec_orig.begin() , vec_orig.end() , pred ) ;

        std::vector<size_t>     vec_res  ( contr.begin() , contr.end() ) ;
        if ( vec_res != vec_orig )
            BOOST_ERROR ( "\n  !: ERROR sort_pred method ;\n" ) ;
    }


    //  input data must be ordered:
    template < class _Contr >
    void unique ( _Contr &  contr , size_t  n_dupl )
    {
        const size_t    sz_orig = contr . size ( ) ;

        contr . unique ( ) ;

        const size_t    sz_res  = contr . size ( ) ;
        if ( sz_orig != sz_res*n_dupl )
            BOOST_ERROR ( "\n  !: ERROR unique method ;\n" ) ;
    }


    template < class _Contr , class _Pred >
    void unique_pred ( _Contr &       contr  ,
                       _Pred const &  pred   ,
                       const size_t   n_dupl )
    {
        const size_t    sz_orig = contr . size ( ) ;

        contr . unique ( pred ) ;

        const size_t    sz_res  = contr . size ( ) ;
        if ( sz_orig != sz_res*n_dupl )
            BOOST_ERROR ( "\n  !: ERROR unique_pred method ;\n" ) ;
    }


    //  input data must be ordered:
    template < class _Contr >
    void merge ( _Contr &  contr )
    {
        if ( contr . empty ( ) )
            BOOST_ERROR ( "\n  !: ERROR invalid input container ;\n" ) ;

        _Contr      orig ( contr ) ;
        _Contr      copy ( contr ) ;
        contr . merge ( copy ) ;

        if ( !copy.empty() )
            BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
        if ( contr.size() != 2*orig.size() )
            BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;

        typename _Contr::const_iterator
                    it_curr = contr . begin ( ) ,
                    it_next = contr . begin ( ) ;
        ++it_next ;
        for (  ; it_next != contr.end() ; ++it_curr , ++it_next )
        {
            if ( *it_next < *it_curr )
                BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
        }

        contr = orig ;
    }


    template < class _Contr , class _Pred >
    void merge_pred ( _Contr &  contr , _Pred const &  pred )
    {
        if ( contr . empty ( ) )
            BOOST_ERROR ( "\n  !: ERROR invalid input container ;\n" ) ;

        _Contr      orig ( contr ) ;
        _Contr      copy ( contr ) ;
        contr . merge ( copy , pred ) ;

        if ( !copy.empty() )
            BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
        if ( contr.size() != 2*orig.size() )
            BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;

        typename _Contr::const_iterator
                    it_curr = contr . begin ( ) ,
                    it_next = contr . begin ( ) ;
        ++it_next ;
        for (  ; it_next != contr.end() ; ++it_curr , ++it_next )
        {
            if ( pred ( *it_next , *it_curr ) )
                BOOST_ERROR ( "\n  !: ERROR merge method ;\n" ) ;
        }

        contr = orig ;
    }


    template < class _Contr >
    void push_pop_front ( _Contr &  contr )
    {
        typedef
        typename _Contr::value_type     ValType ;

        const ValType   v_front = contr . front ( ) ;
        const ValType   v_back  = contr . back  ( ) ;

        contr . push_front ( v_back ) ;
        if ( v_back != contr.front() )
            BOOST_ERROR ( "\n  !: ERROR push_front ;\n" ) ;

        contr . pop_front ( ) ;
        if ( v_front != contr.front() )
            BOOST_ERROR ( "\n  !: ERROR pop_back ;\n" ) ;
    }


    template < class _Contr >
    void remove ( _Contr & contr )
    {
        if ( contr . empty ( ) )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;
        contr . sort ( ) ;

        typedef typename _Contr::value_type     ValType  ;
        typedef typename _Contr::size_type      SizeType ;

        typename _Contr::const_iterator
                        citer = contr.end() ;
        //  value does not exist in container
        ValType         val_x = 2*(*(--citer)) ;

        typename _Contr::iterator
                        iter = contr.begin() ;
        size_t          sz   = contr.size()  ;
        move_forw ( iter , sz/2 ) ;

        //  add 3 elements
        contr . insert ( iter , val_x ) ;
        contr . push_front ( val_x ) ;
        contr . push_back  ( val_x ) ;

        SizeType        sz_bef = 0 ;
        SizeType        sz_aft = 0 ;

        sz_bef = contr . size ( ) ;
        contr . remove ( val_x ) ;
        sz_aft = contr . size ( ) ;

        if ( ( sz_bef-sz_aft ) != 3 )
            BOOST_ERROR ( "\n  !: ERROR remove ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr >
    void remove_if ( _Contr & contr )
    {
        if ( contr . empty ( ) )
        {
            BOOST_ERROR ( "\n  !: ERROR invalid input ;\n" ) ;
            return ;
        }

        _Contr          copy ( contr ) ;
        Even<size_t>    pr_even ;
        contr . remove_if ( pr_even ) ;
        if ( !contr.empty() )
            BOOST_ERROR ( "\n  !: ERROR remove_if ;\n" ) ;

        contr = copy ;
    }


    template < class _Contr >
    void update_value_seqce ( _Contr &  contr )
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
        typename _Contr::value_type
                        val_x= *iter ;

        //  existing value
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr != copy )
            BOOST_ERROR ( "\n  !: ERROR write sequence ;\n" ) ;
        if ( iter_bool.second != false )
            BOOST_ERROR ( "\n  !: ERROR write sequence ;\n" ) ;
        if ( iter_bool.first != iter )
            BOOST_ERROR ( "\n  !: ERROR write sequence ;\n" ) ;

        //  new value
        ++val_x ;
        iter_bool = contr . write ( iter , val_x ) ;
        if ( contr == copy )
            BOOST_ERROR ( "\n  !: ERROR write sequence ;\n" ) ;
        if ( iter_bool.second != true )
            BOOST_ERROR ( "\n  !: ERROR write sequence ;\n" ) ;
        if ( iter_bool.first != iter )
            BOOST_ERROR ( "\n  !: ERROR write sequence ;\n" ) ;

        if ( contr.size()!= sz )
            BOOST_ERROR ( "\n  !: ERROR write sequence ;\n" ) ;

        contr = copy ;
    }


    //  this method demonstrates an invalidation error for
    //  a sequence contaner with fast algorithm accumulate()
    template < class _Contr , class _Ty2 >
    void invalidation_error ( const _Contr &  contr_in ,
                              const _Ty2      //val_in
                            )
    {
        _Contr      contr ( contr_in ) ;
        size_t      sz_test = 512 ;
        size_t      n_dupl  = 1   ;
        fill_even_rand
                    ( contr , sz_test , n_dupl ) ;

        _Ty2        val_mid = contr . at ( sz_test/2 ) ;
        ++val_mid ;

        //  this is a demonstration of an unsafe write operation,
        //  modify mid value of a sequence through method at()
        {
            contr . at ( sz_test/2 ) = val_mid ;
        }

        //  alternative option:
        //  this is a safe variant of a write operation
    //    {
    //        typename _Contr::iterator
    //                it_mid = contr.begin() + sz_test/2 ;
    //        contr . write ( it_mid , val_mid ) ;
    //    }

        const _Ty2  sum_0    = _Ty2() ;
        _Ty2        sum_fast = _Ty2() ;
        _Ty2        sum_std  = _Ty2() ;

        typename _Contr::const_iterator
                    it_from = contr.begin() ,
                    it_to   = contr.begin() ;
        move_forw ( it_from , sz_test/4   ) ;
        move_forw ( it_to   , sz_test*3/4 ) ;

        sum_fast = contr.accumulate ( it_from , it_to , sum_0 ) ;
        sum_std  = std:: accumulate ( it_from , it_to , sum_0 ) ;

        if ( sum_fast != sum_std )
        {
            BOOST_ERROR ( "\n  !: ERROR in value of sum ;\n" ) ;
        }
    }


    template < class _Contr >
    void test_sequence ( _Contr &  contr , size_t  sz_test , size_t  n_dupl )
    {
        bool        is_unique = false ;

        fill_even_rand
                    ( contr , sz_test , n_dupl ) ;

        test_common ( contr , sz_test , n_dupl , is_unique ) ;

        constr_seqce ( contr ) ;
        assign       ( contr ) ;
        insert_seqce ( contr ) ;
        front_back   ( contr ) ;
        resize       ( contr ) ;

        at_oper_idx  ( contr ) ;
        capacity     ( contr ) ;

        reverse     ( contr ) ;
        splice      ( contr ) ;
        push_pop_front
                    ( contr ) ;
        remove      ( contr ) ;
        remove_if   ( contr ) ;

        update_value_seqce
                    ( contr ) ;

        Identity<size_t>    f_iden ;
        size_t              val_x = 0 ;
        accumulate ( contr , val_x , f_iden ) ;

        //  un-comment this line to get an error message for
        //  a sequence container with fast algorithm
        //  bp_tree_array_acc::accumulate()
        //invalidation_error ( contr , val_x ) ;


        //  make two copies for further testing
        _Contr      copy_1 ( contr ) ;
        _Contr      copy_2 ( contr ) ;

        sort        ( copy_1 ) ;
        std::greater<size_t>    comp ;
        sort_pred   ( copy_2 , comp ) ;

        //  input data ordered with duplicates
        unique      ( copy_1 , n_dupl ) ;
        std::equal_to<size_t>   equal ;
        unique_pred ( copy_2 , equal , n_dupl ) ;

        //  input data ordered and unique
        std::less<size_t>       cmp_less ;
        merge       ( copy_1 ) ;
        merge_pred  ( copy_1 , cmp_less ) ;
    }

}


#endif  //  _TEST_SEQUENCE_HPP
