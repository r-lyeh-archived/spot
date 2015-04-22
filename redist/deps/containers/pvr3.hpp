// - rlyeh, public domain

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
            ss << "unsupported .pvr3 file" << std::endl;
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
