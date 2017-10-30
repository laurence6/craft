#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "math.hpp"
#include "object.hpp"

using namespace std;

class Camera : public Object {
public:
    Camera() {
        is_static = false;

        pos = vec3(0.f, -0.5f, 0.05f);
        rot = 90.f;
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
        rot = fmod(rot + del_x * rot_speed, 360.f);
        pitch = clamp(pitch + del_y * rot_speed, 1.f, 179.f);
        cam_d = normalize(vec3(
            sin(radians(pitch)) * cos(radians(rot)),
            sin(radians(pitch)) * sin(radians(rot)),
            cos(radians(pitch))
        ));
        cam_l = cross(vec3(0.f, 0.f, 1.f), cam_d);

        update_velocity();
    }

    const mat4 get_mvp() const {
        const mat4 projection = perspective(radians(45.f), 4.f / 3.f, 0.001f, 100.f);
        mat4 view = lookAt(pos, pos + cam_d, vec3(0.f, 0.f, 1.f));
        mat4 mvp = projection * view;
        return mvp;
    }

private:
    void update_velocity() {
        velocity = cam_d * v_forward + cam_l * v_left;
        float len = length(velocity);
        if (len > 0.f) {
            velocity /= len;
            velocity *= cam_speed;
        }
    }

    float cam_speed = 0.0002f;
    float rot_speed = 0.25f;

    vec3 cam_d = vec3(0.f, 1.f, 0.f);
    vec3 cam_l = vec3(-1.f, 0.f, 0.f);
    float v_forward = 0.f;
    float v_left    = 0.f;

    float pitch = 90.f;
};

#endif
