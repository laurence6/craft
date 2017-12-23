#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <cmath>

#include "block.hpp"
#include "math.hpp"

using namespace std;

class Collision {
public:
    Collision(bool _found, bool _grounded) : found(_found), grounded(_grounded) {}

    const bool found;
    const bool grounded;
};

// Cylinder collider.
class Collider {
public:
    Collider(float _radius, float _height_u, float _height_l) : radius(_radius), height_u(_height_u), height_l(_height_l) {}

    Collision collide(const vec3& pos, BlockManager& blocks) {
        int32_t min_x = static_cast<int32_t>(round((pos.x - radius) * 100.f));
        int32_t max_x = static_cast<int32_t>(round((pos.x + radius) * 100.f));
        int32_t min_y = static_cast<int32_t>(round((pos.y - radius) * 100.f));
        int32_t max_y = static_cast<int32_t>(round((pos.y + radius) * 100.f));
        uint8_t min_z = static_cast<uint8_t>(round((pos.z - height_l) * 100.f));
        uint8_t max_z = static_cast<uint8_t>(round((pos.z + height_u) * 100.f));

        bool found    = false;
        bool grounded = false;
        for (uint8_t z = min_z; z <= max_z; z++) {
            for (int32_t x = min_x; x <= max_x; x++) {
                for (int32_t y = min_y; y <= max_y; y++) {
                    Block* b = blocks.get_block(x, y, z);
                    if (b != nullptr) {
                        if (z == min_z) {
                            grounded = true;
                        } else {
                            found = true;
                        }
                    }
                }
            }
        }
        return Collision(found, grounded);
    }

private:
    const float radius;
    const float height_u; // upper part height
    const float height_l; // lower part height
};

#endif
