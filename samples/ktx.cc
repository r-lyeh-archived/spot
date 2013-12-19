// r-lyeh, public domain.

#include <iostream>
#include "spot.hpp"
#include "cimg.hpp"

#include "../redist/deps/rg_etc1/rg_etc1.h"
#include "../redist/deps/rg_etc1/rg_etc1.cpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdlib.h>
#include <string>

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

// timing {
#include <thread>
#include <chrono>
#ifndef USE_OMP
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
void sleep( double secs ) {
    std::chrono::microseconds duration( (int)(secs * 1000000) );
    std::this_thread::sleep_for( duration );    
}
#include <string>
template<typename FN>
std::string bench( const FN &fn ) {
    auto took = -now(); fn(); took += now();
    return std::to_string(took) + "s.";
}
//}

enum {
    SPOT_ETC1_LOW = 1,
    SPOT_ETC1_MEDIUM = 2,
    SPOT_ETC1_HIGH = 4,

    SPOT_ETC1_DITHER = 8,
};

struct pkm {
    struct header {
        uint32_t id;            // "PKM "
        uint16_t version;       // "10"
        uint16_t type;          // 0: ETC1_RGB_NO_MIPMAPS
        uint16_t width;         // after %4; big endian
        uint16_t height;        // after %4; big endian
        uint16_t width_src;     // original width; big endian
        uint16_t height_src;    // original height; big endian
    } hd;

    bool is_pkm() const {
        const char xx[] = {'P','K','M',' ','1','0'};
        return 0 == memcmp(this, xx, sizeof(xx));
    }

    bool is_currently_supported() const {
        return is_pkm() && hd.type == 0;
    }

    std::ostream &debug( std::ostream &ss ) const {
        if( !is_pkm() ) {
            ss << "not a .pkm header" << std::endl;
        }
        else if( !is_currently_supported() ) {
            ss << "unsupported .pkm file" << std::endl;
        }
        else {
            ss << "pkm.id: "; ss.write( (const char *)&hd.id, sizeof(hd.id) ); ss << std::endl;
            ss << "pkm.version: "; ss.write( (const char *)&hd.version, sizeof(hd.version) ); ss << std::endl;
            ss << std::hex;
            ss << "pkm.type: 0x" << hd.type << std::endl;
            ss << std::dec;
            ss << "pkm.width: " << hd.width << std::endl;
            ss << "pkm.height: " << hd.height << std::endl;
            ss << "pkm.width_src: " << hd.width_src << std::endl;
            ss << "pkm.height_src: " << hd.height_src << std::endl;
            ss << std::endl;
        }
        return ss;
    }    
};

struct pvr3 {
    enum table1 {
        PVRTC_2BPP_RGB = 0,
        PVRTC_2BPP_RGBA = 1,
        PVRTC_4BPP_RGB = 2,
        PVRTC_4BPP_RGBA = 3,
        PVRTC_II_2BPP = 4,
        PVRTC_II_4BPP = 5,
        ETC1 = 6,
        DXT1 = 7,
        DXT2 = 8,
        DXT3 = 9,
        DXT4 = 10,
        DXT5 = 11,
        BC1 = 7,
        BC2 = 9,
        BC3 = 11,
        BC4 = 12,
        BC5 = 13,
        BC6 = 14,
        BC7 = 15,
        UYVY = 16,
        YUY2 = 17,
        BW1BPP = 18,
        R9G9B9E5_SHARED_EXPONENT = 19,
        RGBG8888 = 20,
        GRGB8888 = 21,
        ETC2_RGB = 22,
        ETC2_RGBA = 23,
        ETC2_RGB_A1 = 24,
        EAC_R11_UNSIGNED = 25,
        EAC_R11_SIGNED = 26,
        EAC_RG11_UNSIGNED = 27,
        EAC_RG11_SIGNED = 28,
    };
    enum table2 {
        UNSIGNED_BYTE_NORMALIZED = 0,
        SIGNED_BYTE_NORMALIZED = 1,
        UNSIGNED_BYTE = 2,
        SIGNED_BYTE = 3,
        UNSIGNED_SHORT_NORMALIZED = 4,
        SIGNED_SHORT_NORMALIZED = 5,
        UNSIGNED_SHORT = 6,
        SIGNED_SHORT = 7,
        UNSIGNED_INTEGER_NORMALIZED = 8,
        SIGNED_INTEGER_NORMALIZED = 9,
        UNSIGNED_INTEGER = 10,
        SIGNED_INTEGER = 11,
        FLOAT = 12,
    };

    struct header {
        uint32_t version;           // 0x03525650, if endianess does not match ; 0x50565203, if endianess does match
        uint32_t flags;             // 0x02, colour values within the texture have been pre-multiplied by the alpha values
        uint32_t pixel_format_1;    // see table1 above
        uint32_t pixel_format_2;    // 0

        uint32_t color_space;       // 0 linear rgb, 1 standard rgb 
        uint32_t channel_type;      // see table2 above
        uint32_t height;            // 1d texture
        uint32_t width;             // 2d texture; >= 1

        uint32_t depth;             // 3d texture; >= 1
        uint32_t num_surfaces;      // num surfaces in texture array; >= 1
        uint32_t num_faces;         // num faces in cubemap; >= 1
        uint32_t num_mipmaps;       // num levels of mipmaps; >= 1

        uint32_t metadata_size;     // length of following header
    } hd;
    struct metadata {
        uint32_t fourcc;            // any 'PVRx' where x[0..255] are reserved
        uint32_t key;               // opcode
        uint32_t datasize;          // length of following field
        uint32_t data;              // dummy field [raw binary data here]
    } mt;
    struct texturedata {
        uint32_t data;              // dummy field [raw binary data here]
    } tx;


    bool is_pvr() const {
        const char id[] = { 'P', 'V', 'R', '\x3' };
        return 0 == memcmp( id, &hd, sizeof(id) );
    }

    bool is_currently_supported() const {
        return is_pvr() 
            && hd.flags != 0x2           // 0x02, colour values within the texture have been pre-multiplied by the alpha values
            && hd.pixel_format_1 == 6    // see table1 above
            && hd.pixel_format_2 == 0    // 0
            && hd.color_space == 0       // 0 linear rgb, 1 standard rgb 
            && hd.channel_type == 0      // see table2 above
            && hd.height >= 1            // 1d texture
            && hd.width >= 1             // 2d texture; >= 1
            && hd.depth <= 1             // 3d texture; >= 1
            && hd.num_surfaces <= 1      // num surfaces in texture array; >= 1
            && hd.num_faces <= 1         // num faces in cubemap; >= 1
            && hd.num_mipmaps <= 1       // num levels of mipmaps; >= 1
            && hd.metadata_size == 0;    // length of following header            
    }

    std::ostream &debug( std::ostream &ss ) const {
        if( !is_pvr() ) {
            ss << "not a .pvr3 header" << std::endl;
        }
        else if( !is_currently_supported() ) {
            ss << "unsupported .pvr file" << std::endl;
        }
        else {
            ss << std::hex;
            ss << "pvr.version: 0x" << hd.version << std::endl;
            ss << "pvr.flags: 0x" << hd.flags << std::endl;
            ss << "pvr.pixel_format_1: 0x" << hd.pixel_format_1 << std::endl;
            ss << "pvr.pixel_format_2: 0x" << hd.pixel_format_2 << std::endl;
            ss << "pvr.color_space: 0x" << hd.color_space << std::endl;
            ss << "pvr.channel_type: 0x" << hd.channel_type << std::endl;
            ss << std::dec;
            ss << "pvr.height: " << hd.height << std::endl;
            ss << "pvr.width: " << hd.width << std::endl;
            ss << "pvr.depth: " << hd.depth << std::endl;
            ss << "pvr.num_surfaces: " << hd.num_surfaces << std::endl;
            ss << "pvr.num_faces: " << hd.num_faces << std::endl;
            ss << "pvr.num_mipmaps: " << hd.num_mipmaps << std::endl;
            ss << "pvr.metadata_size: " << hd.metadata_size << std::endl;
            ss << std::endl;
        }
        return ss;
    }

};

struct ktx {
    struct header {
        uint32_t identifier0;           // '«', 'K', 'T', 'X', ' ', '1', '1', '»', '\r', '\n', '\x1A', '\n'
        uint32_t identifier1;
        uint32_t identifier2;
        uint32_t endianness;            // 0x04030201 a little endian program must convert from big endian, and a big endian program must convert to little endian

        uint32_t glType;                // compressed == 0
        uint32_t glTypeSize;            // compressed == 1
        uint32_t glFormat;              // STENCIL_INDEX, DEPTH_COMPONENT, DEPTH_STENCIL, RED, GREEN, BLUE, RG, RGB, RGBA, BGR, BGRA, RED_INTEGER, GREEN_INTEGER, BLUE_INTEGER, RG_INTEGER, RGB_INTEGER, RGBA_INTEGER, BGR_INTEGER, BGRA_INTEGER, 
        uint32_t glInternalFormat;      // COMPRESSED_RED, COMPRESSED_RG, COMPRESSED_RGB, COMPRESSED_RGBA, COMPRESSED_SRGB, COMPRESSED_SRGB_ALPHA, COMPRESSED_RED_RGTC1, COMPRESSED_SIGNED_RED_RGTC1, COMPRESSED_RG_RGTC2, COMPRESSED_SIGNED_RG_RGTC2, COMPRESSED_RGBA_BPTC_UNORM, COMPRESSED_SRGB_ALPHA_BPTC_UNORM, COMPRESSED_RGB_BPTC_SIGNED_FLOAT, COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT, COMPRESSED_RGB8_ETC2, COMPRESSED_SRGB8_ETC2, COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, COMPRESSED_RGBA8_ETC2_EAC, COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, COMPRESSED_R11_EAC, COMPRESSED_SIGNED_R11_EAC, COMPRESSED_RG11_EAC, COMPRESSED_SIGNED_RG11_EAC, 

        uint32_t glBaseInternalFormat;  // DEPTH_COMPONENT, DEPTH_STENCIL, RED, RG, RGB, RGBA, STENCIL_INDEX, 
        uint32_t pixelWidth;
        uint32_t pixelHeight;
        uint32_t pixelDepth;

        uint32_t numberOfArrayElements;
        uint32_t numberOfFaces;
        uint32_t numberOfMipmapLevels;
        uint32_t bytesOfKeyValueData;
    } hd;

    struct metadata {
        uint32_t keyAndValueByteSize;
        uint8_t  keyAndValue;
        uint8_t  valuePadding;
    } mt;

    struct texturedata {
        uint32_t size; 
        uint32_t data;
    } tx;   

    bool is_ktx() const {
        const char id[] = { '«', 'K', 'T', 'X', ' ', '1', '1', '»', '\r', '\n', '\x1A', '\n' };
        return 0 == memcmp( id, &hd, sizeof(id) );
    }

    bool is_currently_supported() const {
        return is_ktx() 
            && hd.pixelDepth <= 1 
            && hd.bytesOfKeyValueData == 0 
            && hd.numberOfMipmapLevels <= 1 
            && hd.numberOfArrayElements <= 1 
            && hd.numberOfFaces <= 1;         
    }

    std::ostream &debug( std::ostream &ss ) const {
        if( !is_ktx() ) {
            ss << "not a .ktx header" << std::endl;
        }
        else if( !is_currently_supported() ) {
            ss << "unsupported .ktx file" << std::endl;
        }
        else {
            ss << std::hex;
            ss << "ktx.identifier0: 0x" << hd.identifier0 << std::endl;
            ss << "ktx.identifier1: 0x" << hd.identifier1 << std::endl;
            ss << "ktx.identifier2: 0x" << hd.identifier2 << std::endl;
            ss << "ktx.endianness: 0x" << std::hex << hd.endianness << std::endl;
            ss << "ktx.glType: 0x" << hd.glType << std::endl;
            ss << "ktx.glTypeSize: 0x" << hd.glTypeSize << std::endl;
            ss << "ktx.glFormat: 0x" << std::hex << hd.glFormat << std::endl;
            ss << "ktx.glInternalFormat: 0x" << std::hex << hd.glInternalFormat << std::endl;
            ss << "ktx.glBaseInternalFormat: 0x" << std::hex << hd.glBaseInternalFormat << std::endl;
            ss << std::dec;
            ss << "ktx.pixelWidth: " << hd.pixelWidth << std::endl;
            ss << "ktx.pixelHeight: " << hd.pixelHeight << std::endl;
            ss << "ktx.pixelDepth: " << hd.pixelDepth << std::endl;
            ss << "ktx.numberOfArrayElements: " << hd.numberOfArrayElements << std::endl;
            ss << "ktx.numberOfFaces: " << hd.numberOfFaces << std::endl;
            ss << "ktx.numberOfMipmapLevels: " << hd.numberOfMipmapLevels << std::endl;
            ss << "ktx.bytesOfKeyValueData: " << hd.bytesOfKeyValueData << std::endl;
            ss << std::endl;
        }
        return ss;
    }
};

struct stream {
    uint32_t w, h, fmt;

    uint32_t len;
    void *data;

    bool is_valid() const {
        return w && h; // && data && len
    }

    bool is_etc1() const {
        return is_valid() && 0 == w % 4 && 0 == h % 4 && fmt == pvr3::table1::ETC1;
    }
};

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
    if( sizeof( T::header ) < len ) {
        memcpy( &self.hd, ptr, sizeof(T::header) );
        endianness( self.hd );
        return self.is_currently_supported();
    }
    memset( &self, 0, sizeof(T::header) );
    return false;
}

bool load( std::string &data, const std::string &name, uint32_t reserved = 0 ) {
    std::stringstream ss;
    std::ifstream ifs( name.c_str(), std::ios::binary );
    if( ifs.good() && ss << ifs.rdbuf() ) {
        return data = std::string( reserved, 0 ) + ss.str(), true;
    }
    return data = std::string( reserved, 0 ), false;
}

stream encode_as_etc1( const void *rgba, int w, int h, int bpp = 32, int flags = 0, unsigned reserved = 0 ) {

    stream out { 0,0,pvr3::table1::RGBG8888, 0,0 };

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
    params.m_dithering = (flags & SPOT_ETC1_DITHER ? true : false);
    params.m_quality = rg_etc1::cLowQuality;
    if( flags & SPOT_ETC1_MEDIUM ) {
        params.m_quality = rg_etc1::cMediumQuality;
    }
    if( flags & SPOT_ETC1_HIGH ) {
        params.m_quality = rg_etc1::cHighQuality;
    }

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

    return stream { blockw*4,blockh*4,pvr3::table1::ETC1, len,dst };
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
        p.debug(std::cout);

        out.resize( sizeof(p.hd) + reserved );
        memcpy( &out[0], &p.hd, sizeof(p.hd) );
        return true;
    }
    return out.clear(), false;
}

bool save_pvr_etc1( std::string &out, const stream &sm, unsigned reserved = 0 ) {
    if( sm.is_valid() && sm.is_etc1() ) {
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
        pvr.debug(std::cout);

        out.resize( sizeof(pvr3::header) + reserved );
        memcpy( &out[0], &pvr.hd, sizeof(pvr.hd) );
        return true;
    }
    return out.clear(), false;
}

bool save_ktx_etc1( std::string &out, const stream &sm, unsigned reserved = 0 ) {
    if( sm.is_valid() && sm.is_etc1() ) {
        ktx k;
        k.hd.identifier0 = tole32(0x58544bab); 
        k.hd.identifier1 = tole32(0xbb313120); 
        k.hd.identifier2 = tole32(0xa1a0a0d);  
        k.hd.endianness = tole32(0x4030201);
        k.hd.glType = tole32(0x0); // table 8.2 of opengl 4.4 spec; UNSIGNED_BYTE, UNSIGNED_SHORT_5_6_5, etc.)
        k.hd.glTypeSize = tole32(0x1); // 1 for compressed data
        k.hd.glFormat = tole32(0x0); // table 8.3 of opengl 4.4 spec; RGB, RGBA, BGRA...
        k.hd.glInternalFormat = tole32(0x8d64); // table 8.14 of opengl 4.4 spec; ETC1_RGB8_OES (0x8D64)
        k.hd.glBaseInternalFormat = tole32(0x1907); // table 8.11 of opengl 4.4 spec; GL_RGB (0x1907), RGBA, ALPHA...
        k.hd.pixelWidth = tole32(sm.w);
        k.hd.pixelHeight = tole32(sm.h);
        k.hd.pixelDepth = tole32(0);
        k.hd.numberOfArrayElements = tole32(0);
        k.hd.numberOfFaces = tole32(1);
        k.hd.numberOfMipmapLevels = tole32(1);
        k.hd.bytesOfKeyValueData = tole32(0);
        k.debug(std::cout);

        out.resize( sizeof(ktx::header) + reserved );
        memcpy( &out[0], &k.hd, sizeof(ktx::header) );
        return true;
    }
    return out.clear(), false;
}

std::string save_pvr_etc1( const stream &sm, unsigned reserved = 0 ) {
    std::string out;
    return save_pvr_etc1( out, sm, reserved ) ? out : std::string();
}
std::string save_ktx_etc1( const stream &sm, unsigned reserved = 0 ) {
    std::string out;
    return save_ktx_etc1( out, sm, reserved ) ? out : std::string();
}
std::string save_pkm_etc1( const stream &sm, unsigned reserved = 0 ) {
    std::string out;
    return save_pkm_etc1( out, sm, reserved ) ? out : std::string();
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


int main( int argc, const char **argv ) {
    std::string file = argc > 1 ? argv[1] : "../images/test-dxt5.crn";
    std::string data;
    if( !load(data, file.c_str()) ) {
        return -1; // cannot load file
    }

    unsigned buffer = 0;
    {
        ktx k;
        preload(k, &data[buffer], data.size() );
        k.debug( std::cout );

        pvr3 p;
        preload(p, &data[buffer], data.size() );
        p.debug( std::cout );

        pkm pk;
        preload(pk, &data[buffer], data.size() );
        pk.debug( std::cout );

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
    }

    spot::image img( (void *)&data[buffer], data.size() );
    display( img );

    {
    stream sm = encode_as_etc1( img.rgba32().data(), img.w, img.h, 32, /*SPOT_ETC1_DITHER |*/ SPOT_ETC1_LOW );
    { std::ofstream os("etc1.ktx", std::ios::binary ); os << save_ktx_etc1(sm); os.write((const char *)sm.data, sm.len); }
    { std::ofstream os("etc1.pvr", std::ios::binary ); os << save_pvr_etc1(sm); os.write((const char *)sm.data, sm.len); }
    { std::ofstream os("etc1.pkm", std::ios::binary ); os << save_pkm_etc1(sm); os.write((const char *)sm.data, sm.len); }
    delete [] sm.data;
    }

    std::cout << "fast: " << bench([&]{ encode_as_etc1( img.rgba32().data(), img.w, img.h, 32, SPOT_ETC1_LOW );} ) << std::endl;
    std::cout << "medium: " << bench([&]{ encode_as_etc1( img.rgba32().data(), img.w, img.h, 32, SPOT_ETC1_MEDIUM );} ) << std::endl;
    //std::cout << "high: " << bench([&]{ encode_as_etc1( img.rgba32().data(), img.w, img.h, 32, SPOT_ETC1_HIGH );} ) << std::endl;

    return 0;
}

