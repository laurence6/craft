#include "input.hpp"

#include "player.hpp"
#include "scene.hpp"

static bool window_exclusive = false;

void key_callback(GLFWwindow* window, int key, int, int action, int)
{
    if (window_exclusive)
    {
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
                case GLFW_KEY_ESCAPE:
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    window_exclusive = false;
                    break;
                case GLFW_KEY_W: Player::ins().start_move_forward(); break;
                case GLFW_KEY_S: Player::ins().start_move_backward(); break;
                case GLFW_KEY_A: Player::ins().start_move_left(); break;
                case GLFW_KEY_D: Player::ins().start_move_right(); break;
                case GLFW_KEY_SPACE: Player::ins().jump(); break;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            switch (key)
            {
                case GLFW_KEY_W: Player::ins().stop_move_forward(); break;
                case GLFW_KEY_S: Player::ins().stop_move_backward(); break;
                case GLFW_KEY_A: Player::ins().stop_move_left(); break;
                case GLFW_KEY_D: Player::ins().stop_move_right(); break;
            }
        }
    }
    else
    {
        if (action == GLFW_PRESS)
        {
            switch (key)
            {
                case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
            }
        }
    }
}

void cursor_pos_callback(GLFWwindow*, double posx, double posy)
{
    static double last_posx = posx, last_posy = posy;

    auto del_x = static_cast<float>(last_posx - posx);
    auto del_y = static_cast<float>(posy - last_posy);
    last_posx  = posx;
    last_posy  = posy;

    if (window_exclusive)
    {
        Player::ins().rotate(del_x, del_y);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int)
{
    if (window_exclusive)
    {
        if (action == GLFW_RELEASE)
        {
            switch (button)
            {
                case GLFW_MOUSE_BUTTON_LEFT:
                    if (Player::ins().target.has_value())
                    {
                        BlockID const&   block_id = Player::ins().target->at(0);
                        BlockData const* block    = Scene::ins().block_manager.get_block(block_id);
                        if (block != nullptr)
                        {
                            Player::ins().new_block = *block;
                            Scene::ins().block_manager.del_block(block_id);
                        }
                    }
                    break;
                case GLFW_MOUSE_BUTTON_RIGHT:
                    if (Player::ins().target.has_value())
                    {
                        BlockID          block_id = Player::ins().target->at(0);
                        BlockData const* block    = Scene::ins().block_manager.get_block(block_id);
                        if (block != nullptr && !(block->is_opaque() && block->has_six_faces()))
                        {
                            Scene::ins().block_manager.add_block(block_id, BlockData { Player::ins().new_block });
                        }
                        else
                        {
                            block_id = Player::ins().target->at(1);
                            block    = Scene::ins().block_manager.get_block(block_id);
                            if (block == nullptr || !(block->is_opaque() && block->has_six_faces()))
                            {
                                Scene::ins().block_manager.add_block(block_id, BlockData { Player::ins().new_block });
                            }
                        }
                    }
                    break;
            }
        }
    }
    else
    {
        if (action == GLFW_RELEASE)
        {
            switch (button)
            {
                case GLFW_MOUSE_BUTTON_LEFT:
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    window_exclusive = true;
                    break;
            }
        }
    }
}

void scroll_callback(GLFWwindow*, double, double yoffset)
{
    auto&    type = Player::ins().new_block.type;
    uint32_t tot  = block_config.size();
    type          = (type - 1 + (yoffset < 0.f ? 1u : tot - 1u)) % tot + 1;
}
