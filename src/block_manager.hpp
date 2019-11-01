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
    void init();

    void shutdown();

    void add_block(BlockID const& block_id, BlockData&& block) {
        ChunkID chunk_id { block_id };
        Chunk* chunk = get_chunk(chunk_id);
        if (chunk == nullptr) {
            chunk = new Chunk(chunk_id);
            chunks.emplace(chunk_id, chunk);
        }

        set_chunks_need_update(chunk_id, block_id);

        chunk->add_block(block_id, move(block));
    }

    void del_block(BlockID const& block_id) {
        ChunkID chunk_id { block_id };
        Chunk* chunk = get_chunk(chunk_id);
        if (chunk == nullptr) {
            return;
        }

        set_chunks_need_update(chunk_id, block_id);

        chunk->del_block(block_id);
    }

    BlockData const* get_block(BlockID const& block_id) {
        ChunkID chunk_id { block_id };
        Chunk* chunk = get_chunk(chunk_id);
        if (chunk == nullptr) {
            return nullptr;
        }
        return chunk->get_block(block_id);
    }

    void update(){
        if (!chunks_need_update.empty()) {
            for (auto const& chunk_id : chunks_need_update) {
                Chunk* chunk = get_chunk(chunk_id);
                if (chunk != nullptr) {
                    chunk->update({{
                        get_chunk(chunk_id.add(-1, 0)),
                        get_chunk(chunk_id.add( 1, 0)),
                        get_chunk(chunk_id.add( 0,-1)),
                        get_chunk(chunk_id.add( 0, 1)),
                    }});
                }
            }
            chunks_need_update.clear();
        }
    }

    void render() const {
        for (auto const& chunk : chunks) {
            chunk.second->render();
        }
    }

private:
    Chunk* get_chunk(ChunkID const& chunk_id) {
        auto chunk = chunks.find(chunk_id);
        if (chunk != chunks.end()) {
            return chunk->second;
        }
        return nullptr;
    }

    void set_chunks_need_update(ChunkID const& chunk_id, BlockID const& block_id) {
        chunks_need_update.insert(chunk_id);

        uint64_t x = static_cast<uint64_t>(block_id.x) & BLOCK_INDEX_MASK;
        uint64_t y = static_cast<uint64_t>(block_id.y) & BLOCK_INDEX_MASK;
        if      (x == 0)             chunks_need_update.insert(chunk_id.add(-1, 0));
        else if (x == CHUNK_WIDTH-1) chunks_need_update.insert(chunk_id.add( 1, 0));
        if      (y == 0)             chunks_need_update.insert(chunk_id.add( 0,-1));
        else if (y == CHUNK_WIDTH-1) chunks_need_update.insert(chunk_id.add( 0, 1));
    }
};

#endif
