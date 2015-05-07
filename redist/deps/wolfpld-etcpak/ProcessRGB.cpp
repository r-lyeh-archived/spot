#include <string.h>

#include "Math.hpp"
#include "Common.hpp"
#include "Types.hpp"
#include "Vector.hpp"

static v3i Average( const uint8_t* data )
{
    uint32_t r = 0, g = 0, b = 0;
    for( int i=0; i<8; i++ )
    {
        b += *data++;
        g += *data++;
        r += *data++;
        data++;
    }
    return v3i( r / 8, g / 8, b / 8 );
}

static void CalcErrorBlock( const uint8_t* data, uint err[4] )
{
    for( int i=0; i<8; i++ )
    {
        uint d = *data++;
        err[0] += d;
        err[3] += d*d;
        d = *data++;
        err[1] += d;
        err[3] += d*d;
        d = *data++;
        err[2] += d;
        err[3] += d*d;
        data++;
    }
}

static uint CalcError( const uint block[4], const v3i& average )
{
    uint err = block[3];
    err -= block[0] * 2 * average.z;
    err -= block[1] * 2 * average.y;
    err -= block[2] * 2 * average.x;
    err += 8 * ( sq( average.x ) + sq( average.y ) + sq( average.z ) );
    return err;
}

static void ProcessAverages( v3i* a )
{
    for( int i=0; i<2; i++ )
    {
        for( int j=0; j<3; j++ )
        {
            int32_t c1 = mul8bit( a[i*2+1][j], 31 );
            int32_t c2 = mul8bit( a[i*2][j], 31 );

            int32_t diff = c2 - c1;
            if( diff > 3 ) diff = 3;
            else if( diff < -4 ) diff = -4;

            int32_t co = c1 + diff;

            a[5+i*2][j] = ( c1 << 3 ) | ( c1 >> 2 );
            a[4+i*2][j] = ( co << 3 ) | ( co >> 2 );
        }
    }
    for( int i=0; i<4; i++ )
    {
        a[i].x = g_avg2[mul8bit( a[i].x, 15 )];
        a[i].y = g_avg2[mul8bit( a[i].y, 15 )];
        a[i].z = g_avg2[mul8bit( a[i].z, 15 )];
    }
}

static void EncodeAverages( uint64_t& _d, const v3i* a, size_t idx )
{
    auto d = _d;
    d |= ( idx << 24 );
    size_t base = idx << 1;

    if( ( idx & 0x2 ) == 0 )
    {
        for( int i=0; i<3; i++ )
        {
            d |= uint64_t( a[base+0][i] >> 4 ) << ( i*8 );
            d |= uint64_t( a[base+1][i] >> 4 ) << ( i*8 + 4 );
        }
    }
    else
    {
        for( int i=0; i<3; i++ )
        {
            d |= uint64_t( a[base+1][i] & 0xF8 ) << ( i*8 );
            int32_t c = ( ( a[base+0][i] & 0xF8 ) - ( a[base+1][i] & 0xF8 ) ) >> 3;
            c &= ~0xFFFFFFF8;
            d |= ((uint64_t)c) << ( i*8 );
        }
    }
    _d = d;
}

static uint64_t CheckSolid( const uint8_t* src )
{
    const uint8_t* ptr = src + 4;
    for( int i=1; i<16; i++ )
    {
        if( memcmp( src, ptr, 4 ) != 0 )
        {
            return 0;
        }
        ptr += 4;
    }
    return 0x02000000 |
        ( uint( src[0] & 0xF8 ) << 16 ) |
        ( uint( src[1] & 0xF8 ) << 8 ) |
        ( uint( src[2] & 0xF8 ) );
}

static void PrepareBuffers( uint8_t b23[2][32], const uint8_t* src )
{
    for( int i=0; i<4; i++ )
    {
        memcpy( b23[1]+i*8, src+i*16, 8 );
        memcpy( b23[0]+i*8, src+i*16+8, 8 );
    }
}

static void PrepareAverages( v3i a[8], const uint8_t* b[4], uint err[4] )
{
    for( int i=0; i<4; i++ )
    {
        a[i] = Average( b[i] );
    }
    ProcessAverages( a );

    for( int i=0; i<4; i++ )
    {
        uint errblock[4] = {};
        CalcErrorBlock( b[i], errblock );
        err[i/2] += CalcError( errblock, a[i] );
        err[2+i/2] += CalcError( errblock, a[i+4] );
    }
}

static void FindBestFit( uint64_t terr[2][8], uint tsel[16][8], v3i a[8], const uint32_t* id, const uint8_t* data )
{
    for( size_t i=0; i<16; i++ )
    {
        uint* sel = tsel[i];
        uint bid = id[i];
        uint64_t* ter = terr[bid%2];

        uint8_t b = *data++;
        uint8_t g = *data++;
        uint8_t r = *data++;
        data++;

        int dr = a[bid].x - r;
        int dg = a[bid].y - g;
        int db = a[bid].z - b;

        int pix = dr * 77 + dg * 151 + db * 28;

        for( int t=0; t<8; t++ )
        {
            const int64_t* tab = g_table256[t];
            uint idx = 0;
            uint64_t err = sq( tab[0] + pix );
            for( int j=1; j<4; j++ )
            {
                uint64_t local = sq( tab[j] + pix );
                if( local < err )
                {
                    err = local;
                    idx = j;
                }
            }
            *sel++ = idx;
            *ter++ += err;
        }
    }
}

uint64_t ProcessRGB( const uint8_t* src )
{
    uint64_t d = CheckSolid( src );
    if( d != 0 ) return d;

    uint8_t b23[2][32];
    const uint8_t* b[4] = { src+32, src, b23[0], b23[1] };
    PrepareBuffers( b23, src );

    v3i a[8];
    uint err[4] = {};
    PrepareAverages( a, b, err );
    size_t idx = GetLeastError( err, 4 );
    EncodeAverages( d, a, idx );

    uint64_t terr[2][8] = {};
    uint tsel[16][8];
    auto id = g_id[idx];
    FindBestFit( terr, tsel, a, id, src );

    return FixByteOrder( EncodeSelectors( d, terr, tsel, id ) );
}
