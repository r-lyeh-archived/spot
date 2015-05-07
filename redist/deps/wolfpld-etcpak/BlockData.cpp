#include <assert.h>
#include <string.h>

#include "BlockData.hpp"
//#include "ColorSpace.hpp"
#include "Common.hpp"
#include "ProcessCommon.hpp"

static int AdjustSizeForMipmaps( const v2i& size, int levels )
{
    int len = 0;
    v2i current = size;
    for( int i=1; i<levels; i++ )
    {
        assert( current.x != 1 || current.y != 1 );
        current.x = std::max( 1, current.x / 2 );
        current.y = std::max( 1, current.y / 2 );
        len += std::max( 4, current.x ) * std::max( 4, current.y ) / 2;
    }
    assert( current.x == 1 && current.y == 1 );
    return len;
}

BlockData::BlockData( const v2i& size, bool mipmap )
    : m_size( size )
    , m_done( false )
    , m_dataOffset( 52 )
    , m_maplen( 52 + m_size.x*m_size.y/2 )
{
    assert( m_size.x%4 == 0 && m_size.y%4 == 0 );
    if( mipmap )
    {
        const int levels = NumberOfMipLevels( size );
        m_maplen += AdjustSizeForMipmaps( size, levels );
    }
    m_data = new uint8_t[m_maplen];
}

BlockData::~BlockData()
{
    if( !m_done ) Finish();
    delete[] m_data;
}

void BlockData::Process( const uint8_t* src, uint32_t blocks, size_t offset, uint quality, Channels type )
{
    auto dst = ((uint64_t*)( m_data + m_dataOffset )) + offset;

    std::lock_guard<std::mutex> lock( m_lock );

    if( type == Channels::Alpha )
    {
        m_work.push_back( std::async( [src, dst, blocks, this]() mutable { do { *dst++ = ProcessAlpha( src ); src += 4*4; } while( --blocks ); } ) );
    }
    else
    {
        switch( quality )
        {
        case 0:
            m_work.push_back( std::async( [src, dst, blocks, this]() mutable { do { *dst++ = ProcessRGB( src ); src += 4*4*4; } while( --blocks ); } ) );
            break;
        case 1:
            //m_work.push_back( std::async( [src, dst, blocks, this]{ ProcessBlocksLab( src, dst, blocks ); } ) );
            break;
        default:
            assert( false );
            break;
        }
    }
}

void BlockData::Finish()
{
    assert( !m_done );
    assert( !m_work.empty() );
    for( auto& f : m_work )
    {
        f.wait();
    }
    m_done = true;
    m_work.clear();
    m_bmp.reset();
}
