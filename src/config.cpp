#include "config.hpp"

array<BlockConfig, 5> block_config { {
    {},
    { true, true, grass_block_tex },
    { true, true, dirt_block_tex },
    { true, true, stone_block_tex },
    { false, false, { { grass_tex } } },
} };
