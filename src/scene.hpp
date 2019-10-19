#ifndef SCENE_HPP
#define SCENE_HPP

#include <algorithm>
#include <vector>

#include <GL/glew.h>

#include "block.hpp"
#include "config.hpp"
#include "object.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "util.hpp"

using namespace std;

class Scene : public Singleton<Scene> {
public:
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

    void update();

    void update_sun_dir() const {
        static uint64_t last_update = 0;
        uint64_t now = time_now_s();
        if (now > last_update) {
            float t = static_cast<float>(now % DAYTIME) / static_cast<float>(DAYTIME) - 0.5f;
            float x = t * 5.f;
            vec3 dir = normalize(vec3(x, 0.f, 2.56f));
            ShaderManager::ins().block_shader.upload_sun_dir(dir);
            last_update = now;
        }
    }

    void render() {
        ShaderManager::ins().block_shader.use();
        ShaderManager::ins().block_shader.upload_MVP(Player::ins().get_mvp());
        block_manager.render();
    }
};

#endif
