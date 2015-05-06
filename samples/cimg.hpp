// - rlyeh, public domain

#pragma once

#include <math.h>
#include <iostream>

// Mean Squared Error is the average squared difference between a reference image and a distorted image. 
// It is computed pixel-by-pixel by adding up the squared differences of all the pixels and dividing by the total pixel count
spot::color mse( const spot::image &A, const spot::image &B ) {
    if( A.w == B.w && A.h == B.h && A.size() ) {
        spot::color mse;
        for( auto end = A.size(), it = end - end; it < end; ++it ) {
            auto diff = B[ it ] - A[ it ];
            mse += diff * diff;
        }
        mse /= A.size();
        return mse;
    }
    return spot::color(0,0,0,0);
}

// Peak Signal-to-Noise Ratio is the ratio between the reference signal and the distortion signal in an image, 
// given in decibels. The higher the PSNR, the closer the distorted image is to the original. In general, a higher PSNR value
// should correlate to a higher quality image, but tests have shown that this isn't always the case. However, PSNR is a popular
// quality metric because it's easy and fast to calculate while still giving okay results.
double psnr( spot::color mse, const spot::image &A, const spot::image &B ) {
    if( A.w == B.w && A.h == B.h ) {
        mse *= 255; // use [0..255] range instead
        double max2 = 255 * 255;
        double psnr_h = 10 * log10( max2 / mse.h );
        double psnr_s = 10 * log10( max2 / mse.s );
        double psnr_l = 10 * log10( max2 / mse.l );
        if( mse.a <= 0 ) {
            return ( psnr_h + psnr_s + psnr_l ) / 3;
        }
        double psnr_a = 10 * log10( max2 / mse.a );
        return ( psnr_h + psnr_s + psnr_l + psnr_a ) / 4;
    }
    return 0;
}

spot::image diff( const spot::image &A, const spot::image &B, double *psnr_ = 0 ) {
    double psnr = 0;
    if( A.w == B.w && A.h == B.h ) {
        float x4 = 4.f;
        spot::color mse = ::mse( A, B );
		psnr = ::psnr( mse, A, B );
        std::cout << " (psnr: " << psnr << ")";
        spot::image diff( A.w, A.h );
        for( auto end = A.size(), it = end - end; it < end; ++it ) {
            auto error2 = ( ( B[ it ] - A[ it ] ) * ( B[ it ] - A[ it ] ) );
            auto error = ( ( sqrt(error2.h) + sqrt(error2.s) + sqrt(error2.l) ) / 3 ) * x4;
            diff[ it ] = spot::hsl( 0, 0, error ).clamp();
        }
	    if( psnr_ ) *psnr_ = psnr;
        return diff;
    }
    if( psnr_ ) *psnr_ = psnr;
    return spot::image();
}

#include <algorithm>
spot::image concat( const spot::image &pic1, const spot::image &pic2, const spot::image &pic3 = spot::image() ) {
    spot::image seq( pic1.w + pic2.w + pic3.w, std::max( std::max( pic1.h, pic2.h ), pic3.h ) );
    seq = seq.paste( 0, 0, pic1 );
    seq = seq.paste( pic1.w, 0, pic2 );
    seq = seq.paste( pic1.w+pic2.w, 0, pic3 );
    return seq;
}

spot::image alphagrid( const spot::image &pic, int cell_size = 16 ) {
    spot::image check(pic.w, pic.h);
    // cell_size = pic.w * 0.10f; // cell size is 10% of image, check pattern is 10 cells wide.
    for (size_t y = 0; y < check.h; ++y) {
        for (size_t x = 0; x < check.w; ++x) {
            int xc = x / cell_size;
            int yc = y / cell_size;
            float light = 0.5f + 0.5f * ((xc % 2) ^ (yc % 2));
            check.at(x, y) = spot::hsla(0.f, 0.f, light, 1.f);
        }
    }
    // blend image
    for (size_t p = 0, pend = pic.size(); p < pend; ++p) {
        float a = pic.at(p).a;
        check.at(p) = pic.at(p) * a + check.at(p) * (1 - a);
        check.at(p).a = 1;
    }
    return check;
}

#ifdef __ANDROID_API__
// following code wont compile in android (missing X11 headers in cimg.h)
#else
// CImg.h code following
#ifdef SPOT_CIMG_NO_EXCEPTIONS
#define try if(1)
#define catch(...) if(0)
#define throw (void)
#include "cimg.h"
#undef throw
#undef catch
#undef try
#else
#include "cimg.h"
#endif
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
void display(const spot::image &pic, const std::string title = std::string(), bool checkgrid = true) {
    if (pic.size()) {
        using namespace spot;
        // check pattern that covers whole image.
        spot::image check = checkgrid ? alphagrid(pic) : pic;
        // display blend
        auto &blend = check;
        cimg_library::CImg<unsigned char> ctexture(blend.w, blend.h, 1, 4, 0);
        for (size_t y = 0; y < blend.h; ++y) {
            for (size_t x = 0; x < blend.w; ++x) {
                spot::pixel pix = blend.at(x, y);
                ctexture(x, y, 0) = pix.r;
                ctexture(x, y, 1) = pix.g;
                ctexture(x, y, 2) = pix.b;
                ctexture(x, y, 3) = pix.a;
            }
        }
        ctexture.display(title.c_str(), false);
    }
}
#endif

// timing - rlyeh, public domain {
#if defined(_MSC_VER) || defined(__ANDROID_API__)
#define USE_OMP
#endif
#ifndef USE_OMP
#include <chrono>
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
template<typename FN>
double bench( const FN &fn ) {
    auto took = -now();
    return ( fn(), took + now() );
}
#include <chrono>
#include <thread>
void sleep( double secs ) {
    std::chrono::microseconds duration( (int)(secs * 1000000) );
    std::this_thread::sleep_for( duration );    
}
// } timing
