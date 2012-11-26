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


#include <iostream>
#include <string> 
#include "bpt_sequence.hpp"


namespace BPTreeDemo
{

    //  the example type StringChar can be regarded as 
    //  an improved std::vector<char> that supports 
    //  logarithmic time splice and split operations of 
    //  move semantics along with logarithmic time 
    //  insert and erase copy semantics operations 
    //  for a single element anywhere within a container
    typedef std_ext_adv::sequence<char>     StringChar ; 


    std::ostream &  operator << ( std::ostream &  os , StringChar const &  str ) 
    {
        StringChar::const_iterator    cur = str . begin ( ) ; 
        StringChar::const_iterator    end = str . end   ( ) ; 
        for (  ; cur != end ; ++cur ) 
        {
            os << *cur ; 
        } 

        return os ; 
    }


    void StringConcatenate ( ) 
    {
        std::cout << "    StringConcatenate()" << std::endl ;

        std::string     std_str_a1 ( "The quick " ) ;
        std::string     std_str_b1 ( "brown fox." ) ; 
        //  conversion from std::string
        StringChar      string_a ( std_str_a1.begin() , std_str_a1.end() ) ; 
        StringChar      string_b ( std_str_b1.begin() , std_str_b1.end() ) ; 

        std::cout << string_a << std::endl ;   
        std::cout << string_b << std::endl ;   

        //  concatenation operation
        string_a . splice ( string_a.end() , string_b ) ; 

        std::cout << string_a << std::endl ;   //  "The quick brown fox."
        std::cout << string_b << std::endl ;   //  string_b is empty  

        //  conversion to std::string
        std::string     std_str_a2 ( string_a.begin() , string_a.end() ) ;            
        std::string     std_str_b2 ( string_b.begin() , string_b.end() ) ;            
        std::cout << "std_str_a1=" << std_str_a1 << std::endl ;
        std::cout << "std_str_b1=" << std_str_b1 << std::endl ; 
        std::cout << "std_str_a2=" << std_str_a2 << std::endl ;
        std::cout << "std_str_b2=" << std_str_b2 << std::endl ; 
        std::cout << std::endl ;
    } 


    void SubString ( ) 
    {
        std::cout << "    SubString()" << std::endl ;

        std::string     std_str ( "The quick brown fox." ) ;
        StringChar      string  ( std_str.begin() , std_str.end() ) ; 
        StringChar      sub_str ; 
        std::cout << string  << std::endl ;   
        std::cout << sub_str << std::endl ;   

        //  extract sub-string operation
        StringChar::iterator  pos = string . begin ( ) ; 
        string . split( pos+4 , pos+9 , sub_str ) ; 

        std::cout << string  << std::endl ;   
        std::cout << sub_str << std::endl ;   
        std::cout << std::endl ;
    }


    void ReplaceString ( ) 
    {
        std::cout << "    ReplaceString()" << std::endl ;

        std::string     std_string  ( "The quick yellow fox." ) ;
        std::string     std_sub_new ( "brown" ) ;        

        StringChar      string  ( std_string .begin() , std_string .end() ) ; 
        StringChar      sub_new ( std_sub_new.begin() , std_sub_new.end() ) ; 
        StringChar      sub_str ; 

        std::cout << string  << std::endl ; //  "The quick yellow fox."
        std::cout << sub_new << std::endl ; //  "brown"
        std::cout << sub_str << std::endl ; //  is empty    

        //  replace sub-string operation
        //  note that split() and splice() return updated valid iterators 
        StringChar::iterator  pos = string . begin ( ) ; 
        pos = string . split ( pos+10 , pos+16 , sub_str ) ; 
        pos = string . splice( pos , sub_new ) ; 

        std::cout << string  << std::endl ; //  "The quick brown fox."
        std::cout << sub_new << std::endl ; //  is empty   
        std::cout << sub_str << std::endl ; //  "yellow"   
        std::cout << std::endl ;
    } 

} 


int  main ( int  argc , char*  argv[] )
{
    //  each of these demo functions uses move semantics algorithms, 
    //  which do not copy and do not erase original strings and sub-strings; 
    //  each algorithm just moves a range of characters from one 
    //  string into another; in other words, move operations 
    //  preserve the total number of characters in strings involved; 
    BPTreeDemo::StringConcatenate ( ) ; 
    BPTreeDemo::SubString ( ) ; 
    BPTreeDemo::ReplaceString ( ) ; 

    return 0 ; 
}

