#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>

#include "Bitmap.hpp"
#include "Common.hpp"

Bitmap::Bitmap( const uint32_t* data, uint len, uint width, uint height, uint lines )
    : m_block( nullptr )
    , m_lines( lines )
    , m_padding( 0 )
    , m_alpha( false ) //len > width * height * 3 )
{
    m_size.x = ( width / 4 ) * 4;
    m_size.y = ( height / 4 ) * 4;

    // crop right border if not multiple of 4
    if( width > m_size.x ) {
        m_padding = new uint32_t [m_size.x * m_size.y];
        for( unsigned y = 0; y < m_size.y; ++y ) {
            memcpy( &m_padding[ y * m_size.x ], &data[ y * width ], m_size.x * 4 );
        }
        data = m_padding;
    } 

    m_block = m_data = (const uint32_t *)data;
    m_linesLeft = m_size.y / 4;
}

Bitmap::Bitmap( const Bitmap& src, uint lines )
    : m_lines( lines )
    , m_linesLeft( 0)
    , m_padding( 0 )
    , m_alpha( src.Alpha() )
{
}

Bitmap::~Bitmap() {
    if( m_padding ) {
        delete [] m_padding;
    }
}

const uint32_t* Bitmap::NextBlock( uint& lines, bool& done )
{
    lines = std::min( m_lines, m_linesLeft );
    auto ret = m_block;
    m_block += m_size.x * 4 * lines;
    m_linesLeft -= lines;
    done = m_linesLeft == 0;
    return ret;
}
