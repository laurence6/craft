#include "block_manager.hpp"

#include "db.hpp"
#include "player.hpp"

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

void BlockManager::update()
{
    vec3    player_pos = Player::ins().pos;
    ChunkID chunk_id_0 { player_pos.x, player_pos.y };
    for (int32_t dx = -1; dx <= 1; dx++)
    {
        for (int32_t dy = -1; dy <= 1; dy++)
        {
            ChunkID chunk_id = chunk_id_0.add(dx, dy);
            if (get_chunk(chunk_id) == nullptr)
            {
                chunks.emplace(chunk_id, new Chunk(chunk_id));
                set_chunks_need_update(chunk_id);
            }
        }
    }

    if (!chunks_need_update.empty())
    {
        for (auto const& chunk_id : chunks_need_update)
        {
            Chunk* chunk = get_chunk(chunk_id);
            if (chunk != nullptr)
            {
                chunk->update({ {
                    get_chunk(chunk_id.add(-1, 0)),
                    get_chunk(chunk_id.add(1, 0)),
                    get_chunk(chunk_id.add(0, -1)),
                    get_chunk(chunk_id.add(0, 1)),
                } });
            }
        }
        chunks_need_update.clear();
    }
}
