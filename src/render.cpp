#include "camera.hpp"
#include "render.hpp"

void RenderManager::render() const {
    glUseProgram(program_ID);
    glUniformMatrix4fv(matrix_ID, 1, GL_FALSE, &Camera::instance().get_mvp()[0][0]);

    render_blocks();

    render_objects();

    glUseProgram(ui_program_ID);
    render_ui();
}
