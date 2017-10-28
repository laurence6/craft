#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>
#include <tuple>
#include <vector>

#include <GL/glew.h>

using namespace std;

// 0-------1-------2-------3-------4-------5-------6-------7-------
// |8 z    |28 x mod 0xfffffff         |28 y mod 0xfffffff
inline int64_t hash_block_coord(int64_t x, int64_t y, uint8_t z) noexcept {
    int64_t h = 0;
    h += static_cast<uint64_t>(z) << 56;
    h += (x & 0xfff'ffff) << 28;
    h += (y & 0xfff'ffff) <<  0;
    return h;
}

class Block {
public:
    inline int64_t hash_coord() const noexcept {
        return hash_block_coord(x, y, z);
    }

    const int64_t x;
    const int64_t y;
    const uint8_t z;
    const array<GLfloat, 6> tex;

protected:
    Block(int64_t x, int64_t y, uint8_t z, array<GLfloat, 6> tex) : x(x), y(y), z(z), tex(tex) {}
};

static const array<GLfloat, 6> dirt_block_tex = { 2, 2, 2, 2, 2, 2 };
class DirtBlock : public Block {
public:
    DirtBlock(int64_t x, int64_t y, uint8_t z) : Block(x, y, z, dirt_block_tex) {}
};

static const array<GLfloat, 6> grass_block_tex = { 0, 1, 1, 1, 1, 2 };
class GrassBlock : public Block {
public:
    GrassBlock(int64_t x, int64_t y, uint8_t z) : Block(x, y, z, grass_block_tex) {}
};

static const array<GLfloat, 6> stone_block_tex = { 3, 3, 3, 3, 3, 3 };
class StoneBlock : public Block {
public:
    StoneBlock(int64_t x, int64_t y, uint8_t z) : Block(x, y, z, stone_block_tex) {}
};

#endif
