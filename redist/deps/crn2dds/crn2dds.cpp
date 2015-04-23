// crn2dds, based on code by SpartanJ and Evan Parker.
// - rlyeh, public domain

#include "crn2dds.hpp"

#ifdef _WIN32
#ifndef WIN32
#define WIN32
#endif
#endif

#include <stddef.h> // For NULL, size_t
#include <cstring> // for malloc etc

#ifndef USE_MALLOC_USABLE_SIZE
// Define HAVE_MALLOC_USABLE_SIZE to 1 to indicate that the current
// system has malloc_usable_size(). Which takes the address of a malloc-ed
// pointer, and return the size of the underlying storage block.
// This is useful to optimize heap memory usage.
// Including at least one C library header is required to define symbols
// like __GLIBC__. Choose carefully because some headers like <stddef.h>
// are actually provided by the compiler, not the C library and do not
// define the macros we need.
#include <stdint.h>
#if defined(__GLIBC__)
#  include <malloc.h>
#  define USE_MALLOC_USABLE_SIZE  1
#elif defined(__APPLE__) || defined(__FreeBSD__)
#  include <malloc/malloc.h>
#  define malloc_usable_size  malloc_size
#  define USE_MALLOC_USABLE_SIZE  1
#elif defined(__ANDROID_API__)
#  include <malloc.h>
extern "C" size_t dlmalloc_usable_size(void*);
#  define malloc_usable_size dlmalloc_usable_size
#  define USE_MALLOC_USABLE_SIZE  1
#elif defined(_WIN32)
#  include <malloc.h>
#  define malloc_usable_size _msize
#  define USE_MALLOC_USABLE_SIZE  1
#else
#  define USE_MALLOC_USABLE_SIZE  0
#endif
#endif // USE_MALLOC_USABLE_SIZE

#include "crn_decomp.h"

extern "C" {
    unsigned int crn_get_width(const void *src, unsigned int src_size);
    unsigned int crn_get_height(const void *src, unsigned int src_size);
    unsigned int crn_get_levels(const void *src, unsigned int src_size);
    unsigned int crn_get_dxt_format(const void *src, unsigned int src_size);
    unsigned int crn_get_uncompressed_size(const void *p, unsigned int size);
    void crn_decompress(const void *src, unsigned int src_size, void *dst, unsigned int dst_size);
}

unsigned int crn_get_width(const void *src, unsigned int src_size) {
    crnd::crn_texture_info tex_info;
    crnd::crnd_get_texture_info(static_cast<const crn_uint8*>(src), src_size, &tex_info);
    return tex_info.m_width;
}

unsigned int crn_get_height(const void *src, unsigned int src_size) {
    crnd::crn_texture_info tex_info;
    crnd::crnd_get_texture_info(static_cast<const crn_uint8*>(src), src_size, &tex_info);
    return tex_info.m_height;
}

unsigned int crn_get_levels(const void *src, unsigned int src_size) {
    crnd::crn_texture_info tex_info;
    crnd::crnd_get_texture_info(static_cast<const crn_uint8*>(src), src_size, &tex_info);
    return tex_info.m_levels;
}

unsigned int crn_get_dxt_format(const void *src, unsigned int src_size) {
    crnd::crn_texture_info tex_info;
    crnd::crnd_get_texture_info(static_cast<const crn_uint8*>(src), src_size, &tex_info);
    return tex_info.m_format;
}

unsigned int crn_get_uncompressed_size(const void *src, unsigned int src_size) {
    crnd::crn_texture_info tex_info;
    crnd::crnd_get_texture_info(static_cast<const crn_uint8*>(src), src_size, &tex_info);
    const crn_uint32 width = tex_info.m_width;
    const crn_uint32 height = tex_info.m_height;
    const crn_uint32 blocks_x = (width + 3) >> 2;
    const crn_uint32 blocks_y = (height + 3) >> 2;
    const crn_uint32 row_pitch = blocks_x * crnd::crnd_get_bytes_per_dxt_block(tex_info.m_format);
    const crn_uint32 total_face_size = row_pitch * blocks_y;
    return total_face_size;
}

void crn_decompress(const void *src, unsigned int src_size, void *dst, unsigned int dst_size) {
    crnd::crn_texture_info tex_info;
    crnd::crnd_get_texture_info(static_cast<const crn_uint8*>(src), src_size, &tex_info);
    const crn_uint32 width = tex_info.m_width;
    const crn_uint32 height = tex_info.m_height;
    const crn_uint32 blocks_x = (width + 3) >> 2;
    const crn_uint32 blocks_y = (height + 3) >> 2;
    const crn_uint32 row_pitch = blocks_x * crnd::crnd_get_bytes_per_dxt_block(tex_info.m_format);

    crnd::crnd_unpack_context pContext =
      crnd::crnd_unpack_begin(static_cast<const crn_uint8*>(src), src_size);
    void *pDecomp_images[1];
    pDecomp_images[0] = dst;
    crnd::crnd_unpack_level(pContext, pDecomp_images, dst_size, row_pitch, 0);
    crnd::crnd_unpack_end(pContext);
}

// from soil2

#include <stdint.h>

typedef struct
{
    uint32_t dwMagic;
    uint32_t dwSize;
    uint32_t dwFlags;
    uint32_t dwHeight;
    uint32_t dwWidth;
    uint32_t dwPitchOrLinearSize;
    uint32_t dwDepth;
    uint32_t dwMipMapCount;
    uint32_t dwReserved1[ 11 ];

    /*  DDPIXELFORMAT */
    struct
    {
        uint32_t dwSize;
        uint32_t dwFlags;
        uint32_t dwFourCC;
        uint32_t dwRGBBitCount;
        uint32_t dwRBitMask;
        uint32_t dwGBitMask;
        uint32_t dwBBitMask;
        uint32_t dwAlphaBitMask;
    }
    sPixelFormat;

    /*  DDCAPS2 */
    struct
    {
        uint32_t dwCaps1;
        uint32_t dwCaps2;
        uint32_t dwDDSX;
        uint32_t dwReserved;
    }
    sCaps;
    uint32_t dwReserved2;
}
DDS_header;

/*  the following constants were copied directly off the MSDN website */

/*  The dwFlags member of the original DDSURFACEDESC2 structure
  can be set to one or more of the following values.  */
#define DDSD_CAPS 0x00000001
#define DDSD_HEIGHT 0x00000002
#define DDSD_WIDTH  0x00000004
#define DDSD_PITCH  0x00000008
#define DDSD_PIXELFORMAT  0x00001000
#define DDSD_MIPMAPCOUNT  0x00020000
#define DDSD_LINEARSIZE 0x00080000
#define DDSD_DEPTH  0x00800000

/*  DirectDraw Pixel Format */
#define DDPF_ALPHAPIXELS  0x00000001
#define DDPF_FOURCC 0x00000004
#define DDPF_RGB  0x00000040

/*  The dwCaps1 member of the DDSCAPS2 structure can be
  set to one or more of the following values. */
#define DDSCAPS_COMPLEX 0x00000008
#define DDSCAPS_TEXTURE 0x00001000
#define DDSCAPS_MIPMAP  0x00400000

/*  The dwCaps2 member of the DDSCAPS2 structure can be
  set to one or more of the following values.   */
#define DDSCAPS2_CUBEMAP  0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000
#define DDSCAPS2_VOLUME 0x00200000

#include <stdlib.h>
#include <cassert>
#include <string>

// returns a pure dxt stream (can be uploaded to a GPU directly)
bool crn2dxt( std::string &out, const void *src, size_t zlen, unsigned reserved ) {
    if( zlen > 2 && ((const char *)src)[0] == 'H' && ((const char *)src)[1] == 'x' ) {
        unsigned width( crn_get_width(src,zlen) );
        unsigned height( crn_get_height(src,zlen) );
        unsigned len( crn_get_uncompressed_size(src,zlen) );

        out.resize( reserved + len );
        crn_decompress( src, zlen, &out[reserved], len );
        return true;
    }
    return false;
}
bool crn2dxt( std::string &out, const std::string &in, unsigned reserved ) {
    return crn2dxt( out, &in[0], in.size(), reserved );
}

// returns a cooked dds header + pure dxt stream (can be saved or processed somewhere else)
bool crn2dds( std::string &out, const void *src, size_t zlen ) {
    if( crn2dxt( out, src, zlen, 128 ) ) {
        assert( sizeof(DDS_header) == 128 );

        unsigned width( crn_get_width(src,zlen) );
        unsigned height( crn_get_height(src,zlen) );
        unsigned len( crn_get_uncompressed_size(src,zlen) );

        unsigned channels = 3;
        switch( crn_get_dxt_format(src,zlen) ) {
          default:
          case cCRNFmtDXT1: channels = 3; break;
          case cCRNFmtDXT3: channels = 3; break;

          case cCRNFmtDXT5:
          // Various DXT5 derivatives
          case cCRNFmtDXT5_CCxY:    // Luma-chroma
          case cCRNFmtDXT5_xGxR:    // Swizzled 2-component
          case cCRNFmtDXT5_xGBR:    // Swizzled 3-component
          case cCRNFmtDXT5_AGBR:    // Swizzled 4-component
          // ATI 3DC and X360 DXN
          case cCRNFmtDXN_XY:
          case cCRNFmtDXN_YX: channels = 4; break;

          // DXT5 alpha blocks only
          case cCRNFmtDXT5A: channels = 1; break;
          case cCRNFmtETC1: channels = 3; break;
        }

        DDS_header header;
        memset( &header, 0, sizeof( DDS_header ) );
        header.dwMagic = ('D' << 0) | ('D' << 8) | ('S' << 16) | (' ' << 24);
        header.dwSize = 124;
        header.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_LINEARSIZE;
        header.dwWidth = width;
        header.dwHeight = height;
        header.dwPitchOrLinearSize = len;
        header.sPixelFormat.dwSize = 32;
        header.sPixelFormat.dwFlags = DDPF_FOURCC;
        if( (channels & 1) == 1 ) {
          header.sPixelFormat.dwFourCC = ('D' << 0) | ('X' << 8) | ('T' << 16) | ('1' << 24);
        } else {
          header.sPixelFormat.dwFourCC = ('D' << 0) | ('X' << 8) | ('T' << 16) | ('5' << 24);
        }
        header.sCaps.dwCaps1 = DDSCAPS_TEXTURE;
        
        memcpy( &out[0], &header, 128 );
        return true;
    }

    return false;
}

bool crn2dds( std::string &out, const std::string &in ) {
    return crn2dds( out, &in[0], in.size() );
}
