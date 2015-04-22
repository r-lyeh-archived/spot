// r-lyeh, public domain.

#include <iostream>
#include "spot.hpp"
#include "cimg.hpp"

// timing {
#include <thread>
#include <chrono>
#ifndef USE_OMP
auto const epoch = std::chrono::steady_clock::now(); 
double now() {
    return std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::steady_clock::now() - epoch ).count() / 1000.0;
};
#else
#include <omp.h>
auto const epoch = omp_get_wtime(); 
double now() {
    return omp_get_wtime() - epoch;
};
#endif
void sleep( double secs ) {
    std::chrono::microseconds duration( (int)(secs * 1000000) );
    std::this_thread::sleep_for( duration );    
}
#include <string>
template<typename FN>
std::string bench( const FN &fn ) {
    auto took = -now(); fn(); took += now();
    return std::to_string(took) + "s.";
}
//}

int main( int argc, const char **argv ) {

    spot::devel = true;

    for( int i = 1; i < argc; ++i ) {
        std::cout << "[ .. ] " << argv[i] << std::endl;
        spot::image base( argv[i] );
    }

    return 0;
}

