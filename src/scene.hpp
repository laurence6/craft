#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include <GL/glew.h>

#include "block.hpp"

using namespace std;

class Scene {
public:
    Scene(vector<Block>&& blocks) : blocks(blocks) {
        update();
    }

    vector<GLfloat> const& get_vertices() {
        return vertices;
    }

    vector<GLfloat> const& get_uv() {
        return uv;
    }

private:
    void update() {
        vertices.clear();
        uv.clear();
        for (const auto& block : this->blocks) {
            vertices.insert(vertices.end(), block.vertices.begin(), block.vertices.end());
            uv.insert(uv.end(), block.uv.begin(), block.uv.end());
        }
    }

    vector<Block> blocks;
    vector<GLfloat> vertices;
    vector<GLfloat> uv;
};

#endif
