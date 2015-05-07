// an embeddable/compact console font that supports most european/greek/cyrillic unicode codepoints. aimed to gamedev.
// - rlyeh, zlib/libpng licensed.

// using:
// - [PressStart2P.ttf v2.14](http://www.zone38.net/font/) by Cody "CodeMan38" Boisclair (SIL Open Font License).
// - [UTF-8 dfa decoder](http://bjoern.hoehrmann.de/utf-8/decoder/dfa/) by Bjoern Hoehrmann (MIT license).
// - Gradient retro style mask taken from [DoDonPachi arcade](http://en.wikipedia.org/wiki/DoDonPachi).

//#pragma once
#include <stdint.h>
#include <vector>
#include <map>

#define UNIFONT_VERSION "v1.0.0 (C++0x version)"

template<typename color>
struct unifont {

    struct dim {
        uint32_t w, h;
    };

    enum {
        NORMAL = 0,
        INVERT = 1,
        SHADOW = 2,
        RETRO  = 4,
    };

    struct framebuffer {
        color *pixel;
        uint32_t width;
        color (*make_rgba)( uint8_t r, uint8_t g, uint8_t b, uint8_t a );
    } fb;

    std::vector<color> gradient;   // precomputed gradient colors [8][8]
    std::vector<color> flat;       // precomputed flat colors [8][8]
    color Z, W, P;                 // precomputed base colors

    unifont( color *pixel, uint32_t width, color (*make_rgba)( uint8_t r, uint8_t g, uint8_t b, uint8_t a )  )
        : PressStart2P_ttf(get_font())
    {
        fb.pixel = pixel;
        fb.width = width;
        fb.make_rgba = make_rgba;

        Z = (*fb.make_rgba)(   0,   0,   0, 255 );
        W = (*fb.make_rgba)( 255, 255, 255, 255 );
        P = (*fb.make_rgba)( 255,   0, 255, 255 );

        color A = (*fb.make_rgba)( 255, 165,  82, 255 );
        color B = (*fb.make_rgba)( 255, 115,  33, 255 );
        color C = (*fb.make_rgba)( 247,  66,  16, 255 );
        color D = (*fb.make_rgba)( 206,  24,   8, 255 );
        color E = (*fb.make_rgba)( 156,   0,   0, 255 );
        color F = (*fb.make_rgba)( 115,   0,   0, 255 );

        flat = std::vector<color>( 64, W ); // default white (flat) gradient
        // do don pachi gradient style 
        gradient.push_back(A),gradient.push_back(A),gradient.push_back(A),gradient.push_back(A),gradient.push_back(A),gradient.push_back(B),gradient.push_back(B),gradient.push_back(C);
        gradient.push_back(A),gradient.push_back(A),gradient.push_back(A),gradient.push_back(A),gradient.push_back(B),gradient.push_back(B),gradient.push_back(C),gradient.push_back(C);
        gradient.push_back(A),gradient.push_back(A),gradient.push_back(A),gradient.push_back(B),gradient.push_back(B),gradient.push_back(C),gradient.push_back(C),gradient.push_back(D);
        gradient.push_back(A),gradient.push_back(A),gradient.push_back(B),gradient.push_back(B),gradient.push_back(C),gradient.push_back(C),gradient.push_back(D),gradient.push_back(D);
        gradient.push_back(A),gradient.push_back(B),gradient.push_back(B),gradient.push_back(C),gradient.push_back(C),gradient.push_back(D),gradient.push_back(D),gradient.push_back(E);
        gradient.push_back(B),gradient.push_back(B),gradient.push_back(C),gradient.push_back(C),gradient.push_back(D),gradient.push_back(D),gradient.push_back(E),gradient.push_back(E);
        gradient.push_back(B),gradient.push_back(C),gradient.push_back(C),gradient.push_back(D),gradient.push_back(D),gradient.push_back(E),gradient.push_back(E),gradient.push_back(F);
        gradient.push_back(C),gradient.push_back(C),gradient.push_back(D),gradient.push_back(D),gradient.push_back(E),gradient.push_back(E),gradient.push_back(F),gradient.push_back(F);
    }

    dim render_line( int x0, int y0, int x1, int y1, const color &c ) const {
        if( x1 == x0 ) {
            for( ; y0 < y1 ; ++y0 ) {
                fb.pixel[ x0 + y0 * fb.width ] = c;
            }
        } else {
            float deltax = x1 - x0;
            float deltay = y1 - y0;
            float error = 0.0f;
            float deltaerr = abs(deltay / deltax);
            float y = y0;
            for( float x = x0; x < x1; ++x) {
                fb.pixel[ (uint32_t)(x + y * fb.width) ] = c;
                error += deltaerr;
                while( error >= 0.5f ) {
                    fb.pixel[ (uint32_t)(x + y * fb.width) ] = c;
                    y += y1 - y0 > 0 ? 1 : -1;
                    error -= 1.0f;
                }
            }
        }
        return dim { unsigned(x1 - x0), unsigned(y1 - y0) };
    }

    dim render_string( int ox, int oy, int mode, const std::vector<uint64_t> &glyphs, const color *mask8x8 = 0 ) const {
        if( !mask8x8 ) mask8x8 = (mode & 4 ? &gradient[0] : &flat[0]);

        auto *ptr = &fb.pixel[ ox + oy * fb.width ];
        for( const auto &rune : glyphs ) {
            for( unsigned y = 0; y < 8; ++y ) {
                const unsigned y64 = ( 7 - y ) * 8;
                if( mode & SHADOW ) {
                ptr += fb.width + 1;
                ptr[0] = rune & (0x80ULL << y64) ? Z : ptr[0];
                ptr[1] = rune & (0x40ULL << y64) ? Z : ptr[1];
                ptr[2] = rune & (0x20ULL << y64) ? Z : ptr[2];
                ptr[3] = rune & (0x10ULL << y64) ? Z : ptr[3];
                ptr[4] = rune & (0x08ULL << y64) ? Z : ptr[4];
                ptr[5] = rune & (0x04ULL << y64) ? Z : ptr[5];
                ptr[6] = rune & (0x02ULL << y64) ? Z : ptr[6];
                ptr[7] = rune & (0x01ULL << y64) ? Z : ptr[7];
                ptr -= fb.width + 1;
                }
                if( mode & INVERT ) {
                ptr[0] = rune & (0x80ULL << y64) ? ptr[0] : mask8x8[0+y*8];
                ptr[1] = rune & (0x40ULL << y64) ? ptr[1] : mask8x8[1+y*8];
                ptr[2] = rune & (0x20ULL << y64) ? ptr[2] : mask8x8[2+y*8];
                ptr[3] = rune & (0x10ULL << y64) ? ptr[3] : mask8x8[3+y*8];
                ptr[4] = rune & (0x08ULL << y64) ? ptr[4] : mask8x8[4+y*8];
                ptr[5] = rune & (0x04ULL << y64) ? ptr[5] : mask8x8[5+y*8];
                ptr[6] = rune & (0x02ULL << y64) ? ptr[6] : mask8x8[6+y*8];
                ptr[7] = rune & (0x01ULL << y64) ? ptr[7] : mask8x8[7+y*8];
                } else { // NORMAL
                ptr[0] = rune & (0x80ULL << y64) ? mask8x8[0+y*8] : ptr[0];
                ptr[1] = rune & (0x40ULL << y64) ? mask8x8[1+y*8] : ptr[1];
                ptr[2] = rune & (0x20ULL << y64) ? mask8x8[2+y*8] : ptr[2];
                ptr[3] = rune & (0x10ULL << y64) ? mask8x8[3+y*8] : ptr[3];
                ptr[4] = rune & (0x08ULL << y64) ? mask8x8[4+y*8] : ptr[4];
                ptr[5] = rune & (0x04ULL << y64) ? mask8x8[5+y*8] : ptr[5];
                ptr[6] = rune & (0x02ULL << y64) ? mask8x8[6+y*8] : ptr[6];
                ptr[7] = rune & (0x01ULL << y64) ? mask8x8[7+y*8] : ptr[7];
                }
                ptr += fb.width;
            }
            ptr -= fb.width * 8;
            ptr += 8;
        }

        dim d; d.w = unsigned(glyphs.size() * 8), d.h = unsigned((!glyphs.empty()) * 8);
        return d;
    }

    dim render_string( int ox, int oy, int mode, const std::vector<int32_t> &codepoints, const color *mask8x8 = 0 ) const {
        std::vector<uint64_t> glyphs;
        for( auto &cp : codepoints ) {
            auto found = PressStart2P_ttf.find(cp);
            if( found != PressStart2P_ttf.end() ) {
                glyphs.push_back( found->second );
            } else {
                glyphs.push_back( 0 );
            }
        }
        return render_string( ox, oy, mode, glyphs, mask8x8 );
    }

    dim render_string( int ox, int oy, int mode, const char *utf8, const color *mask8x8 = 0 ) const {
        auto decode = []( std::vector<int32_t> &codepoints, const char *utf8 ) -> const char * {
            uint32_t result, state = 0;
            while( *utf8 ) {
                // Decode utf8 codepoint a byte at a time. Uses explictly user provided state variable,
                // that should be initialized to zero before first use. Places the result to codep.
                // Returns UTF8_ACCEPT when a full codepoint achieved
                enum { UTF8_ACCEPT = 0, UTF8_REJECT = 1 };
                auto decode_state = []( uint32_t *state, uint32_t *codep, uint32_t byte ) -> uint32_t {
                    static const uint8_t utf8d[] = {
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
                      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
                      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
                      7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
                      8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
                      0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
                      0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
                      0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
                      1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
                      1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
                      1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
                      1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
                    };
                    uint32_t type = utf8d[byte];
                    *codep = (*state != UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);
                    *state = utf8d[256 + *state*16 + type];
                    return *state;
                };
                uint32_t res = decode_state( &state, &result, (unsigned char)(*utf8++) );
                /**/ if(res == UTF8_ACCEPT) return codepoints.push_back( result ), utf8;
                else if(res == UTF8_REJECT) break;
            }
            return codepoints.push_back( 0xfffd ), utf8;
        };
        std::vector<int32_t> codepoints;
        while( *utf8 ) {
            utf8 = decode( codepoints, utf8 );
        }
        return render_string( ox, oy, mode, codepoints, mask8x8 );
    }

    dim render_glyphs( int ox, int oy, int mode, const color *mask8x8 = 0 ) const {
        uint32_t h = (PressStart2P_ttf.size() / 16) + 1;
        for( uint32_t y = 0; y < h; y++ ) {
            render_line( ox, oy + y*9, ox + 16*9, oy + y*9, P );
            for( uint32_t x = 0; x < 16; x++ ) {
                render_line( ox + x*9, oy, ox + x*9, oy + h*9, P );
                auto it = PressStart2P_ttf.cbegin();
                std::advance( it, x + y * 16 );
                render_string( 1 + ox + x*9, 1 + oy + y*9, mode, std::vector<int32_t> { it->first }, mask8x8 );
            }
        }
        dim d; d. w =16*9, d.h = h*9; return d; 
    }

    dim render_string( int ox, int oy, int mode, const std::vector<uint64_t> &glyphs, const color &c ) {
        return render_string( ox, oy, mode, glyphs, &std::vector<color>( 64, c )[0] );
    }
    dim render_string( int ox, int oy, int mode, const std::vector<int32_t> &codepoints, const color &c ){
        return render_string( ox, oy, mode, codepoints, &std::vector<color>( 64, c )[0] );
    }
    dim render_string( int ox, int oy, int mode, const char *utf8, const color &c ) {
        return render_string( ox, oy, mode, utf8, &std::vector<color>( 64, c )[0] );
    }
    dim render_glyphs( int ox, int oy, int mode, const color &c ) {
        return render_glyphs( ox, oy, mode, &std::vector<color>( 64, c )[0] );
    }

    /* generated by ttf2mono.cc - https://github.com/r-lyeh */
    const std::map<int,uint64_t> &PressStart2P_ttf;
    static const std::map<int,uint64_t> &get_font() {
    static std::map<int,uint64_t> map;
    if( !map.empty() ) return map; else return
    map[0x0020]=0x0000000000000000,map[0x0021]=0x3838383030003000,map[0x0022]=0x6c6c6c0000000000,
    map[0x0023]=0x6cfe6c6c6cfe6c00,map[0x0024]=0x107cd07c16fc1000,map[0x0025]=0x62a4c810264a8c00,
    map[0x0026]=0x70d8d870dacc7e00,map[0x0027]=0x3030300000000000,map[0x0028]=0x0c18303030180c00,
    map[0x0029]=0x6030181818306000,map[0x002a]=0x006c38fe386c0000,map[0x002b]=0x0018187e18180000,
    map[0x002c]=0x0000000000303060,map[0x002d]=0x0000007e00000000,map[0x002e]=0x0000000000303000,
    map[0x002f]=0x0204081020408000,map[0x0030]=0x384cc6c6c6643800,map[0x0031]=0x1838181818187e00,
    map[0x0032]=0x7cc60e3c78e0fe00,map[0x0033]=0x7e0c183c06c67c00,map[0x0034]=0x1c3c6cccfe0c0c00,
    map[0x0035]=0xfcc0fc0606c67c00,map[0x0036]=0x3c60c0fcc6c67c00,map[0x0037]=0xfec60c1830303000,
    map[0x0038]=0x78c4e4789e867c00,map[0x0039]=0x7cc6c67e060c7800,map[0x003a]=0x0030300030300000,
    map[0x003b]=0x0030300030306000,map[0x003c]=0x0c18306030180c00,map[0x003d]=0x0000fe00fe000000,
    map[0x003e]=0x6030180c18306000,map[0x003f]=0x7cfec60c38003800,map[0x0040]=0x7c82baaabe807c00,
    map[0x0041]=0x386cc6c6fec6c600,map[0x0042]=0xfcc6c6fcc6c6fc00,map[0x0043]=0x3c66c0c0c0663c00,
    map[0x0044]=0xf8ccc6c6c6ccf800,map[0x0045]=0xfec0c0fcc0c0fe00,map[0x0046]=0xfec0c0fcc0c0c000,
    map[0x0047]=0x3e60c0cec6663e00,map[0x0048]=0xc6c6c6fec6c6c600,map[0x0049]=0x7e18181818187e00,
    map[0x004a]=0x0606060606c67c00,map[0x004b]=0xc6ccd8f0f8dcce00,map[0x004c]=0x6060606060607e00,
    map[0x004d]=0xc6eefefed6c6c600,map[0x004e]=0xc6e6f6fedecec600,map[0x004f]=0x7cc6c6c6c6c67c00,
    map[0x0050]=0xfcc6c6c6fcc0c000,map[0x0051]=0x7cc6c6c6decc7a00,map[0x0052]=0xfcc6c6cef8dcce00,
    map[0x0053]=0x78ccc07c06c67c00,map[0x0054]=0x7e18181818181800,map[0x0055]=0xc6c6c6c6c6c67c00,
    map[0x0056]=0xc6c6c6ee7c381000,map[0x0057]=0xc6c6d6fefeeec600,map[0x0058]=0xc6ee7c387ceec600,
    map[0x0059]=0x6666663c18181800,map[0x005a]=0xfe0e1c3870e0fe00,map[0x005b]=0x3c30303030303c00,
    map[0x005c]=0x8040201008040200,map[0x005d]=0x7818181818187800,map[0x005e]=0x386c000000000000,
    map[0x005f]=0x00000000000000fe,map[0x0060]=0x1008000000000000,map[0x0061]=0x00007c067ec67e00,
    map[0x0062]=0xc0c0fcc6c6c67c00,map[0x0063]=0x00007ec0c0c07e00,map[0x0064]=0x06067ec6c6c67e00,
    map[0x0065]=0x00007cc6fec07c00,map[0x0066]=0x0e187e1818181800,map[0x0067]=0x00007ec6c67e067c,
    map[0x0068]=0xc0c0fcc6c6c6c600,map[0x0069]=0x1800381818187e00,map[0x006a]=0x0c001c0c0c0c0c78,
    map[0x006b]=0xc0c0cefcf8dcce00,map[0x006c]=0x3818181818187e00,map[0x006d]=0x0000fcb6b6b6b600,
    map[0x006e]=0x0000fcc6c6c6c600,map[0x006f]=0x00007cc6c6c67c00,map[0x0070]=0x0000fcc6c6fcc0c0,
    map[0x0071]=0x00007ec6c67e0606,map[0x0072]=0x00006e7060606000,map[0x0073]=0x00007cc07c06fc00,
    map[0x0074]=0x18187e1818181800,map[0x0075]=0x0000c6c6c6c67e00,map[0x0076]=0x00006666663c1800,
    map[0x0077]=0x0000b6b6b6b67e00,map[0x0078]=0x0000c6fe38fec600,map[0x0079]=0x0000c6c6c67e067c,
    map[0x007a]=0x0000fe1c3870fe00,map[0x007b]=0x0c18183018180c00,map[0x007c]=0x1818181818181800,
    map[0x007d]=0x6030301830306000,map[0x007e]=0x000070ba1c000000,map[0x007f]=0x00000000006c6c00,
    map[0x00a0]=0x0000000000000000,map[0x00a1]=0x1800181838383800,map[0x00a2]=0x107cd6d0d67c1000,
    map[0x00a3]=0x3c6660fc6060fe00,map[0x00a4]=0x005a2424245a0000,map[0x00a5]=0x66663c7e187e1800,
    map[0x00a6]=0x1818180018181800,map[0x00a7]=0x3c6678241e663c00,map[0x00a8]=0x6c00000000000000,
    map[0x00a9]=0x3c4299a1a199423c,map[0x00aa]=0x783c6c3c00000000,map[0x00ab]=0x00366cd86c360000,
    map[0x00ac]=0x00007e0606000000,map[0x00ad]=0x0000007e00000000,map[0x00ae]=0x3c42b9a5b9a5423c,
    map[0x00af]=0x7c00000000000000,map[0x00b0]=0x1028100000000000,map[0x00b1]=0x18187e1818007e00,
    map[0x00b2]=0x380c183c00000000,map[0x00b3]=0x3c180c3800000000,map[0x00b4]=0x0810000000000000,
    map[0x00b5]=0x0000ccccccccf6c0,map[0x00b6]=0x3e6a4a6a3e0a0a00,map[0x00b7]=0x0000003030000000,
    map[0x00b8]=0x0000000000000830,map[0x00b9]=0x1838183c00000000,map[0x00ba]=0x386c6c3800000000,
    map[0x00bb]=0x00d86c366cd80000,map[0x00bc]=0x42c44852264e8200,map[0x00bd]=0x42c4485622448e00,
    map[0x00be]=0xe24428d2264e8200,map[0x00bf]=0x38003860c6fe7c00,map[0x00c0]=0x2010386cc6fec600,
    map[0x00c1]=0x0810386cc6fec600,map[0x00c2]=0x386c386cc6fec600,map[0x00c3]=0x3458386cc6fec600,
    map[0x00c4]=0x6c00386cc6fec600,map[0x00c5]=0x1028386cc6fec600,map[0x00c6]=0x3e78d8def8d8de00,
    map[0x00c7]=0x3c66c0c0663c0830,map[0x00c8]=0x2010fec0fcc0fe00,map[0x00c9]=0x0810fec0fcc0fe00,
    map[0x00ca]=0x386cfec0fcc0fe00,map[0x00cb]=0x6c00fec0fcc0fe00,map[0x00cc]=0x10087e1818187e00,
    map[0x00cd]=0x08107e1818187e00,map[0x00ce]=0x183c7e1818187e00,map[0x00cf]=0x66007e1818187e00,
    map[0x00d0]=0x786c66f6666c7800,map[0x00d1]=0x3458e6f6fedece00,map[0x00d2]=0x20107cc6c6c67c00,
    map[0x00d3]=0x08107cc6c6c67c00,map[0x00d4]=0x386c7cc6c6c67c00,map[0x00d5]=0x34587cc6c6c67c00,
    map[0x00d6]=0x6c007cc6c6c67c00,map[0x00d7]=0x0044281028440000,map[0x00d8]=0x7cc6ced6e6c67c00,
    map[0x00d9]=0x2010c6c6c6c67c00,map[0x00da]=0x0810c6c6c6c67c00,map[0x00db]=0x386c00c6c6c67c00,
    map[0x00dc]=0x6c00c6c6c6c67c00,map[0x00dd]=0x081066663c181800,map[0x00de]=0xc0fcc6c6c6fcc000,
    map[0x00df]=0x3c66666c66766c00,map[0x00e0]=0x20107c067ec67e00,map[0x00e1]=0x08107c067ec67e00,
    map[0x00e2]=0x386c7c067ec67e00,map[0x00e3]=0x34587c067ec67e00,map[0x00e4]=0x6c007c067ec67e00,
    map[0x00e5]=0x10287c067ec67e00,map[0x00e6]=0x00007c167ed07c00,map[0x00e7]=0x00007ec0c07e0830,
    map[0x00e8]=0x20107cc6fec07c00,map[0x00e9]=0x08107cc6fec07c00,map[0x00ea]=0x386c7cc6fec07c00,
    map[0x00eb]=0x6c007cc6fec07c00,map[0x00ec]=0x2010003818187e00,map[0x00ed]=0x0810003818187e00,
    map[0x00ee]=0x386c003818187e00,map[0x00ef]=0x6c00381818187e00,map[0x00f0]=0x6478987cc6c67c00,
    map[0x00f1]=0x3458fcc6c6c6c600,map[0x00f2]=0x20107cc6c6c67c00,map[0x00f3]=0x08107cc6c6c67c00,
    map[0x00f4]=0x386c7cc6c6c67c00,map[0x00f5]=0x34587cc6c6c67c00,map[0x00f6]=0x6c007cc6c6c67c00,
    map[0x00f7]=0x0018007e00180000,map[0x00f8]=0x00007cced6e67c00,map[0x00f9]=0x2010c6c6c6c67e00,
    map[0x00fa]=0x0810c6c6c6c67e00,map[0x00fb]=0x386c00c6c6c67e00,map[0x00fc]=0x6c00c6c6c6c67e00,
    map[0x00fd]=0x0810c6c6c67e067c,map[0x00fe]=0xc0c0fcc6c6fcc0c0,map[0x00ff]=0x6c00c6c6c67e067c,
    map[0x0100]=0x7c00386cc6fec600,map[0x0101]=0x7c007c067ec67e00,map[0x0102]=0x4438386cc6fec600,
    map[0x0103]=0x44387c067ec67e00,map[0x0104]=0x386cc6fec6040806,map[0x0105]=0x7c067ec67e102018,
    map[0x0106]=0x08103c66c0663c00,map[0x0107]=0x08107ec0c0c07e00,map[0x0108]=0x386c3c66c0663c00,
    map[0x0109]=0x386c7ec0c0c07e00,map[0x010a]=0x18003c66c0663c00,map[0x010b]=0x18007ec0c0c07e00,
    map[0x010c]=0x663c3c66c0663c00,map[0x010d]=0x6c387ec0c0c07e00,map[0x010e]=0x6c38f8ccc6ccf800,
    map[0x010f]=0x1b1b7ad8d8d87800,map[0x0110]=0x786c66f6666c7800,map[0x0111]=0x0c1e7ccccccc7c00,
    map[0x0112]=0x7c00fec0fcc0fe00,map[0x0113]=0x7c007cc6fec07c00,map[0x0114]=0x4438fec0fcc0fe00,
    map[0x0115]=0x44387cc6fec07c00,map[0x0116]=0x1800fec0fcc0fe00,map[0x0117]=0x18007cc6fec07c00,
    map[0x0118]=0xfec0fcc0fe102018,map[0x0119]=0x7cc6fec07c102018,map[0x011a]=0x6c38fec0fcc0fe00,
    map[0x011b]=0x6c387cc6fec07c00,map[0x011c]=0x386c7ec0cec67e00,map[0x011d]=0x386c7ec6c67e067c,
    map[0x011e]=0x44387ec0cec67e00,map[0x011f]=0x44387ec6c67e067c,map[0x0120]=0x18007ec0cec67e00,
    map[0x0121]=0x18007ec6c67e067c,map[0x0122]=0x3e60c0ce663e1830,map[0x0123]=0x18307ec6c67e067c,
    map[0x0124]=0x38eec6c6fec6c600,map[0x0125]=0x38ecc0fcc6c6c600,map[0x0126]=0xc6fec6fec6c6c600,
    map[0x0127]=0x60f07c6666666600,map[0x0128]=0x324c7e1818187e00,map[0x0129]=0x324c003818187e00,
    map[0x012a]=0x7e007e1818187e00,map[0x012b]=0x7c00381818187e00,map[0x012c]=0x24187e1818187e00,
    map[0x012d]=0x4438381818187e00,map[0x012e]=0x7e1818187e102018,map[0x012f]=0x180038187e102018,
    map[0x0130]=0x18007e1818187e00,map[0x0131]=0x0000381818187e00,map[0x0132]=0xf6666666f6067c00,
    map[0x0133]=0x6600ee666666063c,map[0x0134]=0x1c36060606c67c00,map[0x0135]=0x1c36001c0c0c0c78,
    map[0x0136]=0xccd8f0f8dcce3060,map[0x0137]=0xc0cefcf8dcce3060,map[0x0138]=0x0000c6ccf8ccc600,
    map[0x0139]=0x0870606060607e00,map[0x013a]=0x102070303030fc00,map[0x013b]=0x60606060607e1830,
    map[0x013c]=0x38181818187e1830,map[0x013d]=0x6666646060607e00,map[0x013e]=0x763634303030fc00,
    map[0x013f]=0x60606c6c60607e00,map[0x0140]=0x703036363030fc00,map[0x0141]=0x606070e060607e00,
    map[0x0142]=0x38181c3818187e00,map[0x0143]=0x0810e6f6fedece00,map[0x0144]=0x0810fcc6c6c6c600,
    map[0x0145]=0xe6f6fedecec61830,map[0x0146]=0x00fcc6c6c6c61830,map[0x0147]=0x6c38e6f6fedece00,
    map[0x0148]=0x6c38fcc6c6c6c600,map[0x0149]=0xc0c0bc3636363600,map[0x014a]=0xe6f6fedecec6063c,
    map[0x014b]=0x00fcc6c6c6c6063c,map[0x014c]=0x7c007cc6c6c67c00,map[0x014d]=0x7c007cc6c6c67c00,
    map[0x014e]=0x44387cc6c6c67c00,map[0x014f]=0x44387cc6c6c67c00,map[0x0150]=0x24487cc6c6c67c00,
    map[0x0151]=0x24487cc6c6c67c00,map[0x0152]=0x7ed8d8ded8d87e00,map[0x0153]=0x00007cd6ded07c00,
    map[0x0154]=0x0810fccef8dcce00,map[0x0155]=0x08106e7060606000,map[0x0156]=0xfcc6cef8dcce1830,
    map[0x0157]=0x006e706060603060,map[0x0158]=0x2810fccef8dcce00,map[0x0159]=0x361c6e7060606000,
    map[0x015a]=0x08107cc07c06fc00,map[0x015b]=0x08107cc07c06fc00,map[0x015c]=0x386c7cc07c06fc00,
    map[0x015d]=0x386c7cc07c06fc00,map[0x015e]=0x7cc07c06c67c0830,map[0x015f]=0x007cc07c06fc0830,
    map[0x0160]=0x6c387cc07c06fc00,map[0x0161]=0x6c387cc07c06fc00,map[0x0162]=0x7e18181818180830,
    map[0x0163]=0x18187e1818180830,map[0x0164]=0x24187e1818181800,map[0x0165]=0x2418187e18181800,
    map[0x0166]=0x7e18181c38181800,map[0x0167]=0x18187e181c381800,map[0x0168]=0x3458c6c6c6c67c00,
    map[0x0169]=0x3458c6c6c6c67e00,map[0x016a]=0x7c00c6c6c6c67c00,map[0x016b]=0x7c00c6c6c6c67e00,
    map[0x016c]=0x4438c6c6c6c67c00,map[0x016d]=0x4438c6c6c6c67e00,map[0x016e]=0x386cbac6c6c67c00,
    map[0x016f]=0x386cbac6c6c67e00,map[0x0170]=0x2448c6c6c6c67c00,map[0x0171]=0x2448c6c6c6c67e00,
    map[0x0172]=0xc6c6c6c67c102018,map[0x0173]=0xc6c6c6c67e102018,map[0x0174]=0x386cc6d6fefeee00,
    map[0x0175]=0x386cb6b6b6b67e00,map[0x0176]=0x182466663c181800,map[0x0177]=0x386cc6c6c67e067c,
    map[0x0178]=0x660066663c181800,map[0x0179]=0x0810fe1c3870fe00,map[0x017a]=0x0810fe1c3870fe00,
    map[0x017b]=0x1800fe1c3870fe00,map[0x017c]=0x1800fe1c3870fe00,map[0x017d]=0x6c38fe1c3870fe00,
    map[0x017e]=0x6c38fe1c3870fe00,map[0x017f]=0x0e18181818181800,map[0x0192]=0x0e187e1818187000,
    map[0x02c6]=0x386c000000000000,map[0x02c7]=0x6c38000000000000,map[0x02c9]=0x7c00000000000000,
    map[0x02ca]=0x0810000000000000,map[0x02cb]=0x1008000000000000,map[0x02d7]=0x0000007c00000000,
    map[0x02d8]=0x4438000000000000,map[0x02d9]=0x1800000000000000,map[0x02da]=0x386c380000000000,
    map[0x02db]=0x0000000000102018,map[0x02dc]=0x3458000000000000,map[0x02dd]=0x2448000000000000,
    map[0x037a]=0x000000000000100c,map[0x037e]=0x0030300030306000,map[0x0384]=0x0810000000000000,
    map[0x0385]=0x4a10000000000000,map[0x0386]=0x58bc66667e666600,map[0x0387]=0x0000003030000000,
    map[0x0388]=0x7fb0303c30303f00,map[0x0389]=0x73b3333f33333300,map[0x038a]=0x5e8c0c0c0c0c1e00,
    map[0x038c]=0x5eb3333333331e00,map[0x038e]=0x73b3331e0c0c0c00,map[0x038f]=0x5cb6636363367700,
    map[0x0390]=0x9420703030301c00,map[0x0391]=0x386cc6c6fec6c600,map[0x0392]=0xfcc6c6fcc6c6fc00,
    map[0x0393]=0x7e60606060606000,map[0x0394]=0x386cc6c6c6c6fe00,map[0x0395]=0xfec0c0fcc0c0fe00,
    map[0x0396]=0xfe0e1c3870e0fe00,map[0x0397]=0xc6c6c6fec6c6c600,map[0x0398]=0x7cc6c6fec6c67c00,
    map[0x0399]=0x7e18181818187e00,map[0x039a]=0xc6ccd8f0f8dcce00,map[0x039b]=0x386cc6c6c6c6c600,
    map[0x039c]=0xc6eefefed6c6c600,map[0x039d]=0xc6e6f6fedecec600,map[0x039e]=0xfe00007c0000fe00,
    map[0x039f]=0x7cc6c6c6c6c67c00,map[0x03a0]=0xfec6c6c6c6c6c600,map[0x03a1]=0xfcc6c6c6fcc0c000,
    map[0x03a3]=0xfe66301c3066fe00,map[0x03a4]=0x7e18181818181800,map[0x03a5]=0x6666663c18181800,
    map[0x03a6]=0x107cd6d6d67c1000,map[0x03a7]=0xc6ee7c387ceec600,map[0x03a8]=0xdbdbdb7e18183c00,
    map[0x03a9]=0x386cc6c6c66cee00,map[0x03aa]=0x66007e1818187e00,map[0x03ab]=0x660066663c181800,
    map[0x03ac]=0x08107ec6c6c67e00,map[0x03ad]=0x08107ec07cc07e00,map[0x03ae]=0x0810dc6666666606,
    map[0x03af]=0x1020703030301c00,map[0x03b0]=0x9420cc6666663c00,map[0x03b1]=0x00007ec6c6c67e00,
    map[0x03b2]=0x7cc6c6fcc6c6fcc0,map[0x03b3]=0x0000c666663c1818,map[0x03b4]=0x0e180c7ec6c67c00,
    map[0x03b5]=0x00007ec07cc07e00,map[0x03b6]=0x7c3060c0c07c060c,map[0x03b7]=0x0000dc6666666606,
    map[0x03b8]=0x386cc6fec66c3800,map[0x03b9]=0x0000703030301c00,map[0x03ba]=0x0000c6ccf8ccc600,
    map[0x03bb]=0xc06030386cc6c600,map[0x03bc]=0x0000ccccccccf6c0,map[0x03bd]=0x0000c666663c1800,
    map[0x03be]=0xfc603860c0780c18,map[0x03bf]=0x00007cc6c6c67c00,map[0x03c0]=0x0000fe6c6c6ce600,
    map[0x03c1]=0x00007cc6c6fcc0c0,map[0x03c2]=0x00007cc0c07c067c,map[0x03c3]=0x00007ed8cccc7800,
    map[0x03c4]=0x00007e1818180e00,map[0x03c5]=0x0000cc6666663c00,map[0x03c6]=0x00004cd6d6d67c10,
    map[0x03c7]=0x0000c66c386cc6c6,map[0x03c8]=0x0018dbdbdbdb7e18,map[0x03c9]=0x000044d6d6d66c00,
    map[0x03ca]=0xd800703030301c00,map[0x03cb]=0x6c00cc6666663c00,map[0x03cc]=0x08107cc6c6c67c00,
    map[0x03cd]=0x0810cc6666663c00,map[0x03ce]=0x081044d6d6d66c00,map[0x0400]=0x2010fec0fcc0fe00,
    map[0x0401]=0x6c00fec0fcc0fe00,map[0x0402]=0xf8607c666666660c,map[0x0403]=0x08107e6060606000,
    map[0x0404]=0x3c66c0fcc0663c00,map[0x0405]=0x78ccc07c06c67c00,map[0x0406]=0x7e18181818187e00,
    map[0x0407]=0x66007e1818187e00,map[0x0408]=0x0606060606c67c00,map[0x0409]=0x3070dcd6d6d6dc00,
    map[0x040a]=0xd0d0dcf6d6d6dc00,map[0x040b]=0xf8607c6666666600,map[0x040c]=0x0810c6ccf8ccc600,
    map[0x040d]=0x2010cedefef6e600,map[0x040e]=0x4438c6c67e06fc00,map[0x040f]=0xc6c6c6c6c6c6fe10,
    map[0x0410]=0x386cc6c6fec6c600,map[0x0411]=0xfcc0c0fcc6c6fc00,map[0x0412]=0xfcc6c6fcc6c6fc00,
    map[0x0413]=0x7e60606060606000,map[0x0414]=0x1e3636666666fec3,map[0x0415]=0xfec0c0fcc0c0fe00,
    map[0x0416]=0xd6d67c387cd6d600,map[0x0417]=0x7cc6063c06c67c00,map[0x0418]=0xc6cedefef6e6c600,
    map[0x0419]=0x4438cedefef6e600,map[0x041a]=0xc6ccd8f0f8dcce00,map[0x041b]=0x1e366666c6c6c600,
    map[0x041c]=0xc6eefefed6c6c600,map[0x041d]=0xc6c6c6fec6c6c600,map[0x041e]=0x7cc6c6c6c6c67c00,
    map[0x041f]=0xfec6c6c6c6c6c600,map[0x0420]=0xfcc6c6c6fcc0c000,map[0x0421]=0x3c66c0c0c0663c00,
    map[0x0422]=0x7e18181818181800,map[0x0423]=0xc6c6c67e06c67c00,map[0x0424]=0x107cd6d6d67c1000,
    map[0x0425]=0xc6ee7c387ceec600,map[0x0426]=0xccccccccccccfe06,map[0x0427]=0xc6c6c67e06060600,
    map[0x0428]=0xd6d6d6d6d6d6fe00,map[0x0429]=0xd6d6d6d6d6d6ff03,map[0x042a]=0xe0607c6666667c00,
    map[0x042b]=0xc6c6f6dededef600,map[0x042c]=0xc0c0fcc6c6c6fc00,map[0x042d]=0x78cc067e06cc7800,
    map[0x042e]=0xcedbdbfbdbdbce00,map[0x042f]=0x7ec6c6e63e76e600,map[0x0430]=0x00007c067ec67e00,
    map[0x0431]=0x7ec0dce6c6c67c00,map[0x0432]=0x0000fcc6fcc6fc00,map[0x0433]=0x00007e6060606000,
    map[0x0434]=0x00007ec6c67e067c,map[0x0435]=0x00007cc6fec07c00,map[0x0436]=0x0000d6d67cd6d600,
    map[0x0437]=0x0000fc067c06fc00,map[0x0438]=0x0000c6ced6e6c600,map[0x0439]=0x4438c6ced6e6c600,
    map[0x043a]=0x0000c6ccf8ccc600,map[0x043b]=0x00003e6666c6c600,map[0x043c]=0x0000c6eed6d6c600,
    map[0x043d]=0x0000c6c6fec6c600,map[0x043e]=0x00007cc6c6c67c00,map[0x043f]=0x0000fec6c6c6c600,
    map[0x0440]=0x0000fcc6c6fcc0c0,map[0x0441]=0x00007ec0c0c07e00,map[0x0442]=0x00007e1818181800,
    map[0x0443]=0x0000c6c6c67e067c,map[0x0444]=0x00107cd6d6d67c10,map[0x0445]=0x0000c6fe38fec600,
    map[0x0446]=0x0000ccccccccfe06,map[0x0447]=0x0000c6c67e060600,map[0x0448]=0x0000d6d6d6d6fe00,
    map[0x0449]=0x0000d6d6d6d6ff03,map[0x044a]=0x0000e0607c667c00,map[0x044b]=0x0000c6c6f6def600,
    map[0x044c]=0x0000c0c0fcc6fc00,map[0x044d]=0x00007cc61ec67c00,map[0x044e]=0x0000cedbfbdbce00,
    map[0x044f]=0x00007ec67ec6c600,map[0x0450]=0x20107cc6fec07c00,map[0x0451]=0x6c007cc6fec07c00,
    map[0x0452]=0x60f8607c6666660c,map[0x0453]=0x08107e6060606000,map[0x0454]=0x00007cc6f0c67c00,
    map[0x0455]=0x00007cc07c06fc00,map[0x0456]=0x1800381818187e00,map[0x0457]=0x6c00381818187e00,
    map[0x0458]=0x0c001c0c0c0c0c78,map[0x0459]=0x0000307cd6d6dc00,map[0x045a]=0x0000d0dcf6d6dc00,
    map[0x045b]=0x60f8607c66666600,map[0x045c]=0x0810c6ccf8ccc600,map[0x045d]=0x2010c6ced6e6c600,
    map[0x045e]=0x4438c6c6c67e067c,map[0x045f]=0x0000c6c6c6c6fe10,map[0x2013]=0x000000fe00000000,
    map[0x2014]=0x000000ff00000000,map[0x2015]=0x000000fe00000000,map[0x2018]=0x1830300000000000,
    map[0x2019]=0x1818300000000000,map[0x201a]=0x0000000018183000,map[0x201c]=0x366c6c0000000000,
    map[0x201d]=0x36366c0000000000,map[0x201e]=0x000000006c6cd800,map[0x2020]=0x18187e1818181800,
    map[0x2021]=0x18187e187e181800,map[0x2022]=0x0000183c3c180000,map[0x2026]=0x0000000000929200,
    map[0x2030]=0x62a4c8102f559e00,map[0x2039]=0x000c1830180c0000,map[0x203a]=0x0060301830600000,
    map[0x2044]=0x0204081020408000,map[0x20ac]=0x1c36f860f8361c00,map[0x20af]=0x6090949a9a9aec08,
    map[0x2116]=0xc4aaaaaaa4a0ae00,map[0x2122]=0x7e2e2a2a00000000,map[0x2190]=0x10307fff7f301000,
    map[0x2191]=0x10387cfe38383838,map[0x2192]=0x080cfefffe0c0800,map[0x2193]=0x38383838fe7c3810,
    map[0x2202]=0x78cc067ec6c67c00,map[0x2206]=0x386cc6c6c6c6fe00,map[0x220f]=0xfec6c6c6c6c6c600,
    map[0x2211]=0xfe66301c3066fe00,map[0x221a]=0x1e181818d8783800,map[0x221e]=0x00006c92926c0000,
    map[0x222b]=0x0c1a181818185830,map[0x2248]=0x0064980064980000,map[0x2260]=0x0204fe10fe408000,
    map[0x2264]=0x0c1830180c003c00,map[0x2265]=0x30180c1830003c00,map[0x25b2]=0x101038387c7cfe00,
    map[0x25b6]=0x80e0f8fef8e08000,map[0x25bc]=0xfe7c7c3838101000,map[0x25c0]=0x020e3efe3e0e0200,
    map[0x25ca]=0x1028448244281000,map[0x2605]=0x1010fe7c386c4400,map[0x2606]=0x1010ee44287c4400,
    map[0x2660]=0x10387cfefe387c00,map[0x2663]=0x1818666618183c00,map[0x2665]=0x6cfefefe7c381000,
    map[0x2666]=0x10387cfe7c381000,map[0x266a]=0x1018141274f06000,map[0xf101]=0x00000000001c7c00,
    map[0xf8ff]=0x0876fdfdfb7e2c00,map[0xfb01]=0x3860fe6666666600,map[0xfb02]=0x3e66fe6666666600, map; }
};
