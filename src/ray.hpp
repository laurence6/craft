#ifndef RAY_HPP
#define RAY_HPP

#include "block.hpp"
#include "math.hpp"

class Ray {
public:
    static Block* cast_block(vec3 const& p0, vec3 const& dir, float max_distance);
};

#endif
