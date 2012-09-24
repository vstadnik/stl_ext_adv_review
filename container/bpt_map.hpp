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

#ifndef _BPT_MAP_HPP
#define _BPT_MAP_HPP

#include <functional>
#include "bp_tree_array.hpp"


_STD_EXT_ADV_OPEN


//
//  class template map supports the interface of the C++03 std::map container. 
//  A map is a container adapter, its behaviour is defined by an underlying 
//  container based on a B+ tree. A map is an associative container, which 
//  stores elements that represent {key, value} pairs. A map stores elements 
//  with unique keys. A map supports:
//  - logarithmic time search operations using keys of the container elements; 
//  - random access iterators; 
//  - logarithmic time insert and erase operations for a single element 
//    anywhere within the container.
//  - logarithmic time split operations for any ranges of consecutive elements 
//    and any positions in the containers involved.
//
template
<
    class _K    ,
    class _Ty   ,
    class _Pr = std::less<_K> ,
    class _A  = std::allocator< std::pair <const _K, _Ty> > ,
    template < class , class , class , class , class , class , class >
    class _BPTree = bp_tree_array
>
class map
{
    typedef _BPTree <_K, _Ty, std::pair <const _K, _Ty> ,
                     Get1st<std::pair<const _K,_Ty>,const _K>,
                     Get2nd<std::pair<const _K,_Ty>,_Ty>, _Pr, _A >
                                                        _BPTreeType ;

public:
    //  types
    typedef map<_K, _Ty, _Pr, _A, _BPTree>              this_type       ;
    typedef _K                                          key_type        ;
    typedef _Ty                                         mapped_type     ;
    typedef std::pair < const _K, _Ty >                 value_type      ;
    typedef _Pr                                         key_compare     ;
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

    class value_compare :
        public std::binary_function<value_type, value_type, bool>
    {
    friend class map<_K, _Ty, _Pr, _A, _BPTree> ;
    public:
        bool operator() ( const value_type &  val_x ,
                          const value_type &  val_y ) const
        {
            return comp(val_x.first, val_y.first) ;
        }
    protected:
        value_compare ( _Pr  pred ) : comp(pred) { }
        _Pr     comp ;
    } ;

    //  constructors, copy and assignment
    explicit
    map ( const key_compare &    pred=key_compare()    ,
          const allocator_type & alr =allocator_type() ) :
        m_contr ( pred, false, true, alr ) { }

    template < class _InpIter >
    map ( _InpIter                pos_a ,
          _InpIter                pos_b ,
          const key_compare &     pred=key_compare()    ,
          const allocator_type &  alr =allocator_type() ) :
        m_contr ( pred, false, true, alr )
        { m_contr.insert_map ( pos_a , pos_b ) ; }

    map ( const this_type &  that ) : m_contr ( that.m_contr ) { }
    this_type &
    operator = ( const this_type &  that )
    { m_contr = that.m_contr ; return *this ; }

    allocator_type  get_allocator ( ) const { return m_contr.get_allocator(); }

    //  iterators
    const_iterator          begin ( ) const { return m_contr.begin() ; }
    iterator                begin ( )       { return m_contr.begin() ; }
    const_iterator          end   ( ) const { return m_contr.end()   ; }
    iterator                end   ( )       { return m_contr.end()   ; }
    const_reverse_iterator  rbegin( ) const
                            { return const_reverse_iterator (end()  ); }
    reverse_iterator        rbegin( )
                            { return reverse_iterator       (end()  ); }
    const_reverse_iterator  rend  ( ) const
                            { return const_reverse_iterator (begin()); }
    reverse_iterator        rend  ( )
                            { return reverse_iterator       (begin()); }

    //  capacity
    size_type               size     ( ) const { return m_contr.size()     ; }
    size_type               max_size ( ) const { return m_contr.max_size() ; }
    bool                    empty    ( ) const { return m_contr.empty()    ; }

    //  map element access
    mapped_type &   operator [ ] ( const key_type &  key_x )
    {
        iterator    pos = insert( value_type( key_x, _Ty()) ).first ;
        return (*pos).second ;
    }

    //  modifiers
    std::pair<iterator, bool>
                insert ( const value_type &  val_x )
                { return m_contr.insert(val_x) ; }
    iterator    insert ( iterator  pos, const value_type &  val_x )
                { return m_contr.insert(pos, val_x) ; }
    template < class _InpIter >
    void        insert ( _InpIter  pos_a, _InpIter  pos_b )
                { m_contr.insert_map(pos_a, pos_b) ; }

    void        clear ( )
                      { m_contr.clear() ; }
    iterator    erase ( iterator  pos )
                      { return m_contr.erase(pos) ; }
    iterator    erase ( iterator  pos_a, iterator  pos_b )
                      { return m_contr.erase( pos_a, pos_b) ; }
    size_type   erase ( const key_type &  key_x )
                      { return m_contr.erase(key_x) ; }

    iterator    split ( iterator  pos , this_type &  that )
                      { return m_contr.split ( pos , that.m_contr ) ; }
    iterator    split ( iterator  pos_a , iterator  pos_b , this_type &  that )
                      { return m_contr.split ( pos_a , pos_b , that.m_contr ) ; }

    void        merge ( this_type &  that )
                      { m_contr.merge_map( that.m_contr ) ; }

    void        swap  ( this_type &  ctr_x )  { m_contr.swap(ctr_x.m_contr) ; }

    std::pair<iterator, bool>
                write ( iterator  pos, const mapped_type &  val_new )
    {
        m_contr.write_shallow( pos, val_new ) ;
        return std::pair<iterator, bool> ( pos , true ) ;
    }

    //  map observers
    key_compare     key_comp   ( ) const { return m_contr.key_comp() ; }
    value_compare   value_comp ( ) const
                    { return value_compare( m_contr.key_comp() ) ; }

    //  map operations
    size_type       count       ( const key_type &  key_x ) const
                                { return m_contr.count(key_x); }
    const_iterator  find        ( const key_type &  key_x ) const
                                { return m_contr.find(key_x) ; }
    iterator        find        ( const key_type &  key_x )
                                { return m_contr.find(key_x) ; }
    const_iterator  lower_bound ( const key_type &  key_x ) const
                                { return m_contr.lower_bound(key_x) ; }
    iterator        lower_bound ( const key_type &  key_x )
                                { return m_contr.lower_bound(key_x) ; }
    const_iterator  upper_bound ( const key_type &  key_x ) const
                                { return m_contr.upper_bound(key_x) ; }
    iterator        upper_bound ( const key_type &  key_x )
                                { return m_contr.upper_bound(key_x) ; }
    std::pair<const_iterator, const_iterator>
                    equal_range ( const key_type &  key_x ) const
                                { return m_contr.equal_range(key_x) ; }
    std::pair<iterator, iterator>
                    equal_range ( const key_type &  key_x )
                                { return m_contr.equal_range(key_x) ; }

    //  specialized algorithms
    mapped_type accumulate ( const_iterator  it_start ,
                             const_iterator  it_end   ,
                             mapped_type     val_in   ) const
                { return m_contr.accumulate( it_start, it_end, val_in ) ; }


protected:
    _BPTreeType     m_contr ;

} ;


#define TEMPL_DECL_MAP  template < class _K, class _Ty, class _Pr, class _A, \
        template < class , class , class , class , class , class , class > \
        class _BPTree > inline
#define MAP_BPT         map < _K , _Ty , _Pr , _A , _BPTree >


//  comparisons
TEMPL_DECL_MAP
bool operator == ( const MAP_BPT &  ctr_x ,
                   const MAP_BPT &  ctr_y   )
{
    return ( (ctr_x.size()==ctr_y.size()) &&
             std::equal ( ctr_x.begin() , ctr_x.end() , ctr_y.begin() ) ) ;
}

TEMPL_DECL_MAP
bool operator != ( const MAP_BPT &  ctr_x ,
                   const MAP_BPT &  ctr_y   )
{
    return (!(ctr_x == ctr_y));
}

TEMPL_DECL_MAP
bool operator <  ( const MAP_BPT &  ctr_x ,
                   const MAP_BPT &  ctr_y   )
{
    return std::lexicographical_compare(ctr_x.begin(), ctr_x.end(),
                                        ctr_y.begin(), ctr_y.end() ) ;
}

TEMPL_DECL_MAP
bool operator >  ( const MAP_BPT &  ctr_x ,
                   const MAP_BPT &  ctr_y   )
{
    return (ctr_y < ctr_x);
}

TEMPL_DECL_MAP
bool operator <= ( const MAP_BPT &  ctr_x ,
                   const MAP_BPT &  ctr_y   )
{
    return (!(ctr_y < ctr_x));
}

TEMPL_DECL_MAP
bool operator >= ( const MAP_BPT &  ctr_x ,
                   const MAP_BPT &  ctr_y   )
{
    return (!(ctr_x < ctr_y));
}


//  specialized algorithms
TEMPL_DECL_MAP
void swap ( MAP_BPT &  ctr_x , MAP_BPT &  ctr_y )
{
     ctr_x . swap ( ctr_y ) ;
}


#undef TEMPL_DECL_MAP
#undef MAP_BPT



//
//  class template multimap supports the interface of the C++03 std::multimap container. 
//  A multimap is a container adapter, its behaviour is defined by an underlying 
//  container based on a B+ tree. A multimap is an associative container, which 
//  stores elements that represent {key, value} pairs. A multimap stores elements 
//  with multiple equivalent keys. A multimap supports:
//  - logarithmic time search operations using keys of the container elements; 
//  - random access iterators;
//  - logarithmic time insert and erase operations for a single element 
//    anywhere within the container;
//  - logarithmic time split operations for any ranges of consecutive elements 
//    and any positions in the containers involved.
//
template
<
    class _K    ,
    class _Ty   ,
    class _Pr = std::less<_K> ,
    class _A  = std::allocator< std::pair<const _K, _Ty> > ,
    template < class , class , class , class , class , class , class >
    class _BPTree = bp_tree_array
>
class multimap
{
    typedef _BPTree <_K, _Ty, std::pair<const _K, _Ty> ,
                     Get1st<std::pair<const _K,_Ty>,const _K>,
                     Get2nd<std::pair<const _K,_Ty>,_Ty>, _Pr, _A >
                                                        _BPTreeType ;
public:
    //  types
    typedef multimap<_K, _Ty, _Pr, _A, _BPTree>         this_type       ;
    typedef _K                                          key_type        ;
    typedef _Ty                                         mapped_type     ;
    typedef std::pair <const _K , _Ty >                 value_type      ;
    typedef _Pr                                         key_compare     ;
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

    class value_compare :
        public std::binary_function<value_type, value_type, bool>
    {
    friend class multimap<_K, _Ty, _Pr, _A, _BPTree> ;
    public:
        bool operator() (const value_type &  val_x ,
                         const value_type &  val_y ) const
        {
            return comp(val_x.first, val_y.first);
        }
    protected:
        value_compare ( _Pr  pred ) : comp(pred) {  }
        _Pr     comp ;
    } ;

    //  constructors, copy and assignment
    explicit
    multimap ( const key_compare &    pred=key_compare()    ,
               const allocator_type & alr =allocator_type() ) :
        m_contr ( pred, true, true, alr ) {  }

    template < class _InpIter >
    multimap ( _InpIter                pos_a ,
               _InpIter                pos_b ,
               const key_compare &     pred=key_compare()    ,
               const allocator_type &  alr =allocator_type() ) :
        m_contr ( pred, true, true, alr )
        { m_contr.insert_map(pos_a, pos_b) ; }

    multimap   ( const this_type &  that ) : m_contr ( that.m_contr ) { }
    this_type &
    operator = ( const this_type &  that )
    { m_contr = that.m_contr ; return *this ; }

    allocator_type  get_allocator ( ) const { return m_contr.get_allocator(); }

    //  iterators
    const_iterator          begin ( ) const { return m_contr.begin() ; }
    iterator                begin ( )       { return m_contr.begin() ; }
    const_iterator          end   ( ) const { return m_contr.end()   ; }
    iterator                end   ( )       { return m_contr.end()   ; }
    const_reverse_iterator  rbegin( ) const
                            { return const_reverse_iterator (end()  ); }
    reverse_iterator        rbegin( )
                            { return reverse_iterator       (end()  ); }
    const_reverse_iterator  rend  ( ) const
                            { return const_reverse_iterator (begin()); }
    reverse_iterator        rend  ( )
                            { return reverse_iterator       (begin()); }

    //  capacity
    size_type               size     ( ) const { return m_contr.size()     ; }
    size_type               max_size ( ) const { return m_contr.max_size() ; }
    bool                    empty    ( ) const { return m_contr.empty()    ; }

    //  modifiers
    iterator    insert ( const value_type &  val_x )
                { return m_contr.insert(val_x).first; }
    iterator    insert ( iterator  pos, const value_type &  val_x )
                { return m_contr.insert(pos, val_x)  ; }
    template < class _InpIter >
    void        insert ( _InpIter  pos_a, _InpIter  pos_b )
                { m_contr.insert_map ( pos_a , pos_b ) ; }

    void        clear ( )
                      { m_contr.clear() ; }
    iterator    erase ( iterator  pos )
                      { return m_contr.erase(pos) ; }
    iterator    erase ( iterator  pos_a, iterator  pos_b )
                      { return m_contr.erase( pos_a, pos_b) ; }
    size_type   erase ( const key_type &  key_x )
                      { return m_contr.erase(key_x) ; }

    iterator    split ( iterator  pos , this_type &  that )
                      { return m_contr.split ( pos , that.m_contr ) ; }
    iterator    split ( iterator  pos_a , iterator  pos_b , this_type &  that )
                      { return m_contr.split ( pos_a , pos_b , that.m_contr ) ; }

    void        merge ( this_type &  that )
                      { m_contr.merge_map( that.m_contr ) ; }

    void        swap  ( this_type &  ctr_x )  { m_contr.swap(ctr_x.m_contr); }

    std::pair<iterator, bool>
                write ( iterator  pos, const mapped_type &  val_new )
    {
        m_contr.write_shallow( pos, val_new ) ;
        return std::pair<iterator, bool> ( pos , true ) ;
    }

    //  multimap observers
    key_compare     key_comp   ( ) const { return m_contr.key_comp() ; }
    value_compare   value_comp ( ) const
                    { return value_compare( m_contr.key_comp() ) ; }

    //  multimap operations
    size_type       count       ( const key_type &  key_x ) const
                                { return m_contr.count(key_x); }
    const_iterator  find        ( const key_type &  key_x ) const
                                { return m_contr.find(key_x) ; }
    iterator        find        ( const key_type &  key_x )
                                { return m_contr.find(key_x) ; }
    const_iterator  lower_bound ( const key_type &  key_x ) const
                                { return m_contr.lower_bound(key_x) ; }
    iterator        lower_bound ( const key_type &  key_x )
                                { return m_contr.lower_bound(key_x) ; }
    const_iterator  upper_bound ( const key_type &  key_x ) const
                                { return m_contr.upper_bound(key_x) ; }
    iterator        upper_bound ( const key_type &  key_x )
                                { return m_contr.upper_bound(key_x) ; }
    std::pair<const_iterator, const_iterator>
                    equal_range ( const key_type &  key_x ) const
                                { return m_contr.equal_range(key_x) ; }
    std::pair<iterator, iterator>
                    equal_range ( const key_type &  key_x )
                                { return m_contr.equal_range(key_x) ; }

    //  specialized algorithms
    mapped_type accumulate ( const_iterator  it_start ,
                             const_iterator  it_end   ,
                             mapped_type     val_in   ) const
                { return m_contr.accumulate( it_start, it_end, val_in ) ; }

protected:
    _BPTreeType     m_contr ;

} ;


#define TEMPL_DECL_M_MAP template < class _K, class _Ty, class _Pr, class _A, \
        template < class , class , class , class , class , class , class > \
        class _BPTree > inline
#define M_MAP_BPT           multimap < _K, _Ty, _Pr, _A, _BPTree >


//  comparisons
TEMPL_DECL_M_MAP
bool operator == ( const M_MAP_BPT &  ctr_x ,
                   const M_MAP_BPT &  ctr_y   )
{
    return ( (ctr_x.size()==ctr_y.size()) &&
             std::equal ( ctr_x.begin(), ctr_x.end(), ctr_y.begin() ) ) ;
}

TEMPL_DECL_M_MAP
bool operator != ( const M_MAP_BPT &  ctr_x ,
                   const M_MAP_BPT &  ctr_y   )
{
    return (!(ctr_x == ctr_y));
}

TEMPL_DECL_M_MAP
bool operator <  ( const M_MAP_BPT &  ctr_x ,
                   const M_MAP_BPT &  ctr_y   )
{
    return std::lexicographical_compare(ctr_x.begin(), ctr_x.end(),
                                        ctr_y.begin(), ctr_y.end()  );
}

TEMPL_DECL_M_MAP
bool operator >  ( const M_MAP_BPT &  ctr_x ,
                   const M_MAP_BPT &  ctr_y   )
{
    return (ctr_y < ctr_x);
}

TEMPL_DECL_M_MAP
bool operator <= ( const M_MAP_BPT &  ctr_x ,
                   const M_MAP_BPT &  ctr_y   )
{
    return (!(ctr_y < ctr_x));
}

TEMPL_DECL_M_MAP
bool operator >= ( const M_MAP_BPT &  ctr_x ,
                   const M_MAP_BPT &  ctr_y   )
{
    return (!(ctr_x < ctr_y));
}


//  specialized algorithms
TEMPL_DECL_M_MAP
void swap ( M_MAP_BPT &  ctr_x , M_MAP_BPT &  ctr_y )
{
     ctr_x . swap ( ctr_y ) ;
}


#undef TEMPL_DECL_M_MAP
#undef M_MAP_BPT

_STD_EXT_ADV_CLOSE

#endif  //  _BPT_MAP_HPP
