// readme sample
// r-lyeh, public domain.

#include <iostream>
#include "spot.hpp"
#include "cimg.hpp"

int main( int argc, char **argv ) {
    if( argc != 2 ) {
        std::cout << "Usage: " << argv[0] << " image_file" << std::endl;
        return -1;
    }

    spot::image base( argv[1] );

    spot::image saturated = base * spot::hsla( 1, 2, 1, 1 );
    spot::image desaturated = base * spot::hsla( 1, 0.5f, 1, 1 );
    spot::image gray = base * spot::hsla( 1, 0, 1, 1 );
    spot::image light = base * spot::hsla( 1, 1, 1.75f, 1 );
    spot::image hue = base + spot::hsla( 0.5f, 0, 0, 0 );
    spot::image warmer = base + spot::hsla( -0.08f, 0, 0, 0 );
    spot::image colder = base + spot::hsla(  0.08f, 0, 0, 0 );

    spot::image *piece[] = { &base, &saturated, &desaturated, &gray, &light, &hue, &warmer, &colder };
    const char  *name[] = { "base","saturated","desaturated","gray","light","hue","warmer","colder"};

    int block = base.w / 8;
    for( int i = 0; i < 8; ++i ) {
        display( *piece[i], name[i] );
        base = piece[0]->paste( i * block, 0, piece[i]->copy( i * block, 0, block, base.h ) );
    }

    base.save_as_bmp("collage.bmp");
    base.save_as_dds("collage.dds");
    base.save_as_jpg("collage.jpg", 80);
    base.save_as_ktx("collage.ktx", 10);
    base.save_as_pkm("collage.pkm", 10);
    base.save_as_png("collage.png");
    base.save_as_pug("collage.pug", 80);
    base.save_as_pvr("collage.pvr", 10);
    base.save_as_webp("collage.webp", 80);

    display( base, "spot collage sample" );

    return 0;
}
