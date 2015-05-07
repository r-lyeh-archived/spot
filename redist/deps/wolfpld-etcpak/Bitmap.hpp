#ifndef __DARKRL__BITMAP_HPP__
#define __DARKRL__BITMAP_HPP__

#include <memory>

#include "Types.hpp"
#include "Vector.hpp"

class Bitmap
{
public:
    Bitmap( const uint32_t* data, uint len, uint width, uint height, uint lines );
    Bitmap( const v2i& size );
    virtual ~Bitmap();

    const uint32_t* Data() const { return m_data; }
    const v2i& Size() const { return m_size; }
    bool Alpha() const { return m_alpha; }

    const uint32_t* NextBlock( uint& lines, bool& done );

protected:
    Bitmap( const Bitmap& src, uint lines );

    const uint32_t* m_data;
    const uint32_t* m_block;
    uint m_lines;
    uint m_linesLeft;
    uint32_t *m_padding;
    v2i m_size;
    bool m_alpha;
};

typedef std::shared_ptr<Bitmap> BitmapPtr;

#endif
