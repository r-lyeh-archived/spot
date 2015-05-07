#ifndef __DARKRL__UTILS_HPP__
#define __DARKRL__UTILS_HPP__

#include <assert.h>
#include <stddef.h>

#ifdef DEBUG
#  include <sstream>
#  include <stdlib.h>
#  define DBGPRINT(msg) do { std::stringstream __buf; __buf << msg; printf( "%s", __buf.str().c_str() ); } while(0)
#else
#  define DBGPRINT(msg) do { ((void)0); } while(0)
#endif

#include "Bitmap.hpp"
#include "Types.hpp"
#include "Vector.hpp"

float CalcMSE3( const Bitmap& bmp, const Bitmap& out );
float CalcMSE1( const Bitmap& bmp, const Bitmap& out );
void Dither( uint8_t* data );

inline int NumberOfMipLevels( const v2i& size )
{
    //return (int)floor( log2( std::max( size.x, size.y ) ) ) + 1;
    double log2n = log( std::max( size.x, size.y ) ) / log( 2. );
    return (int)floor( log2n ) + 1;
}

extern const int32_t g_table[8][4];
extern const int64_t g_table256[8][4];

extern const uint32_t g_id[4][16];

extern const uint32_t g_avg2[16];

// process {
uint64_t ProcessAlpha( const uint8_t* src );
uint64_t ProcessRGB( const uint8_t* src );

template<class T>
static size_t GetLeastError( const T* err, size_t num )
{
    size_t idx = 0;
    for( size_t i=1; i<num; i++ )
    {
        if( err[i] < err[idx] )
        {
            idx = i;
        }
    }
    return idx;
}

static uint64_t FixByteOrder( uint64_t d )
{
    return ( ( d & 0x00000000FFFFFFFF ) ) |
           ( ( d & 0xFF00000000000000 ) >> 24 ) |
           ( ( d & 0x000000FF00000000 ) << 24 ) |
           ( ( d & 0x00FF000000000000 ) >> 8 ) |
           ( ( d & 0x0000FF0000000000 ) << 8 );
}

template<class T>
static uint64_t EncodeSelectors( uint64_t d, const T terr[2][8], const uint tsel[16][8], const uint32_t* id )
{
    size_t tidx[2];
    tidx[0] = GetLeastError( terr[0], 8 );
    tidx[1] = GetLeastError( terr[1], 8 );

    d |= tidx[0] << 26;
    d |= tidx[1] << 29;
    for( int i=0; i<16; i++ )
    {
        uint64_t t = tsel[i][tidx[id[i]%2]];
        d |= ( t & 0x1 ) << ( i + 32 );
        d |= ( t & 0x2 ) << ( i + 47 );
    }

    return d;
}
// }

#endif
