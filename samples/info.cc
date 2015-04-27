// r-lyeh, public domain.

#include <iostream>
#include "spot.hpp"
#include "cimg.hpp"

int main( int argc, const char **argv ) {
    spot::devel = true;

    for( int i = 1; i < argc; ++i ) {
        std::cout << "[ .. ] " << argv[i] << std::endl;
        spot::image base( argv[i] );
    }

    return 0;
}

