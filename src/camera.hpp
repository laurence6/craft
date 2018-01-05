#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "collider.hpp"
#include "config.hpp"
#include "math.hpp"
#include "object.hpp"

using namespace std;

class Camera : public Object {
private:
    float cam_speed = player_speed;

    vec3 cam_d    = vec3(1.f, 0.f, 0.f);
    vec3 player_f = vec3(1.f, 0.f, 0.f);
    vec3 player_l = vec3(0.f, 1.f, 0.f);
    float v_forward = 0.f;
    float v_left    = 0.f;

    float pitch = 90.f;

public:
    static Camera& instance() {
        static Camera ins;
        return ins;
    }

    void init() {
        pos = vec3(0.f, 0.f, 0.5f);

        collider = new Collider(0.003f, 0.000f, 0.0195f);
    }

    void start_move_forward()  { v_forward = clamp(v_forward + cam_speed, -cam_speed, cam_speed); update_velocity(); }

    void stop_move_forward()   { v_forward = clamp(v_forward - cam_speed, -cam_speed, cam_speed); update_velocity(); }

    void start_move_backward() { v_forward = clamp(v_forward - cam_speed, -cam_speed, cam_speed); update_velocity(); }

    void stop_move_backward()  { v_forward = clamp(v_forward + cam_speed, -cam_speed, cam_speed); update_velocity(); }

    void start_move_left()  { v_left = clamp(v_left + cam_speed, -cam_speed, cam_speed); update_velocity(); }

    void stop_move_left()   { v_left = clamp(v_left - cam_speed, -cam_speed, cam_speed); update_velocity(); }

    void start_move_right() { v_left = clamp(v_left - cam_speed, -cam_speed, cam_speed); update_velocity(); }

    void stop_move_right()  { v_left = clamp(v_left + cam_speed, -cam_speed, cam_speed); update_velocity(); }

    void rotate(float del_x, float del_y) {
        rot = fmod(rot + del_x * cam_rot_speed, 360.f);
        pitch = clamp(pitch + del_y * cam_rot_speed, 1.f, 179.f);
        cam_d = normalize(vec3(
            sin(radians(pitch)) * cos(radians(rot)),
            sin(radians(pitch)) * sin(radians(rot)),
            cos(radians(pitch))
        ));

        player_f = normalize(vec3(cos(radians(rot)), sin(radians(rot)), 0));
        player_l = cross(vec3(0.f, 0.f, 1.f), player_f);

        update_velocity();
    }

    const mat4 get_mvp() const {
        const mat4 projection = perspective(FOVY, ASPECT, Z_NEAR, Z_FAR);
        mat4 view = lookAt(pos, pos + cam_d, vec3(0.f, 0.f, 1.f));
        mat4 mvp = projection * view;
        return mvp;
    }

private:
    Camera() = default;

    Camera(const Camera&) = delete;
    Camera(Camera&&)      = delete;

    Camera& operator=(const Camera&) = delete;
    Camera& operator=(Camera&&)      = delete;

    void update_velocity() {
        vec3 new_v = player_f * v_forward + player_l * v_left;
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
