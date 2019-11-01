#include "collider.hpp"
#include "scene.hpp"

Collision Collider::collide(vec3 const& pos) {
    auto& block_manager = Scene::ins().block_manager;

    float min_x = pos.x - radius;
    float max_x = pos.x + radius;
    float min_y = pos.y - radius;
    float max_y = pos.y + radius;
    float min_z = pos.z - height_l;
    float max_z = pos.z + height_u;
    auto min_x_b = static_cast<int32_t>(floor(min_x + 0.1f));
    auto max_x_b = static_cast<int32_t>(floor(max_x - 0.1f));
    auto min_y_b = static_cast<int32_t>(floor(min_y + 0.1f));
    auto max_y_b = static_cast<int32_t>(floor(max_y - 0.1f));
    auto min_z_b = static_cast<uint8_t>(floor(min_z + 0.1f));
    auto max_z_b = static_cast<uint8_t>(floor(max_z - 0.1f));

    bool found = false;
    for (int32_t z = min_z_b; z <= max_z_b; z++) {
        for (int32_t x = min_x_b; x <= max_x_b; x++) {
            for (int32_t y = min_y_b; y <= max_y_b; y++) {
                auto const* b = block_manager.get_block(BlockID { x, y, z });
                if (b != nullptr && !b->is_null()) {
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
                auto const* b = block_manager.get_block(BlockID { x, y, z });
                if (b != nullptr && !b->is_null()) {
                    highest_z_b = max(highest_z_b, z + 1);
                    break;
                }
            }
        }
    }
    float highest_z = static_cast<float>(highest_z_b);
    float grounded = (min_z - highest_z < 5e-6f) ? highest_z + height_l : NAN;

    return Collision { found, grounded };
}
