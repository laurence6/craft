#ifndef SCENE_HPP
#define SCENE_HPP

#include <algorithm>
#include <vector>

#include <GL/glew.h>

#include "block.hpp"
#include "object.hpp"
#include "render.hpp"
#include "util.hpp"

using namespace std;

class Scene {
private:
    BlockManager  block_manager  = BlockManager();
    ObjectManager object_manager = ObjectManager();

public:
    static Scene& instance() {
        static Scene ins;
        return ins;
    }

    void init() {}

    void add_block(Block* block) {
        block_manager.add_block(block);
    }

    void add_object(Object* obj) {
        object_manager.add_object(obj);
    }

    void update_vertices() {
        block_manager.update_vertices();
        object_manager.update_vertices();
    }

    void move_objects() {
        static uint64_t last_update = time_now();

        uint64_t now = time_now();
        float del_t = static_cast<float>(now - last_update);

        for (Object* object : object_manager.get_objects()) {
            if (object->status == Status::Fixed) {
                continue;
            }

            vec3 del_p = object->velocity * del_t;

            float len = length(del_p);
            if (len > 0.f) {
                object->pos += del_p;
            }
            if (object->collider != nullptr) {
                auto c = object->collider->collide(object->pos, block_manager);
                switch (object->status) {
                    case Status::Normal:
                        if (!c.is_grounded()) {
                            object->status = Status::Falling;
                        } else if (c.found) {
                            object->pos.x -= del_p.x;
                            object->pos.y -= del_p.y;
                        }
                        break;
                    case Status::Jumping:
                        if (c.found) {
                            // FIXME
                        } else {
                            object->velocity.z = jump_speed;
                            object->status = Status::Falling;
                        }
                        break;
                    case Status::Falling:
                        if (c.is_grounded()) {
                            object->status = Status::Normal;
                            object->pos.z = c.grounded;
                            object->velocity.z = 0.f;
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

private:
    Scene() {}

    Scene(const Scene&) = delete;
    Scene(Scene&&)      = delete;

    ~Scene() {}

    Scene& operator=(const Scene&) = delete;
    Scene& operator=(Scene&&)      = delete;
};

#endif
