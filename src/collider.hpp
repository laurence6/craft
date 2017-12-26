#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <algorithm>
#include <cmath>

#include "block.hpp"
#include "math.hpp"

using namespace std;

class Collision {
public:
    Collision(bool _found, float _grounded) : found(_found), grounded(_grounded) {}

    bool is_grounded() const {
        return !isnan(grounded);
    }

    const bool found;
    const float grounded;
};

// Cylinder collider.
class Collider {
public:
    Collider(float _radius, float _height_u, float _height_l) : radius(_radius), height_u(_height_u), height_l(_height_l) {}

    Collision collide(const vec3& pos, BlockManager& blocks) {
        float min_x = pos.x - radius;
        float max_x = pos.x + radius;
        float min_y = pos.y - radius;
        float max_y = pos.y + radius;
        float min_z = pos.z - height_l;
        float max_z = pos.z + height_u;
        int32_t min_x_b = static_cast<int32_t>(floor(min_x * 100.f + 0.001f));
        int32_t max_x_b = static_cast<int32_t>(floor(max_x * 100.f - 0.001f));
        int32_t min_y_b = static_cast<int32_t>(floor(min_y * 100.f + 0.001f));
        int32_t max_y_b = static_cast<int32_t>(floor(max_y * 100.f - 0.001f));
        uint8_t min_z_b = static_cast<uint8_t>(floor(min_z * 100.f + 0.001f));
        uint8_t max_z_b = static_cast<uint8_t>(floor(max_z * 100.f - 0.001f));

        bool found = false;
        for (int32_t z = min_z_b; z <= max_z_b; z++) {
            for (int32_t x = min_x_b; x <= max_x_b; x++) {
                for (int32_t y = min_y_b; y <= max_y_b; y++) {
                    Block* b = blocks.get_block(x, y, z);
                    if (b != nullptr) {
                        found = true;
                        break;
                    }
                }
            }
        }

        int32_t highest_z_b = 0;
        for (int32_t z = min_z_b; z > 0; z--) {
            for (int32_t x = min_x_b; x <= max_x_b; x++) {
                for (int32_t y = min_y_b; y <= max_y_b; y++) {
                    Block* b = blocks.get_block(x, y, z);
                    if (b != nullptr) {
                        highest_z_b = max(highest_z_b, z + 1);
                        break;
                    }
                }
            }
        }
        float highest_z = static_cast<float>(highest_z_b) / 100.f;
        float grounded = (min_z - highest_z < 5e-6f) ? highest_z + height_l : NAN;

        return Collision(found, grounded);
    }

private:
    const float radius;
    const float height_u; // upper part height
    const float height_l; // lower part height
};

#endif
