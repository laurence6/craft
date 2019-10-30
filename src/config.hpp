#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <array>
#include <string>

#include "math.hpp"

using namespace std;

const string SHADER_BLOCK_VERTEX_PATH = "shader/block_vertex.glsl";
const string SHADER_BLOCK_FRAGMENT_PATH = "shader/block_fragment.glsl";
const string SHADER_BLOCK_EDGE_VERTEX_PATH = "shader/block_edge_vertex.glsl";
const string SHADER_BLOCK_EDGE_FRAGMENT_PATH = "shader/block_edge_fragment.glsl";
const string SHADER_LINE_VERTEX_PATH = "shader/line_vertex.glsl";
const string SHADER_LINE_FRAGMENT_PATH = "shader/line_fragment.glsl";
const string TEXTURE_FOLDER_PATH = "texture";
const string MAP_PATH = "map";

constexpr int WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 960;
char const* const WINDOW_TITLE = "craft";

constexpr float FOVY = radians(60.f);
constexpr float ASPECT = 1280.f / 960.f;
constexpr float Z_NEAR = 0.01f, Z_FAR = 1000.f;

constexpr float BLOCK_EDGE_WIDTH = 3.f;

constexpr float CROSSHAIR_X = 30.f / 1280.f, CROSSHAIR_Y = 32.f / 960.f, CROSSHAIR_WIDTH = 4.f;

constexpr uint64_t DAYTIME = 600; // sec

constexpr uint32_t SUB_TEX_WIDTH = 64, SUB_TEX_HEIGHT = 64, N_TILES = 5;
constexpr int32_t N_MIP_LEVEL = 7;

constexpr uint8_t
    FACE_LEFT   = 0, FACE_LEFT_BIT   = 1u << 0u,
    FACE_RIGHT  = 1, FACE_RIGHT_BIT  = 1u << 1u,
    FACE_FRONT  = 2, FACE_FRONT_BIT  = 1u << 2u,
    FACE_BACK   = 3, FACE_BACK_BIT   = 1u << 3u,
    FACE_BOTTOM = 4, FACE_BOTTOM_BIT = 1u << 4u,
    FACE_TOP    = 5, FACE_TOP_BIT    = 1u << 5u;

constexpr array<uint32_t, 6> grass_block_tex = {{ 1, 1, 1, 1, 2, 0 }};
constexpr array<uint32_t, 6> dirt_block_tex  = {{ 2, 2, 2, 2, 2, 2 }};
constexpr array<uint32_t, 6> stone_block_tex = {{ 3, 3, 3, 3, 3, 3 }};
constexpr uint32_t           grass_tex = 4;

constexpr float player_speed = 0.05f  / 10.f;
constexpr float gravity_acc  = 0.015f / 10.f;
constexpr float fall_speed   = 0.3f   / 10.f;
constexpr float jump_speed   = 0.2f   / 10.f;

constexpr float cam_rot_speed = 0.25f;

#endif
