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

#include <cassert>

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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "deps/nanosvg/src/nanosvg.h"
#include "deps/nanosvg/src/nanosvgrast.h"

#include "deps/soil2/stb_image.c"
#include "deps/soil2/stb_image_write.c"

#define DDS_header DDS_header2
#   include "deps/soil2/image_DXT.h"
#   include "deps/soil2/image_DXT.c"
#define clamp clamp2
#   include "deps/soil2/etc1_utils.c"
#   include "deps/soil2/image_helper.c"
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

#include <stdint.h>

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
        const char *str[] = { "bmp", "dds", "jpg", "png", "tga", "webp", "pug", 0 };
        std::vector<std::string> list;
        for( int i = 0; str[i]; ++i ) {
            list.push_back( str[i] );
        }
        return list;
    }

    std::vector<unsigned char> decode8( const void *ptr, size_t size, size_t *w, size_t *h, std::string *error, bool make_squared, bool mirror_w, bool mirror_h ) {
        std::vector<unsigned char> temp;

        if( !ptr ) {
            if( error ) *error = "Error: invalid pointer provided";
            return std::vector<unsigned char>();
        }
            
        if( !size ) {
            if( error ) *error = "Error: invalid size provided";
            return std::vector<unsigned char>();
        }

        // decode
        int imageWidth = 0, imageHeight = 0, imageBpp = 0;
        enum { STBI_DELETER, FREE_DELETER, NEW_DELETER, NEW_ARRAY_DELETER, NO_DELETER } deleter = NO_DELETER;
        stbi_uc *imageuc = 0;

        if( !imageuc )
        {
            imageuc = stbi_load_from_memory( (const unsigned char *)ptr, size, &imageWidth, &imageHeight, &imageBpp, 4 );
            deleter = STBI_DELETER;

            if( !imageuc ) {
                // is it crn?
                bool is_crn = size > 2 && ((const char *)ptr)[0] == 'H' && ((const char *)ptr)[1] == 'x';
                if( is_crn ) {
                    std::string dds;
                    if( crn2dds( dds, ptr, size ) ) {
                        imageuc = stbi_load_from_memory( (const unsigned char *)&dds[0], dds.size(), &imageWidth, &imageHeight, &imageBpp, 4 );
                    }                
                }
            }

            if( imageuc ) {
                // decode alpha if it is a .pug file
                const char *magic = (const char *)ptr + size - 4;
                if( magic[0] == 'p' && magic[1] == 'u' && magic[2] == 'g' && magic[3] == '1' ) {
                    const int32_t color_size = internals::swapbe( *(const int32_t *)((const char *)ptr + size - 12) );
                    const int32_t alpha_size = internals::swapbe( *(const int32_t *)((const char *)ptr + size - 8) );
                    int w2 = 0, h2 = 0, bpp2 = 0;
                    stbi_uc *alpha = stbi_load_from_memory( (const unsigned char *)ptr + color_size, alpha_size, &w2, &h2, &bpp2, 1 );
                    if( alpha ) {
                        stbi_uc *src = &alpha[ 0 ], *end = &alpha[ w2*h2 ], *dst = &imageuc[ 3 ];
                        while( src < end ) {
                            *dst = *src++;
                            dst += 4;
                        }
                        stbi_image_free( alpha );                        
                    }
                }                
            }
        }

        if( !imageuc )
        {
            bool ok = 0 != WebPGetInfo( (const uint8_t *)ptr, size, &imageWidth, &imageHeight );
            if( ok ) {
                imageuc = (stbi_uc *) WebPDecodeRGBA( (const uint8_t *)ptr, size, &imageWidth, &imageHeight );
                deleter = FREE_DELETER;
                imageBpp = 4;
            }
        }

        if( !imageuc )
        {
            // Load SVG, parse and rasterize
            char *str = new char[ size + 1 ];
            memcpy( str, ptr, size );
            str[size] = '\0';

            NSVGimage *image = (str[0] == '<' || str[0] == ' ' || str[0] == '\t' ? nsvgParse( str, "px" /*units*/, 96.f /* dpi */ ) : (NSVGimage *)0 );
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

                double scale = 1.0;
                imageWidth = image->width * scale;
                imageHeight = image->height * scale;
                imageBpp = 4;

                imageuc = (stbi_uc *)malloc(w*h*4);
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
                    nsvgRasterize( local.rasterizer, image, 0,0,scale, imageuc, w, h, w*scale*4 );
                }
                nsvgDelete(image);
            }

            delete [] str;
        }

        if( !imageuc )
        {
            // assert( false );
            // return yellow/black texture instead?
            if( error ) *error = "Error! unable to decode image";
            return std::vector<unsigned char>();
        }

        if( make_squared )
        {
            bool is_power_of_two_w = imageWidth && !(imageWidth & (imageWidth - 1));
            bool is_power_of_two_h = imageHeight && !(imageHeight & (imageHeight - 1));

            if( is_power_of_two_w && is_power_of_two_h )
            {
                temp.resize( imageWidth * imageHeight * 4 );
                memcpy( &temp[0], imageuc, imageWidth * imageHeight * 4 );
            }
            else
            {
                int nw = 1, nh = 1, atx, aty;
                while( nw < imageWidth ) nw <<= 1;
                while( nh < imageHeight ) nh <<= 1;

                // squared as well, cos we want them pixel perfect
                if( nh > nw ) nw = nh; else nh = nw;

                temp.resize( nw * nh * 4, 0 );
                atx = (nw - imageWidth) / 2;
                aty = (nh - imageHeight) / 2;

                //std::cout << wire::string( "\1x\2 -> \3x\4 @(\5,\6)\n", imageWidth, imageHeight, nw, nh, atx, aty);

                for( int y = 0; y < imageHeight; ++y )
                    memcpy( &((stbi_uc*)&temp[0])[ ((atx)+(aty+y)*nw)*4 ], &imageuc[ (y*imageWidth) * 4 ], imageWidth * 4 );

                imageWidth = nw;
                imageHeight = nh;
            }
        }
        else
        {
                temp.resize( imageWidth * imageHeight * 4 );
                memcpy( &temp[0], imageuc, imageWidth * imageHeight * 4 );
        }

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

        if( mirror_w && !mirror_h )
        {
            std::vector<stbi_uc> mirrored( temp.size() );

            for( int c = 0, y = 0; y < imageHeight; ++y )
            for( int x = imageWidth - 1; x >= 0; --x )
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 0 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 1 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 2 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 3 ];

            temp = mirrored;
        }
        else
        if( mirror_h && !mirror_w )
        {
            std::vector<stbi_uc> mirrored( temp.size() );

            for( int c = 0, y = imageHeight - 1; y >= 0; --y )
            for( int x = 0; x < imageWidth; ++x )
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 0 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 1 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 2 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 3 ];

            temp = mirrored;
        }
        else
        if( mirror_w && mirror_h )
        {
            std::vector<stbi_uc> mirrored( temp.size() );

            for( int c = 0, y = imageHeight - 1; y >= 0; --y )
            for( int x = imageWidth - 1; x >= 0; --x )
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 0 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 1 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 2 ],
                mirrored[ c++ ] = temp[ ( x + y * imageWidth ) * 4 + 3 ];

            temp = mirrored;

            std::swap( imageWidth, imageHeight );
        }

        if( w ) *w = imageWidth;
        if( h ) *h = imageHeight;

        return temp;
    }

    std::vector<unsigned char> decode8( const std::string &filename, size_t *w, size_t *h, std::string *error, bool make_squared, bool mirror_w, bool mirror_h ) {
        std::ifstream ifs( filename.c_str(), std::ios::binary );
        std::vector<char> buffer( (std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return decode8( (const unsigned char *)buffer.data(), buffer.size(), w, h, error, make_squared, mirror_w, mirror_h );
    }

    std::vector<unsigned int> decode32( const void *ptr, size_t size, size_t *w, size_t *h, std::string *error, bool make_squared, bool mirror_w, bool mirror_h ) {
        std::vector<unsigned char> decoded = decode8( ptr, size, w, h, error, make_squared, mirror_w, mirror_h ); 
        std::vector<unsigned int> out;
        if( !decoded.empty() ) {
            out.reserve( decoded.size() / 4 );
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

    std::vector<unsigned int> decode32( const std::string &filename, size_t *w, size_t *h, std::string *error, bool make_squared, bool mirror_w, bool mirror_h ) {
        std::ifstream ifs( filename.c_str(), std::ios::binary );
        std::vector<char> buffer( (std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
        return decode32( (const unsigned char *)buffer.data(), buffer.size(), w, h, error, make_squared, mirror_w, mirror_h );
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
}
