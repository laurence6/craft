#include "block_manager.hpp"

void BlockManager::add_block(Block* block) {
    ChunkID chunk_id = ChunkID(block->x, block->y);
    Chunk* chunk = get_chunk(chunk_id);
    if (chunk == nullptr) {
        chunk = new Chunk(chunk_id);
        chunks.insert(make_pair(chunk_id, chunk));
    }

    chunk->add_block(block);

    set_chunks_need_update(chunk_id, block);
}

void BlockManager::del_block(Block*& block) {
    ChunkID chunk_id = ChunkID(block->x, block->y);
    Chunk* chunk = get_chunk(chunk_id);
    if (chunk == nullptr) {
        return;
    }

    chunk->del_block(block);

    set_chunks_need_update(chunk_id, block);

    delete block;
    block = nullptr;
}

void BlockManager::set_chunks_need_update(ChunkID chunk_id, Block const* block) {
    chunks_need_update.insert(chunk_id);

    uint64_t x = static_cast<uint64_t>(block->x) & BLOCK_INDEX_MASK;
    uint64_t y = static_cast<uint64_t>(block->y) & BLOCK_INDEX_MASK;
    if      (x == 0)             chunks_need_update.insert(chunk_id.add(-1, 0));
    else if (x == CHUNK_WIDTH-1) chunks_need_update.insert(chunk_id.add( 1, 0));
    if      (y == 0)             chunks_need_update.insert(chunk_id.add( 0,-1));
    else if (y == CHUNK_WIDTH-1) chunks_need_update.insert(chunk_id.add( 0, 1));
}

void BlockManager::update_vertices() {
    if (!chunks_need_update.empty()) {
        for (auto const& chunk_id : chunks_need_update) {
            Chunk* chunk = get_chunk(chunk_id);
            if (chunk != nullptr) {
                chunk->update_vertices({{
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
