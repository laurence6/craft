#ifndef BLOCK_MANAGER_HPP
#define BLOCK_MANAGER_HPP

#include <unordered_map>
#include <unordered_set>

#include "block.hpp"
#include "chunk.hpp"
#include "util.hpp"

using namespace std;

class BlockManager : private NonCopy<BlockManager> {
private:
    unordered_map<ChunkID, Chunk*, ChunkID::Hasher> chunks {};
    unordered_set<ChunkID, ChunkID::Hasher>         chunks_need_update {};

public:
    void init() {
    }

    void shutdown() {
        for (auto& p : chunks) {
            Chunk::unload(p.second);
        }
        chunks.clear();
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

    void update();

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

    void set_chunks_need_update(ChunkID chunk_id, Block const* block);
};

#endif
