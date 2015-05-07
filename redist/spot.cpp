/** this is an amalgamated file. do not edit.
 */

/* Handy pixel/color and texture/image classes. BOOST licensed.
 * - rlyeh ~~ listening to Twilightning - Painting the blue eyes

 *   Hue - Think of a color wheel. Around 0 and 255 are reds 85 are greens, 170 are blues. Use anything in between 0-255. Values above and below will be modulus 255.
 *   Saturation - 0 is completely denatured (grayscale). 255 is fully saturated (full color).
 *   Lightness - 0 is completely dark (black). 255 is completely light (white). 127 is average lightness.
 *   alpha - Opacity/Transparency value. 0 is fully transparent. 255 is fully opaque. 127 is 50% transparent.

 *   enum hue
 *   {
 *       hred1 = 0,
 *       hgreen = 85,
 *       hblue = 170,
 *       hred2 = 255,

 *       hpurple =  ( ( hblue + hred2 ) / 2 ) % 255,
 *       hcyan =  ( ( hgreen + hblue ) / 2 ) % 255,
 *       hyellow = ( ( hred1 + hgreen ) / 2 ) % 255
 *   };

 *   unsigned char make_hue( float reds, float greens, float blues )
 *   {
 *       return (unsigned char)( int( reds * 255 + greens * 85 + blues * 170 ) % 255 );
 *   }

 * valid color components in range [0..1]
 * components can handle higher values (useful for color algebra), but their values will be clamped when displayed

 * image = array of pixels[] in RGBA or HSLA format
 * texture = strict subset of an image { squared, rgba only }
 * so, image > texture

 * image @todo
 * vg {
 * image.glow( 1px, pixel::black )
 * image.glow( 2px, pixel::red )
 * image.mask(white);
 * image.mask(white).glow( 2x, pixel::blue ).replace(white,transp) --> L4D2 style
 * image.aabb(); -> return aabb for this frame
 * }
 * composition {
 * image.append( img[], align09 = 6 )
 * image.crop( columns/rows to remove, align09 = 2, method = fast/carving )
 * image.shrink( w, h )
 * }
 * image.noisex()
 * image.perlin()
 * image.mipmap()
 * image.upload()/unload()
 * image.download()/capture()
 * image.rotate(deg, bgcolor/transparent?)
 * image.premultiply()/unpremultiply()
 * image.gamma(1.22)
 * image.atlas( image[] )/combine(other)
 * image.blur()/sharpen()
 * image.scale(marquee_scale,content_scale,scale_method)
 * @todo: also image1 * / *= / + / += image2

 * pixel @Todo:
 *  replace float with float8 (microfloat 0.4.4 ; good idea?)
 *  to_rg() // computer vision: r = r/r+g+b, g = g/r+g+b
 *  munsell 5 hues: red, yellow, green, blue, purple
 *  hsl 6 hues: red, yellow, green, cyan, blue, purple
 *  rgy
 *  http://en.wikipedia.org/wiki/HSL_color_space#Disadvantages
 *  https://github.com/aras-p/miniexr/blob/master/miniexr.cpp
 *  http://cgit.haiku-os.org/haiku/plain/src/add-ons/translators/exr/openexr/half/half.h
 *  ftp://www.fox-toolkit.org/pub/fasthalffloatconversion.pdf
 *  std::string str() const { return std::string(); } //#ffffffff

*/

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "spot.hpp"

#define NANOSVG_ALL_COLOR_KEYWORDS  // Include full list of color keywords.
#define NANOSVG_IMPLEMENTATION      // Expands implementation
#define NANOSVGRAST_IMPLEMENTATION  // Expands implementation

#include "deps/nanosvg/src/nanosvg.h"
#include "deps/nanosvg/src/nanosvgrast.h"

/*
#define STBI_MALLOC(x)    spot_malloc(x)
#define STBI_FREE(p)      spot_free(p)
#define STBI_REALLOC(p,x) spot_realloc(p,x)
*/
#define STB_IMAGE_STATIC
#define STBI_NO_STDIO
#include "deps/soil2/stb_image.c"
#include "deps/soil2/stb_image_write.c"

#define DDS_header DDS_header2
#   include "deps/soil2/image_DXT.h"
#   include "deps/soil2/image_DXT.c"
#define clamp clamp2
#   include "deps/etc1/etc1_utils.c"
#   include "deps/soil2/image_helper.c"
#   include "deps/etc1/rg_etc1.cpp"
#undef  clamp

#include "deps/jpge/jpge.h"
#include "deps/jpge/jpge.cpp"

#include "deps/lodepng/lodepng.h"
#include "deps/lodepng/lodepng.cpp"

#include "deps/crn2dds/crn2dds.hpp"
#undef DDS_header
#define DDS_header DDS_header3
#include "deps/crn2dds/crn2dds.cpp"

#if defined(SPOT_OPENGL_HEADER)
#   include SPOT_OPENGL_HEADER
#   include "deps/soil2/SOIL2.h"
#   include "deps/soil2/SOIL2.c"
#endif

#include "deps/jthlim-pvrtccompressor/BitScale.cpp"
#include "deps/jthlim-pvrtccompressor/MortonTable.cpp"
#include "deps/jthlim-pvrtccompressor/PvrTcDecoder.cpp"
#include "deps/jthlim-pvrtccompressor/PvrTcEncoder.cpp"
#include "deps/jthlim-pvrtccompressor/PvrTcPacket.cpp"

namespace spot {
#include "deps/containers/pvr3.hpp"
#include "deps/containers/pkm.hpp"
#include "deps/containers/ktx.hpp"
#include "deps/pvrtc/PVRTDecompress.cpp"
}

#define Bitmap Bitmap2
#include "deps/wolfpld-etcpak/Bitmap.cpp"
#include "deps/wolfpld-etcpak/BlockBitmap.cpp"
#include "deps/wolfpld-etcpak/BlockData.cpp"
#include "deps/wolfpld-etcpak/Dither.cpp"
#include "deps/wolfpld-etcpak/MSE.cpp"
#include "deps/wolfpld-etcpak/ProcessAlpha.cpp"
#define CalcErrorBlock CalcErrorBlock2
#include "deps/wolfpld-etcpak/ProcessRGB.cpp"
#include "deps/wolfpld-etcpak/Tables.cpp"

#include "deps/unifont/unifont.hpp"


// portable endianness stuff. rlyeh, public domain {
#include <stdint.h>
#if __STDC_VERSION__ >= 199901L
#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})
#else
#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)
#endif
uint16_t swap16( uint16_t x ) {
    return ( x << 8 ) | ( x >> 8 );
}
uint32_t swap32( uint32_t x ) {
    return ( x << 24 ) | (( x << 8 ) & 0xff0000) | (( x >> 8 ) & 0xff00) | ( x >> 24 );
}
uint16_t tobe16( uint16_t x ) {
    return IS_BIG_ENDIAN ? x : swap16(x);
}
uint32_t tobe32( uint32_t x ) {
    return IS_BIG_ENDIAN ? x : swap32(x);
}
uint16_t tole16( uint16_t x ) {
    return IS_BIG_ENDIAN ? swap16(x) : x;
}
uint32_t tole32( uint32_t x ) {
    return IS_BIG_ENDIAN ? swap32(x) : x;
}
#define  hton16(x) ( IS_BIG_ENDIAN ? (x) : swap16(x) )
#define  hton32(x) ( IS_BIG_ENDIAN ? (x) : swap32(x) )
#define  ntoh16(x) ( IS_BIG_ENDIAN ? (x) : swap16(x) )
#define  ntoh32(x) ( IS_BIG_ENDIAN ? (x) : swap32(x) )
// }

extern "C"
int stbi_write_dds( char const *filename, int w, int h, int comp, const void *data ) {
   return save_image_as_DDS( filename, w, h, comp, (const unsigned char *const) data );
}

extern "C"
int WebPGetInfo(const uint8_t* data, size_t data_size, int* width, int* height);

extern "C"
uint8_t* WebPDecodeRGBA(const uint8_t* data, size_t data_size, int* width, int* height);

extern "C"
size_t WebPEncodeRGBA(const unsigned char* rgba, int width, int height, int stride, float quality_factor, unsigned char** output);

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// we're ready at this point...

namespace spot {

bool devel = false;

/*
#define STBI_ASSERT assert
#define stbi__errpuc(...) 0
#define stbi__malloc malloc
#define STBI_FREE free
#define stbi_uc unsigned char */
static stbi_uc stbi__compute_y(int r, int g, int b) {
   return (stbi_uc) (((r*77) + (g*150) +  (29*b)) >> 8);
}
static stbi_uc *stbi__convert_format(unsigned char *data, int bpp_src, int bpp_dst, unsigned int x, unsigned int y) {
   int i,j;
   stbi_uc *good;

   if (bpp_dst == bpp_src) return data;
   STBI_ASSERT(bpp_dst >= 1 && bpp_dst <= 4);

   good = (stbi_uc *) stbi__malloc(bpp_dst * x * y);
   if (good == NULL) {
      STBI_FREE(data);
      return stbi__errpuc("outofmem", "Out of memory");
   }

   for (j=0; j < (int) y; ++j) {
      stbi_uc *src  = data + j * x * bpp_src   ;
      stbi_uc *dest = good + j * x * bpp_dst;

      #define COMBO(a,b)  ((a)*8+(b))
      #define CASE(a,b)   case COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
      // convert source image with bpp_src components to one with bpp_dst components;
      // avoid switch per pixel, so use switch per scanline and massive macros
      switch (COMBO(bpp_src, bpp_dst)) {
         CASE(1,2) dest[0]=src[0], dest[1]=255; break;
         CASE(1,3) dest[0]=dest[1]=dest[2]=src[0]; break;
         CASE(1,4) dest[0]=dest[1]=dest[2]=src[0], dest[3]=255; break;
         CASE(2,1) dest[0]=src[0]; break;
         CASE(2,3) dest[0]=dest[1]=dest[2]=src[0]; break;
         CASE(2,4) dest[0]=dest[1]=dest[2]=src[0], dest[3]=src[1]; break;
         CASE(3,4) dest[0]=src[0],dest[1]=src[1],dest[2]=src[2],dest[3]=255; break;
         CASE(3,1) dest[0]=stbi__compute_y(src[0],src[1],src[2]); break;
         CASE(3,2) dest[0]=stbi__compute_y(src[0],src[1],src[2]), dest[1] = 255; break;
         CASE(4,1) dest[0]=stbi__compute_y(src[0],src[1],src[2]); break;
         CASE(4,2) dest[0]=stbi__compute_y(src[0],src[1],src[2]), dest[1] = src[3]; break;
         CASE(4,3) dest[0]=src[0],dest[1]=src[1],dest[2]=src[2]; break;
         default: STBI_ASSERT(0);
      }
      #undef CASE
   }

   STBI_FREE(data);
   return good;
}
/*
    if( (req_comp <= 4) && (req_comp >= 1) ) {
        //  user has some requirements, meet them
        if( req_comp != img_n ) {
            pkm_res_data = stbi__convert_format( pkm_res_data, img_n, req_comp, img_x, img_y );
        }
    }
*/

static stbi_uc *decode_etc1_stream(const void *stream, int len, int width, int height, unsigned int *zlen)
{
    int img_x = width, img_y = height, img_n = 3;

    unsigned int align = 0;
    unsigned int stride = ((width * 3) + align) & ~align;
    unsigned int size = stride * height;

    stbi_uc *unpacked = (stbi_uc *)malloc(size);
    if( unpacked ) {
        // pixelSize 2 is an GL_UNSIGNED_SHORT_5_6_5 image, 3 is a GL_BYTE RGB image.
        if( 0 == etc1_decode_image((const etc1_byte*)stream, (etc1_byte*)unpacked, width, height, 3, stride) ) {
            if( zlen ) *zlen = size;
            return (stbi_uc *)unpacked;            
        }
        free( unpacked );
    }

    return NULL;
}

static stbi_uc *decode_pvrtc_stream(const void *stream, int len, int width, int height, int px_type, unsigned int *zlen)
{
    int img_x = width, img_y = height, img_n = 3;
    int bitmode = 0;

    switch( px_type ) {
        default: return 0;
        case pvr3::table1::PVRTC_2BPP_RGB:
        case pvr3::table1::PVRTC_2BPP_RGBA: bitmode = 1; img_n = 4; break;
        case pvr3::table1::PVRTC_4BPP_RGB:
        case pvr3::table1::PVRTC_4BPP_RGBA: img_n = 4; break;
    }

    // Load only the first mip map level
    unsigned int bytes = img_x * img_y * 4;
    stbi_uc *unpacked = (stbi_uc *)malloc( bytes );
    if( unpacked ) {
        Decompress( (AMTC_BLOCK_STRUCT*)stream, bitmode, img_x, img_y, 1, (unsigned char*)unpacked );
        if( zlen ) *zlen = bytes;
        return unpacked;
    }

    return 0;
}

bool stream::is_valid() const {
    return w && h && error.empty(); // && data && len
}

bool stream::is_etc1() const {
    return is_valid() && 0 == w % 4 && 0 == h % 4 && fmt == pvr3::table1::ETC1;
}

bool stream::is_compressed() const {
    return is_valid() && w * h * ( d >= 1 ? d : 1 ) * comp / 4 < len;
}

template<typename T>
void endianness( T &self ) {
}
template<>
void endianness( pkm::header &hd ) {
    hd.width = ntoh16(hd.width);
    hd.height = ntoh16(hd.height);
    hd.width_src = ntoh16(hd.width_src);
    hd.height_src = ntoh16(hd.height_src);
 }

template<typename T>
bool preload( T &self, const void *ptr, size_t len ) {
    if( sizeof( typename T::header ) < len ) {
        memcpy( &self.hd, ptr, sizeof( typename T::header ) );
        endianness( self.hd );
        return self.is_currently_supported();
    }
    memset( &self, 0, sizeof( typename T::header ) );
    return false;
}

/*
bool load( std::string &data, const std::string &name, uint32_t reserved = 0 ) {
    std::stringstream ss;
    std::ifstream ifs( name.c_str(), std::ios::binary );
    if( ifs.good() && ss << ifs.rdbuf() ) {
        return data = std::string( reserved, 0 ) + ss.str(), true;
    }
    return data = std::string( reserved, 0 ), false;
}
*/

stream encode_as_pvrtc( const void *bgra, int w, int h, int bpp = 32, int quality = 0, unsigned reserved = 0 ) {
    stream out = {};

    /*
    auto is_power_of_two = []( unsigned x ) {
        return ((x & (x - 1)) == 0);
    };
    if( !is_power_of_two(w) ) return out;
    if( !is_power_of_two(h) ) return out;
    */
    bool is_4bpp = true; //quality >= 50;
    if( w % 4 ) return out;
    if( h % 4 ) return out;

    // @todo check is %4
    // Alloc mem
    int len = ( is_4bpp ? w * h / 2 : w * h / 4 ) + 4;
    uint8_t *dst = new uint8_t [ reserved + len ];
    if( !dst ) return out;
    uint8_t *pvrtc = &dst[ reserved ];

    // init
    out.w = w;
    out.h = h;
    out.d = 1;
    out.out = dst;
    out.len = len;

    /****/ if( bpp == 24 ) {
        Javelin::RgbBitmap bitmap( w, h );
        memcpy( bitmap.GetData(), bgra, w * h * 3 );
        //if( is_4bpp ) {
            out.fmt = pvr3::table1::PVRTC_4BPP_RGB;
            PvrTcEncoder::EncodeRgb4Bpp(pvrtc, bitmap);
        /*} else {
            out.fmt = pvr3::table1::PVRTC_2BPP_RGB;
            PvrTcEncoder::EncodeRgb2Bpp(pvrtc, bitmap);            
        }*/
    } else if( bpp == 32 ) {
        Javelin::RgbaBitmap bitmap( w, h );
        memcpy( bitmap.GetData(), bgra, w * h * 4 );
        //if( is_4bpp ) {
            out.fmt = pvr3::table1::PVRTC_4BPP_RGBA;
            PvrTcEncoder::EncodeRgba4Bpp(pvrtc, bitmap);
        /*} else {
            out.fmt = pvr3::table1::PVRTC_2BPP_RGBA;
            PvrTcEncoder::EncodeRgba2Bpp(pvrtc, bitmap);            
        }*/
    } 

    return out;    
}

stream encode_as_etc1_etcpak( const void *rgba, int w, int h, int bpp = 32, int quality = 0, unsigned reserved = 0 ) {
    stream out = {};

    bool alpha = false;
    bool mipmap = false;
    bool dither = quality >= 50;
    /*int*/ quality = 0;

    {
        auto bmp = std::make_shared<Bitmap>( (const uint32_t *)rgba, w * h * (bpp/8), w, h, std::numeric_limits<uint>::max() );

        auto bd = std::make_shared<BlockData>( bmp->Size(), false );
        auto block = std::make_shared<BlockBitmap>( bmp, Channels::RGB );
        if( dither ) block->Dither();
        bd->Process( block->Data(), bmp->Size().x * bmp->Size().y / 16, 0, quality, Channels::RGB );
        bd->Finish();

        BlockDataPtr bda;        
        if( alpha && bpp == 32 ) {
            bda = std::make_shared<BlockData>( bmp->Size(), false );
            auto blocka = std::make_shared<BlockBitmap>( bmp, Channels::Alpha );
            // do not dither alpha
            bda->Process( blocka->Data(), bmp->Size().x * bmp->Size().y / 16, 0, quality, Channels::RGB );
            bda->Finish();
        }

        if( bd ) {
            out.w = bd->m_size.x;
            out.h = bd->m_size.y;
            out.d = 1;
            out.fmt = pvr3::table1::ETC1;
            out.len = out.w * out.h / 2;
            out.out = new unsigned char [ out.len ];
            memcpy( out.out, (const char *)&bd->m_data[ bd->m_dataOffset ], out.len );
        }        
        if( bda ) {
            // to be done
        }

        bd.reset();
        bda.reset();        
    }

    return out;
}
stream encode_as_etc1( const void *rgba, int w, int h, int bpp = 32, int quality = 0, unsigned reserved = 0 ) {

    if( quality < 50 ) return encode_as_etc1_etcpak( rgba, w, h, bpp, quality * 2, reserved );
    else quality = (quality - 50) * 2;

    stream out = {};

    /* Check for power of 2 {
    auto is_power_of_two = []( unsigned x ) {
        return ((x & (x - 1)) == 0);
    };
    if( !is_power_of_two(w) ) return out;
    if( !is_power_of_two(h) ) return out;
    // } */

    // Alloc mem
    unsigned pitch = w * (bpp / 8); 
    unsigned blockw = w/4;
    unsigned blockh = h/4;
    size_t len = blockw * blockh * 8;
    uint8_t *dst = new uint8_t [ reserved + len ];
    if( !dst ) return out;
    uint8_t *data = &dst[ reserved ];

    // init
    rg_etc1::pack_etc1_block_init();
    rg_etc1::etc1_pack_params params;

    int step = 100/6;
    /**/ if( quality >= 100 - step * 1 ) params.m_quality = rg_etc1::cHighQuality,   params.m_dithering = true;
    else if( quality >= 100 - step * 2 ) params.m_quality = rg_etc1::cHighQuality,   params.m_dithering = false;
    else if( quality >= 100 - step * 3 ) params.m_quality = rg_etc1::cMediumQuality, params.m_dithering = true;
    else if( quality >= 100 - step * 4 ) params.m_quality = rg_etc1::cMediumQuality, params.m_dithering = false;
    else if( quality >= 100 - step * 5 ) params.m_quality = rg_etc1::cLowQuality,    params.m_dithering = true;
    else                                 params.m_quality = rg_etc1::cLowQuality,    params.m_dithering = false;

    // RGBA to ETC1
#pragma omp parallel for 
    for( unsigned y = 0; y < blockh; y++ ) {
        for( unsigned x = 0; x < blockw; x++ ) {
            uint32_t block[16];
            for( unsigned iy = 0; iy < 4; iy++ ) {
                memcpy(block + 4 * iy, (uint8_t*)rgba + ((y * 4 + iy) * (pitch/4) + x * 4) * 4, 16);
            }
            rg_etc1::pack_etc1_block((data) + (blockw * y + x) * 8, block, params);
        }
    }

    out.w = blockw*4;
    out.h = blockh*4;
    out.d = 1;
    out.fmt = pvr3::table1::ETC1;
    out.out = dst;
    out.len = len;
    return out;
}


bool save_pkm_etc1( std::string &out, const stream &sm, unsigned reserved ) {
    if( sm.is_valid() && sm.is_etc1() ) {
        pkm p;
        p.hd.id = hton32('PKM ');
        p.hd.version = hton16('10');
        p.hd.type = hton16(0);
        p.hd.width = hton16(sm.w - sm.w % 4);
        p.hd.height = hton16(sm.h - sm.h % 4);
        p.hd.width_src = hton16(sm.w);
        p.hd.height_src = hton16(sm.h);
        if( devel ) p.debug(std::cout);

        out.resize( sizeof(p.hd) + reserved );
        memcpy( &out[0], &p.hd, sizeof(p.hd) );
        return true;
    }
    return out.clear(), false;
}

bool save_pvr( std::string &out, const stream &sm, unsigned reserved = 0 ) {
    if( sm.is_valid() /* && sm.is_etc1() */ ) {
        pvr3 pvr;
        pvr.hd.version = tole32(0x03525650);     // 0x03525650, if endianess does not match ; 0x50565203, if endianess does match
        pvr.hd.flags = tole32(0);                // 0x02, colour values within the texture have been pre-multiplied by the alpha values
        pvr.hd.pixel_format_1 = tole32(sm.fmt);  // see table1 above
        pvr.hd.pixel_format_2 = tole32(0);       // 0
        pvr.hd.color_space = tole32(0);          // 0 linear rgb, 1 standard rgb 
        pvr.hd.channel_type = tole32(0);         // see table2 above
        pvr.hd.height = tole32(sm.h);            // 1d texture
        pvr.hd.width = tole32(sm.w);             // 2d texture; >= 1
        pvr.hd.depth = tole32(1);                // 3d texture; >= 1
        pvr.hd.num_surfaces = tole32(1);         // num surfaces in texture array; >= 1
        pvr.hd.num_faces = tole32(1);            // num faces in cubemap; >= 1
        pvr.hd.num_mipmaps = tole32(1);          // num levels of mipmaps; >= 1
        pvr.hd.metadata_size = tole32(0);        // length of following header
        if( devel ) pvr.debug(std::cout);

        out.resize( sizeof(pvr3::header) + reserved );
        memcpy( &out[0], &pvr.hd, sizeof(pvr.hd) );
        return true;
    }
    return out.clear(), false;
}

bool save_ktx( std::string &out, const stream &sm, unsigned reserved = 0 ) {
    if( sm.is_valid() ) {
        ktx k;
        auto &hd = k.hd;
        hd.identifier0 = tole32(0x58544bab); 
        hd.identifier1 = tole32(0xbb313120); 
        hd.identifier2 = tole32(0xa1a0a0d);  
        hd.endianness = tole32(0x4030201);
        hd.glType = tole32(0x0); // table 8.2 of opengl 4.4 spec; UNSIGNED_BYTE, UNSIGNED_SHORT_5_6_5, etc.)
        hd.glTypeSize = tole32(0x1); // 1 for compressed data
        hd.glFormat = tole32(0x0); // table 8.3 of opengl 4.4 spec; RGB, RGBA, BGRA...
        // glInternalFormat:     table 8.14 of opengl 4.4 spec; ETC1_RGB8_OES (0x8D64)
        // glBaseInternalFormat: table 8.11 of opengl 4.4 spec; GL_RGB (0x1907), RGBA, ALPHA...
        /**/ if( sm.fmt == pvr3::table1::ETC1 )            { hd.glInternalFormat = tole32(0x8d64), hd.glBaseInternalFormat = tole32(0x1907); }
        else if( sm.fmt == pvr3::table1::PVRTC_2BPP_RGB )  { hd.glInternalFormat = tole32(0x8c01), hd.glBaseInternalFormat = tole32(0x1907); }
        else if( sm.fmt == pvr3::table1::PVRTC_2BPP_RGBA ) { hd.glInternalFormat = tole32(0x8c03), hd.glBaseInternalFormat = tole32(0x1908); }
        else if( sm.fmt == pvr3::table1::PVRTC_4BPP_RGB )  { hd.glInternalFormat = tole32(0x8c00), hd.glBaseInternalFormat = tole32(0x1907); }
        else if( sm.fmt == pvr3::table1::PVRTC_4BPP_RGBA ) { hd.glInternalFormat = tole32(0x8c02), hd.glBaseInternalFormat = tole32(0x1908); }
        else return false;
        hd.pixelWidth = tole32(sm.w);
        hd.pixelHeight = tole32(sm.h);
        hd.pixelDepth = tole32(0);
        hd.numberOfArrayElements = tole32(0);
        hd.numberOfFaces = tole32(1);
        hd.numberOfMipmapLevels = tole32(1);
        hd.bytesOfKeyValueData = tole32(0);
        if( devel ) k.debug(std::cout);

        out.resize( sizeof(ktx::header) + reserved );
        memcpy( &out[0], &k.hd, sizeof(ktx::header) );
        return true;
    }
    return out.clear(), false;
}

std::string save_pvr( const stream &sm, unsigned reserved = 0 ) {
    std::string out;
    return save_pvr( out, sm, reserved ) ? out : std::string();
}
std::string save_ktx( const stream &sm, unsigned reserved = 0 ) {
    std::string out;
    return save_ktx( out, sm, reserved ) ? out : std::string();
}
std::string save_pkm_etc1( const stream &sm, unsigned reserved = 0 ) {
    std::string out;
    return save_pkm_etc1( out, sm, reserved ) ? out : std::string();
}

}

/*
@todo:
.pvr3
    for each MIP-Map Level in MIP-Map Count {
        for each Surface in Num. Surfaces {
            for each Face in Num. Faces {
                for each Slice in Depth {
                    for each Row in Height {
                        for each Pixel in Width {
                            Byte data[Size_Based_On_PixelFormat]
    }}}}}

.ktx
    [header]
    
    for each keyValuePair that fits in bytesOfKeyValueData
        uint32_t   keyAndValueByteSize
        uint8_t    keyAndValue[keyAndValueByteSize]
        uint8_t    valuePadding[3 - ((keyAndValueByteSize + 3) % 4)]
    end

    for each mipmap_level in numberOfMipmapLevels* {
        uint32_t imageSize; 
        for each array_element in numberOfArrayElements* {
           for each face in numberOfFaces {
               for each z_slice in pixelDepth* {
                   for each row or row_of_blocks in pixelHeight* {
                       for each pixel or block_of_pixels in pixelWidth {
                           uint8_t data[format-specific-number-of-bytes]**
                       }
                   }
               }
               uint8_t cubePadding[0-3]
           }
        }
        uint8_t mipPadding[3 - ((imageSize + 3) % 4)]
    } 

    * Replace with 1 if this field is 0.
    ** Uncompressed texture data matches a GL_UNPACK_ALIGNMENT of 4.
*/


namespace spot
{
    namespace internals {

        int32_t swapbe( int32_t v ) {
            // swap bytes on big endian only; little remains unchanged
            union autodetect {
                int word;
                char byte[ sizeof(int) ];
                autodetect() : word(1) 
                {}
            } _;
            bool is_big = _.byte[0] == 0;
            if( is_big ) {
                unsigned char *p = (unsigned char *)&v;
                std::swap( p[0], p[3] );
                std::swap( p[1], p[2] );
            }
            return v;
        }

        std::string encode_png( unsigned w, unsigned h, const void *data, unsigned stride ) {
            if( w && h && data && stride ) {
                auto mode = LCT_RGBA;
                /**/ if( stride == 3 ) mode = LCT_RGB;
                else if( stride == 2 ) mode = LCT_GREY_ALPHA;
                else if( stride == 1 ) mode = LCT_GREY;
                unsigned char* png;
                size_t pngsize;
                unsigned bpp = 8;
                unsigned error = lodepng_encode_memory_std( &png, &pngsize, (const unsigned char *)data, w, h, mode, bpp );
                if( !error && pngsize ) {
                    std::string buf;
                    buf.resize(pngsize);
                    memcpy(&buf[0],png,pngsize);
                    free(png);
                    return buf;
                }
            }
            return std::string();
        }

        std::string encode_jpg( unsigned w, unsigned h, const void *data, unsigned quality ) {
            if( w && h && data && quality ) {
                std::string buf( 1024 + w * h * 3, '\0' );
                jpge::params p;
                p.m_quality = (int)quality;
                p.m_two_pass_flag = true; // slower but slighty smaller
                int buf_size = (int)buf.size();
                if( jpge::compress_image_to_jpeg_file_in_memory(&buf[0], buf_size, w, h, 4, (const jpge::uint8 *)data, p) ) {
                    if( buf_size > 0 ) {
                        buf.resize((unsigned)(buf_size));
                        return buf;
                    }
                }
            }
            return std::string();
        }

        std::string encode_wbp( unsigned w, unsigned h, const void *data, unsigned quality ) {
            if( w && h && data && quality ) {
                unsigned char *buf = 0;
                size_t buflen = WebPEncodeRGBA( (const unsigned char *)data, w, h, w * 4, quality, &buf );
                if( buflen && buf ) {
                    std::string s;
                    s.resize(buflen);
                    memcpy(&s[0],buf,buflen);
                    free(buf);
                    return s;
                }
            }
            return std::string();
        }

        std::string encode_pug( unsigned w, unsigned h, const void *data, unsigned quality ) {
            if( w && h && data && quality ) {
                // encode color as jpg
                std::string jpg = encode_jpg( w, h, data, quality );
                // encode alpha as png (gray, lum8)
                std::vector<unsigned char> alpha( w * h );
                unsigned char *ptr = ((unsigned char *)data) + 3;
                for( unsigned x = 0; x < w * h; ++x, ptr += 4 ) alpha[ x ] = *ptr;
                std::string png = encode_png( w, h, alpha.data(), 1 );
                // glue and footer
                int32_t size24 = internals::swapbe( int32_t(jpg.size()) );
                int32_t size08 = internals::swapbe( int32_t(png.size()) );
                if( size24 && size08 ) {
                    std::stringstream ss;
                    ss.write( &jpg[0], size24 );
                    ss.write( &png[0], size08 );
                    ss.write( (const char *)&size24, 4 );
                    ss.write( (const char *)&size08, 4 );
                    ss.write( "pug1", 4 );
                    if( ss.good() ) {
                        return ss.str();
                    }
                }
            }
            return std::string();
        }        

        std::string encode_ktx( unsigned w, unsigned h, const void *data, unsigned quality ) {
            if( w && h && data && quality ) {
                stream sm = encode_as_etc1( data, w, h, 32, quality );
                std::stringstream ss;
                ss << save_ktx(sm);
                uint32_t len32( sm.len );
                ss.write( (const char *)&len32, 4 );
                ss.write( (const char *)sm.in, sm.len );
                delete [] ((uint8_t *)sm.out);
                return ss.str();
            }
            return std::string();
        }        

        std::string encode_pvr( unsigned w, unsigned h, const void *data, unsigned quality ) {
            if( w && h && data && quality ) {
                //stream sm = encode_as_etc1( data, w, h, 32, quality );
                stream sm = encode_as_pvrtc( data, w, h, 32, quality );
                std::stringstream ss;
                ss << save_pvr(sm);
                ss.write( (const char *)sm.in, sm.len );
                delete [] ((uint8_t *)sm.out);
                return ss.str();
            }
            return std::string();
        }        

        std::string encode_pkm( unsigned w, unsigned h, const void *data, unsigned quality ) {
            if( w && h && data && quality ) {
                stream sm = encode_as_etc1( data, w, h, 32, quality );
                std::stringstream ss;
                ss << save_pkm_etc1(sm);
                ss.write( (const char *)sm.in, sm.len );
                delete [] ((uint8_t *)sm.out);
                return ss.str();
            }
            return std::string();
        }        

        /*
            transcoding
            ===========

            std::cout << "fast: " << bench([&]{ encode_as_etc1( img.rgba32().data(), img.w, img.h, 32, SPOT_ETC1_LOW );} ) << std::endl;
            std::cout << "medium: " << bench([&]{ encode_as_etc1( img.rgba32().data(), img.w, img.h, 32, SPOT_ETC1_MEDIUM );} ) << std::endl;
            //std::cout << "high: " << bench([&]{ encode_as_etc1( img.rgba32().data(), img.w, img.h, 32, SPOT_ETC1_HIGH );} ) << std::endl;

            if( k.is_currently_supported() ) {
                auto header = save_pkm_etc1( stream { k.hd.pixelWidth, k.hd.pixelHeight, pvr3::table1::ETC1 } );
                buffer = buffer + sizeof(k.hd) + 4 - header.size();
                uint32_t len = tole32( uint32_t( header.size() ) );
                memcpy( &data[buffer - 4], &len, 4 );
                memcpy( &data[buffer], &header[0], header.size() );

                preload(pk, &data[buffer], data.size() - buffer );
                pk.debug( std::cout );
            }
            if( p.is_currently_supported() ) {
                auto header = save_pkm_etc1( stream { p.hd.width, p.hd.height, pvr3::table1::ETC1 } );
                buffer = buffer + sizeof(p.hd) - header.size();
                memcpy( &data[buffer], &header[0], header.size() );

                preload(pk, &data[buffer], data.size() - buffer );
                pk.debug( std::cout );
            }
            if( pk.is_currently_supported() ) {
                const stbi_uc *rgba = decode_etc1_stream( &data[buffer+sizeof(pkm::header)], data.size(), pk.hd.width, pk.hd.height, 3, &zlen);
                spot::texture tx( rgba, zlen, pk.hd.width, pk.hd.height );
                spot::image img = tx;
                display( img );
            }
        }        
        */

        bool writefile( const std::string &filename, const std::string &data ) {
            if( !data.empty() ) {
                std::ofstream ofs( filename.c_str(), std::ios::binary );
                ofs.write( &data[0], data.size() );
                return ofs.good();                
            }
            return false;
        }
    }

    std::vector<std::string> list_supported_inputs() {
        const char *str[] = { "bmp", "dds", "gif", "hdr", "jpg", "pic", "pkm", "png", "psd", "pvr", "svg", "tga", "webp", "pnm", "pug", "crn", 0 };
        std::vector<std::string> list;
        for( int i = 0; str[i]; ++i ) {
            list.push_back( str[i] );
        }
        return list;
    }
    std::vector<std::string> list_supported_outputs() {
        const char *str[] = { "bmp", "dds", "jpg", "png", "tga", "webp", "pug", "ktx", "pvr", "pkm", 0 };
        std::vector<std::string> list;
        for( int i = 0; str[i]; ++i ) {
            list.push_back( str[i] );
        }
        return list;
    }

    enum { NO_DELETER, STBI_DELETER, FREE_DELETER, NEW_DELETER, NEW_ARRAY_DELETER };
    enum { UNK, IS_STBI, IS_CRN, IS_WEBP, IS_SVG, IS_KTX, IS_PVR3, IS_PKM };

    bool info( stream &sm, const void *src, size_t len ) {
        stream blank = {};
        sm = blank;
        sm.in = src;
        sm.len = len;

        const char *src8 = (const char *)src;

        // crn?
        bool is_crn = len > 2 && (src8[0] == 'H' && src8[1] == 'x');
        if( is_crn ) {
            sm.w = (( src8[0xC] << 8) | src8[0xD] );
            sm.h = (( src8[0xE] << 8) | src8[0xF] );
            sm.comp = 4; 
            sm.hint = IS_CRN;
            sm.deleter = STBI_DELETER;
            return true;
        }

        // pkm? (etc1)
        {
            pkm f;
            preload( f, src, len );
            if( devel ) f.debug( std::cout );
            if( f.is_currently_supported() ) {
                sm.w = int(f.hd.width);
                sm.h = int(f.hd.height);
                sm.fmt = f.get_spot_fmt();
                sm.comp = 3;
                sm.hint = IS_PKM;
                return true;
            }
        }

        // ktx? (etc1)
        {
            ktx f;
            preload( f, src, len );
            if( devel ) f.debug( std::cout );
            if( f.is_currently_supported() ) {
                sm.w = int(f.hd.pixelWidth);
                sm.h = int(f.hd.pixelHeight);
                sm.fmt = f.get_spot_fmt();
                sm.comp = 3;
                sm.hint = IS_KTX;
                return true;
            }
        }

        // pvr3? (etc1)
        {
            pvr3 f;
            preload( f, src, len );
            if( devel ) f.debug( std::cout );
            if( f.is_currently_supported() ) {
                sm.w = int(f.hd.width);
                sm.h = int(f.hd.height);
                sm.fmt = f.get_spot_fmt();
                sm.comp = 3;
                sm.hint = IS_PVR3;
                return true;
            }
        }

        // pvr2 ?
        int iscompressed = false;
        int ok = stbi__pvr_info_from_memory( (stbi_uc const *)src, int(len), &sm.w, &sm.h, &sm.comp, &iscompressed );
        if( ok ) {
            return sm.hint = IS_STBI, sm.deleter = STBI_DELETER, true;
        }

        // dds ?
        ok = stbi__dds_info_from_memory( (stbi_uc const *)src, int(len), &sm.w, &sm.h, &sm.comp, &iscompressed );
        if( ok ) {
            return sm.hint = IS_STBI, sm.deleter = STBI_DELETER, true;
        }

        // try most
        ok = stbi_info_from_memory( (stbi_uc const *)src, int(len), &sm.w, &sm.h, &sm.comp );
        if( ok ) {
            // pug?
            const char *magic = (const char *)src + len - 4;
            if( magic[0] == 'p' && magic[1] == 'u' && magic[2] == 'g' && magic[3] == '1' ) sm.comp = 4;

            return sm.hint = IS_STBI, sm.deleter = STBI_DELETER, true;
        }

        // webp ?
        ok = 0 != WebPGetInfo( (const uint8_t *)src, len, &sm.w, &sm.h );
        if( ok ) return sm.hint = IS_WEBP, sm.deleter = FREE_DELETER, sm.comp = 4, true;

        // svg ?
        if( src8[0] == '<' || src8[0] == ' ' || src8[0] == '\t' ) {
            // Load SVG, parse and rasterize
            char *str = new char[ len + 1 ];
            memcpy( str, src, len );
            str[ len ] = '\0';

            NSVGimage *image = nsvgParse( str, "px" /*units*/, 96.f /* dpi */ );
            if( image ) {
                double scale = 1.0;
                sm.hint = IS_SVG;
                sm.w = int(image->width * scale);
                sm.h = int(image->height * scale);
                sm.comp = 4;

                nsvgDelete(image);
                delete [] str;
                return sm.deleter = FREE_DELETER, true;
            } else {
                delete [] str;
            }
        }

        return false;
    }

    bool decode( stream &dst, const stream &src )
    {
        if( !src.in || !dst.out ) {
            dst.error = "Error: invalid pointer provided";
            return false;
        }
            
        if( !src.len || !dst.len ) {
            dst.error = "Error: invalid size provided";
            return false;
        }

        // decode
        int imageWidth = 0, imageHeight = 0, imageComp = 0, imageHint = 0;
        int deleter = NO_DELETER;

        if( !src.hint ) {
            if( !info( dst, src.in, src.len ) ) {
                return false;
            }
        } 

        imageWidth = src.w;
        imageHeight = src.h;
        imageComp = src.comp;
        imageHint = src.hint;

        stbi_uc *imageuc = 0;

        switch( imageHint ) {
        default:
        break; 
        case IS_CRN :{
            std::string dds;
            if( crn2dds( dds, src.in, src.len ) ) {
                imageuc = stbi_load_from_memory( (const unsigned char *)&dds[0], dds.size(), &imageWidth, &imageHeight, &imageComp, 4 );
                deleter = STBI_DELETER;
                imageComp = 4;
            }
        }
        break; 
        case IS_PKM :{
            unsigned int zlen;
            const stbi_uc *data = (const stbi_uc *)src.in;
            size_t offset = sizeof(pkm::header);
            imageuc = decode_etc1_stream( &data[offset], src.len - offset, imageWidth, imageHeight, &zlen);
            deleter = FREE_DELETER;
            imageComp = 3;
        }
        break; 
        case IS_KTX :{
            unsigned int zlen;
            const stbi_uc *data = (const stbi_uc *)src.in;
            size_t offset = sizeof(ktx::header) + ((ktx::header *)src.in)->bytesOfKeyValueData + 4;
            /****/ if( src.fmt == pvr3::table1::ETC1 ) {
                imageuc = decode_etc1_stream( &data[offset], src.len - offset, imageWidth, imageHeight, &zlen);
                imageComp = 3;
            } else if( src.fmt <= pvr3::table1::PVRTC_4BPP_RGBA ) {
                imageuc = decode_pvrtc_stream( &data[offset], src.len - offset, imageWidth, imageHeight, src.fmt, &zlen);
                imageComp = (src.fmt == pvr3::table1::PVRTC_2BPP_RGBA || pvr3::table1::PVRTC_4BPP_RGBA) ? 4 : 3;
            }
            deleter = FREE_DELETER;
        }
        break; 
        case IS_PVR3: {
            unsigned int zlen;
            const stbi_uc *data = (const stbi_uc *)src.in;
            size_t offset = sizeof(pvr3::header) + ((pvr3::header *)src.in)->metadata_size;
            /****/ if( src.fmt == pvr3::table1::ETC1 ) {
                imageuc = decode_etc1_stream( &data[offset], src.len - offset, imageWidth, imageHeight, &zlen);
                imageComp = 3;                
            } else if( src.fmt <= pvr3::table1::PVRTC_4BPP_RGBA ) {
                imageuc = decode_pvrtc_stream( &data[offset], src.len - offset, imageWidth, imageHeight, src.fmt, &zlen);
                imageComp = (src.fmt == pvr3::table1::PVRTC_2BPP_RGBA || pvr3::table1::PVRTC_4BPP_RGBA) ? 4 : 3;                
            }
            deleter = FREE_DELETER;
        }
        break; 
        case IS_STBI: {
            imageuc = stbi_load_from_memory( (const stbi_uc *)src.in, src.len, &imageWidth, &imageHeight, &imageComp, src.comp < 3 ? 3 : src.comp );
            imageComp = src.comp < 3 ? 3 : src.comp;
            deleter = STBI_DELETER;
            // if it is a .pug file, then decode alpha 
            const char *magic = (const char *)src.in + src.len - 4;
            if( magic[0] == 'p' && magic[1] == 'u' && magic[2] == 'g' && magic[3] == '1' ) {
                imageComp = 4;
                const int32_t color_size = internals::swapbe( *(const int32_t *)((const char *)src.in + src.len - 12) );
                const int32_t alpha_size = internals::swapbe( *(const int32_t *)((const char *)src.in + src.len - 8) );
                int w2 = 0, h2 = 0, bpp2 = 0;
                stbi_uc *alpha = stbi_load_from_memory( (const unsigned char *)src.in + color_size, alpha_size, &w2, &h2, &bpp2, 1 );
                if( alpha ) {
                    stbi_uc *data8 = &alpha[ 0 ], *end8 = &alpha[ w2*h2 ], *dst8 = &imageuc[ 3 ];
                    while( data8 < end8 ) {
                        *dst8 = *data8++;
                        dst8 += 4;
                    }
                    stbi_image_free( alpha );
                }
            }  
        }
        break; 
        case IS_WEBP: {
            imageuc = (stbi_uc *) WebPDecodeRGBA( (const uint8_t *)src.in, src.len, &imageWidth, &imageHeight );
            deleter = FREE_DELETER;
            imageComp = 4;
        }
        break; 
        case IS_SVG :{
            // Load SVG, parse and rasterize
            char *str = new char[ src.len + 1 ];
            memcpy( str, src.in, src.len );
            str[ src.len ] = '\0';

            NSVGimage *image = nsvgParse( str, "px" /*units*/, 96.f /* dpi */ );
            if( image ) {
                // Create rasterizer (can be used to render multiple images).
                static struct install {
                    NSVGrasterizer *rasterizer;
                     install() { rasterizer = nsvgCreateRasterizer(); }
                    ~install() { if( rasterizer ) nsvgDeleteRasterizer( rasterizer ); rasterizer = 0; }
                } local;
                // Allocate memory for image
                int w = image->width;
                int h = image->height;

                imageuc = (stbi_uc *)malloc(w*h*4); //imageWidth*imageHeight*4
                deleter = FREE_DELETER;
                if( imageuc ) {
                    // Rasterizes SVG image, returns RGBA image (non-premultiplied alpha)
                    //   r - pointer to rasterizer context
                    //   image - pointer to image to rasterize
                    //   tx,ty - image offset (applied after scaling)
                    //   scale - image scale
                    //   dst - pointer to destination image data, 4 bytes per pixel (RGBA)
                    //   w - width of the image to render
                    //   h - height of the image to render
                    //   stride - number of bytes per scaleline in the destination buffer
                    double scale = 1.0;
                    nsvgRasterize( local.rasterizer, image, 0,0,scale, imageuc, w, h, w*scale*4 );
                }
                nsvgDelete(image);
            }

            delete [] str;
        }
        };

        if( !imageuc )
        {
            // assert( false );
            // return yellow/black texture instead?
            dst.error = "Error! unable to decode image";
            return false;
        }

        memcpy( dst.out, imageuc, imageWidth * imageHeight * imageComp ); //dst.len );

        /****/ if( deleter == STBI_DELETER ) {
            stbi_image_free( imageuc );
        } else if( deleter == FREE_DELETER ) {
            free(imageuc);
        } else if( deleter == NEW_DELETER ) {
            delete imageuc;
        } else if( deleter == NEW_ARRAY_DELETER ) {
            delete [] (imageuc);
        }
        imageuc = 0;

        dst.w = imageWidth;
        dst.h = imageHeight;
        dst.d = 1;
        dst.comp = imageComp;
        dst.fmt = RGBA_8888;

        return true;
    }

    std::vector<unsigned char> decode8( const void *src, size_t size, size_t *w, size_t *h, size_t *comp, std::string *error ) {
        stream in = {};
        stream out = {};
        if( info( in, src, size ) ) {
            if( in.is_valid() ) {
                std::vector<unsigned char> dst( in.w * in.h * 4 /*srccomp*/ );
                out.out = &dst[0];
                out.len = int(dst.size());
                out.fmt = RGBA_8888;
                if( decode( out, in ) ) {
                    if( w     ) *w = size_t(out.w);
                    if( h     ) *h = size_t(out.h);
                    if( comp  ) *comp = size_t(out.comp);
                    if( error ) *error = out.error;
                    return dst;
                } 
            }            
        }
        return std::vector<unsigned char>();
    }

    std::vector<unsigned char> decode8( const std::string &filename, size_t *w, size_t *h, size_t *comp, std::string *error ) {
        std::ifstream ifs( filename.c_str(), std::ios::binary );
        std::vector<char> buffer( (std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return decode8( (const unsigned char *)buffer.data(), buffer.size(), w, h, comp, error );
    }

    std::vector<unsigned int> decode32( const void *ptr, size_t size, size_t *w, size_t *h, size_t *comp, std::string *error ) {
        std::vector<unsigned char> decoded = decode8( ptr, size, w, h, comp, error ); 
        std::vector<unsigned int> out;
        if( !decoded.empty() ) {
            out.reserve( decoded.size() / 4 );
            if( comp && *comp == 3 )
            for( unsigned char *data8 = &decoded[0], *end8 = data8 + decoded.size(); data8 != end8; ) {
                pixel p;
                p.r = *data8++;
                p.g = *data8++;
                p.b = *data8++;
                p.a = 255;
                out.push_back( p.rgba );
            }
            if( comp && *comp == 4 )
            for( unsigned char *data8 = &decoded[0], *end8 = data8 + decoded.size(); data8 != end8; ) {
                pixel p;
                p.r = *data8++;
                p.g = *data8++;
                p.b = *data8++;
                p.a = *data8++;
                out.push_back( p.rgba );
            }
            return out;
        }
        return out;
    }

    std::vector<unsigned int> decode32( const std::string &filename, size_t *w, size_t *h, size_t *comp, std::string *error ) {
        std::ifstream ifs( filename.c_str(), std::ios::binary );
        std::vector<char> buffer( (std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return decode32( (const unsigned char *)buffer.data(), buffer.size(), w, h, comp, error );
    }

    void hsl2rgb( const float *hsl, float *rgb )
    {
        // Given H,S,L in range of 0-1
        // Returns a Color (RGB struct) in range of 0-1

        const float &h = hsl[0];
        const float &s = hsl[1];
        const float &l = hsl[2];

        float &r = rgb[0];
        float &g = rgb[1];
        float &b = rgb[2];

        float v;

        r = l;   // default to gray
        g = l;
        b = l;
        v = (l <= 0.5f) ? (l * (1.f + s)) : (l + s - l * s);

        if (v > 0)
        {
              float m = l + l - v;
              float sv = (v - m ) / v;
              float h6 = h * 6.f;
              int sextant = (int)h6;
              float fract = h6 - sextant;
              float vsf = v * sv * fract;
              float mid1 = m + vsf;
              float mid2 = v - vsf;

              switch (sextant)
              {
                    default:
                    case 0:
                          r = v;
                          g = mid1;
                          b = m;
                          break;
                    case 1:
                          r = mid2;
                          g = v;
                          b = m;
                          break;
                    case 2:
                          r = m;
                          g = v;
                          b = mid1;
                          break;
                    case 3:
                          r = m;
                          g = mid2;
                          b = v;
                          break;
                    case 4:
                          r = mid1;
                          g = m;
                          b = v;
                          break;
                    case 5:
                          r = v;
                          g = m;
                          b = mid2;
                          break;
              }
        }
    }

    void rgb2hsl( const float *rgb, float *hsl )
    {
        // Given a Color (RGB Struct) in range of 0-1
        // Return H,S,L in range of 0-1

        const float &r = rgb[0];
        const float &g = rgb[1];
        const float &b = rgb[2];
        float v;
        float m;
        float vm;
        float r2, g2, b2;

        float &h = hsl[0];
        float &s = hsl[1];
        float &l = hsl[2];

        h = 0; // default to black
        s = 0;
        l = 0;
        v = ( r > g ? r : g );
        v = ( v > b ? v : b );
        m = ( r < g ? r : g );
        m = ( m < b ? m : b );
        l = (m + v) / 2.f;
        if (l <= 0.f)
        {
              return;
        }
        vm = v - m;
        s = vm;
        if (s > 0.f)
        {
              s /= (l <= 0.5f) ? (v + m ) : (2.f - v - m) ;
        }
        else
        {
              return;
        }
        r2 = (v - r) / vm;
        g2 = (v - g) / vm;
        b2 = (v - b) / vm;
        if (r == v)
        {
              h = (g == m ? 5.f + b2 : 1.f - g2);
        }
        else if (g == v)
        {
              h = (b == m ? 1.f + r2 : 3.f - b2);
        }
        else
        {
              h = (r == m ? 3.f + g2 : 5.f - r2);
        }
        h /= 6.f;
    }

    pixel::operator color() const {
        pixel c = this->clamp().to_hsla();
        return spot::color(c.r / 255.f, c.g / 255.f, c.b / 255.f, c.a / 255.f);
    }

    static pixel rgba8888( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) {
        return pixel(r,g,b,a);
    }
    static color rgba8888_alt( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) {
        return pixel(r,g,b,a);
    }

    void texture::print( int x, int y, const char *utf8 ) {
        unifont<pixel> f( &(this->operator[](0)), this->w, &rgba8888 );
        f.render_string( x, y, 0|2, utf8 );
    }
    void image::print( int x, int y, const char *utf8 ) {
        unifont<color> f( &(this->operator[](0)), this->w, &rgba8888_alt );
        f.render_string( x, y, 0|2, utf8 );
    }
}
