#include <fstream>

#include "config.hpp"
#include "db.hpp"

class DBFile
{
private:
    fstream db;

public:
    DBFile(fstream::openmode mode)
    {
        db.open(DB_PATH, fstream::binary | mode);
    }

    template<typename T>
    void write(T&& p, size_t n = 1)
    {
        db.write(reinterpret_cast<char const*>(&p), sizeof(T) * n);
    }

    template<typename T>
    void read(T& p, size_t n = 1)
    {
        db.read(reinterpret_cast<char*>(&p), sizeof(T) * n);
    }

    fstream* operator->()
    {
        return &db;
    }
};

void DB::init()
{
    DBFile db { fstream::in };

    if (!db->is_open() || db->eof())
        return;

    size_t n_chunks;
    db.read(n_chunks);
    for (size_t i = 0; i < n_chunks; i++)
    {
        ChunkID chunk_id;
        db.read(chunk_id);
        size_t n_blocks;
        db.read(n_blocks);
        auto& chunk = chunks[chunk_id];
        chunk.resize(n_blocks);
        db.read(*chunk.data(), n_blocks);
    }

    vec3 _player_pos;
    db.read(_player_pos);
    player_pos = _player_pos;
}

void DB::shutdown()
{
    DBFile db { fstream::out };

    if (!db->is_open())
        return;

    db.write(chunks.size());
    for (auto const& p : chunks)
    {
        db.write(p.first);
        db.write(p.second.size());
        db.write(*p.second.data(), p.second.size());
    }

    db.write(*player_pos);
}
