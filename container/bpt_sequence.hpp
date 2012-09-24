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

#ifndef _BPT_SEQUENCE_HPP
#define _BPT_SEQUENCE_HPP

#include <functional>
#include "bp_tree_array.hpp"


_STD_EXT_ADV_OPEN


//
//  class template sequence supports the union of interfaces of the C++03 
//  sequence containers: std::list, std::deque and std::vector. 
//  A sequence is a container adapter, its behaviour is defined by 
//  an underlying container based on a B+ tree. A sequence supports:
//  - random access iterators;
//  - logarithmic time insert and erase operations for a single element 
//    anywhere within the container; 
//  - logarithmic time splice and split operations for any ranges of 
//    consecutive elements and any positions in the containers involved;
//
template
<
    class _K                        ,
    class _A = std::allocator<_K>   ,
    template < class , class , class , class , class , class , class >
    class _BPTree = bp_tree_array
>
class sequence
{
    typedef _BPTree < _K, _K, _K, GetSelf<_K>,
                      GetSelf<_K>, std::less<_K>, _A >
                                                        _BPTreeType     ;
    typedef std::less<_K>                               key_compare     ;

public:
    //  types
    typedef sequence <_K, _A, _BPTree>                  this_type       ;
    typedef _K                                          value_type      ;
    typedef _A                                          allocator_type  ;
    typedef typename allocator_type::reference          reference       ;
    typedef typename allocator_type::const_reference    const_reference ;
    typedef typename allocator_type::pointer            pointer         ;
    typedef typename allocator_type::const_pointer      const_pointer   ;
    typedef typename allocator_type::size_type          size_type       ;
    typedef typename allocator_type::difference_type    difference_type ;

    typedef typename _BPTreeType::iterator              iterator        ;
    typedef typename _BPTreeType::const_iterator        const_iterator  ;
    typedef std::reverse_iterator< iterator >           reverse_iterator;
    typedef std::reverse_iterator< const_iterator >     const_reverse_iterator;


    //  constructors, copy and assignment
    explicit
    sequence ( const allocator_type &  alr=allocator_type() ) :
        m_contr ( key_compare(), true, false, alr ) { }
    explicit
    sequence ( size_type               sz  ,
               const value_type &      val =value_type() ,
               const allocator_type &  alr =allocator_type() ) :
        m_contr ( key_compare(), true, false, alr )
        { m_contr._insert_seqce_count ( m_contr.begin(), sz, val ) ; }
    template <class _InpIter>
    sequence ( _InpIter                pos_a ,
               _InpIter                pos_b ,
               const allocator_type &  alr = allocator_type() ) :
        m_contr ( key_compare(), true, false, alr )
        { m_contr._insert_seqce_iter ( m_contr.begin(), pos_a, pos_b ) ; }

    sequence   ( const this_type &  that ) : m_contr ( that.m_contr ) { }
    this_type &
    operator = ( const this_type &  that )
    { m_contr = that.m_contr ; return *this ; }

    void        assign ( size_type  cnt, const value_type &  val )
    {
        m_contr.clear() ;
        m_contr._insert_seqce_count( m_contr.begin(), cnt, val ) ;
    }
    template<class _InpIter>
    void        assign (_InpIter  pos_a, _InpIter  pos_b )
    {
        m_contr.clear() ;
        m_contr._insert_seqce_iter( m_contr.begin(), pos_a, pos_b ) ;
    }

    allocator_type
                get_allocator ( ) const { return m_contr.get_allocator(); }

    //  iterators
    iterator                begin ( )       { return m_contr.begin() ; }
    const_iterator          begin ( ) const { return m_contr.begin() ; }
    iterator                end   ( )       { return m_contr.end()   ; }
    const_iterator          end   ( ) const { return m_contr.end()   ; }
    reverse_iterator        rbegin( )
                            { return reverse_iterator       (end()  ); }
    const_reverse_iterator  rbegin( ) const
                            { return const_reverse_iterator (end()  ); }
    reverse_iterator        rend  ( )
                            { return reverse_iterator       (begin()); }
    const_reverse_iterator  rend  ( ) const
                            { return const_reverse_iterator (begin()); }

    //  capacity
    bool        empty    ( ) const { return m_contr.empty()    ; }
    size_type   size     ( ) const { return m_contr.size()     ; }
    size_type   max_size ( ) const { return m_contr.max_size() ; }
    size_type   capacity ( ) const { return m_contr.capacity() ; }
    void        reserve  ( size_type  sz )
                                   { return m_contr.reserve(sz); }

    //  element access
    reference
    operator[ ] ( size_type  ind )       { return m_contr[ind] ; }
    const_reference
    operator[ ] ( size_type  ind ) const { return m_contr[ind] ; }
    reference       at ( size_type  ind )       { return m_contr.at(ind) ; }
    const_reference at ( size_type  ind ) const { return m_contr.at(ind) ; }

    reference               front ( )       { return m_contr.front() ; }
    const_reference         front ( ) const { return m_contr.front() ; }
    reference               back  ( )       { return m_contr.back()  ; }
    const_reference         back  ( ) const { return m_contr.back()  ; }

    //  modifiers
    iterator    insert ( iterator  pos, const value_type &  val )
                { return m_contr._insert_seqce ( pos, val ) ; }
    void        insert ( iterator  pos, size_type  cnt, const value_type &  val )
                { m_contr._insert_seqce_count ( pos, cnt, val ) ; }
    template <class _InpIter>
    void        insert ( iterator  pos, _InpIter  pos_a, _InpIter  pos_b )
    {
        this_type   ctr_copy ( pos_a, pos_b, get_allocator() ) ;
        splice ( pos, ctr_copy ) ;
    }
    void  push_back  ( const value_type &  val ) { m_contr.push_back (val); }
    void  push_front ( const value_type &  val ) { m_contr.push_front(val); }

    void        clear  ( )
                       { m_contr.clear() ; }
    iterator    erase  ( iterator  pos )
                       { return m_contr.erase(pos) ; }
    iterator    erase  ( iterator  pos_a, iterator  pos_b )
                       { return m_contr.erase ( pos_a, pos_b ) ; }
    void        pop_front ( ) { m_contr.pop_front() ; }
    void        pop_back  ( ) { m_contr.pop_back () ; }

    void        remove ( const value_type &  val )
                       { m_contr.remove(val) ; }
    template < class _PredRem >
    void        remove_if ( _PredRem  pred )
                       { m_contr.remove_if( pred ) ; }
    void        unique ( )
                       { m_contr.unique() ; }
    template < class _PredRem >
    void        unique ( _PredRem  pred )
                       { m_contr.unique(pred) ; }

    void        resize ( size_type  sz_new )
                       { m_contr.resize( sz_new ) ; }
    void        resize ( size_type  sz_new, value_type  val )
                       { m_contr.resize( sz_new, val ) ; }
    iterator    splice ( iterator  pos, this_type &  that )
                { return m_contr.splice( pos, that.m_contr ) ; }
    iterator    splice ( iterator  pos, this_type &  that, iterator  pos_a )
                { return m_contr.splice( pos, that.m_contr, pos_a ) ; }
    iterator    splice ( iterator  pos    , this_type & that    ,
                         iterator  pos_a  , iterator    pos_b   )
                { return m_contr.splice( pos, that.m_contr, pos_a, pos_b ) ; }

    iterator    split ( iterator  pos , this_type &  that )
                { return m_contr.split ( pos , that.m_contr ) ; }
    iterator    split ( iterator  pos_a , iterator  pos_b , this_type &  that )
                { return m_contr.split ( pos_a , pos_b , that.m_contr ) ; }

    void        merge ( this_type &  that )
                { m_contr.merge( that.m_contr, key_compare() ) ; }
    template < class _Pred >
    void        merge ( this_type &  that, _Pred  pred )
                { m_contr.merge( that.m_contr, pred ) ; }

    void        swap ( this_type &  ctr_x )  { m_contr.swap(ctr_x.m_contr) ; }

    void        reverse ( )           { m_contr.reverse() ; }
    void        sort ( )              { m_contr.sort() ; }
    template < class _Pred >
    void        sort ( _Pred  pred )  { m_contr.sort( pred ) ; }

    std::pair<iterator, bool>
                write ( iterator  pos, const value_type &  val_new )
    {
        if ( !key_compare()( *pos , val_new ) &&
             !key_compare()( val_new , *pos )    )
            return std::pair<iterator, bool> ( pos , false ) ;

        m_contr.write_shallow( pos, val_new ) ;
        return std::pair<iterator, bool> ( pos , true ) ;
    }

    //  specialized algorithms
    value_type  accumulate ( const_iterator  it_start ,
                             const_iterator  it_end   ,
                             value_type      val_in   ) const
                { return m_contr.accumulate( it_start, it_end, val_in ) ; }

protected:

    _BPTreeType     m_contr;

} ;


#define TEMPL_DECL  template < class _K, class _A, \
        template < class , class , class , class , class , class , class > \
        class _BPTree > inline
#define SEQCE_BPT   sequence < _K, _A, _BPTree >


//  comparisons
TEMPL_DECL
bool operator == ( const SEQCE_BPT &  ctr_x ,
                   const SEQCE_BPT &  ctr_y   )
{
    return ( (ctr_x.size()==ctr_y.size()) &&
             std::equal ( ctr_x.begin() , ctr_x.end() , ctr_y.begin() ) ) ;
}

TEMPL_DECL
bool operator != ( const SEQCE_BPT &  ctr_x ,
                   const SEQCE_BPT &  ctr_y   )
{
    return ( !(ctr_x == ctr_y) ) ;
}

TEMPL_DECL
bool operator <  ( const SEQCE_BPT &  ctr_x ,
                   const SEQCE_BPT &  ctr_y   )
{
    return std::lexicographical_compare(ctr_x.begin(), ctr_x.end(),
                                        ctr_y.begin(), ctr_y.end()  ) ;
}

TEMPL_DECL
bool operator >  ( const SEQCE_BPT &  ctr_x ,
                   const SEQCE_BPT &  ctr_y   )
{
    return (ctr_y < ctr_x);
}

TEMPL_DECL
bool operator <= ( const SEQCE_BPT &  ctr_x ,
                   const SEQCE_BPT &  ctr_y   )
{
    return (!(ctr_y < ctr_x));
}

TEMPL_DECL
bool operator >= ( const SEQCE_BPT &  ctr_x ,
                   const SEQCE_BPT &  ctr_y   )
{
    return (!(ctr_x < ctr_y));
}


//  specialized algorithms
TEMPL_DECL
void swap ( SEQCE_BPT &  ctr_x , SEQCE_BPT &  ctr_y )
{
     ctr_x . swap ( ctr_y ) ;
}


#undef TEMPL_DECL
#undef SEQCE_BPT

_STD_EXT_ADV_CLOSE

#endif  //  _BPT_SEQUENCE_HPP
