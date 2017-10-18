#ifndef _CAMERA_H
#define _CAMERA_H

#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

class Camera {
public:
    Camera() {
        update_mvp();
    }

    void start_move_forward()  { v_forward = clamp(v_forward + cam_speed, -cam_speed, cam_speed); }

    void stop_move_forward()   { v_forward = clamp(v_forward - cam_speed, -cam_speed, cam_speed); }

    void start_move_backward() { v_forward = clamp(v_forward - cam_speed, -cam_speed, cam_speed); }

    void stop_move_backward()  { v_forward = clamp(v_forward + cam_speed, -cam_speed, cam_speed); }

    void start_move_left()  { v_left = clamp(v_left + cam_speed, -cam_speed, cam_speed); }

    void stop_move_left()   { v_left = clamp(v_left - cam_speed, -cam_speed, cam_speed); }

    void start_move_right() { v_left = clamp(v_left - cam_speed, -cam_speed, cam_speed); }

    void stop_move_right()  { v_left = clamp(v_left + cam_speed, -cam_speed, cam_speed); }

    void rotate(float del_x, float del_y) {
        yaw = fmod(yaw + del_x * rot_speed, 360.f);
        pitch = clamp(pitch + del_y * rot_speed, 1.f, 179.f);
        cam_d = normalize(vec3(
            sin(radians(pitch)) * cos(radians(yaw)),
            sin(radians(pitch)) * sin(radians(yaw)),
            cos(radians(pitch))
        ));
        update_mvp();
    }

    void lerp_move() {
        using namespace chrono;

        static auto last_update = system_clock::now();

        auto now = system_clock::now();
        auto del_t = duration_cast<milliseconds>(now - last_update);

        if (del_t > 16ms) {
            float dt = static_cast<float>(del_t.count());

            vec3 del_p = v_forward * cam_d
                       + v_left * cross(vec3(0., 0., 1.), cam_d);
            float len = length(del_p);
            if (len > cam_speed) {
                del_p = del_p / len * cam_speed;
            }
            del_p *= dt;

            cam_pos += del_p;

            last_update = now;
        }

        update_mvp();
    }

    void update_mvp() {
        const mat4 projection = perspective(radians(45.), 4. / 3., 0.001, 100.);
        mat4 view = lookAt(cam_pos, cam_pos + cam_d, vec3(0., 0., 1.));
        mvp = projection * view;
    }

    mat4 const& get_mvp() const {
        return mvp;
    }

private:
    float cam_speed = 0.0001f;
    float rot_speed = 0.25f;

    vec3 cam_pos = vec3(0., -0.5, 0.05);
    vec3 cam_d   = vec3(0., 1., 0.);
    float v_forward = 0;
    float v_left = 0;

    float yaw = 90.f;
    float pitch = 90.f;

    mat4 mvp;
};

#endif
