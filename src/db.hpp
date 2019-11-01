#ifndef DB_HPP
#define DB_HPP

#include <optional>
#include <unordered_map>
#include <vector>

#include "chunk.hpp"
#include "math.hpp"
#include "util.hpp"

using namespace std;

class DB : public Singleton<DB> {
public:
    unordered_map<ChunkID, vector<uint32_t>, ChunkID::Hasher> chunks {};

    optional<vec3> player_pos {};

public:
    void init();

    void shutdown();
};

#endif
