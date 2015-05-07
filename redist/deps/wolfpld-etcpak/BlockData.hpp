#ifndef __BLOCKDATA_HPP__
#define __BLOCKDATA_HPP__

#include <future>
#include <memory>
#include <mutex>
#include <vector>

#include "BlockBitmap.hpp"
#include "Bitmap.hpp"
#include "Types.hpp"
#include "Vector.hpp"

class BlockData
{
public:
    BlockData( const v2i& size, bool mipmap );
    ~BlockData();

    void Process( const uint8_t* src, uint32_t blocks, size_t offset, uint quality, Channels type );
    void Finish();

    uint8_t* m_data;
    v2i m_size;
    size_t m_dataOffset;
private:

    BlockBitmapPtr m_bmp;
    bool m_done;
    std::vector<std::future<void>> m_work;
    std::mutex m_lock;
    size_t m_maplen;
};

typedef std::shared_ptr<BlockData> BlockDataPtr;

#endif
