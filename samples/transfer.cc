// color image transfer
// r-lyeh, public domain.

// [ref] https://github.com/jrosebr1/color_transfer/
// [ref] https://ece.uwaterloo.ca/~nnikvand/Coderep/colorspace_pascal/colorspace/colorspace.c

#include <iostream>
#include "spot.hpp"
#include "cimg.hpp"

#define VERSION "1.0.1"

spot::image transfer( const spot::image &dst, const spot::image &src );

int main( int argc, char **argv ) {
    if( argc < 3 ) {
        std::cout << argv[0] << " v" VERSION " (libspot v" SPOT_VERSION ") - quick color transfer. https://github.com/r-lyeh/spot" << std::endl << std::endl;
        std::cout << "Usage: " << argv[0] << " image1 image2" << std::endl;
        return -1;
    }

    spot::image pic1( argv[1] );
    spot::image pic2( argv[2] );
    spot::image pic3 = transfer( pic1, pic2 );
    for( auto end = pic1.size(), it = end - end; it < end; ++it ) {
        pic3[it].a = pic1[it].a;
    }

    spot::image pic4( pic1.w + pic2.w + pic1.w, std::max(pic1.h, pic2.h) );
    pic4 = pic4.paste( 0, 0, pic1 );
    pic4 = pic4.paste( pic1.w, 0, pic2 );
    pic4 = pic4.paste( pic1.w + pic2.w, 0, pic3 );

    display( pic4, argv[1] );
    return 0;
}

#include <vector>

void Rgb2Lab(double *L, double *a, double *b, double R, double G, double B);
void Lab2Rgb(double *R, double *G, double *B, double L, double a, double b);

using plane  = std::vector<float>;
using planes = std::vector<plane>;

planes split( const plane &p, unsigned stride = 4 ) {
    planes ps;
    if( stride > 0 ) {
        ps.resize( stride );
        unsigned x = 0;
        for( auto &c : p ) {
            ps[x].push_back( c );
            x++;
            if( x >= stride ) x = 0;
        }
    }
    return ps;
}

plane merge( const planes &ps ) {
    plane p;
    unsigned numplanes = ps.size();
    unsigned numitems = ps[0].size();
    p.resize( numplanes * numitems );
    switch( numplanes ) {
        default:
        break; case 1: for( unsigned x = 0; x < numitems; ++x ) p[x*1+0] = ps[0][x];
        break; case 2: for( unsigned x = 0; x < numitems; ++x ) p[x*2+0] = ps[0][x], p[x*2+1] = ps[1][x];
        break; case 3: for( unsigned x = 0; x < numitems; ++x ) p[x*3+0] = ps[0][x], p[x*3+1] = ps[1][x], p[x*3+2] = ps[2][x];
        break; case 4: for( unsigned x = 0; x < numitems; ++x ) p[x*4+0] = ps[0][x], p[x*4+1] = ps[1][x], p[x*4+2] = ps[2][x], p[x*4+3] = ps[3][x];
    }
    return p;
}

struct stats {
    double lMean, lStd;
    double aMean, aStd;
    double bMean, bStd;
};

double mean( const plane &p ){
    // calculate mean
    double accum = 0;
    for( auto &c : p ){
        accum += c;
    }
    return accum /= p.size();
}

double sample_variance( const plane &p, double mean ) {
    double sum_squares = 0;
    for( auto end = p.size(), it = end - end; it < end; it++ ) {
        sum_squares += (p[it]-mean) * (p[it]-mean);
    }
    return sum_squares / (p.size()-1);
}

double stddev( const plane &p, double mean ) {
    return sqrt( sample_variance(p,mean) );
}

stats mkstats( const plane &image ) {
    auto L = split(image);
    auto &l = L[0], &a = L[1], &b = L[2];

    stats st;

    st.lStd = stddev(l, st.lMean = mean(l));
    st.aStd = stddev(a, st.aMean = mean(a));
    st.bStd = stddev(b, st.bMean = mean(b));

    return st;
}

plane to_lab( const spot::image &img ) {
    double l,a,b,R,G,B;
    auto rgba = img.rgba();
    plane p; p.reserve( rgba.size() );
    for( unsigned x = 0, end = img.w * img.h; x < end; ++x ) {
        R = rgba[x*4+0];
        G = rgba[x*4+1];
        B = rgba[x*4+2];
        Rgb2Lab( &l, &a, &b, R, G, B);
        p.push_back( l );
        p.push_back( a );
        p.push_back( b );
        p.push_back( 0 );
    }
    return p;
}

spot::image from_lab( const plane &p, unsigned w, unsigned h, unsigned stride = 4 ) {
    spot::image img( w, h );
    double l,a,b,R,G,B;
    for( unsigned x = 0, end = img.w * img.h; x < end; ++x ) {
        l = p[x*stride+0];
        a = p[x*stride+1];
        b = p[x*stride+2];
        Lab2Rgb( &R, &G, &B, l, a, b );
        // clamp the pixel intensities to [0, 255]
        if( R > 255 ) R = 255; else if( R < 0 ) R = 0;
        if( G > 255 ) G = 255; else if( G < 0 ) G = 0;
        if( B > 255 ) B = 255; else if( B < 0 ) B = 0;
        img.at( x ) = spot::rgba( (unsigned char)R, (unsigned char)G, (unsigned char)B, 255 );
    }
    return img;
}

spot::image transfer( const spot::image &dst, const spot::image &src ) {
    // convert the images from the RGB to L*ab* color space, being
    // sure to utilizing the floating point data type.
    auto source = to_lab( src ), target = to_lab( dst );

    // compute color statistics for the source and target images
    stats sSrc = mkstats( source );
    stats sTar = mkstats( target );

    // subtract the means from the target image
    // scale by the standard deviations
    // add in the source mean
    auto L = split( target, 4 );
    auto &l = L[0], &a = L[1], &b = L[2], &alpha = L[3];
    for( auto &c : l ) {
        c = (sSrc.lStd / sTar.lStd) * (c - sTar.lMean) + sSrc.lMean;
    }
    for( auto &c : a ) {
        c = (sSrc.aStd / sTar.aStd) * (c - sTar.aMean) + sSrc.aMean;
    }
    for( auto &c : b ) {
        c = (sSrc.bStd / sTar.bStd) * (c - sTar.bMean) + sSrc.bMean;
    }

    // merge the channels together and convert back to the RGB color
    // space, being sure to utilize the 8-bit unsigned integer data
    // type
    auto joint = merge( planes { l, a, b } );

    // return the color transferred image
    return from_lab( joint, dst.w, dst.h, 3 );
}


/*
License (BSD)

Copyright © 2005–2010, Pascal Getreuer
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define num double

#include <math.h>

/** @brief Min of A and B */
#define MIN(A,B)    (((A) <= (B)) ? (A) : (B))

/** @brief Max of A and B */
#define MAX(A,B)    (((A) >= (B)) ? (A) : (B))

/** @brief Min of A, B, and C */
#define MIN3(A,B,C) (((A) <= (B)) ? MIN(A,C) : MIN(B,C))

/** @brief Max of A, B, and C */
#define MAX3(A,B,C) (((A) >= (B)) ? MAX(A,C) : MAX(B,C))

#ifndef M_PI
/** @brief The constant pi */
#define M_PI    3.14159265358979323846264338327950288
#endif


/** @brief XYZ color of the D65 white point */
#define WHITEPOINT_X    0.950456
#define WHITEPOINT_Y    1.0
#define WHITEPOINT_Z    1.088754


/**
 * @brief sRGB gamma correction, transforms R to R'
 * http://en.wikipedia.org/wiki/SRGB
 */
#define GAMMACORRECTION(t)  \
    (((t) <= 0.0031306684425005883) ? \
    (12.92*(t)) : (1.055*pow((t), 0.416666666666666667) - 0.055))

/**
 * @brief Inverse sRGB gamma correction, transforms R' to R
 */
#define INVGAMMACORRECTION(t)   \
    (((t) <= 0.0404482362771076) ? \
    ((t)/12.92) : pow(((t) + 0.055)/1.055, 2.4))

/**
 * @brief CIE L*a*b* f function (used to convert XYZ to L*a*b*)
 * http://en.wikipedia.org/wiki/Lab_color_space
 */
#define LABF(t) \
    ((t >= 8.85645167903563082e-3) ? \
    pow(t,0.333333333333333) : (841.0/108.0)*(t) + (4.0/29.0))

/**
 * @brief CIE L*a*b* inverse f function
 * http://en.wikipedia.org/wiki/Lab_color_space
 */
#define LABINVF(t)  \
    ((t >= 0.206896551724137931) ? \
    ((t)*(t)*(t)) : (108.0/841.0)*((t) - (4.0/29.0)))




/*
 * == CIE color transformations ==
 *
 * The following routines implement transformations between sRGB and
 * the CIE color spaces XYZ, L*a*b, L*u*v*, and L*C*H*.  These
 * transforms assume a 2 degree observer angle and a D65 illuminant.
 */


/**
 * @brief Transform sRGB to CIE XYZ with the D65 white point
 *
 * @param X, Y, Z pointers to hold the result
 * @param R, G, B the input sRGB values
 *
 * Poynton, "Frequently Asked Questions About Color," page 10
 * Wikipedia: http://en.wikipedia.org/wiki/SRGB
 * Wikipedia: http://en.wikipedia.org/wiki/CIE_1931_color_space
 */
void Rgb2Xyz(num *X, num *Y, num *Z, num R, num G, num B)
{
    R = INVGAMMACORRECTION(R);
    G = INVGAMMACORRECTION(G);
    B = INVGAMMACORRECTION(B);
    *X = (num)(0.4123955889674142161*R + 0.3575834307637148171*G + 0.1804926473817015735*B);
    *Y = (num)(0.2125862307855955516*R + 0.7151703037034108499*G + 0.07220049864333622685*B);
    *Z = (num)(0.01929721549174694484*R + 0.1191838645808485318*G + 0.9504971251315797660*B);
}


/**
 * @brief Transform CIE XYZ to sRGB with the D65 white point
 *
 * @param R, G, B pointers to hold the result
 * @param X, Y, Z the input XYZ values
 *
 * Official sRGB specification (IEC 61966-2-1:1999)
 * Poynton, "Frequently Asked Questions About Color," page 10
 * Wikipedia: http://en.wikipedia.org/wiki/SRGB
 * Wikipedia: http://en.wikipedia.org/wiki/CIE_1931_color_space
 */
void Xyz2Rgb(num *R, num *G, num *B, num X, num Y, num Z)
{
    num R1, B1, G1, Min;


    R1 = (num)( 3.2406*X - 1.5372*Y - 0.4986*Z);
    G1 = (num)(-0.9689*X + 1.8758*Y + 0.0415*Z);
    B1 = (num)( 0.0557*X - 0.2040*Y + 1.0570*Z);

    Min = MIN3(R1, G1, B1);

    /* Force nonnegative values so that gamma correction is well-defined. */
    if(Min < 0)
    {
        R1 -= Min;
        G1 -= Min;
        B1 -= Min;
    }

    /* Transform from RGB to R'G'B' */
    *R = GAMMACORRECTION(R1);
    *G = GAMMACORRECTION(G1);
    *B = GAMMACORRECTION(B1);
}


/**
 * Convert CIE XYZ to CIE L*a*b* (CIELAB) with the D65 white point
 *
 * @param L, a, b pointers to hold the result
 * @param X, Y, Z the input XYZ values
 *
 * Wikipedia: http://en.wikipedia.org/wiki/Lab_color_space
 */
void Xyz2Lab(num *L, num *a, num *b, num X, num Y, num Z)
{
    X /= WHITEPOINT_X;
    Y /= WHITEPOINT_Y;
    Z /= WHITEPOINT_Z;
    X = LABF(X);
    Y = LABF(Y);
    Z = LABF(Z);
    *L = 116*Y - 16;
    *a = 500*(X - Y);
    *b = 200*(Y - Z);
}


/**
 * Convert CIE L*a*b* (CIELAB) to CIE XYZ with the D65 white point
 *
 * @param X, Y, Z pointers to hold the result
 * @param L, a, b the input L*a*b* values
 *
 * Wikipedia: http://en.wikipedia.org/wiki/Lab_color_space
 */
void Lab2Xyz(num *X, num *Y, num *Z, num L, num a, num b)
{
    L = (L + 16)/116;
    a = L + a/500;
    b = L - b/200;
    *X = WHITEPOINT_X*LABINVF(a);
    *Y = WHITEPOINT_Y*LABINVF(L);
    *Z = WHITEPOINT_Z*LABINVF(b);
}


/*
 * == Glue functions for multi-stage transforms ==
 */

void Rgb2Lab(num *L, num *a, num *b, num R, num G, num B)
{
    num X, Y, Z;
    Rgb2Xyz(&X, &Y, &Z, R, G, B);
    Xyz2Lab(L, a, b, X, Y, Z);
}


void Lab2Rgb(num *R, num *G, num *B, num L, num a, num b)
{
    num X, Y, Z;
    Lab2Xyz(&X, &Y, &Z, L, a, b);
    Xyz2Rgb(R, G, B, X, Y, Z);
}
