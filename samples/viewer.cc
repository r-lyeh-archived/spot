// r-lyeh, public domain.

#include <iostream>
#include "spot.hpp"
#include "cimg.hpp"

#define VERSION "1.0.5"

int main( int argc, char **argv ) {
    if( argc < 2 ) {
        std::cout << argv[0] << " v" VERSION "/v" SPOT_VERSION " - quick image viewer. https://github.com/r-lyeh/spot" << std::endl << std::endl;
        std::cout << "Usage: " << argv[0] << " image [image [...]]" << std::endl;
        return -1;
    }

    for( int i = 1; i < argc; ++i ) {
        spot::image base( argv[i] );
        display( base, argv[i] );        
    }

    return 0;
}
