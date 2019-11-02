#ifndef RAY_HPP
#define RAY_HPP

#include <array>
#include <optional>

#include "block.hpp"
#include "math.hpp"

using namespace std;

namespace Ray {
    optional<array<BlockID, 2>> cast_block(vec3 const& p0, vec3 const& dir, float max_distance);
}

#endif
