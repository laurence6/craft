#ifndef SCENE_HPP
#define SCENE_HPP

#include <algorithm>
#include <vector>

#include <GL/glew.h>

#include "block.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "object.hpp"
#include "shader.hpp"
#include "util.hpp"

using namespace std;

class Scene : public Singleton<Scene> {
private:
    BlockManager  block_manager  = BlockManager();
    ObjectManager object_manager = ObjectManager();

public:
    void init() {
        block_manager.init();
    }

    void add_block(Block* block) {
        block_manager.add_block(block);
    }

    void add_object(Object* obj) {
        object_manager.add_object(obj);
    }

    void move_objects() {
        static uint64_t last_update = time_now_ms();

        uint64_t now = time_now_ms();
        auto del_t = static_cast<float>(now - last_update);

        for (Object* object : object_manager.get_objects()) {
            if (object->state == State::Fixed) {
                continue;
            }

            vec3 del_p = object->velocity * del_t;

            float len = length(del_p);
            if (len > 0.f) {
                object->pos += del_p;
            }
            if (object->collider != nullptr) {
                Collision c = object->collider->collide(object->pos, block_manager);
                switch (object->state) {
                    case State::Normal:
                        if (!c.is_grounded()) {
                            object->transit_state(State::Falling);
                        } else if (c.found) {
                            object->pos.x -= del_p.x;
                            object->pos.y -= del_p.y;
                        }
                        break;
                    case State::Falling:
                        if (c.is_grounded() && object->velocity.z < 0.f) {
                            object->transit_state(State::Normal);
                            object->pos.z = c.grounded;
                        } else if (c.found) {
                            // FIXME
                        } else {
                            object->velocity.z = max(-fall_speed, object->velocity.z - gravity_acc);
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        last_update = now;

        update_vertices();
    }

    void update_vertices() {
        block_manager.update_vertices();
        object_manager.update_vertices();
    }

    void update_sun_dir() const {
        static uint64_t last_update = 0;
        uint64_t now = time_now_s();
        if (now > last_update) {
            float t = static_cast<float>(now % DAYTIME) / static_cast<float>(DAYTIME) - 0.5f;
            float x = t * 5.f;
            vec3 dir = normalize(vec3(x, 0.f, 2.56f));
            ShaderManager::instance().block_shader.upload_sun_dir(dir);
            last_update = now;
        }
    }

    void render() {
        ShaderManager::instance().block_shader.upload_MVP(Camera::instance().get_mvp());
        ShaderManager::instance().block_shader.use();
        block_manager.render();
    }
};

#endif
