#ifndef SCENE_HPP
#define SCENE_HPP

#include <algorithm>
#include <vector>

#include "block.hpp"
#include "config.hpp"
#include "object.hpp"
#include "opengl.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "util.hpp"

using namespace std;

class Scene : public Singleton<Scene>
{
public:
    BlockManager  block_manager {};
    ObjectManager object_manager {};

public:
    void init()
    {
        block_manager.init();
    }

    void shutdown()
    {
        block_manager.shutdown();
    }

    void add_block(BlockID const& block_id, BlockData&& block)
    {
        block_manager.add_block(block_id, forward<BlockData>(block));
    }

    void add_object(Object* obj)
    {
        object_manager.add_object(obj);
    }

    void update();

    static void update_sun_dir()
    {
        static uint64_t last_update = 0;
        uint64_t        now         = time_now_s();
        if (now > last_update)
        {
            float t   = static_cast<float>(now % DAYTIME) / static_cast<float>(DAYTIME) - 0.5f;
            float x   = t * 5.f;
            vec3  dir = normalize(vec3(x, 0.f, 2.56f));
            ShaderManager::ins().block_shader.upload_sun_dir(dir);
            last_update = now;
        }
    }

    void render()
    {
        ShaderManager::ins().block_shader.use();
        ShaderManager::ins().block_shader.upload_MVP(Player::ins().get_mvp());
        block_manager.render();
    }
};

#endif
