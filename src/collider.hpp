#ifndef COLLIDER_HPP
#define COLLIDER_HPP

#include <algorithm>
#include <cmath>

#include "block_manager.hpp"
#include "math.hpp"

using namespace std;

class Collision
{
public:
    const bool  found;
    const float grounded;

public:
    Collision(bool _found, float _grounded) : found(_found), grounded(_grounded)
    {
    }

    bool is_grounded() const
    {
        return !isnan(grounded);
    }
};

// Cylinder collider.
class Collider
{
private:
    const float radius;
    const float height_u; // upper part height
    const float height_l; // lower part height

public:
    Collider(float _radius, float _height_u, float _height_l) : radius(_radius), height_u(_height_u), height_l(_height_l)
    {
    }

    Collision collide(vec3 const& pos);
};

#endif
