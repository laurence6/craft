#ifndef RAY_HPP
#define RAY_HPP

#include <optional>

#include "block.hpp"
#include "math.hpp"

using namespace std;

class Ray {
public:
    static optional<BlockID> cast_block(vec3 const& p0, vec3 const& dir, float max_distance);
};

#endif
