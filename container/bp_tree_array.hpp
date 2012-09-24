/////////////////////////////////////////////////////////////////
//
//          Copyright Vadim Stadnik 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
/////////////////////////////////////////////////////////////////
//
//  See folder "../doc" for documentation
//
/////////////////////////////////////////////////////////////////


#ifndef _BP_TREE_ARRAY_HPP
#define _BP_TREE_ARRAY_HPP


#include <iterator>
#include <utility>
#include <memory>
#include <algorithm>
#include <vector>
#include "bpt_helpers.hpp"


_STD_EXT_ADV_OPEN


//
//  class template bp_tree_array represents a general purpose container 
//  that supports interfaces of the C++03 sequences and associative containers. 
//  A bp_tree_array implements a partition of an array into a list of sub-arrays, 
//  thus, the container elements are not stored in contiguous memory. 
//  A bp_tree_array can be used to store the elements with multiple or 
//  unique keys in the order specified by a comparison object. 
//  A bp_tree_array supports:
//  - random access iterators; 
//  - logarithmic time search operations using keys of the container elements 
//    if the elements are ordered;
//  - logarithmic time insert and erase operations for a single element 
//    anywhere within the container; 
//  - logarithmic time splice and split operations for any ranges of 
//    consecutive elements and any positions in the containers involved.
//
template
<
    class _Ty_Key ,
    class _Ty_Map ,
    class _Ty_Val ,
    class _KeyOfV ,
    class _MapOfV ,
    class _Pred   ,
    class _Alloc
>
class bp_tree_array
{
public:
    //  types
    typedef bp_tree_array < _Ty_Key , _Ty_Map , _Ty_Val ,
                            _KeyOfV , _MapOfV , _Pred   , _Alloc >
                                                this_type       ;
    typedef _Ty_Key                             key_type        ;
    typedef _Ty_Map                             mapped_type     ;
    typedef _Ty_Val                             value_type      ;
    typedef typename _Alloc::size_type          size_type       ;
    typedef typename _Alloc::difference_type    difference_type ;
    typedef typename _Alloc::pointer            pointer         ;
    typedef typename _Alloc::const_pointer      const_pointer   ;
    typedef typename _Alloc::reference          reference       ;
    typedef typename _Alloc::const_reference    const_reference ;

protected:

    struct  _NodeLight ;
    typedef _NodeLight *    _NodeLightPtr ;
    struct  _NodeHeavy ;
    typedef _NodeHeavy *    _NodeHeavyPtr ;

    struct  _NodeLight
    {
        _NodeLight ( value_type const &  ty_x = value_type ( ) ) :
            elem      ( ty_x ) ,
            p_heavy_predr( 0 )
        {  }

        reference           _elem( )        { return elem ; }
        const_reference     _elem( ) const  { return elem ; }

        value_type      elem ;
        _NodeHeavyPtr   p_heavy_predr ;

    private:
        _NodeLight &
        operator = ( const _NodeLight & ) ;
        _NodeLight ( const _NodeLight & ) ;
    } ;

    struct  _NodeHeavy
    {
        void             _set_node_light ( _NodeLightPtr  p_light )
                                         { p_node_light = p_light ; }
        _NodeLightPtr    _get_node_light ( ) const
                                         { return p_node_light ; }
        const_reference  _elem() const   { return p_node_light->elem ; }

        _NodeHeavyPtr   p_next  ;
        _NodeHeavyPtr   p_prev  ;
        _NodeHeavyPtr   p_succr ;
        _NodeHeavyPtr   p_predr ;
        size_type       m_subsz ;

    private:
        _NodeLightPtr   p_node_light ;
    } ;


public:

    //  base class for random access iterators
    class _iter_base
    {
    friend class bp_tree_array < _Ty_Key , _Ty_Map , _Ty_Val ,
                                 _KeyOfV , _MapOfV , _Pred   , _Alloc  > ;

    protected:
        _iter_base ( ) : m_index(0), m_ptr(0), m_p_cont(0) { }
        _iter_base ( difference_type   ind ,
                     _NodeLightPtr     pln ,
                     this_type const * ctr
                   ) :
                     m_index(ind), m_ptr(pln), m_p_cont((this_type*)ctr) { }
        _iter_base ( difference_type   ind ,
                     _NodeLightPtr     pln ,
                     this_type *       ctr
                   ) :
                     m_index(ind), m_ptr(pln), m_p_cont(ctr) { }

    public:
        bool operator == ( const _iter_base & it_x ) const
                         { return m_index == it_x.m_index ; }
        bool operator != ( const _iter_base & it_x ) const
                         { return m_index != it_x.m_index ; }
        bool operator <  ( const _iter_base & it_x ) const
                         { return m_index <  it_x.m_index ; }
        bool operator <= ( const _iter_base & it_x ) const
                         { return m_index <= it_x.m_index ; }
        bool operator >  ( const _iter_base & it_x ) const
                         { return m_index >  it_x.m_index ; }
        bool operator >= ( const _iter_base & it_x ) const
                         { return m_index >= it_x.m_index ; }

    protected:
        bool valid_data ( ) const
        {
            return m_p_cont!=0 && m_ptr!=0 ;
        }

        difference_type     _size ( )      const
            { return valid_data() ? m_p_cont->_size_dt() : 0 ; }
        _NodeLightPtr       _end  ( )      const
            { return valid_data() ? m_p_cont->_external_end() : 0 ; }
        _NodeLightPtr       _lt_pointer ( ) const    { return m_ptr    ; }
        difference_type     _index      ( ) const    { return m_index  ; }
        const this_type *   _container  ( ) const    { return m_p_cont ; }

        bool _index_valid ( difference_type  ind ) const
        {
            if ( !valid_data( ) )
                return false ;
            if ( ind<0 || ind>=_size( ) )
                return false ;
            return true ;
        }

        void _set_index ( const difference_type  ind )
        {
            m_index = ind ;
        }

        void _move_idx_impl ( const difference_type  dist_idx )
        {
            if ( dist_idx == 0 )
                return ;

            m_index += dist_idx ;
            if ( !_index_valid(m_index) )
            {
                m_ptr = _end ( ) ;
                return ;
            }

            difference_type     idx_node   = ( m_ptr==_end() ) ? _size() :
                                             ( m_index-dist_idx ) ;
            difference_type     dist_nodes = m_index - idx_node ;

            if ( dist_nodes >= 0 )
            {
                if ( size_type(dist_nodes) <= m_p_cont->_min_degree_ext() )
                {
                    while ( dist_nodes )
                    {
                        _inc_pointer ( m_ptr ) ;
                        --dist_nodes ;
                    }
                }
                else if ( size_type(dist_nodes) <= m_p_cont->_local_limit() )
                {
                    while ( dist_nodes )
                    {
                        _inc_pointer ( m_ptr ) ;
                        --dist_nodes ;
                    }
                }
                else
                {
                    m_ptr  = m_p_cont->_find_node_light( size_type(m_index) ) ;
                }
            }
            else
            {
                dist_nodes = -dist_nodes ;
                size_type   dist_temp = size_type ( dist_nodes ) ;
                if ( dist_temp <= m_p_cont->_min_degree_ext() )
                {
                    while ( dist_temp )
                    {
                        _dec_pointer ( m_ptr ) ;
                        --dist_temp  ;
                    }
                }
                else if ( dist_temp <= m_p_cont->_local_limit() )
                {
                    while ( dist_temp )
                    {
                        _dec_pointer ( m_ptr ) ;
                        --dist_temp ;
                    }
                }
                else
                {
                    m_ptr  = m_p_cont->_find_node_light( size_type(m_index) ) ;
                }
            }
        }

        void _increment ( )
        {
            ++m_index ;
            if ( _index_valid(m_index) )
            {
                _inc_pointer ( m_ptr ) ;
            }
            else
            {
                m_ptr  = _end ( ) ;
            }
        }

        void _decrement ( )
        {
            --m_index ;
            if ( _index_valid(m_index) )
            {
                _dec_pointer ( m_ptr ) ;
            }
            else
            {
                m_ptr  = _end ( ) ;
            }
        }

        difference_type     m_index  ;
        _NodeLightPtr       m_ptr    ;
        this_type *         m_p_cont ;
    } ;


    template < class _Cat, class _Val, class _Diff, class _Ptr, class _Ref >
    class _iter_base_stl : public _iter_base
    {
    public:
        typedef _Cat        iterator_category ;
        typedef _Val        value_type        ;
        typedef _Diff       difference_type   ;
        typedef _Diff       distance_type     ;
        typedef _Ptr        pointer           ;
        typedef _Ref        reference         ;

    protected:
        _iter_base_stl ( ) : _iter_base ( ) {  }
        _iter_base_stl ( difference_type   ind ,
                         _NodeLightPtr     pln ,
                         this_type const * ctr ) :
                         _iter_base ( ind, pln, ctr ) { }
        _iter_base_stl ( difference_type   ind ,
                         _NodeLightPtr     pln ,
                         this_type *       ctr ) :
                         _iter_base ( ind, pln, ctr ) { }
    } ;


    class iterator ;
    friend
    class iterator ;
    typedef _iter_base_stl< std::random_access_iterator_tag,
                            value_type, difference_type, pointer, reference >
                            _iter_base_stl_alias ;


    //  random access iterator
    class iterator : public _iter_base_stl_alias
    {
    friend class bp_tree_array < _Ty_Key , _Ty_Map , _Ty_Val ,
                                 _KeyOfV , _MapOfV , _Pred   , _Alloc  > ;

        iterator ( difference_type   ind ,
                   _NodeLightPtr     pln ,
                   this_type *       ctr ) :
                   _iter_base_stl_alias ( ind, pln, ctr ) { }

    public:
        iterator ( ) : _iter_base_stl_alias ( ) { }

        reference   operator*  ( ) const
                    { return _iter_base_stl_alias::m_ptr->_elem() ; }
        pointer     operator-> ( ) const    { return (&**this) ; }
        reference   operator[ ]( difference_type  ind ) const
                                            { return ( *(*this + ind) ) ; }

        iterator &  operator ++ ( )
                    { _iter_base_stl_alias::_increment() ; return (*this) ; }
        iterator &  operator -- ( )
                    { _iter_base_stl_alias::_decrement() ; return (*this) ; }
        iterator    operator ++ ( int )
                    { iterator tmp = *this ; ++(*this) ; return tmp ; }
        iterator    operator -- ( int )
                    { iterator tmp = *this ; --(*this) ; return tmp ; }
        iterator &  operator += ( difference_type _m )
                    { _iter_base_stl_alias::_move_idx_impl(+_m); return *this;}
        iterator &  operator -= ( difference_type _m )
                    { _iter_base_stl_alias::_move_idx_impl(-_m); return *this;}
        iterator    operator +  ( difference_type _m ) const
                    { iterator tmp = *this ; return ( tmp += _m ) ; }
        iterator    operator -  ( difference_type _m ) const
                    { iterator tmp = *this ; return ( tmp -= _m ) ; }

        difference_type operator -  ( const iterator & it_x ) const
                    { return ( _iter_base_stl_alias::m_index -
                               it_x._iter_base_stl_alias::m_index ) ; }
    } ;


    class const_iterator ;
    friend
    class const_iterator ;

    //  constant random access iterator
    class const_iterator : public _iter_base_stl_alias
    {
    friend class bp_tree_array < _Ty_Key , _Ty_Map , _Ty_Val ,
                                 _KeyOfV , _MapOfV , _Pred   , _Alloc  > ;

        const_iterator ( difference_type   ind ,
                         _NodeLightPtr     pln ,
                         this_type const * ctr ) :
                         _iter_base_stl_alias ( ind, pln, ctr ) { }

    public:
        const_iterator ( ) : _iter_base_stl_alias ( ) { }
        const_iterator ( const iterator &  iter ) :
            _iter_base_stl_alias ( iter._iter_base_stl_alias::m_index  ,
                                   iter._iter_base_stl_alias::m_ptr    ,
                                   iter._iter_base_stl_alias::m_p_cont ) { }

        const_reference     operator*  ( ) const
                            { return _iter_base_stl_alias::m_ptr->_elem() ; }
        const_pointer       operator-> ( ) const    { return ( &**this ) ; }
        const_reference     operator[ ]( difference_type  ind ) const
                                                    { return (*(*this+ind)) ; }

        const_iterator &    operator ++ ( )
                    { _iter_base_stl_alias::_increment() ; return (*this) ; }
        const_iterator &    operator -- ( )
                    { _iter_base_stl_alias::_decrement() ; return (*this) ; }
        const_iterator      operator ++ ( int )
                    { const_iterator tmp = *this ; ++(*this) ; return tmp ; }
        const_iterator      operator -- ( int )
                    { const_iterator tmp = *this ; --(*this) ; return tmp ; }
        const_iterator &    operator += ( difference_type _m )
                    { _iter_base_stl_alias::_move_idx_impl(+_m); return *this ;}
        const_iterator &    operator -= ( difference_type _m )
                    { _iter_base_stl_alias::_move_idx_impl(-_m); return *this ;}
        const_iterator      operator +  ( difference_type _m ) const
                    { const_iterator tmp = *this ; return ( tmp += _m ) ; }
        const_iterator      operator -  ( difference_type _m ) const
                    { const_iterator tmp = *this ; return ( tmp -= _m ) ; }

        difference_type     operator -  ( const const_iterator & cit_x ) const
                    { return ( _iter_base_stl_alias::m_index -
                               cit_x._iter_base_stl_alias::m_index ) ; }
    } ;


    //  constructors, copy, assignment and destructor
    bp_tree_array  ( const _Pred &     pred ,
                     bool              mul  ,
                     bool              ord  ,
                     const _Alloc &    alr  ) ;
    bp_tree_array  ( const this_type & that ) ;
    this_type &
    operator =     ( const this_type & that ) ;
    ~bp_tree_array ( ) ;

    _Alloc          get_allocator ( ) const { return m_allr_ty_val ; }

    //  iterators
    iterator        begin ( )
                    { return iterator      (0, _external_begin(), this) ; }
    const_iterator  begin ( ) const
                    { return const_iterator(0, _external_begin(), this) ; }
    iterator        end   ( )
                    { return iterator      (_size_dt(), _external_end(), this);}
    const_iterator  end   ( ) const
                    { return const_iterator(_size_dt(), _external_end(), this);}

    //  capacity
    size_type   size     ( ) const { return m_size_light ; }
    size_type   max_size ( ) const { return m_allr_ty_val.max_size() ; }
    bool        empty    ( ) const { return ( size ( ) == 0 ) ; }
    size_type   capacity ( ) const ;
    void        reserve  ( size_type   sz ) ;

    //  sequence element access
    reference           operator[] ( size_type  ind ) ;
    const_reference     operator[] ( size_type  ind ) const ;
    reference           at ( size_type  ind ) ;
    const_reference     at ( size_type  ind ) const ;

    reference           front ( ) ;
    const_reference     front ( ) const ;
    reference           back  ( ) ;
    const_reference     back  ( ) const ;

    //  sequence modifiers
    iterator    _insert_seqce ( iterator            pos ,
                                const value_type &  val ) ;
    void        _insert_seqce_count
                              ( iterator            pos ,
                                size_type           cnt ,
                                const value_type &  val ) ;
    template<class _InpIter>
    void        _insert_seqce_iter ( iterator  pos   ,
                                     _InpIter  pos_a ,
                                     _InpIter  pos_b )
                { _insert_iter_seqce ( pos , pos_a , pos_b ) ; }

    void  push_front( const value_type &  val ) ;
    void  push_back ( const value_type &  val ) ;
    void  pop_front ( ) ;
    void  pop_back  ( ) ;

    void  resize ( size_type           sz_new ) ;
    void  resize ( size_type           sz_new ,
                   const value_type &  val    ) ;

    void  reverse ( ) ;

    iterator splice ( iterator    pos    ,
                      this_type & that   ) ;
    iterator splice ( iterator    pos    ,
                      this_type & that   ,
                      iterator    pos_a  ) ;
    iterator splice ( iterator    pos    ,
                      this_type & that   ,
                      iterator    pos_a  ,
                      iterator    pos_b  ) ;

    void    remove ( const value_type &  val )
            {
                _equal_rem<value_type>   pr_equal ( val ) ;
                _remove_if ( pr_equal ) ;
            }
    template < class _PredRem >
    void    remove_if ( _PredRem  pred ) { _remove_if ( pred ) ; }

    void    unique ( ) { _unique_pred ( _equivalence<value_type>() ) ; }
    template < class _PredRem >
    void    unique ( _PredRem  pred ) { _unique_pred ( pred ) ; }

    template < class _PredMerge >
    void    merge  ( this_type &  that , _PredMerge  pred )
            { _merge_pred ( that , pred , value_type() ) ; }

    void    sort ( )                 { _sort_pred_vector ( key_comp() ) ; }
    template < class _PredSort >
    void    sort ( _PredSort  pred ) { _sort_pred_vector ( pred ) ;       }

    //  associative containers modifiers
    std::pair<iterator, bool>
                insert ( const value_type &  elem_x ) ;
    iterator    insert ( iterator  pos , const value_type &  elem_x ) ;
    template <class _InpIter>
    void        insert_set ( _InpIter  pos_a , _InpIter  pos_b )
                { _insert_iter_set ( pos_a , pos_b ) ; }
    template <class _InpIter>
    void        insert_map ( _InpIter  pos_a , _InpIter  pos_b )
                { _insert_iter_map ( pos_a , pos_b ) ; }

    size_type   erase ( const _Ty_Key &  key_x ) ;

    void        merge_map ( this_type &  that )
                {
                    typedef std::pair<_Ty_Key,_Ty_Map>  value_type_map ;
                    _less_sort<value_type_map>          key_comp ;
                    _merge_pred ( that , key_comp , value_type_map() ) ;
                }

    //  general containers modifiers
    void        clear ( )  { _clear ( ) ; }
    iterator    erase ( iterator     pos   ) ;
    iterator    erase ( iterator     pos_a ,
                        iterator     pos_b ) ;

    iterator    split ( iterator     pos   ,
                        this_type &  that  ) ;
    iterator    split ( iterator     pos_a ,
                        iterator     pos_b ,
                        this_type &  that  ) ;

    void        swap  ( this_type &  ctr_x ) ;

    void        write_shallow ( iterator             pos     ,
                                const mapped_type &  val_new ) ;
    iterator    write_deep    ( iterator             pos     ,
                                const value_type &   val_new ) ;

    //  associative containers observers
    _Pred       key_comp ( ) const { return m_k_comp  ; }

    //  associative containers operations
    iterator        find        ( const _Ty_Key &  key_x ) ;
    const_iterator  find        ( const _Ty_Key &  key_x ) const ;
    size_type       count       ( const _Ty_Key &  key_x ) const ;
    iterator        lower_bound ( const _Ty_Key &  key_x ) ;
    const_iterator  lower_bound ( const _Ty_Key &  key_x ) const ;
    iterator        upper_bound ( const _Ty_Key &  key_x ) ;
    const_iterator  upper_bound ( const _Ty_Key &  key_x ) const ;
    std::pair<iterator, iterator>
                    equal_range ( const _Ty_Key &  key_x ) ;
    std::pair<const_iterator, const_iterator>
                    equal_range ( const _Ty_Key &  key_x ) const ;

    //  specialized algorithms
    mapped_type     accumulate  ( const_iterator   pos_a  ,
                                  const_iterator   pos_b  ,
                                  mapped_type      val_in ) const ;


protected:
    static difference_type  _offset()  { return 1 ; }
    static size_type        _size_block_cells ( )
                            { return ( _max_degree_ext() +
                                       2*size_type(_offset()) + 2 ) ; }

    _NodeHeavyPtr   _parent           ( _NodeLightPtr   p_lt_pos  ) const ;
    bool            _is_child_position( _NodeHeavyPtr   p_parent  ,
                                        _NodeLightPtr   p_lt_pos  ) const ;

    static void     _inc_pointer ( _NodeLightPtr &  ptr_lt ) ;
    static void     _dec_pointer ( _NodeLightPtr &  ptr_lt ) ;

    _NodeLightPtr   _create_block        ( _NodeHeavyPtr   p_parent  ,
                                           size_type       capacity  ) ;
    _NodeLightPtr   _create_block_end    ( _NodeHeavyPtr   p_parent  ) ;
    void            _delete_block_shallow( _NodeHeavyPtr   p_parent  ) ;
    void            _delete_block_deep   ( _NodeHeavyPtr   p_parent  ) ;
    void            _delete_block_end    ( _NodeLightPtr   p_lt_elem ) ;

    _NodeHeavyPtr   _insert_block        ( _NodeLightPtr       p_lt_pos ,
                                           const value_type &  val_x    ) ;
    void            _insert_block        ( _NodeHeavyPtr       p_parent ,
                                           _NodeLightPtr       p_lt_pos ,
                                           const value_type &  val_x    ) ;
    _NodeHeavyPtr   _erase_block         ( _NodeLightPtr       p_lt_pos ) ;
    void            _construct           ( pointer             ptr      ,
                                           const value_type &  val      ) ;

   _NodeHeavyPtr    _create_node_heavy ( _NodeLightPtr     p_light  ) ;
    void            _delete_node_heavy ( _NodeHeavyPtr &   p_node   ) ;
    void            _create_head_heavy ( ) ;

    void     _init ( ) ;
    void     _copy ( const this_type &  that ) ;
    void     _clear   ( ) ;
    void     _destroy ( ) ;

    std::pair<iterator, bool>
             _insert_ordered         ( const value_type &  elem_x  ) ;
    iterator _insert_seqce_posn      ( difference_type     index   ,
                                       _NodeLightPtr       p_lt_pos,
                                       const value_type &  elem_x  ) ;
    void     _insert_b_tree          ( _NodeHeavyPtr   p_parent    ,
                                       _NodeLightPtr & p_lt_elem_new ) ;
    void     _insert_heavy_node      ( _NodeHeavyPtr   p_posn      ,
                                       _NodeLightPtr   p_lt_elem_0 ) ;
    void     _insert_heavy_node_botm ( _NodeHeavyPtr   p_posn      ) ;
    void     _insert_link_node_botm  ( _NodeHeavyPtr   p_posn      ) ;
    void     _insert_b_tree_heavy_node(_NodeHeavyPtr   p_parent    ,
                                       _NodeHeavyPtr   p_parent_up ) ;
    void     _split_block            ( _NodeHeavyPtr   p_parent    ,
                                       _NodeLightPtr & p_lt_pos_upd) ;
    void     _split_block_unbalanced ( _NodeHeavyPtr   p_parent    ,
                                       _NodeLightPtr   p_lt_pos    ,
                                       _NodeLightPtr & p_lt_pos_upd) ;
    void     _split_node             ( _NodeHeavyPtr   p_parent    ,
                                       _NodeHeavyPtr   p_pos       ) ;
    void     _insert_top_level     ( ) ;
    void     _increase_tree_height ( ) ;

    void     _push_back_count        ( size_type          cnt      ,
                                       const value_type & val      ) ;
    void     _fill_value_count       ( size_type          cnt      ,
                                       const value_type & val      ) ;
    void     _insert_b_tree_count    ( _NodeHeavyPtr      p_parent ,
                                       size_type          cnt_elem ) ;

    iterator _erase_bp_tree    ( iterator              pos       ) ;
    void     _erase_heavy_node ( _NodeHeavyPtr &       p_posn    ) ;
    void     _erase_top_level  ( ) ;
    void     _clear_botm_level ( ) ;
    void     _merge_block      ( _NodeHeavyPtr         p_parent  ,
                                 _NodeLightPtr &       p_lt_res_upd) ;
    void     _merge_node_with  ( _NodeHeavyPtr         p_left    ,
                                 size_type &           subsize   ) ;
    void     _balance          ( const difference_type n_ch_left ,
                                 const difference_type n_ch_right,
                                 const _NodeHeavyPtr   p_mid     ) ;
    void     _balance_external ( const _NodeHeavyPtr   p_parent  ,
                                 _NodeLightPtr &       p_lt_res_upd) ;
    void     _update_data      ( const _NodeHeavyPtr   p_parent  ,
                                 const size_type       sub_sz    ) ;
    void     _link_to_parent   ( _NodeHeavyPtr         p_parent  ) ;
    bool     _find_two_groups  ( const _NodeHeavyPtr   ptr_level ,
                                 const _NodeHeavyPtr   ptr_input ,
                                 _NodeHeavyPtr &       ptr_left  ,
                                 _NodeHeavyPtr &       ptr_mid   ,
                                 _NodeHeavyPtr &       ptr_right ) const ;

    difference_type
                _size_dt ( )  const { return difference_type(size()) ; }
    size_type   _count_level  ( _NodeHeavyPtr          p_l_end ) const ;
    size_type   _count_level  ( const _NodeHeavyPtr    p_from  ,
                                const _NodeHeavyPtr    p_to    ) const ;
    void        _move_level   ( _NodeHeavyPtr &        ptr     ,
                                const difference_type  n_move  ) ;
    size_type   _move_subsize ( _NodeHeavyPtr &        ptr     ,
                                const difference_type  n_move  ) ;

    size_type       _local_limit ( ) const ;
    _NodeHeavyPtr   _bottom_begin( ) const
                    { return m_p_head_heavy->p_predr->p_next ; }
    _NodeHeavyPtr   _bottom_end  ( ) const
                    { return m_p_head_heavy->p_predr ; }
    _NodeHeavyPtr   _top_begin   ( ) const
                    { return m_p_head_heavy->p_succr->p_next ; }
    _NodeHeavyPtr   _top_end     ( ) const
                    { return m_p_head_heavy->p_succr ; }

    _NodeLightPtr   _external_begin ( ) const
    {
        _NodeHeavyPtr   p_hn_beg = (m_size_light>0)?_bottom_begin():
                                                    _bottom_end()    ;
        _NodeLightPtr   p_lt_res = p_hn_beg->_get_node_light() ;
        return p_lt_res ;
    }
    _NodeLightPtr   _external_end   ( ) const
    {
        _NodeHeavyPtr   p_hn_end = _bottom_end() ;
        _NodeLightPtr   p_lt_res = p_hn_end->_get_node_light() ;
        return p_lt_res ;
    }
    _NodeLightPtr   _external_last  ( ) const
    {
        _NodeHeavyPtr   p_parent = _bottom_end()->p_prev ;
        _NodeLightPtr   p_lt_pos = p_parent->_get_node_light() ;
        difference_type n_last   = difference_type( p_parent->m_subsz ) ;
        p_lt_pos += n_last ;
        --p_lt_pos ;
        return p_lt_pos ;
    }


    void            _find_lower_bound(const _Ty_Key &   key_x   ,
                                      difference_type & index   ,
                                     _NodeHeavyPtr &    p_parent,
                                      _NodeLightPtr &   p_lt_pos) const ;
    void            _find_upper_bound(const _Ty_Key &   key_x   ,
                                      difference_type & index   ,
                                     _NodeHeavyPtr &    p_parent,
                                      _NodeLightPtr &   p_lt_pos) const ;
    _NodeLightPtr   _find_node_light (const size_type   idx_pos ) const ;

    size_type       _tree_height ( ) const ;
    bool            _size_second_top_is_less
                                     ( const size_type  sz_lim  ) const ;
    _NodeHeavyPtr   _move_to_parent  ( _NodeHeavyPtr    p_1st_ch) const ;

    iterator        _splice_impl ( iterator         pos     ,
                                   this_type &      that    ,
                                   iterator         pos_a   ,
                                   iterator         pos_b   ) ;

    template<class _InpIter>
    void _push_back_array ( _InpIter  pos_a, _InpIter  pos_b )
    {
        size_type const sz_fill  = _max_degree_ext() - 1 ;
        size_type       cnt_elem = 0 ;
        size_type       cnt_node = 1 ;
        _NodeLightPtr   p_lt_cur = _bottom_end()->p_prev->_get_node_light() ;
        pointer         p_elem   = 0 ;

        while ( pos_a != pos_b )
        {
            p_elem = &(p_lt_cur->elem) ;
            _construct ( p_elem , *pos_a ) ;
            p_lt_cur->p_heavy_predr = 0 ;
            ++m_size_light ;
            ++cnt_elem ;
            ++p_lt_cur ;
            ++pos_a ;

            if ( pos_a == pos_b )
                break ;

            if ( cnt_elem == sz_fill )
            {
                _insert_b_tree_count ( _bottom_end()->p_prev , cnt_elem ) ;
                _insert_link_node_botm ( _bottom_end() ) ;
                ++cnt_node ;
                cnt_elem = 0 ;
                p_lt_cur = _bottom_end()->p_prev->_get_node_light() ;
            }
        }

        if ( cnt_node == 1 )
            _bottom_end()->p_prev->m_subsz = cnt_elem ;

        if ( cnt_node > 1 )
        {
            _insert_b_tree_count ( _bottom_end()->p_prev , cnt_elem ) ;
            _NodeLightPtr   p_lt_upd = 0 ;
            _balance_external ( _bottom_end()->p_prev->p_prev , p_lt_upd ) ;
        }
    }


    template < class _val_type >
    struct _less_sort
    {
        bool operator ( ) ( const _val_type &  _x , const _val_type &  _y ) const
        {
            return _Pred()( _KeyOfV()(_x) , _KeyOfV()(_y) ) ;
        }
    } ;

    template < class _val_type >
    struct _equivalence
    {
        bool operator ( ) ( const _val_type &  _x , const _val_type &  _y ) const
        {
            if ( _Pred()( _KeyOfV()(_x) , _KeyOfV()(_y) ) ||
                 _Pred()( _KeyOfV()(_y) , _KeyOfV()(_x) )    )
                return false ;
            return true ;
        }
    } ;

    template < class _val_type >
    struct _equal_rem
    {
        _equal_rem ( const _val_type &  _v ) : m_val ( _v ) {  }

        bool operator ( ) ( const _val_type &  _x ) const
        {
            if ( _Pred()( _KeyOfV()(_x   ) , _KeyOfV()(m_val) ) ||
                 _Pred()( _KeyOfV()(m_val) , _KeyOfV()(_x   ) )    )
                return false ;
            return true ;
        }

    private:
        const _val_type &   m_val ;
        _equal_rem &
        operator = ( const _equal_rem & ) ;
    } ;


    template <class _InpIter , class value_type_sort >
    void _insert_iter_sort ( _InpIter  pos_a , _InpIter  pos_b , const value_type_sort & )
    {
        _less_sort<value_type_sort>         less  ;
        _equivalence<value_type_sort>       equal ;

        std::vector<value_type_sort>        vec_data ( pos_a , pos_b ) ;
        std::sort ( vec_data.begin() , vec_data.end() , less ) ;

        typename
        std::vector<value_type_sort>::iterator  end_data = vec_data.end() ;
        if ( ! this->m_multi )
            end_data = std::unique ( vec_data.begin() , vec_data.end() , equal ) ;

        if ( this->empty() )
        {
            this->_push_back_array ( vec_data.begin() , end_data ) ;
            return ;
        }

        const size_type     sz_this = this->size() ;
        const size_type     sz_add  = size_type ( end_data-vec_data.begin() )  ;
        const size_type     cost    = _max_degree_ext() ;
        if ( sz_add*cost < sz_this )
        {
            typename
            std::vector<value_type_sort>::iterator
                            cur = vec_data.begin() ;
            for (  ; cur != end_data ; ++cur )
                this->insert ( *cur ) ;
        }
        else
        {
            std::vector<value_type_sort>    vec_this ( this->begin() , this->end() ) ;
            this->clear() ;

            std::vector<value_type_sort>    vec_res ( sz_this + sz_add ) ;
            typename
            std::vector<value_type_sort>::iterator
                                            end_res ;

            end_res = std::merge
                      ( vec_this.begin() , vec_this.end() ,
                        vec_data.begin() , end_data       ,
                        vec_res .begin() , less           ) ;

            if ( ! this->m_multi )
                end_res = std::unique ( vec_res.begin(), vec_res.end(), equal ) ;

            vec_this . clear() ;
            vec_data . clear() ;
            this->_push_back_array ( vec_res.begin() , end_res ) ;
        }
    }

    template <class _InpIter>
    void _insert_iter_map ( _InpIter  pos_a , _InpIter  pos_b )
    {
        typedef std::pair<_Ty_Key,_Ty_Map>  value_type_map ;
        value_type_map                      vt_dummy ;
        _insert_iter_sort ( pos_a , pos_b , vt_dummy ) ;
    }

    template <class _InpIter>
    void _insert_iter_set ( _InpIter  pos_a , _InpIter  pos_b )
    {
        value_type                          vt_dummy ;
        _insert_iter_sort ( pos_a , pos_b , vt_dummy ) ;
    }

    template<class _InpIter>
    void _insert_iter_seqce ( iterator  pos, _InpIter  pos_a, _InpIter  pos_b )
    {
        if ( pos_a == pos_b )
            return ;

        _InpIter  pos_x ( pos_a ) ;
        if ( ++pos_x == pos_b )
        {
            value_type      val_a ( *pos_a ) ;
            _insert_seqce ( pos , val_a ) ;
            return ;
        }

        this_type   tree_cnt ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
        tree_cnt . _push_back_array ( pos_a, pos_b ) ;
        this->_splice_other ( pos , tree_cnt , tree_cnt.begin() , tree_cnt.end() ) ;
    }

    template < class _PredRem >
    void _remove_if ( _PredRem  pred )
    {
        iterator        end_res ;
        end_res = std::remove_if ( this->begin() , this->end() , pred ) ;
        this->erase ( end_res , this->end() ) ;
    }

    template < class _PredRem >
    void _unique_pred ( _PredRem  pred )
    {
        iterator        end_res ;
        end_res = std::unique ( this->begin() , this->end() , pred ) ;
        this->erase ( end_res , this->end() ) ;
    }

    template < class _PredMerge , class value_type_impl >
    void _merge_pred ( this_type &  that , _PredMerge  pred , const value_type_impl & )
    {
        if ( this==&that )
            return ;

        if ( that . empty ( ) )
            return ;

        iterator        a_beg = this->begin() ;
        iterator        a_cur = a_beg ;
        iterator        a_end = this->end  () ;
        iterator        b_beg = that. begin() ;
        iterator        b_cur = b_beg ;
        iterator        b_end = that. end  () ;
        difference_type a_front_dist = 0 ;

        if ( this->empty() )
        {
            this->splice ( a_end , that ) ;
            return ;
        }

        if ( pred ( *(b_end-1) , *a_beg ) )
        {
            this->splice ( a_beg , that ) ;
            return ;
        }

        if ( m_multi )
        {
            if ( !pred( *b_beg , *(a_end-1) ) )
            {
                this->splice ( a_end , that ) ;
                return ;
            }
        }
        else
        {
            if ( pred( *(a_end-1) , *b_beg ) )
            {
                this->splice ( a_end , that ) ;
                return ;
            }
        }


        this_type       b_front_seqce ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
        this_type       b_tail_seqce  ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
        this_type       middle_seqce  ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;

        if ( pred ( *b_beg , *a_beg ) )
        {
            iterator    pos_a_beg = std::lower_bound ( b_beg , b_end , *a_beg , pred ) ;
            b_front_seqce . splice ( b_front_seqce.end() , that , b_beg , pos_a_beg ) ;
            b_beg = that. begin() ;
            b_cur = b_beg ;
            b_end = that. end() ;
        }
        else
        {
            iterator    pos_b_beg = std::lower_bound ( a_beg , a_end , *b_beg , pred ) ;
            a_front_dist = pos_b_beg - a_beg ;
            a_beg = pos_b_beg ;
            a_cur = pos_b_beg ;
        }

        std::vector<value_type_impl>     vec_temp ;
        while ( ( a_cur < a_end ) && ( b_cur < b_end ) )
        {
            if      ( pred ( *b_cur , *a_cur ) )
            {
                vec_temp . push_back ( *b_cur ) ; ++b_cur ;
            }
            else
            {
                vec_temp . push_back ( *a_cur ) ; ++a_cur ;
            }
        }

        typename
        std::vector<value_type_impl>::iterator   temp_end = vec_temp.end() ;
        if ( ! m_multi )
        {
            _equivalence<value_type_impl>    equal ;
            temp_end = std::unique ( vec_temp.begin(), vec_temp.end(), equal ) ;

            while ( (b_cur < b_end)  && !pred( *(a_end-1), *b_cur ) )
                ++b_cur ;
        }

        if ( b_cur < b_end )
            b_tail_seqce . splice ( b_tail_seqce.end() , that , b_cur , b_end ) ;

        that . clear ( ) ;

        middle_seqce . splice ( middle_seqce.end() , *this , a_beg , a_cur ) ;
        middle_seqce . clear  ( ) ;
        middle_seqce . _insert_seqce_iter ( middle_seqce.end() ,
                                            vec_temp.begin() , temp_end ) ;
        vec_temp . clear ( ) ;

        this->splice ( this->begin() +
                       a_front_dist  , middle_seqce  ) ;
        this->splice ( this->begin() , b_front_seqce ) ;
        this->splice ( this->end()   , b_tail_seqce  ) ;
    }

    void    sort_tree ( )                   { _sort_pred_tree   ( key_comp() ) ; }
    template < class _PredSort >
    void    sort_tree   ( _PredSort  pred ) { _sort_pred_tree   ( pred ) ;       }

    void    sort_vector ( )                 { _sort_pred_vector ( key_comp() ) ; }
    template < class _PredSort >
    void    sort_vector ( _PredSort  pred ) { _sort_pred_vector ( pred ) ;       }

    template < class _PredSort >
    void _sort_pred_tree ( _PredSort  pred )
    {
        bp_tree_array <_Ty_Key, mapped_type, value_type,
                       _KeyOfV, _MapOfV, _PredSort, _Alloc>
            contr_ord( pred, this->m_multi, true, this->get_allocator() ) ;

        while ( ! this->empty() )
        {
            contr_ord.insert ( *(this->begin()) ) ;
            this->erase      (   this->begin()  ) ;
        }

        while ( !contr_ord.empty() )
        {
            this->push_back   (*(contr_ord.begin()) ) ;
            contr_ord . erase (  contr_ord.begin()  ) ;
        }
    }

    template < class _PredSort >
    void _sort_pred_vector ( _PredSort  pred )
    {
        std::vector<value_type>     vect_copy ( this->begin() , this->end() ) ;

        this->clear() ;
        std::sort ( vect_copy.begin() , vect_copy.end() , pred ) ;
        this->_push_back_array ( vect_copy.begin() , vect_copy.end() ) ;
    }

    iterator        _splice_other( iterator         pos     ,
                                   this_type &      other   ,
                                   iterator         pos_a   ,
                                   iterator         pos_b   ) ;
    iterator        _splice_self ( iterator         pos     ,
                                   iterator         pos_a   ,
                                   iterator         pos_b   ) ;
    void            _splice_tree ( this_type &      that    ) ;
    void            _split_tree  ( iterator         pos     ,
                                   this_type &      that    ) ;
    void            _restore_balance
                                 ( const bool       front   ,
                                   const size_type  n_level ,
                                   const size_type  i_balan ) ;
    void            _reconnect   ( this_type &      other   ) ;


    //  data members
    typedef typename _Alloc::template rebind<_NodeLight>::other
                                _NodeLightAllocr  ;

    _NodeLightAllocr            m_allr_node_light ;
    size_type                   m_size_light      ;

    typedef typename _Alloc::template rebind<_NodeHeavy>::other
                                _NodeHeavyAllocr  ;

    _NodeHeavyAllocr            m_allr_node_heavy ;
    _Alloc                      m_allr_ty_val     ;
    _NodeHeavyPtr               m_p_head_heavy    ;
    _Pred                       m_k_comp          ;
    bool                        m_multi           ;
    bool                        m_ordered         ;

    static size_type _min_degree_int() { return 8 ; }
    static size_type _max_degree_int() { return 2*_min_degree_int() ; }
    static size_type _min_degree_ext() { return 64 ; }
    static size_type _max_degree_ext() { return 2*_min_degree_ext() ; }

} ;


#define TEMPL_DECL  template< class _Ty_Key , class _Ty_Map , class _Ty_Val , \
                              class _KeyOfV , class _MapOfV , \
                              class _Pred   , class _Alloc  > inline
#define BP_TREE_TY  bp_tree_array < _Ty_Key , _Ty_Map , _Ty_Val , \
                                    _KeyOfV , _MapOfV , _Pred   , _Alloc >


TEMPL_DECL
typename BP_TREE_TY::_NodeHeavyPtr
BP_TREE_TY::_parent ( _NodeLightPtr  p_lt_pos ) const
{
    while ( p_lt_pos->p_heavy_predr == 0 )
    {
        ++p_lt_pos ;
    }

    _NodeHeavyPtr   p_parent = p_lt_pos->p_heavy_predr ;
    return p_parent ;
}


TEMPL_DECL
bool
BP_TREE_TY::_is_child_position ( _NodeHeavyPtr   p_parent ,
                                 _NodeLightPtr   p_lt_pos ) const
{
    difference_type n      = difference_type ( p_parent->m_subsz ) ;
    _NodeLightPtr   p_lt_0 = p_parent->_get_node_light() ;
    _NodeLightPtr   p_lt_n = p_lt_0 + n ;
    return ( p_lt_0 <= p_lt_pos ) && ( p_lt_pos < p_lt_n ) ;
}


TEMPL_DECL
void
BP_TREE_TY::_inc_pointer ( _NodeLightPtr &  ptr_lt )
{
    ++ptr_lt ;
    if ( ptr_lt->p_heavy_predr != 0 )
    {
        ptr_lt = ptr_lt->p_heavy_predr->p_next->_get_node_light() ;
    }
}


TEMPL_DECL
void
BP_TREE_TY::_dec_pointer ( _NodeLightPtr &  ptr_lt )
{
    --ptr_lt ;
    if ( ptr_lt->p_heavy_predr != 0 )
    {
        _NodeHeavyPtr   p_parent = ptr_lt->p_heavy_predr ;
        p_parent = p_parent->p_prev ;
        ptr_lt   = p_parent->_get_node_light() ;
        ptr_lt  += difference_type ( p_parent->m_subsz ) ;
        --ptr_lt ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::_NodeLightPtr
BP_TREE_TY::_create_block ( _NodeHeavyPtr  p_parent    ,
                            size_type      capacity    )
{
    size_type           sz_block   = _size_block_cells ( ) ;
    difference_type     n_ptr      = difference_type(sz_block) ;
    _NodeLightPtr       p_lt_block = m_allr_node_light . allocate ( sz_block , 0 ) ;
    _NodeLightPtr       pos_end    = p_lt_block + _offset() ;
    difference_type     i     = 0 ;
    difference_type     n_off = _offset() + difference_type(capacity) ;

    for ( i = 0 ; i < _offset() ; ++i , ++p_lt_block )
    {
        p_lt_block->p_heavy_predr = p_parent ;
    }

    for ( i = _offset() ; i < n_off ; ++i , ++p_lt_block )
    {
        p_lt_block->p_heavy_predr = 0 ;
    }

    for ( i = n_off ; i < n_ptr ; ++i , ++p_lt_block )
    {
        p_lt_block->p_heavy_predr = p_parent ;
    }

    return pos_end ;
}


TEMPL_DECL
typename BP_TREE_TY::_NodeLightPtr
BP_TREE_TY::_create_block_end ( _NodeHeavyPtr  p_parent )
{
    size_type           sz_end     = size_type(1 + 2*_offset()) ;
    _NodeLightPtr       p_lt_block = m_allr_node_light . allocate ( sz_end , 0 ) ;
    _NodeLightPtr       pos_end    = p_lt_block + _offset() ;

    for ( size_type     k = 0 ; k < sz_end ; ++k , ++p_lt_block )
    {
        p_lt_block->p_heavy_predr = p_parent ;
    }

    return pos_end ;
}


TEMPL_DECL
typename BP_TREE_TY::_NodeHeavyPtr
BP_TREE_TY::_insert_block ( _NodeLightPtr       p_lt_pos ,
                            const value_type &  val_x    )
{
    _NodeHeavyPtr   p_parent = _parent ( p_lt_pos ) ;
    _insert_block ( p_parent , p_lt_pos , val_x ) ;
    return p_parent ;
}


TEMPL_DECL
void
BP_TREE_TY::_insert_block ( _NodeHeavyPtr       p_parent ,
                            _NodeLightPtr       p_lt_pos ,
                            const value_type &  val_x    )
{
    difference_type n_elems = difference_type ( p_parent->m_subsz ) ;
    _NodeLightPtr   p_lt_cur= p_parent->_get_node_light() ;
    p_lt_cur += n_elems ;
    pointer         p_elem  = &(p_lt_cur->elem) ;

    p_lt_cur->p_heavy_predr = 0 ;

    while ( p_lt_cur != p_lt_pos )
    {
        _construct ( p_elem , (p_lt_cur-1)->_elem() ) ;
        --p_lt_cur ;
        p_elem = &(p_lt_cur->elem) ;
        m_allr_ty_val . destroy ( p_elem ) ;
    }

    _construct ( p_elem , val_x ) ;
    ++m_size_light ;
}


TEMPL_DECL
typename BP_TREE_TY::_NodeHeavyPtr
BP_TREE_TY::_erase_block ( _NodeLightPtr  p_lt_pos )
{
    _NodeHeavyPtr   p_parent = 0 ;
    pointer         p_elem   = 0 ;

    ++p_lt_pos ;
    while ( p_lt_pos->p_heavy_predr == 0 )
    {
        p_elem = &( (p_lt_pos-1)->_elem() ) ;
        m_allr_ty_val . destroy ( p_elem ) ;
        _construct ( p_elem , p_lt_pos->_elem() ) ;
        ++p_lt_pos ;
    }

    p_parent = p_lt_pos->p_heavy_predr ;
    --p_lt_pos ;
    p_lt_pos->p_heavy_predr = p_parent ;
    p_elem = &(p_lt_pos->_elem()) ;
    m_allr_ty_val . destroy ( p_elem ) ;
    --m_size_light ;

    return p_parent ;
}


TEMPL_DECL
void BP_TREE_TY::_delete_block_shallow ( _NodeHeavyPtr  p_parent )
{
    _NodeLightPtr   p_lt_elem_0 = p_parent->_get_node_light() ;
    _NodeLightPtr   p_lt_arr    = p_lt_elem_0 - _offset() ;
    size_type       sz_block    = _size_block_cells ( ) ;
    m_allr_node_light . deallocate ( p_lt_arr , sz_block ) ;
}


TEMPL_DECL
void BP_TREE_TY::_delete_block_deep ( _NodeHeavyPtr  p_parent )
{
    _NodeLightPtr   p_lt_arr = p_parent->_get_node_light() ;
    p_lt_arr -= _offset()  ;
    m_size_light -= p_parent->m_subsz ;
    size_type       sz_elems = p_parent->m_subsz ;
    _NodeLightPtr   p_lt_i   = p_parent->_get_node_light() ;
    pointer         p_elem   = 0 ;
    for ( size_type i = 0 ; i < sz_elems ; ++i , ++p_lt_i )
    {
        p_elem = &(p_lt_i->elem) ;
        m_allr_ty_val . destroy ( p_elem ) ;
    }

    size_type       sz_block = _size_block_cells ( ) ;
    m_allr_node_light . deallocate ( p_lt_arr , sz_block ) ;
}


TEMPL_DECL
void BP_TREE_TY::_delete_block_end ( _NodeLightPtr  p_lt_elem )
{
    _NodeLightPtr   p_lt_arr = p_lt_elem ;
    p_lt_arr -= _offset()  ;
    size_type       sz_end   = size_type(1 + 2*_offset()) ;
    m_allr_node_light . deallocate ( p_lt_arr , sz_end ) ;
}


TEMPL_DECL
void BP_TREE_TY::_construct ( pointer  ptr , const value_type &  val )
{
    try
    {
        m_allr_ty_val . construct ( ptr , val ) ;
    }
    catch ( ... )
    {
        m_allr_ty_val . destroy ( ptr ) ;
        throw ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::_NodeHeavyPtr
BP_TREE_TY::_create_node_heavy ( _NodeLightPtr  p_light )
{
    _NodeHeavyPtr   p_n = m_allr_node_heavy . allocate ( 1 , 0 ) ;

    p_n->p_next  = 0 ;
    p_n->p_prev  = 0 ;
    p_n->p_succr = 0 ;
    p_n->p_predr = 0 ;
    p_n->m_subsz = 0 ;
    p_n->_set_node_light ( p_light ) ;

    return p_n ;
}


TEMPL_DECL
void BP_TREE_TY::_delete_node_heavy ( _NodeHeavyPtr &  p_node )
{
    if ( p_node )
    {
        m_allr_node_heavy . deallocate ( p_node , 1 ) ;
        p_node = 0 ;
    }
}


TEMPL_DECL
BP_TREE_TY::bp_tree_array ( const _Pred &   pred ,
                            bool            mul  ,
                            bool            ord  ,
                            const _Alloc &  alr  ) :
    m_allr_node_light ( alr ) ,
    m_size_light      (   0 ) ,
    m_allr_node_heavy ( alr ) ,
    m_allr_ty_val     ( alr ) ,
    m_p_head_heavy    (   0 ) ,
    m_k_comp          ( pred) ,
    m_multi           ( mul ) ,
    m_ordered         ( ord )
{
    _init ( ) ;
}


TEMPL_DECL
BP_TREE_TY::bp_tree_array ( const BP_TREE_TY &  that ) :
    m_allr_node_light ( that . m_allr_node_light ) ,
    m_size_light      (  0 ) ,
    m_allr_node_heavy ( that . m_allr_node_heavy ) ,
    m_allr_ty_val     ( that . m_allr_ty_val     ) ,
    m_p_head_heavy    (  0 ) ,
    m_k_comp          ( that . m_k_comp  ) ,
    m_multi           ( that . m_multi   ) ,
    m_ordered         ( that . m_ordered )
{
    _init ( ) ;
    _copy ( that ) ;
}


TEMPL_DECL
BP_TREE_TY &
BP_TREE_TY::operator = ( const BP_TREE_TY &  that )
{
    if ( this != &that )
    {
        _clear ( ) ;
        m_k_comp  = that . m_k_comp  ;
        m_multi   = that . m_multi   ;
        m_ordered = that . m_ordered ;
        _copy ( that ) ;
    }
    return ( *this ) ;
}


TEMPL_DECL
BP_TREE_TY::~bp_tree_array ( )
{
    _destroy ( ) ;
}


TEMPL_DECL
void BP_TREE_TY::_init ( )
{
    _create_head_heavy ( ) ;
    _insert_top_level  ( ) ;

    _NodeHeavyPtr   p_btm_end = _bottom_end() ;
    _NodeLightPtr   p_lt_end  = _create_block_end ( p_btm_end ) ;
    p_btm_end     ->_set_node_light ( p_lt_end ) ;
    m_p_head_heavy->_set_node_light ( p_lt_end ) ;

    _insert_heavy_node_botm ( p_btm_end ) ;
}


TEMPL_DECL
void BP_TREE_TY::_copy ( const BP_TREE_TY & that )
{
    const_iterator  cur = that . begin ( ) ;
    const_iterator  end = that . end   ( ) ;
    _push_back_array ( cur , end ) ;
}


TEMPL_DECL
void BP_TREE_TY::_clear ( )
{
    size_type       h = _tree_height ( ) ;
    while ( h > 1 )
    {
        _erase_top_level ( ) ;
        --h ;
    }

    _clear_botm_level ( ) ;
}


TEMPL_DECL
void BP_TREE_TY::_destroy ( )
{
    _clear ( ) ;

    _NodeHeavyPtr   p_cur = _bottom_begin ( ) ;
    _delete_block_shallow ( p_cur ) ;
    _erase_heavy_node     ( p_cur ) ;

    p_cur = _bottom_end ( ) ;
    _delete_block_end ( p_cur->_get_node_light() ) ;

    p_cur->_set_node_light ( 0 ) ;
    p_cur->p_predr->p_succr = p_cur->p_succr ;
    p_cur->p_succr->p_predr = p_cur->p_predr ;
    _delete_node_heavy ( p_cur ) ;

    m_p_head_heavy->_set_node_light ( 0 ) ;
    _delete_node_heavy ( m_p_head_heavy ) ;
}


TEMPL_DECL
void BP_TREE_TY::_create_head_heavy ( )
{
    m_p_head_heavy = _create_node_heavy ( 0 ) ;
    m_p_head_heavy->p_next  = 0 ;
    m_p_head_heavy->p_prev  = 0 ;
    m_p_head_heavy->p_succr = m_p_head_heavy ;
    m_p_head_heavy->p_predr = m_p_head_heavy ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_heavy_node ( _NodeHeavyPtr  p_posn      ,
                                      _NodeLightPtr  p_lt_elem_0 )
{
    _NodeHeavyPtr   p_new = _create_node_heavy ( p_lt_elem_0 ) ;
    _NodeHeavyPtr   p_bef = p_posn->p_prev ;
    p_new ->p_prev = p_bef  ;
    p_new ->p_next = p_posn ;
    p_posn->p_prev = p_new  ;
    p_bef ->p_next = p_new  ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_heavy_node_botm ( _NodeHeavyPtr  p_posn )
{
    _insert_heavy_node ( p_posn , 0 ) ;

    _NodeHeavyPtr   p_btm_new = p_posn->p_prev ;
    _NodeLightPtr   p_lt_0    = _create_block ( p_btm_new , 0 ) ;
    p_btm_new->_set_node_light ( p_lt_0 ) ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_link_node_botm ( _NodeHeavyPtr  p_posn )
{
    _NodeHeavyPtr  p_left = p_posn->p_prev ;
    _NodeHeavyPtr  p_new  = 0 ;

    _insert_heavy_node_botm ( p_posn ) ;
    p_new = p_left->p_next ;
    p_new->p_predr = p_left->p_predr ;
    p_new->p_succr = 0 ;
}


TEMPL_DECL
void BP_TREE_TY::_split_node
    (
        _NodeHeavyPtr   p_parent ,
        _NodeHeavyPtr   p_pos
    )
{
    _insert_heavy_node ( p_parent->p_next , p_pos->_get_node_light() ) ;

    _NodeHeavyPtr   p_par_new = p_parent->p_next ;
    p_par_new->p_predr = p_parent->p_predr ;
    p_par_new->p_succr = p_pos ;

    size_type       sub_sz = 0 ;
    _NodeHeavyPtr   p_tmp  = p_pos ;
    while ( p_tmp->p_predr == p_parent )
    {
        sub_sz        += p_tmp->m_subsz ;
        p_tmp->p_predr = p_par_new      ;
        p_tmp          = p_tmp->p_next  ;
    }

    p_par_new->m_subsz  = sub_sz ;
    p_parent ->m_subsz -= sub_sz ;
}


TEMPL_DECL
void BP_TREE_TY::_split_block ( _NodeHeavyPtr   p_parent     ,
                                _NodeLightPtr & p_lt_pos_upd )
{
    pointer         p_elem       = 0 ;
    size_type       sz_elems     = p_parent->m_subsz ;
    size_type       sz_left      = _min_degree_ext() ;
    size_type       sz_right     = sz_elems - sz_left ;
    _NodeLightPtr   p_lt_left_0  = p_parent->_get_node_light() ;
    difference_type dist_right   = -1 ;
    if ( _is_child_position ( p_parent , p_lt_pos_upd ) )
    {
        dist_right = ( p_lt_pos_upd - p_lt_left_0 ) -
                     difference_type( sz_left ) ;
    }

    _insert_link_node_botm ( p_parent->p_next ) ;

    _NodeHeavyPtr   p_par_new      = p_parent->p_next ;
    _NodeLightPtr   p_lt_cur_left  = p_lt_left_0 + difference_type( sz_left ) ;
    _NodeLightPtr   p_lt_cur_right = p_par_new->_get_node_light() ;
    for ( size_type i = 0 ; i < sz_right ; ++i, ++p_lt_cur_left, ++p_lt_cur_right )
    {
        p_elem = &(p_lt_cur_right->elem) ;
        _construct ( p_elem , p_lt_cur_left->_elem() ) ;
        p_lt_cur_right->p_heavy_predr = 0 ;
        p_elem = &(p_lt_cur_left->elem) ;
        m_allr_ty_val . destroy ( p_elem ) ;
        p_lt_cur_left->p_heavy_predr = p_parent ;
    }

    p_parent ->m_subsz = sz_left  ;
    p_par_new->m_subsz = sz_right ;

    if ( dist_right >= 0 )
    {
        p_lt_pos_upd   = p_par_new->_get_node_light() + dist_right ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_split_block_unbalanced
                              ( _NodeHeavyPtr    p_parent ,
                                _NodeLightPtr    p_lt_pos ,
                                _NodeLightPtr &  p_lt_pos_upd
                              )
{
    _insert_link_node_botm ( p_parent->p_next ) ;

    _NodeHeavyPtr   p_par_new  = p_parent->p_next ;
    pointer         p_elem     = 0 ;
    _NodeLightPtr   p_lt_left  = p_lt_pos ;
    _NodeLightPtr   p_lt_right = p_par_new->_get_node_light() ;
    size_type       sub_sz     = 0 ;
    while ( p_lt_left->p_heavy_predr == 0 )
    {
        p_elem = &(p_lt_right->elem) ;
        _construct ( p_elem , p_lt_left->_elem() ) ;
        p_lt_right->p_heavy_predr = 0 ;
        p_elem = &(p_lt_left->elem) ;
        m_allr_ty_val . destroy ( p_elem ) ;
        p_lt_left->p_heavy_predr = p_parent ;
        ++sub_sz ;
        ++p_lt_left  ;
        ++p_lt_right ;
    }

    p_parent ->m_subsz -= sub_sz ;
    p_par_new->m_subsz  = sub_sz ;
    p_lt_pos_upd = p_par_new->_get_node_light() ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_top_level ( )
{
    _NodeHeavyPtr   p_top   = m_p_head_heavy->p_succr ;
    _NodeLightPtr   p_light = m_p_head_heavy->_get_node_light() ;
    _NodeHeavyPtr   p_new   = _create_node_heavy ( p_light ) ;
    _NodeHeavyPtr   p_bef   = p_top->p_predr ;
    p_new->p_predr = p_bef ;
    p_new->p_succr = p_top ;
    p_top->p_predr = p_new ;
    p_bef->p_succr = p_new ;
    p_new->p_next  = p_new ;
    p_new->p_prev  = p_new ;
}


TEMPL_DECL
void BP_TREE_TY::_increase_tree_height ( )
{
    _NodeHeavyPtr   p_top_beg = _top_begin() ;
    _NodeHeavyPtr   p_top_end = m_p_head_heavy->p_succr ;
    _NodeLightPtr   p_lt_beg  = p_top_beg->_get_node_light() ;

    _insert_top_level  ( ) ;
    _insert_heavy_node ( m_p_head_heavy->p_succr, p_lt_beg ) ;

    _NodeHeavyPtr   p_new_top_beg = _top_begin() ;
    p_new_top_beg->p_succr = p_top_beg ;
    p_new_top_beg->p_predr = 0 ;
    p_new_top_beg->m_subsz = m_size_light ;

    while ( p_top_beg != p_top_end )
    {
        p_top_beg->p_predr = p_new_top_beg ;
        p_top_beg = p_top_beg->p_next ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_insert_b_tree_heavy_node
    (
        _NodeHeavyPtr   p_parent    ,
        _NodeHeavyPtr   p_parent_up
    )
{
    _NodeHeavyPtr   p_forw = 0 ;
    _NodeHeavyPtr   p_back = 0 ;
    size_type       n_back = 0 ;
    size_type       n_forw = 0 ;

    while ( p_parent_up != 0 )
    {
        p_forw = p_parent->p_next ;
        n_forw = 0 ;
        while ( p_forw->p_predr == p_parent_up )
        {   p_forw = p_forw->p_next ; ++n_forw ; }

        p_back = p_parent ;
        n_back = 0 ;
        while ( p_back->p_predr == p_parent_up )
        {   p_back = p_back->p_prev ; ++n_back ; }

        _NodeHeavyPtr   p_mid = p_parent->p_next ;
        p_parent    = p_parent_up ;
        p_parent_up = p_parent->p_predr ;

        if ( ( n_forw + n_back ) <= _max_degree_int() )
            break ;

        difference_type n_move = difference_type ( _min_degree_int() ) -
                                 difference_type ( n_back ) ;
        _move_level ( p_mid    , n_move ) ;
        _split_node ( p_parent , p_mid  ) ;
    }

    if ( ( p_parent_up==0 ) &&
         ( _count_level(m_p_head_heavy->p_succr) > _min_degree_int() ) )
    {
        _increase_tree_height ( ) ;
    }
}


TEMPL_DECL
std::pair<typename BP_TREE_TY::iterator, bool>
BP_TREE_TY::_insert_ordered ( const value_type &  elem_x )
{
    difference_type     index    = 0 ;
    _NodeHeavyPtr       p_parent = 0 ;
    _NodeLightPtr       p_lt_pos = 0 ;

    if ( m_multi )
    {
        _find_upper_bound ( _KeyOfV() ( elem_x ) , index , p_parent , p_lt_pos ) ;
    }
    else
    {
        _find_lower_bound ( _KeyOfV() ( elem_x ) , index , p_parent , p_lt_pos ) ;
        bool  contains = ( ( p_lt_pos!=_external_end() ) &&
              !m_k_comp( _KeyOfV()(elem_x), _KeyOfV()( p_lt_pos->_elem() )) ) ;
        if ( contains )
            return std::pair<iterator, bool>
                   ( iterator( index, p_lt_pos, this) , false ) ;
    }

    if ( p_lt_pos == _external_end() )
    {
        p_parent = _bottom_end()->p_prev ;
        difference_type   sz_last  = difference_type( p_parent->m_subsz ) ;
        p_lt_pos = p_parent->_get_node_light() ;
        p_lt_pos+= sz_last ;
    }

    _insert_block  ( p_parent , p_lt_pos , elem_x ) ;
    _insert_b_tree ( p_parent , p_lt_pos ) ;
    iterator    it_ins ( index , p_lt_pos , this ) ;
    return std::pair<iterator, bool> ( it_ins , true ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_insert_seqce_posn ( difference_type     index    ,
                                 _NodeLightPtr       p_lt_pos ,
                                 const value_type &  elem_x   )
{
    _NodeHeavyPtr   p_parent= 0 ;

    if ( p_lt_pos == _external_end() )
    {
        p_parent = _bottom_end()->p_prev ;
        difference_type   sz_last  = difference_type( p_parent->m_subsz ) ;
        p_lt_pos = p_parent->_get_node_light() ;
        p_lt_pos+= sz_last ;
        _insert_block  ( p_parent , p_lt_pos , elem_x ) ;
    }
    else
    {
        p_parent = _insert_block ( p_lt_pos , elem_x ) ;
    }

    _insert_b_tree ( p_parent , p_lt_pos ) ;
    iterator    it_ins ( index , p_lt_pos , this ) ;
    return it_ins ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_b_tree
    (
        _NodeHeavyPtr       p_parent      ,
        _NodeLightPtr &     p_lt_elem_new
    )
{
    _NodeHeavyPtr   p_tmp = p_parent ;
    while ( p_tmp != 0 )
    {
        ++( p_tmp->m_subsz ) ;
        p_tmp = p_tmp->p_predr ;
    }

    if ( p_parent->m_subsz <= _max_degree_ext() )
        return ;

    _split_block ( p_parent , p_lt_elem_new ) ;

    _NodeHeavyPtr   p_parent_up = p_parent->p_predr ;
    _insert_b_tree_heavy_node ( p_parent    ,
                                p_parent_up ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_erase_bp_tree ( iterator  pos )
{
    difference_type     idx            = pos._index() ;
    _NodeLightPtr       p_lt_pos_erase = pos._lt_pointer ( ) ;
    _NodeLightPtr       p_lt_pos_after = p_lt_pos_erase ;
    if ( (p_lt_pos_after+1)->p_heavy_predr != 0 )
        _inc_pointer ( p_lt_pos_after ) ;

    _NodeHeavyPtr       p_parent    = 0 ;
    _NodeHeavyPtr       p_level_cur = 0 ;
    _NodeHeavyPtr       p_level_top = 0 ;
    p_parent = _erase_block ( p_lt_pos_erase ) ;

    _NodeHeavyPtr   p_tmp = p_parent ;
    do
    {
        --( p_tmp->m_subsz ) ;
        p_tmp = p_tmp->p_predr ;
    }
    while ( p_tmp ) ;

    _NodeHeavyPtr       p_left  = 0 ;
    _NodeHeavyPtr       p_mid   = 0 ;
    _NodeHeavyPtr       p_right = 0 ;
    size_type           sub_size_tot = 0 ;
    bool                balanced = false ;

    p_level_cur = _bottom_end() ;
    _find_two_groups ( p_level_cur , p_parent , p_left , p_mid , p_right ) ;

    if ( p_mid && ( p_left->m_subsz + p_mid->m_subsz <= _max_degree_ext() ) )
    {
        _merge_block     ( p_left , p_lt_pos_after ) ;
        _merge_node_with ( p_left , sub_size_tot   ) ;
    }
    else
    {
        _balance_external( p_left , p_lt_pos_after ) ;
        balanced = true ;
    }

    iterator        it_res ( idx, p_lt_pos_after, this ) ;
    if ( balanced )
    {
        return it_res ;
    }

    size_type       n_ch_left   = 0 ;
    size_type       n_ch_right  = 0 ;
    _NodeHeavyPtr   p_par_break = 0 ;

    p_level_cur = _bottom_end() ;
    p_level_top = _top_end () ;
    while ( p_level_cur != p_level_top )
    {
        p_level_cur= p_level_cur->p_predr ;
        p_parent   = p_left     ->p_predr ;
        if ( !_find_two_groups ( p_level_cur , p_parent ,
                                 p_left , p_mid , p_right ) )
            break ;

        n_ch_left  = _count_level ( p_left->p_succr , p_mid  ->p_succr ) ;
        n_ch_right = _count_level ( p_mid ->p_succr , p_right->p_succr ) ;
        if ( ( n_ch_left + n_ch_right ) <= _max_degree_int() )
        {
            _link_to_parent  ( p_left ) ;
            _merge_node_with ( p_left , sub_size_tot ) ;
            continue ;
        }

        p_par_break = p_left ;
        break ;
    }

    _update_data ( p_par_break , sub_size_tot ) ;
    _balance ( difference_type(n_ch_left)  ,
               difference_type(n_ch_right) ,
               p_par_break                 ) ;

    if ( _size_second_top_is_less ( _min_degree_int() ) )
        _erase_top_level ( ) ;

    return it_res ;
}


TEMPL_DECL
void BP_TREE_TY::_erase_heavy_node ( _NodeHeavyPtr &  p_posn )
{
    p_posn->p_prev->p_next = p_posn->p_next ;
    p_posn->p_next->p_prev = p_posn->p_prev ;
    if ( p_posn->p_succr )  p_posn->p_succr->p_predr = 0 ;
    if ( p_posn->p_predr )  p_posn->p_predr->p_succr = 0 ;
    _delete_node_heavy ( p_posn ) ;
}


TEMPL_DECL
void BP_TREE_TY::_erase_top_level ( )
{
    _NodeHeavyPtr   p_tl_end = m_p_head_heavy->p_succr ;
    _NodeHeavyPtr   p_cur    = p_tl_end->p_next ;
    _NodeHeavyPtr   p_temp   = 0 ;

    while ( p_cur != p_tl_end )
    {
        p_temp = p_cur->p_next ;
        _erase_heavy_node ( p_cur ) ;
        p_cur = p_temp ;
    }

    p_tl_end->p_predr->p_succr = p_tl_end->p_succr ;
    p_tl_end->p_succr->p_predr = p_tl_end->p_predr ;
    _delete_node_heavy ( p_tl_end ) ;

    p_tl_end = m_p_head_heavy->p_succr ;
    p_cur    = p_tl_end->p_next ;
    while ( p_cur != p_tl_end )
    {
        p_cur->p_predr = 0 ;
        p_cur = p_cur->p_next ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_clear_botm_level ( )
{
    _NodeHeavyPtr   p_end  = _bottom_end ( ) ;
    _NodeHeavyPtr   p_cur  = p_end->p_next ;
    _NodeHeavyPtr   p_temp = 0 ;

    while ( p_cur->p_next != p_end )
    {
        p_temp = p_cur->p_next ;
        _delete_block_deep ( p_cur ) ;
        _erase_heavy_node  ( p_cur ) ;
        p_cur = p_temp ;
    }

    size_type       sz_elems = p_cur->m_subsz ;
    _NodeLightPtr   p_lt_i   = p_cur->_get_node_light() ;
    pointer         p_elem   = 0 ;
    for ( size_type i = 0 ; i < sz_elems ; ++i , ++p_lt_i )
    {
        p_elem = &(p_lt_i->elem) ;
        m_allr_ty_val . destroy ( p_elem ) ;
        p_lt_i->p_heavy_predr = p_cur ;
    }

    p_cur->m_subsz = 0 ;
    m_size_light   = 0 ;
}


TEMPL_DECL
void BP_TREE_TY::_merge_block
    ( _NodeHeavyPtr   p_parent     ,
      _NodeLightPtr & p_lt_res_upd )
{
    _NodeHeavyPtr       p_left      = p_parent ;
    _NodeHeavyPtr       p_right     = p_left->p_next ;
    difference_type     n_left      = difference_type ( p_left ->m_subsz ) ;
    difference_type     n_right     = difference_type ( p_right->m_subsz ) ;
    _NodeLightPtr       p_lt_0_left = p_left ->_get_node_light() ;
    _NodeLightPtr       p_lt_0_right= p_right->_get_node_light() ;
    _NodeLightPtr       p_lt_n_left = p_lt_0_left  + n_left  ;
    difference_type     dist_upd    = -1 ;
    if ( _is_child_position ( p_right , p_lt_res_upd ) )
    {
        dist_upd = p_lt_res_upd - p_lt_0_right ;
    }

    pointer               p_elem   = 0 ;
    _NodeLightPtr         p_lt_i_L = p_lt_n_left  ;
    _NodeLightPtr         p_lt_i_R = p_lt_0_right ;
    for ( difference_type i = 0 ; i < n_right ; ++i, ++p_lt_i_L, ++p_lt_i_R )
    {
        p_elem = &(p_lt_i_L->elem) ;
        _construct ( p_elem , p_lt_i_R->_elem() ) ;
        p_elem = &(p_lt_i_R->_elem()) ;
        m_allr_ty_val . destroy ( p_elem ) ;
        p_lt_i_L->p_heavy_predr = 0 ;
        p_lt_i_R->p_heavy_predr = p_right ;
    }

    if ( dist_upd >= 0 )
    {
        p_lt_res_upd = p_lt_0_left + n_left + dist_upd ;
    }

    _delete_block_shallow ( p_right ) ;
    p_right->_set_node_light ( 0 ) ;
}


TEMPL_DECL
void BP_TREE_TY::_merge_node_with
    (
        _NodeHeavyPtr    p_left      ,
        size_type &      subsz_accum
    )
{
    _NodeHeavyPtr   p_mid   = p_left->p_next ;
    _NodeHeavyPtr   p_right = p_mid->p_next  ;
    size_type       sz_mid  = p_mid->m_subsz ;

    if ( p_mid->p_predr && p_mid->p_predr->p_succr==p_mid )
    {
        _NodeHeavyPtr   p_mid_par  = p_mid->p_predr ;
        _NodeHeavyPtr   p_left_par = p_mid_par->p_prev ;

        p_mid_par->p_succr = p_right ;
        subsz_accum += sz_mid ;

        p_mid_par->m_subsz  -= subsz_accum ;
        p_left_par->m_subsz += subsz_accum ;
    }
    else
    {
        subsz_accum = 0 ;
    }

    p_left->m_subsz += sz_mid ;
    p_mid ->p_succr  = 0 ;
    p_mid ->p_predr  = 0 ;
    _erase_heavy_node ( p_mid ) ;
}


TEMPL_DECL
void BP_TREE_TY::_balance ( const difference_type  n_ch_left  ,
                            const difference_type  n_ch_right ,
                            const _NodeHeavyPtr    p_parent   )
{
    if ( 0 == p_parent )
        return ;

    const _NodeHeavyPtr  p_mid  = p_parent->p_next ;
    difference_type      n_move = ( n_ch_right - n_ch_left ) / 2 ;
    if ( 0 == n_move )
        return ;

    _NodeHeavyPtr   p_mid_new  = p_mid->p_succr ;
    size_type       move_subsz = _move_subsize ( p_mid_new , n_move ) ;

    _NodeHeavyPtr   p_tmp = p_mid ;
    do
    {
        p_tmp->_set_node_light ( p_mid_new->_get_node_light ( ) ) ;

        if ( n_move >= 0 )
        {
            p_tmp->m_subsz         -= move_subsz ;
            p_tmp->p_prev->m_subsz += move_subsz ;
        }
        else
        {
            p_tmp->m_subsz         += move_subsz ;
            p_tmp->p_prev->m_subsz -= move_subsz ;
        }
        p_tmp = _move_to_parent ( p_tmp ) ;
    }
    while ( p_tmp != 0 ) ;

    difference_type n = n_move ;
    _NodeHeavyPtr   p_upd_ch     = (n>0) ? p_mid->p_succr : p_mid_new      ;
    _NodeHeavyPtr   p_upd_ch_end = (n>0) ? p_mid_new      : p_mid->p_succr ;
    _NodeHeavyPtr   p_upd_parent = (n>0) ? p_mid->p_prev  : p_mid          ;
    while ( p_upd_ch != p_upd_ch_end )
    {
        p_upd_ch->p_predr = p_upd_parent ;
        p_upd_ch = p_upd_ch->p_next ;
    }

    p_mid->p_succr = p_mid_new ;
}


TEMPL_DECL
void BP_TREE_TY::_balance_external
    (
        const _NodeHeavyPtr p_parent     ,
        _NodeLightPtr &     p_lt_res_upd
    )
{
    if ( p_parent==0 )
        return ;

    const _NodeHeavyPtr     p_left    = p_parent   ;
    const _NodeHeavyPtr     p_right   = p_left->p_next ;
    const difference_type   n_ch_left = difference_type ( p_left ->m_subsz ) ;
    const difference_type   n_ch_right= difference_type ( p_right->m_subsz ) ;
    const difference_type   n_move    = ( n_ch_right - n_ch_left ) / 2 ;
    difference_type         n_mv      = (n_move>0) ? n_move : -n_move ;
    if ( 0 == n_move )
        return ;

    _NodeLightPtr   p_lt_0_left      = p_left->_get_node_light() ;
    _NodeLightPtr   p_lt_n_left      = p_left->_get_node_light() + n_ch_left ;
    _NodeLightPtr   p_lt_0_right     = p_right->_get_node_light() ;
    _NodeLightPtr   p_lt_0_right_new = p_lt_0_right ;

    if ( n_move > 0 )
    {
        difference_type     dist_R = -1 ;
        if ( _is_child_position ( p_right , p_lt_res_upd ) )
        {
            dist_R = p_lt_res_upd - p_lt_0_right ;
        }

        while ( n_mv > 0 )
        {
            _insert_block ( p_left , p_lt_n_left , p_lt_0_right->_elem() ) ;
            ++(p_left->m_subsz) ;
            _erase_block  ( p_lt_0_right ) ;
            ++p_lt_n_left ;
            --n_mv ;
        }
        p_left->m_subsz -= size_type( n_move ) ;

        if ( dist_R >= 0 )
        {
            if ( dist_R < n_move )
            {
                p_lt_res_upd = p_lt_0_left + n_ch_left + dist_R ;
            }
            else
            {
                p_lt_res_upd = p_lt_res_upd - n_move ;
            }
        }
    }
    else
    {
        difference_type     dist_L = -1 ;
        difference_type     dist_R = -1 ;
        if ( _is_child_position ( p_left , p_lt_res_upd ) )
        {
            dist_L = ( p_lt_res_upd - p_lt_0_left ) -
                     ( n_ch_left - n_mv ) ;
        }
        if ( _is_child_position ( p_right , p_lt_res_upd ) )
        {
            dist_R = p_lt_res_upd - p_lt_0_right ;
        }

        while ( n_mv > 0 )
        {
            _insert_block ( p_right , p_lt_0_right , (p_lt_n_left-1)->_elem() ) ;
            ++(p_right->m_subsz) ;
            _erase_block  ( p_lt_n_left-1 ) ;
            --p_lt_n_left ;
            --n_mv ;
        }
        p_right->m_subsz -= size_type( -n_move ) ;

        if ( dist_R >= 0 )
        {
            p_lt_res_upd = p_lt_res_upd - n_move ;
        }
        if ( dist_L >= 0 )
        {
            p_lt_res_upd = p_lt_0_right + dist_L ;
        }
    }

    _NodeHeavyPtr   p_tmp = p_right ;
    do
    {
        p_tmp->_set_node_light ( p_lt_0_right_new ) ;

        if ( n_move >= 0 )
        {
            p_tmp->m_subsz         -= size_type ( n_move ) ;
            p_tmp->p_prev->m_subsz += size_type ( n_move ) ;
        }
        else
        {
            p_tmp->m_subsz         += size_type ( -n_move ) ;
            p_tmp->p_prev->m_subsz -= size_type ( -n_move ) ;
        }

        p_tmp = _move_to_parent ( p_tmp ) ;
    }
    while ( p_tmp != 0 ) ;
}


TEMPL_DECL
void BP_TREE_TY::_update_data
    (
        const _NodeHeavyPtr   p_par_start  ,
        const size_type       sub_size_tot
    )
{
    if ( ! p_par_start )
        return ;

    _NodeHeavyPtr   p_upd      = p_par_start->p_next ;
    _NodeLightPtr   p_lt_succr = p_upd->p_succr->_get_node_light( ) ;

    p_upd->_set_node_light  ( p_lt_succr ) ;
    p_upd = _move_to_parent ( p_upd ) ;
    while ( p_upd )
    {
        p_upd->_set_node_light ( p_lt_succr )  ;
        p_upd->m_subsz         -= sub_size_tot ;
        p_upd->p_prev->m_subsz += sub_size_tot ;
        p_upd = _move_to_parent ( p_upd ) ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_link_to_parent ( _NodeHeavyPtr  p_parent )
{
    _NodeHeavyPtr   p_mid   = p_parent->p_next ;
    _NodeHeavyPtr   p_right = p_mid   ->p_next ;
    _NodeHeavyPtr   p_cur   = p_mid  ->p_succr ;
    _NodeHeavyPtr   p_end   = p_right->p_succr ;

    while ( p_cur != p_end )
    {
        p_cur->p_predr = p_parent ;
        p_cur = p_cur->p_next ;
    }
}


TEMPL_DECL
bool BP_TREE_TY::_find_two_groups
    (
        const _NodeHeavyPtr  p_level_end ,
        const _NodeHeavyPtr  p_input     ,
        _NodeHeavyPtr &      ptr_left    ,
        _NodeHeavyPtr &      ptr_mid     ,
        _NodeHeavyPtr &      ptr_right
    ) const
{
    ptr_left  = 0 ;
    ptr_mid   = 0 ;
    ptr_right = 0 ;

    if ( p_input == 0 )
        return false ;

    if      ( p_input->p_next != p_level_end )
    {
        ptr_left = p_input ;
        ptr_mid  = ptr_left->p_next ;
        ptr_right= ptr_mid ->p_next ;
    }
    else if ( p_input->p_prev != p_level_end )
    {
        ptr_mid  = p_input ;
        ptr_left = ptr_mid->p_prev ;
        ptr_right= ptr_mid->p_next ;
    }

    return ( ptr_left != 0 ) ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_count_level ( _NodeHeavyPtr  p_level_end ) const
{
    return _count_level ( p_level_end->p_next , p_level_end ) ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_count_level ( const _NodeHeavyPtr  p_from ,
                           const _NodeHeavyPtr  p_to   ) const
{
    size_type       ne    = 0 ;
    _NodeHeavyPtr   p_cur = p_from ;
    for (  ; p_cur != p_to ; p_cur = p_cur->p_next , ++ne ) { }
    return ne ;
}


TEMPL_DECL
void BP_TREE_TY::_move_level ( _NodeHeavyPtr &        ptr    ,
                               const difference_type  n_move )
{
    if ( n_move < 0 )
    {
        for ( difference_type i = n_move ; i < 0 ; ++i )
            ptr = ptr->p_prev ;
    }
    else
    {
        for ( difference_type i = 0 ; i < n_move ; ++i )
            ptr = ptr->p_next ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_move_subsize ( _NodeHeavyPtr &        ptr    ,
                            const difference_type  n_move )
{
    size_type   tot_subsz = 0 ;

    if ( n_move < 0 )
    {
        for ( difference_type i = n_move ; i < 0 ; ++i )
        {
            ptr        = ptr->p_prev ;
            tot_subsz += ptr->m_subsz ;
        }
    }
    else
    {
        for ( difference_type i = 0 ; i < n_move ; ++i )
        {
            tot_subsz += ptr->m_subsz ;
            ptr        = ptr->p_next ;
        }
    }

    return tot_subsz ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_tree_height ( ) const
{
    size_type       n_res = 0 ;
    _NodeHeavyPtr   p_end = m_p_head_heavy ;
    _NodeHeavyPtr   p_cur = p_end->p_succr ;
    while ( p_cur != p_end )
    {
        ++n_res ;
        p_cur = p_cur->p_succr ;
    }

    return n_res ;
}


TEMPL_DECL
bool BP_TREE_TY::_size_second_top_is_less ( const size_type  sz_lim ) const
{
    _NodeHeavyPtr   p_level_end = m_p_head_heavy->p_succr->p_succr ;
    if ( p_level_end == m_p_head_heavy )
        return false ;

    size_type       count = 0 ;
    _NodeHeavyPtr   p_cur = p_level_end->p_next ;
    while ( p_cur != p_level_end )
    {
        ++count ;
        if ( count >= sz_lim )
            break ;
        p_cur = p_cur->p_next ;
    }

    return ( count < sz_lim ) ;
}


TEMPL_DECL
typename BP_TREE_TY::_NodeHeavyPtr
BP_TREE_TY::_move_to_parent ( _NodeHeavyPtr  p_1st_child ) const
{
    _NodeHeavyPtr   p_parent = p_1st_child->p_predr ;
    if ( p_parent && p_parent->p_succr==p_1st_child )
        return p_parent ;
    else
        return 0 ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::_local_limit ( ) const
{
    return ( _tree_height( ) + 1 ) * _min_degree_int() ;
}


TEMPL_DECL
void BP_TREE_TY::_find_lower_bound ( const _Ty_Key &    key_x ,
                                     difference_type &  index ,
                                     _NodeHeavyPtr &    p_par_res ,
                                     _NodeLightPtr &    p_lt_posn ) const
{
    if ( m_size_light == 0 )
    {
        index     = 0 ;
        p_par_res = _bottom_end ( ) ;
        p_lt_posn = _external_end( ) ;
        return ;
    }

    _NodeLightPtr   p_lt_begin = _external_begin() ;
    _NodeLightPtr   p_lt_last  = _external_last () ;

    if      ( ! m_k_comp ( _KeyOfV( ) ( p_lt_begin->_elem() ) , key_x ) )
    {
        index     = 0 ;
        p_par_res = _bottom_begin ( ) ;
        p_lt_posn = _external_begin( ) ;
    }
    else if (   m_k_comp ( _KeyOfV( ) ( p_lt_last->_elem() )  , key_x ) )
    {
        index     = _size_dt() ;
        p_par_res = _bottom_end ( ) ;
        p_lt_posn = _external_end( ) ;
    }
    else
    {
        _NodeHeavyPtr   p_h_cur = _top_end( )->p_prev ;
        index  = _size_dt() ;
        index -= difference_type (p_h_cur->m_subsz) ;

        while ( p_h_cur )
        {
            while ( ! m_k_comp ( _KeyOfV( ) ( p_h_cur->_elem() ) , key_x ) )
            {
                p_h_cur = p_h_cur->p_prev  ;
                index  -= difference_type (p_h_cur->m_subsz) ;
            }

            if ( p_h_cur->p_succr == 0 )
                break ;

            index  += difference_type (p_h_cur->m_subsz) ;
            p_h_cur = p_h_cur->p_next->p_succr->p_prev ;
            index  -= difference_type (p_h_cur->m_subsz) ;
        }

        difference_type
        dist      = difference_type (p_h_cur->m_subsz) ;
        p_lt_posn = p_h_cur->_get_node_light() ;
        p_lt_posn+= (dist-1) ;
        index    += (dist-1) ;
        while ( ! m_k_comp ( _KeyOfV( ) ( p_lt_posn->_elem() ) , key_x ) )
        {
            --p_lt_posn ;
            --index ;
        }

        p_par_res = p_h_cur ;
        _NodeLightPtr   p_saved = p_lt_posn ;
        _inc_pointer ( p_lt_posn ) ;
        ++index ;
        ++p_saved ;
        if ( p_lt_posn != p_saved )
            p_par_res = p_h_cur->p_next ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_find_upper_bound ( const _Ty_Key &    key_x ,
                                     difference_type &  index ,
                                     _NodeHeavyPtr &    p_par_res ,
                                     _NodeLightPtr &    p_lt_posn ) const
{
    if ( m_size_light == 0 )
    {
        index     = 0 ;
        p_par_res = _bottom_end ( ) ;
        p_lt_posn = _external_end( ) ;
        return ;
    }

    _NodeLightPtr   p_lt_beg  = _external_begin() ;
    _NodeLightPtr   p_lt_last = _external_last () ;

    if (    ! m_k_comp ( key_x , _KeyOfV( ) ( p_lt_last->_elem() ) ) )
    {
        index     = _size_dt() ;
        p_par_res = _bottom_end ( ) ;
        p_lt_posn = _external_end( ) ;
    }
    else if ( m_k_comp ( key_x , _KeyOfV( ) ( p_lt_beg->_elem()  ) ) )
    {
        index     = 0 ;
        p_par_res = _bottom_begin ( ) ;
        p_lt_posn = _external_begin( ) ;
    }
    else
    {
        _NodeHeavyPtr   p_h_cur = _top_end( )->p_prev ;
        index  = _size_dt() ;
        index -= difference_type (p_h_cur->m_subsz) ;

        while ( p_h_cur )
        {
            while ( m_k_comp ( key_x , _KeyOfV( ) ( p_h_cur->_elem() ) ) )
            {
                p_h_cur = p_h_cur->p_prev  ;
                index  -= difference_type (p_h_cur->m_subsz) ;
            }

            if ( p_h_cur->p_succr == 0 )
                break ;

            index  += difference_type (p_h_cur->m_subsz) ;
            p_h_cur = p_h_cur->p_next->p_succr->p_prev ;
            index  -= difference_type (p_h_cur->m_subsz) ;
        }

        difference_type
        dist      = difference_type (p_h_cur->m_subsz) ;
        p_lt_posn = p_h_cur->_get_node_light() ;
        p_lt_posn+= (dist-1) ;
        index    += (dist-1) ;
        while ( m_k_comp ( key_x , _KeyOfV( ) ( p_lt_posn->_elem() ) ) )
        {
            --index ;
            --p_lt_posn ;
        }

        p_par_res = p_h_cur ;
        _NodeLightPtr   p_saved = p_lt_posn ;
        ++index ;
        _inc_pointer ( p_lt_posn ) ;
        ++p_saved ;
        if ( p_lt_posn != p_saved )
            p_par_res = p_h_cur->p_next ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::_NodeLightPtr
BP_TREE_TY::_find_node_light ( const size_type  idx_pos ) const
{
    if ( idx_pos >= size ( ) )
        return _external_end ( ) ;
    if ( idx_pos == 0 )
        return _external_begin ( ) ;

    _NodeHeavyPtr   p_cur = _top_begin ( ) ;
    size_type       n_tot = 0 ;

    do
    {
        while ( n_tot < idx_pos )
        {
            n_tot += p_cur->m_subsz ;
            p_cur  = p_cur->p_next  ;
        }

        if ( n_tot > idx_pos )
        {
            p_cur  = p_cur->p_prev  ;
            n_tot -= p_cur->m_subsz ;
        }

        if ( p_cur->p_succr )
            p_cur = p_cur->p_succr ;
        else
            break ;
    }
    while ( p_cur ) ;

    _NodeLightPtr   p_lt_res = p_cur->_get_node_light() ;
    p_lt_res += difference_type ( idx_pos ) -
                difference_type ( n_tot   ) ;

    return p_lt_res ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_insert_seqce ( iterator  pos , const value_type &  val )
{
    difference_type idx  = pos._index ( ) ;
    _NodeLightPtr   posn = pos._lt_pointer() ;
    iterator        res  = _insert_seqce_posn ( idx , posn , val ) ;
    return res ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_seqce_count ( iterator            pos ,
                                       size_type           cnt ,
                                       const value_type &  val )
{
    size_type   cnt_min = ( _min_degree_ext() < 4 ) ? _min_degree_ext() : 4 ;
    if ( cnt < cnt_min )
    {
        while ( cnt-- )
        {
            pos = _insert_seqce ( pos , val ) ;
            ++pos ;
        }
    }
    else
    {
        this_type   tree_cnt ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
        tree_cnt . _push_back_count ( cnt , val ) ;
        this->_splice_other ( pos , tree_cnt , tree_cnt.begin() , tree_cnt.end() ) ;
    }
}


TEMPL_DECL
void BP_TREE_TY::_fill_value_count ( size_type           count ,
                                     const value_type &  val   )
{
    _NodeHeavyPtr   p_parent = _bottom_end()->p_prev ;
    _NodeLightPtr   p_lt_cur = p_parent->_get_node_light() ;
    pointer         p_elem   = 0 ;

    for ( size_type k = 0 ; k < count ; ++k, ++p_lt_cur )
    {
        p_elem = &(p_lt_cur->elem) ;
        _construct ( p_elem , val ) ;
        p_lt_cur->p_heavy_predr = 0 ;
    }
    m_size_light += count ;
}


TEMPL_DECL
void BP_TREE_TY::_insert_b_tree_count
    (
        _NodeHeavyPtr       p_parent      ,
        size_type           cnt_elems
    )
{
    _NodeHeavyPtr   p_tmp = p_parent ;
    while ( p_tmp != 0 )
    {
        p_tmp->m_subsz += cnt_elems  ;
        p_tmp = p_tmp->p_predr ;
    }

    _NodeHeavyPtr   p_parent_up = p_parent->p_predr ;
    _insert_b_tree_heavy_node ( p_parent    ,
                                p_parent_up ) ;
}


TEMPL_DECL
void BP_TREE_TY::_push_back_count ( size_type           cnt ,
                                    const value_type &  val )
{
    if ( cnt < _max_degree_ext() )
    {
        _fill_value_count ( cnt , val ) ;
        _insert_b_tree_count ( _bottom_end()->p_prev , cnt ) ;
        return ;
    }

    size_type       cnt_fill = 3 * _min_degree_ext() / 2 ;
    while ( cnt > 2*_max_degree_ext() )
    {
        _fill_value_count ( cnt_fill , val ) ;
        _insert_b_tree_count ( _bottom_end()->p_prev , cnt_fill ) ;
        _insert_link_node_botm ( _bottom_end() ) ;
        cnt = cnt - cnt_fill ;
    }

    cnt_fill = cnt / 2 ;
    _fill_value_count ( cnt_fill , val ) ;
    _insert_b_tree_count ( _bottom_end()->p_prev , cnt_fill ) ;
    _insert_link_node_botm ( _bottom_end() ) ;

    cnt_fill = cnt - cnt / 2 ;
    _fill_value_count ( cnt_fill , val ) ;
    _insert_b_tree_count ( _bottom_end()->p_prev , cnt_fill ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_splice_impl ( iterator    pos   ,
                           this_type & that  ,
                           iterator    pos_a ,
                           iterator    pos_b )
{
    iterator    pos_res ;

    if ( this == &that )
    {
        pos_res = _splice_self ( pos , pos_a , pos_b ) ;
    }
    else
    {
        pos_res = _splice_other ( pos , that , pos_a , pos_b ) ;
    }

    return pos_res ;
}


TEMPL_DECL
std::pair<typename BP_TREE_TY::iterator, bool>
BP_TREE_TY::insert ( const value_type &  elem_x )
{
    std::pair<iterator, bool>
                    ib_res = _insert_ordered ( elem_x ) ;
    return ib_res ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::insert ( iterator  , const value_type &  elem_x )
{
    std::pair<iterator, bool>
                    ib_res = _insert_ordered ( elem_x ) ;
    return ib_res . first ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::erase ( iterator  pos )
{
    iterator    it_res = _erase_bp_tree ( pos ) ;
    return it_res ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::erase ( iterator  pos_a , iterator  pos_b )
{
    difference_type     cnt = pos_b - pos_a ;

    if ( cnt <= 0 )
    {
        return pos_a ;
    }
    else if ( size_type(cnt) < _min_degree_ext() )
    {
        while ( cnt-- )
        {
            pos_a = erase ( pos_a ) ;
        }
        return pos_a ;
    }
    else
    {
        this_type       tail_b ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
        this_type       tail_a ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
        difference_type dist_a = pos_a - begin() ;

        _split_tree  ( pos_b , tail_b ) ;
        pos_a = begin() + dist_a ;
        _split_tree  ( pos_a , tail_a ) ;
        _splice_tree ( tail_b ) ;
        pos_a = begin() + dist_a ;

        return pos_a ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::erase ( const _Ty_Key &  key_x )
{
    std::pair<iterator, iterator>
                eq_r = equal_range ( key_x ) ;
    size_type   cnt  = size_type ( eq_r.second - eq_r.first ) ;
    erase ( eq_r.first , eq_r.second ) ;
    return cnt ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::find ( const _Ty_Key &  key_x )
{
    iterator        pos = lower_bound ( key_x ) ;
    if ( pos == end( ) || m_k_comp ( key_x  , _KeyOfV( ) (*pos) ) )
        return end( ) ;
    return pos ;
}


TEMPL_DECL
typename BP_TREE_TY::const_iterator
BP_TREE_TY::find ( const _Ty_Key &  key_x ) const
{
    const_iterator  pos = lower_bound ( key_x ) ;
    if ( pos == end( ) || m_k_comp ( key_x  , _KeyOfV( ) (*pos) ) )
        return end( ) ;
    return pos ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::lower_bound ( const _Ty_Key &  key_x )
{
    difference_type     i_low    = 0 ;
    _NodeHeavyPtr       p_parent = 0 ;
    _NodeLightPtr       p_lt_pos = 0 ;
    _find_lower_bound  ( key_x , i_low , p_parent , p_lt_pos ) ;
    return iterator ( i_low , p_lt_pos , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::const_iterator
BP_TREE_TY::lower_bound ( const _Ty_Key &  key_x ) const
{
    difference_type     i_low    = 0 ;
    _NodeHeavyPtr       p_parent = 0 ;
    _NodeLightPtr       p_lt_pos = 0 ;
    _find_lower_bound ( key_x , i_low , p_parent , p_lt_pos ) ;
    return  const_iterator ( i_low , p_lt_pos , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::upper_bound ( const _Ty_Key &  key_x )
{
    difference_type     i_upp    = 0 ;
    _NodeHeavyPtr       p_parent = 0 ;
    _NodeLightPtr       p_lt_pos = 0 ;
    _find_upper_bound ( key_x , i_upp , p_parent , p_lt_pos ) ;
    return  iterator ( i_upp , p_lt_pos , this ) ;
}


TEMPL_DECL
typename BP_TREE_TY::const_iterator
BP_TREE_TY::upper_bound ( const _Ty_Key &  key_x ) const
{
    difference_type     i_upp    = 0 ;
    _NodeHeavyPtr       p_parent = 0 ;
    _NodeLightPtr       p_lt_pos = 0 ;
    _find_upper_bound ( key_x , i_upp , p_parent , p_lt_pos ) ;
    return  const_iterator ( i_upp , p_lt_pos , this ) ;
}


TEMPL_DECL
std::pair < typename BP_TREE_TY::iterator ,
            typename BP_TREE_TY::iterator >
BP_TREE_TY::equal_range ( const _Ty_Key &  key_x )
{
    return std::pair<iterator, iterator> ( lower_bound ( key_x ) ,
                                           upper_bound ( key_x ) ) ;
}


TEMPL_DECL
std::pair < typename BP_TREE_TY::const_iterator ,
            typename BP_TREE_TY::const_iterator >
BP_TREE_TY::equal_range ( const _Ty_Key &  key_x ) const
{
    return std::pair<const_iterator, const_iterator>( lower_bound ( key_x ) ,
                                                      upper_bound ( key_x ) ) ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::count ( const _Ty_Key &  key_x ) const
{
    std::pair<const_iterator, const_iterator>
                eq_r = equal_range ( key_x ) ;
    return size_type ( eq_r . second - eq_r . first ) ;
}


TEMPL_DECL
void BP_TREE_TY::swap ( BP_TREE_TY &  ctr_x )
{
    std::swap ( m_size_light   , ctr_x . m_size_light   ) ;
    std::swap ( m_p_head_heavy , ctr_x . m_p_head_heavy ) ;
    std::swap ( m_k_comp       , ctr_x . m_k_comp       ) ;
    std::swap ( m_multi        , ctr_x . m_multi        ) ;
    std::swap ( m_ordered      , ctr_x . m_ordered      ) ;
}


TEMPL_DECL
typename BP_TREE_TY::size_type
BP_TREE_TY::capacity ( ) const
{
    return max_size ( ) ;
}


TEMPL_DECL
void BP_TREE_TY::reserve ( size_type  sz )
{
    sz = sz ;
}


TEMPL_DECL
typename BP_TREE_TY::const_reference
BP_TREE_TY::at ( size_type  ind ) const
{
    if ( ind >= size() )
        throw std::out_of_range("sequence: index out of range") ;

    _NodeLightPtr   p_elem = _find_node_light ( ind ) ;
    const_reference cref   = p_elem->_elem ( ) ;
    return cref ;
}


TEMPL_DECL
typename BP_TREE_TY::reference
BP_TREE_TY::at ( size_type  ind )
{
    if ( ind >= size() )
        throw std::out_of_range("sequence: index out of range") ;

    _NodeLightPtr   p_elem = _find_node_light ( ind ) ;
    reference       ref    = p_elem->_elem ( ) ;
    return ref ;
}


TEMPL_DECL
typename BP_TREE_TY::const_reference
BP_TREE_TY::operator[] ( size_type  ind ) const
{
    _NodeLightPtr   p_elem = _find_node_light ( ind ) ;
    const_reference cref   = p_elem->_elem ( ) ;
    return cref ;
}


TEMPL_DECL
typename BP_TREE_TY::reference
BP_TREE_TY::operator[] ( size_type  ind )
{
    _NodeLightPtr   p_elem = _find_node_light ( ind ) ;
    reference       ref    = p_elem->_elem ( ) ;
    return ref ;
}


TEMPL_DECL
typename BP_TREE_TY::reference
BP_TREE_TY::front ( )
{
    reference       ref = _external_begin()->_elem ( ) ;
    return ref ;
}


TEMPL_DECL
typename BP_TREE_TY::const_reference
BP_TREE_TY::front ( ) const
{
    const_reference cref = _external_begin()->_elem ( ) ;
    return cref ;
}


TEMPL_DECL
typename BP_TREE_TY::reference
BP_TREE_TY::back ( )
{
    reference       ref = _external_last()->_elem ( ) ;
    return ref ;
}


TEMPL_DECL
typename BP_TREE_TY::const_reference
BP_TREE_TY::back ( ) const
{
    const_reference cref = _external_last()->_elem ( ) ;
    return cref ;
}


TEMPL_DECL
void BP_TREE_TY::push_back ( const value_type &  val )
{
    _NodeHeavyPtr       p_parent = _bottom_end()->p_prev ;
    difference_type     n_elems  = difference_type ( p_parent->m_subsz ) ;
    _NodeLightPtr       p_lt_pos = p_parent->_get_node_light() ;
    p_lt_pos += n_elems ;

    _insert_block  ( p_parent , p_lt_pos , val ) ;
    _insert_b_tree ( p_parent , p_lt_pos ) ;
}


TEMPL_DECL
void BP_TREE_TY::push_front ( const value_type &  val )
{
    _NodeHeavyPtr       p_parent = _bottom_begin() ;
    _NodeLightPtr       p_lt_pos = p_parent->_get_node_light() ;

    _insert_block  ( p_parent , p_lt_pos , val ) ;
    _insert_b_tree ( p_parent , p_lt_pos ) ;
}


TEMPL_DECL
void BP_TREE_TY::pop_front ( )
{
    if ( ! empty ( ) )
        erase ( begin ( ) ) ;
}


TEMPL_DECL
void BP_TREE_TY::pop_back ( )
{
    if ( ! empty ( ) )
        erase ( --end ( ) ) ;
}


TEMPL_DECL
void BP_TREE_TY::resize ( size_type  sz_new )
{
    resize ( sz_new , value_type() ) ;
}


TEMPL_DECL
void BP_TREE_TY::resize ( size_type  sz_new , const value_type &  val )
{
    if ( size() < sz_new )
    {
        _insert_seqce_count ( end() , sz_new-size() , val ) ;
    }
    else if ( sz_new < size() )
    {
        erase ( begin() + difference_type(sz_new) , end() ) ;
    }
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::splice ( iterator  pos , this_type &  that )
{
    return _splice_impl ( pos , that , that.begin() , that.end() ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::splice ( iterator  pos , this_type &  that , iterator  pos_a )
{
    iterator    pos_b ( pos_a ) ;
    ++pos_b ;
    return _splice_impl ( pos , that , pos_a , pos_b ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::splice ( iterator    pos   ,
                     this_type & that  ,
                     iterator    pos_a ,
                     iterator    pos_b )
{
    return _splice_impl ( pos , that , pos_a , pos_b ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::split ( iterator    pos  ,
                    this_type & that )
{
    return split ( pos , end() , that ) ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::split ( iterator    pos_a ,
                    iterator    pos_b ,
                    this_type & that  )
{
    if ( ! that.empty() )
        throw std::invalid_argument("non-empty container") ;

    difference_type     dist_a = pos_a - this->begin() ;
    that . splice ( that.end() , *this , pos_a , pos_b ) ;
    pos_a = this->begin() + dist_a ;
    return pos_a ;
}


TEMPL_DECL
void BP_TREE_TY::reverse ( )
{
    const size_type     sz_min = 2 ;
    if ( size() < sz_min )
        return ;

    iterator        p_forw = begin() ;
    iterator        p_back = end  () ;
    --p_back ;

    do
    {
        reference       val_f = *p_forw ;
        reference       val_b = *p_back ;
        std::swap ( val_f , val_b ) ;
        ++p_forw ;
        if ( p_forw == p_back )
            break ;
        --p_back ;
    }
    while ( p_forw != p_back ) ;
}


TEMPL_DECL
typename BP_TREE_TY::mapped_type
BP_TREE_TY::accumulate ( const_iterator  it_start ,
                         const_iterator  it_end   ,
                         mapped_type     val_in   ) const
{
    if ( ( size_type( it_start._index() ) > size() ) ||
         ( size_type( it_end  ._index() ) > size() ) || ( it_end < it_start) )
        throw std::range_error("accumulate: range error") ;

    _Ty_Map         sum_res = val_in ;
    while ( it_start != it_end )
    {
        sum_res += _MapOfV()( *it_start ) ;
        ++it_start ;
    }

    return sum_res ;
}


TEMPL_DECL
void BP_TREE_TY::write_shallow ( iterator             pos     ,
                                 const mapped_type &  val_new )
{
    _Ty_Map &       ref_map = (_Ty_Map&)(_MapOfV()(*pos)) ;
    ref_map = val_new ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::write_deep ( iterator            pos     ,
                         const value_type &  val_new )
{
    iterator    res ;
    res = erase  ( pos ) ;
    res = insert ( res , val_new ) ;
    return res ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_splice_other ( iterator    pos   ,
                            this_type & other ,
                            iterator    pos_a ,
                            iterator    pos_b )
{
    iterator        pos_res ;
    difference_type index  = pos . _index ( ) ;
    difference_type n_add  = pos_b - pos_a ;
    difference_type dist_a = pos_a - other.begin() ;
    this_type       this_tail    ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
    this_type       other_middle ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;
    this_type       other_tail   ( m_k_comp , m_multi , m_ordered , m_allr_ty_val ) ;

    other._split_tree  ( pos_b , other_tail   ) ;
    pos_a = other.begin() + dist_a ;
    other._split_tree  ( pos_a , other_middle ) ;
    this->_split_tree  ( pos   , this_tail    ) ;
    this->_splice_tree ( other_middle  ) ;
    this->_splice_tree ( this_tail     ) ;
    other._splice_tree ( other_tail    ) ;
    pos_res = begin() + ( index + n_add ) ;
    return pos_res ;
}


TEMPL_DECL
typename BP_TREE_TY::iterator
BP_TREE_TY::_splice_self ( iterator    pos   ,
                           iterator    pos_a ,
                           iterator    pos_b )
{
    iterator        pos_res ;

    if ( pos_a == pos_b )
    {
        pos_res = pos ;
        return pos_res ;
    }

    if ( ( pos_a<=pos ) && ( pos<pos_b ) )
    {
        pos_res = pos ;
        return pos_res ;
    }

    this_type   tree_12 ( m_k_comp, m_multi, m_ordered, m_allr_ty_val );
    this_type   tree_23 ( m_k_comp, m_multi, m_ordered, m_allr_ty_val );
    this_type   tree_34 ( m_k_comp, m_multi, m_ordered, m_allr_ty_val );

    iterator            t1 ;
    iterator            t2 ;
    iterator            t3 ;
    difference_type     dist_1   = 0 ;
    difference_type     dist_2   = 0 ;
    difference_type     count    = pos_b - pos_a ;
    difference_type     dist_res = 0 ;

    if ( pos <  pos_a )
    {
        dist_1   = pos   - begin() ;
        dist_2   = pos_a - begin() ;
        t3       = pos_b ;
        dist_res = dist_1 + count ;
    }
    else
    if ( pos >= pos_b )
    {
        dist_1   = pos_a - begin() ;
        dist_2   = pos_b - begin() ;
        t3       = pos ;
        dist_res = pos   - begin() ;
    }

    _split_tree  ( t3 , tree_34 ) ; t2 = begin() + dist_2 ;
    _split_tree  ( t2 , tree_23 ) ; t1 = begin() + dist_1 ;
    _split_tree  ( t1 , tree_12 ) ;

    _splice_tree ( tree_23 ) ;
    _splice_tree ( tree_12 ) ;
    _splice_tree ( tree_34 ) ;

    pos_res = begin() + dist_res ;
    return pos_res ;
}


TEMPL_DECL
void BP_TREE_TY::_reconnect ( this_type &  other )
{
    if ( this->empty() && other.empty() )
        return ;

    _NodeHeavyPtr   h_heavy_a = this->m_p_head_heavy ;
    _NodeLightPtr   h_light_a = h_heavy_a->_get_node_light() ;
    _NodeHeavyPtr   h_heavy_b = other.m_p_head_heavy ;
    _NodeLightPtr   h_light_b = h_heavy_b->_get_node_light() ;

    _NodeHeavyPtr   p_top_a   = this->_top_end()     ;
    _NodeHeavyPtr   p_botm_a  = this->_bottom_end()  ;
    _NodeHeavyPtr   p_top_b   = other._top_end()     ;
    _NodeHeavyPtr   p_botm_b  = other._bottom_end()  ;
    _NodeHeavyPtr   p_cur     = 0 ;

    for ( p_cur=p_botm_a ; p_cur!=h_heavy_a ; p_cur=p_cur->p_predr )
    {
        p_cur->_set_node_light ( h_light_b ) ;
    }

    for ( p_cur=p_botm_b ; p_cur!=h_heavy_b ; p_cur=p_cur->p_predr )
    {
        p_cur->_set_node_light ( h_light_a ) ;
    }

    p_top_a  ->p_predr = h_heavy_b ;
    p_botm_a ->p_succr = h_heavy_b ;
    h_heavy_a->p_succr = p_top_b   ;
    h_heavy_a->p_predr = p_botm_b  ;
    p_top_b  ->p_predr = h_heavy_a ;
    p_botm_b ->p_succr = h_heavy_a ;
    h_heavy_b->p_succr = p_top_a   ;
    h_heavy_b->p_predr = p_botm_a  ;

    size_type           sz_end   = size_type(1 + 2*_offset()) ;
   _NodeLightPtr        p_lt_a_k = h_light_a - _offset() ;
   _NodeLightPtr        p_lt_b_k = h_light_b - _offset() ;
    for ( size_type     k = 0 ; k < sz_end ; ++k , ++p_lt_a_k , ++p_lt_b_k )
    {
        p_lt_a_k->p_heavy_predr = p_botm_b ;
        p_lt_b_k->p_heavy_predr = p_botm_a ;
    }

    std::swap ( this->m_size_light , other.m_size_light   ) ;
}


TEMPL_DECL
void BP_TREE_TY::_splice_tree ( BP_TREE_TY &  that )
{
    if ( that.empty() )
        return ;
    if ( this->empty() )
    {
        _reconnect ( that ) ;
        return ;
    }

    size_type       n_levels_a = this->_tree_height() ;
    size_type       n_levels_b = that. _tree_height() ;
    size_type       min_n_lev ( (n_levels_a<n_levels_b) ? n_levels_a :
                                                          n_levels_b ) ;

    size_type       size_a     = this->m_size_light + that.m_size_light ;
    size_type       size_b     = 0 ;
    size_type       i          = 0 ;
    size_type       sub_sz_tot = 0 ;
    _NodeHeavyPtr   p_top_upd  = ( n_levels_a < n_levels_b ) ?
                                 this->_top_begin() : that._top_begin() ;
    _NodeHeavyPtr   p_parent_a = this->_top_end()->p_prev ;
    _NodeHeavyPtr   p_parent_b = that. _top_end()->p_next ;
    _NodeHeavyPtr   p_parent_up= 0 ;
    _NodeHeavyPtr   p_parent   = 0 ;
    _NodeHeavyPtr   p_left     = 0 ;

    _NodeHeavyPtr   p_end_a = this->_bottom_end() ;
    _NodeHeavyPtr   p_end_b = that. _bottom_end() ;
    for ( i = 0 ; i < min_n_lev ; ++i , p_end_a=p_end_a->p_predr ,
                                        p_end_b=p_end_b->p_predr   )
    {
        p_end_a->p_prev->p_next = p_end_b->p_next ;
        p_end_b->p_next->p_prev = p_end_a->p_prev ;
        p_end_a->p_prev         = p_end_b->p_prev ;
        p_end_b->p_prev->p_next = p_end_a ;
        p_end_b->p_next = p_end_b ;
        p_end_b->p_prev = p_end_b ;
    }

    if ( n_levels_a != n_levels_b )
    {
        _NodeHeavyPtr   p_par_new = ( n_levels_a < n_levels_b ) ?
                                    p_end_b->p_next : p_end_a->p_prev ;
        _NodeHeavyPtr   p_tmp = p_top_upd ;
        while ( p_tmp->p_predr == 0 )
        {
            sub_sz_tot    += p_tmp->m_subsz ;
            p_tmp->p_predr = p_par_new ;
            p_tmp = p_tmp->p_next ;
        }

        if ( n_levels_a < n_levels_b )
        {
            p_par_new->p_succr = p_top_upd ;

            _NodeHeavyPtr    a_head = this->m_p_head_heavy ;
            _NodeHeavyPtr    b_head = that. m_p_head_heavy ;
            _NodeHeavyPtr    a_top  = a_head ->p_succr ;
            _NodeHeavyPtr    b_cur  = p_end_b ;
            _NodeHeavyPtr    b_last = p_end_b->p_succr ;
            _NodeHeavyPtr    b_top  = b_head ->p_succr  ;

            a_top ->p_predr = b_cur  ;
            b_cur ->p_succr = a_top  ;
            b_top ->p_predr = a_head ;
            a_head->p_succr = b_top  ;
            b_last->p_predr = b_head ;
            b_head->p_succr = b_last ;

            _NodeLightPtr   p_lt_begin = p_top_upd->_get_node_light() ;
            _NodeLightPtr   p_lt_end   = this->m_p_head_heavy->
                                         _get_node_light() ;
            _NodeHeavyPtr   a_cur      = p_par_new ;
            for ( i = n_levels_a ; i < n_levels_b ; ++i, a_cur=a_cur->p_predr,
                                                         b_cur=b_cur->p_predr )
            {
                a_cur->m_subsz += sub_sz_tot ;
                a_cur->_set_node_light ( p_lt_begin ) ;
                b_cur->_set_node_light ( p_lt_end   ) ;
            }
        }
        else
        {
            _NodeHeavyPtr   p_cur = p_par_new ;
            for ( i = n_levels_b ; i < n_levels_a ; ++i, p_cur=p_cur->p_predr )
            {
                p_cur->m_subsz += sub_sz_tot ;
            }
        }

        p_parent_up = p_par_new ;
        p_parent    = p_parent_up->p_succr ;

        if ( p_top_upd->m_subsz < _min_degree_ext() )
        {
            if ( n_levels_a < n_levels_b )
                p_left = p_top_upd ;
            else
                p_left = p_top_upd->p_prev ;
        }
    }
    else
    {
        if ( n_levels_a == 1 )
        {
            size_type   sz_a = p_parent_a->m_subsz ;
            size_type   sz_b = p_parent_b->m_subsz ;
            if ( sz_a<_min_degree_ext() || sz_b<_min_degree_ext() )
            {
                p_left = p_parent_a ;
            }
        }

        p_parent_up = 0 ;
    }

    if ( p_left )
    {
        _NodeHeavyPtr   p_mid = p_left->p_next  ;
        size_type       sz_1  = p_left->m_subsz ;
        size_type       sz_2  = p_mid ->m_subsz ;
        size_type       sub_sz= 0 ;

        if ( sz_1+sz_2 <= _max_degree_ext() )
        {
            _NodeLightPtr   p_lt_upd = 0 ;
            _merge_block     ( p_left , p_lt_upd ) ;
            _merge_node_with ( p_left , sub_sz   ) ;
        }
        else
        {
            _NodeLightPtr   p_lt_upd = 0 ;
            _balance_external ( p_left , p_lt_upd ) ;
        }
    }

    for ( i = 1 ; i < min_n_lev ; ++i )
        that._erase_top_level() ;

    _NodeHeavyPtr   p_btm_end_b = that._bottom_end() ;
    if ( p_btm_end_b == p_btm_end_b->p_prev )
    {
        that._insert_heavy_node_botm ( p_btm_end_b ) ;
    }

    this->m_size_light = size_a ;
    that. m_size_light = size_b ;

    _insert_b_tree_heavy_node ( p_parent    ,
                                p_parent_up ) ;
}


TEMPL_DECL
void BP_TREE_TY::_split_tree
    (
        iterator      pos  ,
        BP_TREE_TY &  other
    )
{
    if ( ! other.empty() )
        return ;
    if ( this->empty() )
        return ;
    if ( pos == this->end() )
        return ;
    if ( pos == this->begin() )
    {
        _reconnect ( other ) ;
        return ;
    }

    _NodeLightPtr       p_lt_pos = pos._lt_pointer() ;
    _NodeLightPtr       p_lt_pos_upd = 0 ;
    _NodeHeavyPtr       p_parent = _parent ( p_lt_pos ) ;
    _NodeHeavyPtr       p_pos    = 0 ;
    _NodeHeavyPtr       p_end_a  = 0 ;
    _NodeHeavyPtr       p_end_b  = 0 ;

    size_type           i_balan  = 0 ;
    size_type           n_levels = this->_tree_height() ;
    size_type           i        = 0 ;
    size_type           size_a   = size_type ( pos - begin() ) ;
    size_type           size_b   = size() - size_a  ;

    if ( p_parent->_get_node_light() == p_lt_pos )
    {
        p_pos = p_parent ;
        ++i_balan ;
        p_lt_pos_upd = p_lt_pos ;
    }
    else
    {
        _split_block_unbalanced ( p_parent , p_lt_pos , p_lt_pos_upd ) ;
        p_pos = p_parent->p_next ;
    }

    p_pos    = p_pos ;
    p_parent = p_pos->p_predr ;
    while ( p_parent && p_parent->p_succr==p_pos )
    {
        ++i_balan ;
        p_pos    = p_pos->p_predr ;
        p_parent = p_pos->p_predr ;
    }

    while ( p_parent )
    {
        _split_node ( p_parent , p_pos ) ;
        p_pos    = p_parent->p_next ;
        p_parent = p_pos->p_predr ;
    }

    p_end_b = other._top_end() ;
    _NodeHeavyPtr   p_last_b = p_end_b->p_prev ;
    _delete_block_deep ( p_last_b ) ;
    _erase_heavy_node  ( p_last_b ) ;

    p_end_a = this->_bottom_end() ;
    for ( i = 0 ; i < n_levels ; ++i , p_end_a=p_end_a->p_predr )
    {
        if ( i > 0 )
            other._insert_top_level() ;

        p_end_b = other._top_end() ;
        p_end_a->p_next->p_prev = p_end_b ;
        p_end_b->p_next         = p_end_a->p_next ;
        p_end_b->p_prev         = p_end_a ;
        p_end_a->p_next         = p_end_b ;
    }

    p_end_a = this->_bottom_end() ;
    p_end_b = other._bottom_end() ;
    p_pos   = _parent ( p_lt_pos_upd ) ;
    for ( i = 0 ; i < n_levels ; ++i , p_end_a=p_end_a->p_predr ,
                                       p_end_b=p_end_b->p_predr ,
                                       p_pos  =p_pos  ->p_predr )
    {
        p_end_a->p_prev->p_next = p_end_b ;
        p_end_b->p_prev         = p_end_a->p_prev ;
        p_end_a->p_prev         = p_pos->p_prev   ;
        p_pos  ->p_prev->p_next = p_end_a ;
        p_end_a->p_next         = p_end_b->p_next ;
        p_end_b->p_next->p_prev = p_end_a ;
        p_end_b->p_next         = p_pos   ;
        p_pos  ->p_prev         = p_end_b ;
    }

    this->m_size_light = size_a ;
    other.m_size_light = size_b ;

    this->_restore_balance ( false , n_levels , i_balan ) ;
    other._restore_balance ( true  , n_levels , i_balan ) ;
}


TEMPL_DECL
void BP_TREE_TY::_restore_balance
    (
        const bool          front      ,
        const size_type     n_h_levels ,
        const size_type     i_balan
    )
{
    _NodeHeavyPtr       p_parent = 0 ;
    _NodeHeavyPtr       p_left   = 0 ;
    _NodeHeavyPtr       p_mid    = 0 ;
    _NodeHeavyPtr       p_right  = 0 ;

    size_type           sub_sz   = 0 ;
    size_type           i        = 0 ;
    size_type           n_lev_min= 2 ;
    size_type           i_stop ( (i_balan>n_lev_min) ? i_balan : n_lev_min ) ;
    _NodeHeavyPtr       p_end_cur= _top_end() ;
    for ( i = n_h_levels ; i >= i_stop ; --i , p_end_cur = p_end_cur->p_succr )
    {
        p_parent = front ? p_end_cur->p_next : p_end_cur->p_prev ;
        if ( !_find_two_groups(p_end_cur , p_parent , p_left , p_mid , p_right))
            continue ;

        _link_to_parent  ( p_left ) ;
        _merge_node_with ( p_left , sub_sz ) ;
    }

    if ( i_balan == 0 )
    {
        _NodeHeavyPtr   p_end_botm = _bottom_end() ;
        p_parent = front ? p_end_botm->p_next : p_end_botm->p_prev ;

        if ( _find_two_groups ( p_end_botm , p_parent ,
                                p_left , p_mid , p_right ) )
        {
            size_type   sz_1 = p_left->m_subsz ;
            size_type   sz_2 = p_mid ->m_subsz ;
            if ( sz_1+sz_2 <= _max_degree_ext() )
            {
                _NodeLightPtr   p_lt_upd = 0 ;
                _merge_block     ( p_left , p_lt_upd ) ;
                _merge_node_with ( p_left , sub_sz   ) ;
            }
            else
            {
                _NodeLightPtr   p_lt_upd = 0 ;
                _balance_external ( p_left , p_lt_upd ) ;
            }
        }
    }

    size_type       n_child = 0 ;
    p_end_cur = p_end_cur->p_predr ;
    for ( i = i_stop ; i <= n_h_levels ; ++i , p_end_cur = p_end_cur->p_predr )
    {
        p_parent = front ? p_end_cur->p_next : p_end_cur->p_prev ;
        n_child  = _count_level( p_parent->p_succr, p_parent->p_next->p_succr );

        if ( n_child > _max_degree_int() )
        {
            _NodeHeavyPtr   p_pos   = p_parent->p_succr ;
            difference_type n_steps = difference_type( n_child / 2 ) ;
            _move_level ( p_pos    , n_steps ) ;
            _split_node ( p_parent , p_pos   ) ;
        }
    }

    while ( _size_second_top_is_less ( _min_degree_int() ) )
        _erase_top_level ( ) ;
}


#undef TEMPL_DECL
#undef BP_TREE_TY

_STD_EXT_ADV_CLOSE

#endif  //  _BP_TREE_ARRAY_HPP
