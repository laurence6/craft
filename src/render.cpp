#include "camera.hpp"
#include "render.hpp"

void RenderManager::render() const {
    block_shader.upload_MVP(Camera::instance().get_mvp());

    render_blocks();

    render_objects();

    render_ui();
}
