// - rlyeh, public domain

struct pkm {
    struct header {
        uint32_t id;                // "PKM "
        uint16_t version;           // "10"
        uint16_t type;              // 0: ETC1_RGB_NO_MIPMAPS
        uint16_t width;             // after %4; big endian
        uint16_t height;            // after %4; big endian
        uint16_t width_src;         // original width; big endian
        uint16_t height_src;        // original height; big endian
    } hd;
    struct texturedata {
        uint32_t data;              // dummy field [raw binary data here]
    } tx;

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
