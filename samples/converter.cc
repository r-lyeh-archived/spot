// quick image converter.
// r-lyeh, public domain.

#include <algorithm>
#include <deque>
#include <iostream>
#include <string>
#include <vector>
#include "spot.hpp"
#include "cimg.hpp"

#define VERSION "1.0.6"

// app options
unsigned quality = 90;
std::string folder, hint = "png", ext;
bool unique_extension = false;
bool generate_mipmaps = false;
bool preview = false;
bool verbose = false;

// app formats
const std::vector<std::string> readable( spot::list_supported_inputs() );
const std::vector<std::string> writeable( spot::list_supported_outputs() );

// mipmap tools
spot::image &halve( spot::image &img, unsigned factor = 2 ) {
    if( factor >= 2 ) {
        for( unsigned y = 0, h = img.h / factor; y < h; ++y ) {
            for( unsigned x = 0, w = img.w / factor; x < w; ++x ) {
                img[x + y * w] = img[x * factor + y * factor * img.w];
            }
        }
        img.w /= factor;
        img.h /= factor;
        img.resize( img.w * img.h );
    }
    return img;
}

spot::image build_mipmaps( spot::image img, unsigned maxlevels = 0 ) {
    auto edge = img.w, bottom = img.h, left = edge - edge, top = bottom - bottom;
    spot::image cpy( edge + edge / 2, bottom );
    cpy = cpy.paste( left, top, img );
    do {
        img = halve( img );
        cpy = cpy.paste( edge, top, img );
        top += img.h;
    } while( img.w > 1 && img.h > 1 );
    return cpy;
}

// usage
int help( int argc, const char **argv ) {
    std::cout << argv[0] << " v" VERSION " (libspot v" SPOT_VERSION ") - quick image converter. https://github.com/r-lyeh/spot" << std::endl << std::endl;

    std::cout << "syntax:   " << argv[0] << " ext image.file [...] [options]" << std::endl << std::endl;

    std::cout << "options:  " << std::endl;
    std::cout << "          -h, --help                       this help" << std::endl;
    std::cout << "          -m, --mipmaps                    generate mipmaps" << std::endl;
    std::cout << "          -p, --preview                    preview conversion" << std::endl;
    std::cout << "          -u, --unique-extension           do not append new extension in output basename (default: disabled)" << std::endl;
    std::cout << "          -v, --verbose                    print loading/encoding time and image stats" << std::endl;
    std::cout << "          -f ext, --force-extension ext    use provided extension in output basename despite conversion format (default: disabled)" << std::endl;
    std::cout << "          -o folder, --output folder       set folder of output files (default: input image folder)" << std::endl;
    std::cout << "          -q number, --quality number      applies to lossy format images (default: 90, range: 0..100)" << std::endl << std::endl;

    std::cout << "inputs:   ";
    for( std::vector<std::string>::const_iterator it = readable.begin(), end = readable.end(); it != end; ++it ) {
        std::cout << *it << ", ";
    }
    std::cout << std::endl;
    std::cout << "outputs:  ";
    for( std::vector<std::string>::const_iterator it = writeable.begin(), end = writeable.end(); it != end; ++it ) {
        std::cout << *it << ", ";
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "examples:" << std::endl << std::endl;

    std::cout << argv[0] << " png image1.tga" << std::endl;
    std::cout << "[ OK ] image1.tga -> image1.tga.png" << std::endl << std::endl;

    std::cout << argv[0] << " png image1.tga image2.jpg image3.webp --unique-extension" << std::endl;
    std::cout << "[ OK ] image1.tga -> image1.png" << std::endl;
    std::cout << "[ OK ] image2.jpg -> image2.png" << std::endl;
    std::cout << "[ OK ] image3.webp -> image3.png" << std::endl << std::endl;

    std::cout << argv[0] << " jpg *.pkr image2.hdr image3.pvr --output my/path --force-extension img --quality 90" << std::endl;
    std::cout << "[ OK ] image1.pkr -> my/path/image1.img (as jpg) (quality 90)" << std::endl;
    std::cout << "[ OK ] image2.hdr -> my/path/image2.img (as jpg) (quality 90)" << std::endl;
    std::cout << "[ OK ] image3.pvr -> my/path/image3.img (as jpg) (quality 90)" << std::endl << std::endl;

    return -1;
}

// app core
bool convert( const std::string &in, const std::string &out, const std::string &hint ) {
    spot::image img;

    double loading = bench( [&]{ 
        img = spot::image( in );
    } );

    if( !img.loaded() ) {
        return false;
    }

    spot::image backup = img;

    double mipmaps = !generate_mipmaps ? 0 : bench( [&]{ 
        img = build_mipmaps( img ); 
    } );

    bool ok = false;
    double encoding = bench( [&] {
        /**/ if( hint == "png" ) {
            ok = img.save_as_png( out );
        }
        else if( hint == "jpg" ) {
            ok = img.save_as_jpg( out, quality );
        }
        else if( hint == "pug" ) {
            ok = img.save_as_pug( out, quality );
        }
        else if( hint == "bmp" ) {
            ok = img.save_as_bmp( out );
        }
        else if( hint == "tga" ) {
            ok = img.save_as_tga( out );
        }
        else if( hint == "dds" ) {
            ok = img.save_as_dds( out );
        }
        else if( hint == "webp" ) {
            ok = img.save_as_webp( out, quality );
        }
        else if( hint == "ktx" ) {
            ok = img.save_as_ktx( out, quality );
        }
        else if( hint == "pvr" ) {
            ok = img.save_as_pvr( out, quality );
        }
        else if( hint == "pkm" ) {
            ok = img.save_as_pkm( out, quality );
        }
    } );

    if( verbose ) {
        std::cout << " (loading: " << int(loading*1000) << " ms) (build-mipmaps: " << int(mipmaps*1000) << " ms) (encoding: " << int(encoding*1000) << " ms)";
    }

    if( ok && preview ) {
        img = spot::image( out );
        if( img.loaded() ) {
            display( concat(backup, img, diff(backup, img)), in );
        }
    }

    return ok;
}

// app interface
int main( int argc, const char **argv ) {

#   ifdef _WIN32
    char slash = '\\';
#   else
    char slash = '/';
#   endif

    std::deque< std::string > list;
    
    for( int i = 1; i < argc; ++i ) {
        if( argv[i][0] != '-' ) {
            list.push_back( argv[i] );
        } else {
            std::string arg = argv[i];
            bool has_second_arg = (i + 1 < argc);

            /**/ if( arg == "-h" || arg == "--help" ) return help( argc, argv );
            else if( arg == "-m" || arg == "--mipmaps" ) generate_mipmaps = true;
            else if( arg == "-p" || arg == "--preview" ) preview = true;
            else if( arg == "-v" || arg == "--verbose" ) verbose = true;
            else if( arg == "-u" || arg == "--unique-extension" ) unique_extension = true;
            else if((arg == "-f" || arg == "--force-extension" ) && has_second_arg ) ext = argv[++i];
            else if((arg == "-o" || arg == "--output" ) && has_second_arg ) folder = argv[++i];
            else if((arg == "-q" || arg == "--quality" ) && has_second_arg ) quality = strtoul( argv[++i], 0, 0 );
            else {
                std::cerr << "[WARN] option ignored: " << argv[i] << std::endl;
            }
        }
    }

    if( list.size() < 2 ) {
        return help( argc, argv );
    } else {
        hint = list.front();
        list.pop_front();        
    }

    if( std::find(writeable.begin(), writeable.end(), hint) == writeable.end() ) {
        std::cout << "[FAIL] unsupported '" << hint << "' file format." << std::endl;
        return -1;
    }

    if( quality > 100 ) {
        quality = 100;
    }

    auto leftof = []( const std::string &input, char ch ) -> std::string {
        auto left = input.substr( 0, input.find_last_of(ch) );
        return input != left ? left : std::string();
    };
    auto rightof = []( const std::string &input, char ch ) -> std::string {
        auto right = input.substr( input.find_last_of(ch) + 1 );
        return input != right ? right : right;
    };

    for( auto &file : list ) {
        std::string out = file;

        if( !folder.empty() ) {
            out = folder + slash + rightof(file, slash);
        }

        if( unique_extension ) {
            out = leftof(out, '.') + '.' + (ext.empty() ? hint : ext);
        } else {
            out = out + "." + (ext.empty() ? hint : ext);
        }

        std::cout << "[    ] " << file << " -> " << out;
        bool ok = convert( file, out, hint );
        if( ok ) {
            if( !ext.empty() ) {
                std::cout << " (as " << hint << ")";
            }
            if( hint == "webp" || hint == "pug" || hint == "jpg" ) {
                std::cout << " (quality " << quality << ")";
            }
            if( generate_mipmaps ) {
                std::cout << " (mipmaps)";
            }
        }
        std::cout << "\r" << (ok ? "[ OK ]" : "[FAIL]") << "\n";
    }

    return 0;
}
