#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <array>
#include <limits>
#include <tuple>
#include <vector>

#include <GL/glew.h>

#include "util.hpp"

using namespace std;

using BlockCoord = tuple<int64_t, int64_t, uint8_t>;

// int64:
// ----------------------------------------------------------------
// |28 x                       |28 y                       |8 z
//
// int32:
// --------------------------------
// |12 x       |12 y       |8 z
struct BlockCoordHasher {
    inline size_t operator()(const BlockCoord& block_coord) const noexcept {
        constexpr size_t sizet_width = numeric_limits<size_t>::digits;

        int64_t x = get<0>(block_coord);
        int64_t y = get<1>(block_coord);
        uint8_t z = get<2>(block_coord);

        size_t h = 0;
        if (sizet_width == 64) {
            h += (x & 0xfff'ffff) << 36;
            h += (y & 0xfff'ffff) << 8;
            h += z;
        } else if (sizet_width == 32) {
            h += (x & 0xfff) << 20;
            h += (y & 0xfff) << 8;
            h += z;
        } else {
            _exit(1);
        }

        return h;
    }
};

class Block {
public:
    const BlockCoord coord;
    const array<GLfloat, 6> tex;

protected:
    Block(int64_t x, int64_t y, uint8_t z, array<GLfloat, 6> tex) : coord(make_tuple(x, y, z)), tex(tex) {}
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
