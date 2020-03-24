#include "ray.hpp"

#include "scene.hpp"

constexpr float MAX_DISTANCE = 4294967295.f;

optional<array<BlockID, 2>> Ray::cast_block(vec3 const& p0, vec3 const& dir, float max_distance)
{
    array<int32_t, 3> curr;
    array<int32_t, 3> step;
    array<float, 3>   next_distance;

    auto compute_next_distance = [&](int i) { next_distance[i] = (static_cast<float>(curr[i] + (step[i] < 0 ? 0 : 1)) - p0[i]) / dir[i]; };

    for (int i = 0; i < 3; i++)
    {
        if (dir[i] != 0.f)
        {
            curr[i] = static_cast<int32_t>(floor(p0[i]));
            step[i] = dir[i] > 0.f ? 1 : -1;
            compute_next_distance(i);
        }
        else
        {
            next_distance[i] = MAX_DISTANCE;
        }
    }

    for (;;)
    {
        int i = 0;
        if (next_distance[1] < next_distance[i])
            i = 1;
        if (next_distance[2] < next_distance[i])
            i = 2;

        if (next_distance[i] > max_distance)
            break;

        curr[i] += step[i];

        BlockID          block_id { curr[0], curr[1], curr[2] };
        BlockData const* block = Scene::ins().block_manager.get_block(block_id);
        if (block != nullptr)
        {
            curr[i] -= step[i];
            BlockID block_id_front { curr[0], curr[1], curr[2] };
            return { { { block_id, block_id_front } } };
        }

        compute_next_distance(i);
    }

    return nullopt;
}
