Sp◉t <a href="https://travis-ci.org/r-lyeh/spot"><img src="https://api.travis-ci.org/r-lyeh/spot.svg?branch=master" align="right" /></a>
=====

- Spot is a compact and embeddable pixel/image library (C++11).
- Spot supports both RGBA/HSLA pixel types.
- Spot provides both pixel and image algebra for pixel/image manipulation.
- Spot loads WebP, JPG, progressive JPG, PNG, TGA, DDS DXT1/2/3/4/5, BMP, PSD, GIF, PKM (ETC1), PVR (PVRTC), HDR, PIC, PNM (PPM/PGM), CRN, PUG and vectorial SVG files.
- Spot saves WebP, JPG, PNG, TGA, BMP, PUG and DDS files.
- Spot is self-contained. All libraries are included and amalgamated.
- Spot is tiny. A couple of source files.
- Spot is cross-platform.
- Spot is BOOST licensed.

## File format support

| File format  | Read | Write |
| :-------------|:-------------:| :-----:|
| BMP files | yes | yes |
| CRN files | yes | no |
| DDS DXT1/2/3/4/5 files | yes | yes |
| GIF files | yes | no |
| HDR files | yes | no |
| JPG files (progressive) | yes | no |
| JPG files | yes | yes |
| PIC files | yes | no |
| PKM (ETC1) files | yes | no |
| PNG files | yes | yes |
| PNM (PPM/PGM) files | yes | no |
| PSD files | yes | no |
| PUG files | yes | yes |
| PVR (PVRTC) files | yes | no |
| SVG files (rasterized) | yes | no |
| TGA files | yes | yes |
| WebP files | yes | yes |

## todo
- document api
- create more image filters
- clean up code

```c++
template<typename color>
class image : public std::vector<color>
{
    public:

    size_t w, h;
    float delay; // frame delay, when loading an animation

    image();
    image( size_t _w, size_t _h, const color &filler = color::hsla() );
    explicit image( const std::string &filename, bool mirror_w = false, bool mirror_h = false );
    explicit image( const void *ptr, size_t len, bool mirror_w = false, bool mirror_h = false );

    bool load( const void *ptr, size_t len, bool mirror_w = false, bool mirror_h = false );
    bool load( const std::string &filename, bool mirror_w = false, bool mirror_h = false );

    bool save_as_bmp( const std::string &filename ) const;
    bool save_as_dds( const std::string &filename ) const;
    bool save_as_tga( const std::string &filename ) const;
    bool save_as_png( const std::string &filename, unsigned stride = 4 ) const;
    bool save_as_pug( const std::string &filename, unsigned quality = 90 ) const;
    bool save_as_jpg( const std::string &filename, unsigned quality = 90 ) const;
    bool save_as_webp( const std::string &filename, unsigned quality = 90 ) const;

    std::string encode_as_png( unsigned stride = 4 ) const;
    std::string encode_as_pug( unsigned quality = 90 ) const;
    std::string encode_as_jpg( unsigned quality = 90 ) const;
    std::string encode_as_webp( unsigned quality = 90 ) const;

    inline const size_t size() const { return this->std::vector<color>::size(); }
    bool loaded() const { return this->std::vector<color>::size() != 0; }

    inline color &at( size_t offset ) { return this->std::vector<color>::at( offset ); }
    inline color &at( size_t x, size_t y ) { return this->std::vector<color>::at( x + y * w ); }
    inline color &atf( float x01, float y01 ) { return this->at( x01 * (w-1), y01 * (h-1) ); }

    inline const color &at( size_t offset ) const { return this->std::vector<color>::at( offset ); }
    inline const color &at( size_t x, size_t y ) const { return this->std::vector<color>::at( x + y * w ); }
    inline const color &atf( float x01, float y01 ) const { return this->at( x01 * (w-1), y01 * (h-1) ); }

    image copy( size_t ox, size_t oy, size_t w = ~0, size_t h = ~0 ) const;
    image paste( size_t at_x, size_t at_y, const image &other ) const;
    image crop( size_t left, size_t right, size_t top, size_t bottom ) const; // each param is number of row/cols to crop

    image flip_w() const;
    image flip_h() const;
    image rotate_left() const;
    image rotate_right() const;

    image operator *( const color &other ) const;
    image operator /( const color &other ) const;
    image operator +( const color &other ) const;
    image operator -( const color &other ) const;

    image &operator *=( const color &other );
    image &operator /=( const color &other );
    image &operator +=( const color &other );
    image &operator -=( const color &other );

    image to_rgba() const;
    image to_hsla() const;

    image to_premultiplied_alpha() const;
    image to_straight_alpha() const;

    std::vector<unsigned int>  rgba_data_32() const;
    std::vector<unsigned char> rgba_data() const;
    std::vector<unsigned char> rgb_data() const;
    std::vector<unsigned char> ya_data() const;
    std::vector<unsigned char> y_data() const;
};
```

## sample
```c++
    spot::image img( filename );

    spot::image saturated = img * spot::hsla( 1, 2, 1, 1 );
    spot::image desaturated = img * spot::hsla( 1, 0.5f, 1, 1 );
    spot::image gray = img * spot::hsla( 1, 0, 1, 1 );
    spot::image light = img * spot::hsla( 1, 1, 1.75f, 1 );
    spot::image hue = img + spot::hsla( 0.5f, 0, 0, 0 );
    spot::image warmer = img + spot::hsla( -0.08f, 0, 0, 0 );
    spot::image colder = img + spot::hsla(  0.08f, 0, 0, 0 );

    gray.save_as_png ( "output.png" );
    gray.save_as_jpg ( "output.jpg", 85 );
    gray.save_as_pug ( "output.pug", 85 );
    gray.save_as_webp( "output.webp", 85 );
```

![image](https://raw.github.com/r-lyeh/depot/master/spot_collage.jpg)

## color transfer sample
![image](https://raw.github.com/r-lyeh/depot/master/spot_color_transfer.jpg)

## licenses
- [spot](https://github.com/r-lyeh/spot) (BOOST licensed).
- [pug](https://github.com/r-lyeh/pug) (Public Domain).
- [soil2](https://bitbucket.org/SpartanJ/soil2/) by Martin Lucas Golini and Jonathan Dummer (Public Domain).
- [stb_image](http://github.com/nothings/stb) by Sean Barrett (Public Domain).
- [DDS writer](http://www.lonesock.net/soil.html) by Jonathan Dummer (Public Domain).
- [jpge](https://code.google.com/p/jpeg-compressor/) by Rich Geldreich (Public Domain).
- [nanosvg](https://github.com/memononen/nanosvg/) by Mikko Mononen (ZLIB license).
- [lodepng](http://lodev.org/lodepng/) by Lode Vandevenne (ZLIB license).
- [libwebp](https://code.google.com/p/webp/) by Google Inc (BSD license).
- [pngrim](https://github.com/fgenesis/pngrim) alpha bleeding algorithm by F-Genesis (Public Domain).
- [crnlib](https://code.google.com/p/crunch/), by Rich Geldreich (ZLIB license).
- [crn2dds](redist/deps/crn2dds) by r-lyeh, SpartanJ and Evan Parker (Public Domain).

## note
- Samples are Public Domain licensed. Samples use CImg.h by David Tschumperle (CeCILL-C license) to display images. 
- gcc users may need strict aliasing disabled if using CRN textures: add `-fno-strict-aliasing` compilation flag.
