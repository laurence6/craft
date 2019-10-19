#include "scene.hpp"
#include "ray.hpp"

void Scene::update() {
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
            Collision c = object->collider->collide(object->pos);
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

    block_manager.update();
    object_manager.update();
    update_sun_dir();
}
