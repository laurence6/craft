#include "camera.hpp"
#include "render.hpp"

GLuint RenderElement::program_ID = 0;
GLuint RenderElement::matrix_ID = 0;
GLuint RenderElement::sampler_ID = 0;

void RenderManager::render() const {
    glUseProgram(RenderElement::program_ID);
    glUniformMatrix4fv(RenderElement::matrix_ID, 1, GL_FALSE, &Camera::instance().get_mvp()[0][0]);
    for (const pair<uint64_t, RenderElement>& chunk : chunks) {
        render_element(chunk.second);
    }
    render_element(*objects);

    glUseProgram(ui_program_ID);
    render_ui();
}
