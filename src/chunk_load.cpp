#include <unordered_map>

#include "chunk.hpp"

static unordered_map<ChunkID, vector<uint32_t>, ChunkID::Hasher> db;

// 32 bits:
//    x :  4,
//    y :  4,
//    z :  8,
//   id : 10,
//      :  6, (currently unused)
static uint32_t marshal(uint16_t x, uint16_t y, uint16_t z, BlockData const& block) {
    uint32_t v = 0;
    v |= (static_cast<uint32_t>(x) & BLOCK_INDEX_MASK) << 28u;
    v |= (static_cast<uint32_t>(y) & BLOCK_INDEX_MASK) << 24u;
    v |=  static_cast<uint32_t>(z) << 16u;
    v |=  static_cast<uint32_t>(block.type) << 6u;
    return v;
}

static tuple<BlockID, BlockData> unmarshal(ChunkID const& chunk_id, uint32_t block) {
    constexpr uint32_t X_MASK  = 0xf000'0000;
    constexpr uint32_t Y_MASK  = 0x0f00'0000;
    constexpr uint32_t Z_MASK  = 0x00ff'0000;
    constexpr uint32_t TYPE_MASK = 0x0000'ffc0;

    auto x = static_cast<int32_t>(((block & X_MASK) >> 28u) | chunk_id.x);
    auto y = static_cast<int32_t>(((block & Y_MASK) >> 24u) | chunk_id.y);
    auto z = static_cast<uint8_t>((block & Z_MASK) >> 16u);
    auto type = static_cast<uint16_t>((block & TYPE_MASK) >> 6u);

    return { BlockID { x, y, z }, BlockData { type } };
}

Chunk::Chunk(ChunkID const& chunk_id) : chunk_id(chunk_id) {
    auto it = db.find(chunk_id);
    if (it != db.end()) {
        for (uint32_t const& b : it->second) {
            auto [block_id, block] = unmarshal(chunk_id, b);
            add_block(block_id, move(block));
        }
    }
}

Chunk::~Chunk() {
    vector<uint32_t> chunk_data {};

    for (uint16_t x = 0; x < CHUNK_WIDTH; x++) {
        for (uint16_t y = 0; y < CHUNK_WIDTH; y++) {
            for (uint16_t z = 0; z < 256; z++) {
                if (auto const& block = blocks[x][y][z]; !block.is_null()) {
                    chunk_data.push_back(marshal(x, y, z, block));
                }
            }
        }
    }

    db.insert_or_assign(chunk_id, chunk_data);
}
