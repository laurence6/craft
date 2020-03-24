#include "block_manager.hpp"

#include "db.hpp"

void BlockManager::init()
{
    for (auto const& p : DB::ins().chunks)
    {
        ChunkID const& chunk_id = p.first;
        chunks.emplace(chunk_id, new Chunk(chunk_id));
        chunks_need_update.insert(chunk_id);
    }
}

void BlockManager::shutdown()
{
    for (auto& p : chunks)
    {
        delete p.second;
    }
    chunks.clear();
    chunks_need_update.clear();
}
