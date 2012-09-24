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

#ifndef _BPT_SET_HPP
#define _BPT_SET_HPP

#include <functional>
#include "bp_tree_array.hpp"


_STD_EXT_ADV_OPEN


//
//  class template set supports the interface of the C++03 std::set container. 
//  A set is a container adapter, its behaviour is defined by an underlying 
//  container based on a B+ tree. A set is an associative container that 
//  stores elements with unique keys. A set supports:
//  - logarithmic time search operations using keys of the container elements;
//  - random access iterators;
//  - logarithmic time insert and erase operations for a single element 
//    anywhere within the container; 
//  - logarithmic time split operations for any ranges of consecutive elements 
//    and any positions in the containers involved;
//
template
<
    class _K                       ,
    class _Pr = std::less<_K>      ,
    class _A  = std::allocator<_K> ,
    template < class , class , class , class , class , class , class >
    class _BPTree = bp_tree_array
>
class set
{
    typedef _BPTree < _K , _K , _K , GetSelf<_K> , GetSelf<_K> , _Pr , _A >
                                                        _BPTreeType     ;

public:
    //  types 
    typedef set<_K, _Pr, _A, _BPTree>                   this_type       ;
    typedef _K                                          key_type        ;
    typedef _K                                          value_type      ;
    typedef _Pr                                         key_compare     ;
    typedef _Pr                                         value_compare   ;
    typedef _A                                          allocator_type  ;

    typedef typename allocator_type::reference          reference       ;
    typedef typename allocator_type::const_reference    const_reference ;
    typedef typename _BPTreeType::iterator              iterator        ;
    typedef typename _BPTreeType::const_iterator        const_iterator  ;
    typedef typename allocator_type::size_type          size_type       ;
    typedef typename allocator_type::difference_type    difference_type ;
    typedef typename allocator_type::pointer            pointer         ;
    typedef typename allocator_type::const_pointer      const_pointer   ;

    typedef std::reverse_iterator< iterator >           reverse_iterator;
    typedef std::reverse_iterator< const_iterator >     const_reverse_iterator;

    //  constructors, copy and assignment
    explicit
    set ( const key_compare &    pred=key_compare()    ,
          const allocator_type & alr =allocator_type() ) :
        m_contr ( pred, false, true, alr ) { }

    template < class _InpIter >
    set ( _InpIter               pos_a ,
          _InpIter               pos_b ,
          const key_compare &    pred=key_compare()    ,
          const allocator_type & alr =allocator_type() ) :
        m_contr ( pred, false, true, alr )
        { m_contr.insert_set ( pos_a, pos_b ) ; }

    set        ( const this_type &  that ) : m_contr ( that.m_contr ) { }
    this_type &
    operator = ( const this_type &  that )
    { m_contr = that.m_contr ; return *this ; }

    allocator_type  get_allocator ( ) const { return m_contr.get_allocator(); }

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
    size_type               size     ( ) const { return m_contr.size()    ; }
    size_type               max_size ( ) const { return m_contr.max_size(); }
    bool                    empty    ( ) const { return m_contr.empty()   ; }

    //  modifiers 
    std::pair<iterator, bool>
                insert ( const value_type &  val_x )
                { return m_contr.insert(val_x) ; }
    iterator    insert ( iterator  pos , const value_type &  val_x )
                { return m_contr.insert(pos, val_x) ; }
    template < class _InpIter >
    void        insert ( _InpIter  pos_a , _InpIter  pos_b )
                { m_contr.insert_set(pos_a, pos_b) ; }

    iterator  erase ( iterator  pos )
                    { return m_contr.erase(pos) ; }
    size_type erase ( const key_type &  key_x )
                    { return m_contr.erase(key_x) ; }
    iterator  erase ( iterator  pos_a, iterator  pos_b )
                    { return m_contr.erase( pos_a, pos_b) ; }

    iterator  split ( iterator  pos , this_type &  that )
                    { return m_contr.split ( pos , that.m_contr ) ; }
    iterator  split ( iterator  pos_a , iterator  pos_b , this_type &  that )
                    { return m_contr.split ( pos_a , pos_b , that.m_contr ) ; }

    void      merge ( this_type &  that )
                    { m_contr.merge( that.m_contr, key_comp() ) ; }

    void      swap  ( this_type &  ctr_x )
                    { m_contr.swap(ctr_x.m_contr) ; }
    void      clear ( )
                    { m_contr.clear() ; }

    std::pair<iterator, bool>
                write ( iterator  pos, const value_type &  val_new )
    {
        if ( !_Pr()( *pos , val_new ) && !_Pr()( val_new , *pos ) )
            return std::pair<iterator, bool> ( pos , false ) ;

        iterator  lb = lower_bound ( val_new ) ;
        bool      contains = ( ( lb!=end() ) && !_Pr()( val_new, *lb ) ) ;
        if ( contains )
            return std::pair<iterator, bool> ( lb , false ) ;

        iterator  it_res = m_contr.write_deep ( pos, val_new ) ;
        return std::pair<iterator, bool> ( it_res , true ) ;
    }

    //  set observers
    key_compare     key_comp   ( ) const { return m_contr.key_comp() ; }
    value_compare   value_comp ( ) const { return m_contr.key_comp() ; }

    //  set operations
    iterator        find        ( const key_type &  key_x )
                                { return m_contr.find(key_x) ; }
    const_iterator  find        ( const key_type &  key_x ) const
                                { return m_contr.find(key_x) ; }
    size_type       count       ( const key_type &  key_x ) const
                                { return m_contr.count(key_x); }
    iterator        lower_bound ( const key_type &  key_x )
                                { return m_contr.lower_bound(key_x) ; }
    const_iterator  lower_bound ( const key_type &  key_x ) const
                                { return m_contr.lower_bound(key_x) ; }
    iterator        upper_bound ( const key_type &  key_x )
                                { return m_contr.upper_bound(key_x) ; }
    const_iterator  upper_bound ( const key_type &  key_x ) const
                                { return m_contr.upper_bound(key_x) ; }
    std::pair<iterator, iterator>
                    equal_range ( const key_type &  key_x )
                                { return m_contr.equal_range(key_x) ; }
    std::pair<const_iterator, const_iterator>
                    equal_range ( const key_type &  key_x ) const
                                { return m_contr.equal_range(key_x) ; }

    //  specialized algorithms
    value_type  accumulate ( const_iterator  pos_a  ,
                             const_iterator  pos_b  ,
                             value_type      val_in ) const
                { return m_contr.accumulate ( pos_a, pos_b, val_in ) ; }

protected:
    _BPTreeType     m_contr ;

} ;


#define TEMPL_DECL_SET  template < class _K , class _Pr , class _A , \
        template < class , class , class , class , class , class , class > \
        class _BPTree > inline
#define SET_BPT         set < _K , _Pr , _A , _BPTree >


//  comparisons
TEMPL_DECL_SET
bool operator == ( const SET_BPT &  ctr_x ,
                   const SET_BPT &  ctr_y   )
{
    return ( (ctr_x.size()==ctr_y.size()) &&
             std::equal ( ctr_x.begin(), ctr_x.end(), ctr_y.begin() ) ) ;
}

TEMPL_DECL_SET
bool operator != ( const SET_BPT &  ctr_x ,
                   const SET_BPT &  ctr_y   )
{
    return (!(ctr_x == ctr_y)) ;
}

TEMPL_DECL_SET
bool operator < ( const SET_BPT &  ctr_x ,
                  const SET_BPT &  ctr_y   )
{
    return std::lexicographical_compare(ctr_x.begin(), ctr_x.end(),
                                        ctr_y.begin(), ctr_y.end() ) ;
}

TEMPL_DECL_SET
bool operator > ( const SET_BPT &  ctr_x ,
                  const SET_BPT &  ctr_y   )
{
    return (ctr_y < ctr_x) ;
}

TEMPL_DECL_SET
bool operator <= ( const SET_BPT &  ctr_x ,
                   const SET_BPT &  ctr_y   )
{
    return (!(ctr_y < ctr_x)) ;
}

TEMPL_DECL_SET
bool operator >= ( const SET_BPT &  ctr_x ,
                   const SET_BPT &  ctr_y   )
{
    return (!(ctr_x < ctr_y)) ;
}


//  specialized algorithms
TEMPL_DECL_SET
void swap ( SET_BPT &  ctr_x , SET_BPT &  ctr_y )
{
    ctr_x . swap ( ctr_y ) ;
}


#undef TEMPL_DECL_SET
#undef SET_BPT



//  
//  class template multiset supports the interface of the C++03 std::multiset container. 
//  A multiset is a container adapter, its behaviour is defined by an underlying 
//  container based on a B+ tree. A multiset is an associative container that 
//  stores elements with multiple equivalent keys. A multiset supports:
//  - logarithmic time search operations using keys of the container elements; 
//  - random access iterators;
//  - logarithmic time insert and erase operations for a single element 
//    anywhere within the container; 
//  - logarithmic time split operations for any ranges of consecutive elements 
//    and any positions in the containers involved; 
//  
template
<
    class _K                       ,
    class _Pr = std::less<_K>      ,
    class _A  = std::allocator<_K> ,
    template < class , class , class , class , class , class , class >
    class _BPTree = bp_tree_array
>
class multiset
{
    typedef _BPTree < _K , _K , _K , GetSelf<_K> , GetSelf<_K> , _Pr , _A >
                                                        _BPTreeType     ;

public:
    //  types
    typedef multiset<_K, _Pr, _A, _BPTree>              this_type       ;
    typedef _K                                          key_type        ;
    typedef _K                                          value_type      ;
    typedef _Pr                                         key_compare     ;
    typedef _Pr                                         value_compare   ;
    typedef _A                                          allocator_type  ;

    typedef typename allocator_type::reference          reference       ;
    typedef typename allocator_type::const_reference    const_reference ;
    typedef typename _BPTreeType::iterator              iterator        ;
    typedef typename _BPTreeType::const_iterator        const_iterator  ;
    typedef typename allocator_type::size_type          size_type       ;
    typedef typename allocator_type::difference_type    difference_type ;
    typedef typename allocator_type::pointer            pointer         ;
    typedef typename allocator_type::const_pointer      const_pointer   ;

    typedef std::reverse_iterator< iterator >           reverse_iterator;
    typedef std::reverse_iterator< const_iterator >     const_reverse_iterator;

    //  constructors, copy and assignment
    explicit
    multiset ( const key_compare &     pred=key_compare()    ,
               const allocator_type &  alr =allocator_type() ) :
        m_contr  ( pred, true, true, alr ) { }

    template < class _InpIter >
    multiset ( _InpIter                pos_a ,
               _InpIter                pos_b ,
               const key_compare &     pred=key_compare()    ,
               const allocator_type &  alr =allocator_type() ) :
        m_contr ( pred, true, true, alr )
        { m_contr.insert_set ( pos_a, pos_b ) ; }

    multiset     ( const this_type &  that ) : m_contr ( that.m_contr ) { }
    this_type &
    operator =   ( const this_type &  that )
    { m_contr = that.m_contr ; return *this ; }

    allocator_type  get_allocator ( ) const { return m_contr.get_allocator(); }

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
    size_type               size     ( ) const { return m_contr.size()     ; }
    size_type               max_size ( ) const { return m_contr.max_size() ; }
    bool                    empty    ( ) const { return m_contr.empty()    ; }

    //  modifiers 
    iterator    insert ( const value_type &  val_x )
                { return m_contr.insert(val_x).first; }
    iterator    insert ( iterator  pos, const value_type &  val_x )
                { return m_contr.insert(pos, val_x) ; }
    template < class _InpIter >
    void        insert ( _InpIter  pos_a, _InpIter  pos_b )
                { m_contr.insert_set ( pos_a , pos_b ) ; }

    iterator    erase ( iterator  pos )
                      { return m_contr.erase(pos) ; }
    size_type   erase ( const key_type &  key_x )
                      { return m_contr.erase(key_x) ; }
    iterator    erase ( iterator  pos_a, iterator  pos_b )
                      { return m_contr.erase( pos_a, pos_b) ; }

    iterator    split ( iterator  pos , this_type &  that )
                      { return m_contr.split ( pos , that.m_contr ) ; }
    iterator    split ( iterator  pos_a , iterator  pos_b , this_type &  that )
                      { return m_contr.split ( pos_a , pos_b , that.m_contr ) ; }

    void        merge ( this_type &  that )
                      { m_contr.merge( that.m_contr, key_comp() ) ; }

    void        swap  ( this_type &  ctr_x )
                      { m_contr.swap(ctr_x.m_contr); }
    void        clear ( )
                { m_contr.clear() ; }

    std::pair<iterator, bool>
                write ( iterator  pos, const value_type &  val_new )
    {
        if ( !_Pr()( *pos, val_new ) && !_Pr()( val_new, *pos ) )
            return std::pair<iterator, bool> ( pos , false ) ;

        iterator  it_res = m_contr.write_deep ( pos, val_new ) ;
        return std::pair<iterator, bool> ( it_res , true ) ;
    }

    //  multiset observers
    key_compare     key_comp   ( ) const { return m_contr.key_comp() ; }
    value_compare   value_comp ( ) const { return m_contr.key_comp() ; }

    //  multiset operations
    iterator        find        ( const key_type &  key_x )
                                { return m_contr.find(key_x) ; }
    const_iterator  find        ( const key_type &  key_x ) const
                                { return m_contr.find(key_x) ; }
    size_type       count       ( const key_type &  key_x ) const
                                { return m_contr.count(key_x); }
    iterator        lower_bound ( const key_type &  key_x )
                                { return m_contr.lower_bound(key_x) ; }
    const_iterator  lower_bound ( const key_type &  key_x ) const
                                { return m_contr.lower_bound(key_x) ; }
    iterator        upper_bound ( const key_type &  key_x )
                                { return m_contr.upper_bound(key_x) ; }
    const_iterator  upper_bound ( const key_type &  key_x ) const
                                { return m_contr.upper_bound(key_x) ; }
    std::pair<iterator, iterator>
                    equal_range ( const key_type &  key_x )
                                { return m_contr.equal_range(key_x) ; }
    std::pair<const_iterator, const_iterator>
                    equal_range ( const key_type &  key_x ) const
                                { return m_contr.equal_range(key_x) ; }

    //  specialized algorithms
    value_type  accumulate ( const_iterator  pos_a  ,
                             const_iterator  pos_b  ,
                             value_type      val_in ) const
                { return m_contr.accumulate ( pos_a, pos_b, val_in ) ; }

protected:
    _BPTreeType     m_contr ;

} ;


#define TEMPL_DECL_M_SET  template < class _K , class _Pr , class _A , \
        template < class , class , class , class , class , class , class > \
        class _BPTree > inline
#define M_SET_BPT         multiset < _K , _Pr , _A , _BPTree >


//  comparisons
TEMPL_DECL_M_SET
bool operator == ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return ( (ctr_x.size()==ctr_y.size()) &&
             std::equal ( ctr_x.begin() , ctr_x.end() , ctr_y.begin() ) ) ;
}

TEMPL_DECL_M_SET
bool operator != ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return (!(ctr_x == ctr_y));
}

TEMPL_DECL_M_SET
bool operator <  ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return std::lexicographical_compare(ctr_x.begin(), ctr_x.end(),
                                        ctr_y.begin(), ctr_y.end()  ) ;
}

TEMPL_DECL_M_SET
bool operator >  ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return (ctr_y < ctr_x);
}

TEMPL_DECL_M_SET
bool operator <= ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return (!(ctr_y < ctr_x));
}

TEMPL_DECL_M_SET
bool operator >= ( const M_SET_BPT &  ctr_x ,
                   const M_SET_BPT &  ctr_y   )
{
    return (!(ctr_x < ctr_y));
}


//  specialized algorithms
TEMPL_DECL_M_SET
void swap ( M_SET_BPT &  ctr_x , M_SET_BPT &  ctr_y )
{
     ctr_x . swap ( ctr_y ) ;
}


#undef TEMPL_DECL_M_SET
#undef M_SET_BPT


_STD_EXT_ADV_CLOSE


#endif  //  _BPT_SET_HPP
