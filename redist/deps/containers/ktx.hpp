// - rlyeh, public domain

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
        const char id[] = { char(0xAB), 'K', 'T', 'X', ' ', '1', '1', char(0xBB), '\r', '\n', '\x1A', '\n' };
        return 0 == memcmp( id, &hd, sizeof(id) );
    }

    bool is_currently_supported() const {
        bool ok = true;
        ok &= is_ktx();
        ok &= hd.pixelDepth <= 1;
//        ok &= hd.bytesOfKeyValueData == 0;
//        ok &= hd.numberOfMipmapLevels <= 1;
        ok &= hd.numberOfArrayElements <= 1;
        ok &= hd.numberOfFaces <= 1;
        ok &= get_spot_fmt() != -1;
        return ok;
    }

    int get_spot_fmt() const {
        if( hd.glInternalFormat == 0x8d64 && hd.glBaseInternalFormat == 0x1907 ) return pvr3::table1::ETC1;            // etc1
        if( hd.glInternalFormat == 0x8c01 && hd.glBaseInternalFormat == 0x1907 ) return pvr3::table1::PVRTC_2BPP_RGB;  // pvrtc-2bpp-rgb
        if( hd.glInternalFormat == 0x8c03 && hd.glBaseInternalFormat == 0x1908 ) return pvr3::table1::PVRTC_2BPP_RGBA; // pvrtc-2bpp-rgba
        if( hd.glInternalFormat == 0x8c00 && hd.glBaseInternalFormat == 0x1907 ) return pvr3::table1::PVRTC_4BPP_RGB;  // pvrtc-4bpp-rgb
        if( hd.glInternalFormat == 0x8c02 && hd.glBaseInternalFormat == 0x1908 ) return pvr3::table1::PVRTC_4BPP_RGBA; // pvrtc-4bpp-rgba
        return -1;
    }

    std::ostream &debug( std::ostream &ss ) const {
        if( !is_ktx() ) {
            ss << "not a .ktx header" << std::endl;
        } else {
            ss << "supported .ktx file: " << is_currently_supported() << std::endl;                
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
