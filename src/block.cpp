#include <iostream>

#include "block.hpp"
#include "util.hpp"

using namespace std;

#define BLOCKS       \
    X(1, GrassBlock) \
    X(2, DirtBlock)  \
    X(3, StoneBlock) \
    X(4, Grass)      \

#define X(ID, BLOCK) uint16_t BLOCK::id() const { return ID; }
    BLOCKS
#undef X

Block* new_block(uint16_t id, int32_t x, int32_t y, uint8_t z) {
    switch (id) {
#define X(ID, BLOCK) case ID: return new BLOCK(x, y, z);
        BLOCKS
#undef X
        default:
            cerr << "Undefined block id " << id << endl;
            _exit(1);
            return nullptr;
    }
}
