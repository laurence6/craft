#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "block.hpp"
#include "collider.hpp"
#include "config.hpp"
#include "math.hpp"
#include "object.hpp"
#include "ray.hpp"
#include "util.hpp"

using namespace std;

class Player : public Singleton<Player>, public Object {
public:
    Block const* target = nullptr;

private:
    float cam_speed = player_speed;

    float rot = 0.f; // rotation around z axis / yaw
    float pitch = 90.f;

    vec3 forward = vec3(1.f, 0.f, 0.f);
    vec3 player_forward = vec3(1.f, 0.f, 0.f);
    vec3 player_left = vec3(0.f, 1.f, 0.f);
    float v_forward = 0.f;
    float v_left = 0.f;

public:
    void init() {
        pos = vec3(0.f, 0.f, 50.f);

        collider = make_unique<Collider>(0.3f, 0.0f, 1.95f);
    }

    void update() {
        target = Ray::cast_block(pos, forward, 24);
    }

#define MOVE_FUNC(d, v, as1, as2)                                                                      \
    void start_move_ ## d () { v = clamp(v as1 cam_speed, -cam_speed, cam_speed); update_velocity(); } \
    void stop_move_  ## d () { v = clamp(v as2 cam_speed, -cam_speed, cam_speed); update_velocity(); }

    MOVE_FUNC(forward,  v_forward, +, -)

    MOVE_FUNC(backward, v_forward, -, +)

    MOVE_FUNC(left,  v_left, +, -)

    MOVE_FUNC(right, v_left, -, +)

#undef MOVE

    void rotate(float del_x, float del_y) {
        rot = fmod(rot + del_x * cam_rot_speed, 360.f);
        pitch = clamp(pitch + del_y * cam_rot_speed, 1.f, 179.f);
        forward = normalize(vec3(
            sin(radians(pitch)) * cos(radians(rot)),
            sin(radians(pitch)) * sin(radians(rot)),
            cos(radians(pitch))
        ));

        player_forward = normalize(vec3(cos(radians(rot)), sin(radians(rot)), 0));
        player_left = cross(vec3(0.f, 0.f, 1.f), player_forward);

        update_velocity();
    }

    mat4 get_mvp() const {
        const mat4 projection = perspective(FOVY, ASPECT, Z_NEAR, Z_FAR);
        mat4 view = lookAt(pos, pos + forward, vec3(0.f, 0.f, 1.f));
        mat4 mvp = projection * view;
        return mvp;
    }

private:
    void update_velocity() {
        vec3 new_v = player_forward * v_forward + player_left * v_left;
        new_v.z = 0.f;
        float len = length(new_v);
        if (len > 0.f) {
            new_v /= len;
            new_v *= cam_speed;
        }

        velocity.x = new_v.x;
        velocity.y = new_v.y;
    }
};

#endif
