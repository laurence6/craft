#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <array>
#include <string>

#include <GL/glew.h>

#include "math.hpp"

using namespace std;

const string SHADER_BLOCK_VERTEX_PATH = "shader/block_vertex.glsl";
const string SHADER_BLOCK_FRAGMENT_PATH = "shader/block_fragment.glsl";
const string SHADER_LINE_VERTEX_PATH = "shader/line_vertex.glsl";
const string SHADER_LINE_FRAGMENT_PATH = "shader/line_fragment.glsl";
const string TEXTURE_FOLDER_PATH = "texture";
const string MAP_PATH = "map";

constexpr int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 960;
const char* const WINDOW_TITLE = "craft";

constexpr float FOVY = radians(60.f);
constexpr float ASPECT = 1280.f / 960.f;
constexpr float Z_NEAR = 0.001f, Z_FAR = 100.f;

constexpr float CROSSHAIR_X = 30.f / 1280.f, CROSSHAIR_Y = 32.f / 960.f, CROSSHAIR_WIDTH = 4.f;

constexpr uint64_t DAYTIME = 600; // sec

constexpr uint32_t SUB_TEX_WIDTH = 64, SUB_TEX_HEIGHT = 64, N_TILES = 5;
constexpr uint8_t N_MIP_LEVEL = 7;

constexpr uint8_t
    BLOCK_FACE_LEFT   = 0,
    BLOCK_FACE_RIGHT  = 1,
    BLOCK_FACE_FRONT  = 2,
    BLOCK_FACE_BACK   = 3,
    BLOCK_FACE_BOTTOM = 4,
    BLOCK_FACE_TOP    = 5;

constexpr array<GLfloat, 6> dirt_block_tex  = {{ 2, 2, 2, 2, 2, 2 }};
constexpr array<GLfloat, 6> grass_block_tex = {{ 1, 1, 1, 1, 2, 0 }};
constexpr array<GLfloat, 6> stone_block_tex = {{ 3, 3, 3, 3, 3, 3 }};
constexpr GLfloat           grass_tex = 4;

constexpr float player_speed = 0.05f  / 1000.f;
constexpr float gravity_acc  = 0.015f / 1000.f;
constexpr float fall_speed   = 0.3f   / 1000.f;
constexpr float jump_speed   = 0.2f   / 1000.f;

constexpr float cam_rot_speed = 0.25f;

#endif
