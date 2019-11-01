#include "block_manager.hpp"

void BlockManager::update() {
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

void BlockManager::set_chunks_need_update(ChunkID const& chunk_id, BlockID const& block_id) {
    chunks_need_update.insert(chunk_id);

    uint64_t _x = static_cast<uint64_t>(block_id.x) & BLOCK_INDEX_MASK;
    uint64_t _y = static_cast<uint64_t>(block_id.y) & BLOCK_INDEX_MASK;
    if      (_x == 0)             chunks_need_update.insert(chunk_id.add(-1, 0));
    else if (_x == CHUNK_WIDTH-1) chunks_need_update.insert(chunk_id.add( 1, 0));
    if      (_y == 0)             chunks_need_update.insert(chunk_id.add( 0,-1));
    else if (_y == CHUNK_WIDTH-1) chunks_need_update.insert(chunk_id.add( 0, 1));
}
