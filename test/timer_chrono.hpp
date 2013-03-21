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


#ifndef _TIMER_CHRONO_HPP
#define _TIMER_CHRONO_HPP

#include <sstream>
#include <string> 

#define BOOST_SYSTEM_DYN_LINK
#define BOOST_CHRONO_HEADER_ONLY
//  GCC specific 
#define BOOST_SYSTEM_NO_DEPRECATED 
#include <boost/chrono/chrono.hpp>


namespace test_performance
{

    class TimerChrono
    {
        typedef boost::chrono::high_resolution_clock    _clock_impl ; 
        typedef _clock_impl::time_point                 _time_point ;
        typedef boost::ratio< 1 , 1000000 >             _micro      ; 
        typedef boost::chrono::duration< double , _micro >   
                                                        _duration   ; 

    public:

        TimerChrono ( ) : start( ) , stop( ) {  } 

        void Start ( ) { start = _clock_impl::now ( ) ; } 
        void Stop  ( ) { stop  = _clock_impl::now ( ) ; }

        double DurationMicroSec ( ) const
        {
            _duration   durn_mcsec = stop - start ; 
            double      time = durn_mcsec . count ( ) ; 
            return time ; 
        } 

        void TimeToString ( std::string const &  info    , 
                            std::string &        str_res ) const 
        {
            double              time = DurationMicroSec ( ) ;
            std::ostringstream  ostr_out ; 

            ostr_out << " T = " << time << " mcs ; " << info << std::endl ; 
            str_res  = ostr_out . str ( ) ;
        } 

    private:

        _time_point     start ; 
        _time_point     stop  ; 
    } ; 

}   //  namespace test_performance


#endif  // _TIMER_CHRONO_HPP 
