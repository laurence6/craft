#include "block_manager.hpp"

void BlockManager::add_block(Block* block) {
    ChunkID chunk_id = ChunkID(block->x, block->y);
    Chunk* chunk = get_chunk(chunk_id);
    if (chunk == nullptr) {
        chunk = new Chunk(chunk_id);
        chunks.insert(make_pair(chunk_id, chunk));
    }

    chunk->add_block(block);

    update_chunks_need_update(chunk_id, block);
}

void BlockManager::del_block(Block*& block) {
    ChunkID chunk_id = ChunkID(block->x, block->y);
    Chunk* chunk = get_chunk(chunk_id);
    if (chunk == nullptr) {
        return;
    }

    chunk->del_block(block);

    update_chunks_need_update(chunk_id, block);

    delete block;
    block = nullptr;
}

void BlockManager::update_vertices() {
    if (!chunks_need_update.empty()) {
        for (auto const& chunk_u : chunks_need_update) {
            ChunkID const& chunk_id = chunk_u.first;
            Chunk* chunk = get_chunk(chunk_id);
            if (chunk != nullptr) {
                chunk->update_vertices(
                    chunk_u.second,
                    {{
                        get_chunk(chunk_id.add(-1, 0)),
                        get_chunk(chunk_id.add( 1, 0)),
                        get_chunk(chunk_id.add( 0,-1)),
                        get_chunk(chunk_id.add( 0, 1)),
                    }}
                );
            }
        }
        chunks_need_update.clear();
    }
}
