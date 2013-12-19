// filt sample
// r-lyeh, public domain.

#include <iostream>
#include "spot.hpp"

// CImg.h code following
#include "cimg.h"
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")

template<typename T>
float distance2( const T &color1, const T &color2 ) {
    float p1 = ( color1.h - color2.h );
    float p2 = ( color1.s - color2.s );
    float p3 = ( color1.l - color2.l );
    return p1 * p1 + p2 * p2 + p3 * p3;
}

template<typename image, typename T>
image remap( image img, const T &from, const T &to, float treshold = 0.25f ) {
    float offset = distance2( from, to );
    for( auto &px : img ) {
        if( distance2(px, from) < treshold * treshold ) {
            px.h = 1-px.h;
        }
    }
    return img;
}

template<typename image>
void display( const image &pic, const char *title = "" ) {
    if( pic.size() ) {
        // display image
        auto rgba = pic.clamp().to_rgba();
        cimg_library::CImg<unsigned char> ctexture( rgba.w, rgba.h, 1, 4, 0 );
        for( size_t y = 0; y < rgba.h; ++y ) {
            for( size_t x = 0; x < rgba.w; ++x ) {
                spot::pixel texel = rgba.at( x, y );
                ctexture( x, y, 0 ) = (unsigned char)(texel.r);
                ctexture( x, y, 1 ) = (unsigned char)(texel.g);
                ctexture( x, y, 2 ) = (unsigned char)(texel.b);
                ctexture( x, y, 3 ) = (unsigned char)(texel.a);
            }
        }
        ctexture.display( title, false );
    }
}

int main( int argc, char **argv ) {

    if( argc == 1 ) {
        std::cout << argv[0] << " - https://github.com/r-lyeh/spot" << std::endl; 
    }

    bool is_hsla = true;
    bool do_bleed = false;
    bool do_check = false;
    bool do_glow  = false;
    bool do_blank = false;

    for( int i = 1; i < argc; ++i ) {
        if( std::string(argv[i]) == "--hsla" )  is_hsla = 1;
        if( std::string(argv[i]) == "--rgba" )  is_hsla = 0;
        if( std::string(argv[i]) == "--bleed" ) do_bleed = 1;
        if( std::string(argv[i]) == "--check" ) do_check = 1;
        if( std::string(argv[i]) == "--glow" )  do_glow = 1;
        if( std::string(argv[i]) == "--blank" ) do_blank = 1;
    }

    for( int i = 1; i < argc; ++i ) {
        if( std::string(argv[i]) == "--hsla" )  continue;
        if( std::string(argv[i]) == "--rgba" )  continue;
        if( std::string(argv[i]) == "--bleed" ) continue;
        if( std::string(argv[i]) == "--check" ) continue;
        if( std::string(argv[i]) == "--glow" )  continue;
        if( std::string(argv[i]) == "--blank" ) continue;

        if( is_hsla ) {
            spot::image pic = spot::image(argv[i]).bleed(do_bleed).glow(do_glow).blank(do_blank).checkered(do_check);
            pic = remap( pic, /*cyan*/spot::color(0.5,0.5,0.5,1), /*green*/spot::color(0.25,0.5,0.5,1) );
            display( pic, argv[i] );
        }
        else {
            spot::texture pic = spot::texture(argv[i]).bleed(do_bleed).glow(do_glow).blank(do_blank).checkered(do_check);
            display( pic, argv[i] );
        }
    }

    return 0;
}
