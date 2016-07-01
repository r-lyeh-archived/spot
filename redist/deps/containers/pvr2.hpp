// - rlyeh, public domain

struct pvr2 {
    enum table1 {
        MGLPT_ARGB_4444 = 0x00,
        MGLPT_ARGB_1555,
        MGLPT_RGB_565,
        MGLPT_RGB_555,
        MGLPT_RGB_888,
        MGLPT_ARGB_8888,
        MGLPT_ARGB_8332,
        MGLPT_I_8,
        MGLPT_AI_88,
        MGLPT_1_BPP,
        MGLPT_VY1UY0,
        MGLPT_Y1VY0U,
        MGLPT_PVRTC2,
        MGLPT_PVRTC4,
        MGLPT_PVRTC2_2,
        MGLPT_PVRTC2_4,
        
        OGL_RGBA_4444 = 0x10,
        OGL_RGBA_5551,
        OGL_RGBA_8888,
        OGL_RGB_565,
        OGL_RGB_555,
        OGL_RGB_888,
        OGL_I_8,
        OGL_AI_88,
        OGL_PVRTC2,
        OGL_PVRTC4,
    };

    enum table2 {
        MIPMAPS_ARE_PRESENT = 0x00000100,
        DATA_IS_TWIDDLED = 0x00000200,
        CONTAINS_NORMAL_DATA = 0x00000400,
        HAS_A_BORDER = 0x00000800,
        IS_A_CUBE_MAP = 0x00001000, // (EVERY_6_SURFACES_MAKE_UP_ONE_CUBE_MAP)
        MIPMAPS_HAVE_DEBUG_COLOURING = 0x00002000,
        IS_A_VOLUME_3D_TEXTURE = 0x00004000, //(NUMSURFACES_IS_INTERPRETED_AS_A_DEPTH_VALUE)
        ALPHA_CHANNEL_DATA_IS_PRESENT = 0x00008000, // (PVRTC_ONLY)
        TEXTURE_DATA_IS_VERTICALLY_FLIPPED = 0x00010000,
    };
#pragma pack(push,1)
    struct header {
        uint32_t version;           // version and header size: 44 for v1, 52 for v2
        uint32_t height;            // 1d texture
        uint32_t width;             // 2d texture; >= 1

        uint32_t num_mipmaps;       // num levels of mipmaps, excluding the top level (if >= 1, mip map flag should be set)
        uint8_t  pixel_format;      // see table1 above
        uint8_t  flags[3];          // 0x02, colour values within the texture have been pre-multiplied by the alpha values

        uint32_t surface_size;
        uint32_t bpp;               //

        uint32_t r_mask;            // see table2 above
        uint32_t g_mask;            // see table2 above
        uint32_t b_mask;            // see table2 above
        uint32_t a_mask;            // see table2 above

        uint32_t magic;             // 3d texture; >= 1
        uint32_t num_surfaces;      // num surfaces in texture array; >= 1
    } hd;
#pragma pack(pop)

    bool is_pvr() const {
        const char pvr1[] = { 44, 0, 0, 0 };
        const char pvr2[] = { 52, 0, 0, 0 };
        return 0 == memcmp( pvr1, &hd, sizeof(pvr1) ) || 0 == memcmp( pvr2, &hd, sizeof(pvr2) );
    }

    bool is_currently_supported() const {
        bool ok = true;
        ok &= is_pvr(); 
        return ok;
    }

    int get_spot_fmt() const {
        /*
        switch( hd.pixel_format_1 ) {
            case RGB_888: 
            case RGBA_8888: 
            case PVRTC2: return pvr3::table1::PVRTC_2BPP_RGB;
            case PVRTC4: return pvr3::table1::PVRTC_4BPP_RGB;
            case ETC:
        }
        */
        return -1;
    }

    std::ostream &debug( std::ostream &ss ) const {
        if( !is_pvr() ) {
            ss << "not a .pvr2 header" << std::endl;
        } else {
            ss << "supported .pvr2 file: " << is_currently_supported() << std::endl;
            ss << std::endl;
        }
        return ss;
    }
};
