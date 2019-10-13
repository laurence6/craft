#ifndef BLOCK_MANAGER_HPP
#define BLOCK_MANAGER_HPP

#include <unordered_map>

#include "block.hpp"
#include "chunk.hpp"

using namespace std;

class BlockManager : private NonCopy<BlockManager> {
private:
    unordered_map<ChunkID, Chunk*, ChunkID::Hasher>  chunks             {};
    unordered_map<ChunkID, uint8_t, ChunkID::Hasher> chunks_need_update {};

public:
    void init() {
    }

    void add_block(Block* block);

    void del_block(Block*& block);

    Block* get_block(int32_t x, int32_t y, uint8_t z) {
        ChunkID chunk_id { x, y };
        Chunk* chunk = get_chunk(chunk_id);
        if (chunk == nullptr) {
            return nullptr;
        }
        return chunk->get_block(x, y, z);
    }

    void update_vertices();

    void render() const {
        for (auto const& chunk : chunks) {
            chunk.second->render();
        }
    }

private:
    Chunk* get_chunk(ChunkID chunk_id) {
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return chunk->second;
        }
        return nullptr;
    }

    void add_chunks_need_update(ChunkID chunk_id, Block const* block) {
        uint8_t flags = boarder_flags(block);
        add_chunk_need_update(chunk_id, flags);
        if (flags != 0) {
            if ((flags & FACE_LEFT_BIT) != 0) add_chunk_need_update(chunk_id.add(-1, 0), FACE_RIGHT_BIT);
            if ((flags & FACE_RIGHT_BIT) != 0) add_chunk_need_update(chunk_id.add( 1, 0), FACE_LEFT_BIT);
            if ((flags & FACE_FRONT_BIT) != 0) add_chunk_need_update(chunk_id.add(0, -1), FACE_BACK_BIT);
            if ((flags & FACE_BACK_BIT) != 0) add_chunk_need_update(chunk_id.add(0, 1), FACE_FRONT_BIT);
        }
    }

    void add_chunk_need_update(ChunkID chunk_id, uint8_t flags) {
        auto chunk_u_iter = chunks_need_update.find(chunk_id);
        if (chunk_u_iter != chunks_need_update.end()) {
            chunk_u_iter->second |= flags;
        } else {
            chunks_need_update.insert(make_pair(chunk_id, flags));
        }
    }

    static uint8_t boarder_flags(Block const* block) {
        const uint8_t x = static_cast<uint64_t>(block->x) & BLOCK_INDEX_MASK;
        const uint8_t y = static_cast<uint64_t>(block->y) & BLOCK_INDEX_MASK;

        uint8_t flags = 0;

        if (x == 0)             flags |= FACE_LEFT_BIT;
        if (x == CHUNK_WIDTH-1) flags |= FACE_RIGHT_BIT;
        if (y == 0)             flags |= FACE_FRONT_BIT;
        if (y == CHUNK_WIDTH-1) flags |= FACE_BACK_BIT;

        return flags;
    }
};

#endif
