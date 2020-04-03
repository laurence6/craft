#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <array>
#include <string>

#include "math.hpp"

using namespace std;

const string DB_PATH = "db";

const string SHADER_BLOCK_VERTEX_PATH        = "shader/block_vertex.glsl";
const string SHADER_BLOCK_FRAGMENT_PATH      = "shader/block_fragment.glsl";
const string SHADER_BLOCK_EDGE_VERTEX_PATH   = "shader/block_edge_vertex.glsl";
const string SHADER_BLOCK_EDGE_FRAGMENT_PATH = "shader/block_edge_fragment.glsl";
const string SHADER_LINE_VERTEX_PATH         = "shader/line_vertex.glsl";
const string SHADER_LINE_FRAGMENT_PATH       = "shader/line_fragment.glsl";
const string TEXTURE_FOLDER_PATH             = "texture";

constexpr int     WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 960;
char const* const WINDOW_TITLE = "craft";

constexpr float FOVY   = radians(60.f);
constexpr float ASPECT = 1280.f / 960.f;
constexpr float Z_NEAR = 0.01f, Z_FAR = 1000.f;

constexpr float BLOCK_EDGE_WIDTH = 3.f;

constexpr float CROSSHAIR_X = 30.f / 1280.f, CROSSHAIR_Y = 32.f / 960.f, CROSSHAIR_WIDTH = 4.f;

constexpr uint64_t DAYTIME = 600; // sec

constexpr float player_speed  = 0.05f / 10.f;
constexpr float gravity_acc   = 0.015f / 10.f;
constexpr float fall_speed    = 0.3f / 10.f;
constexpr float jump_speed    = 0.2f / 10.f;
constexpr float cam_rot_speed = 0.25f;

constexpr uint32_t SUB_TEX_WIDTH = 16, SUB_TEX_HEIGHT = 16, N_TILES = 7;
constexpr int32_t  N_MIP_LEVEL = 5;

constexpr uint8_t FACE_LEFT = 0, FACE_LEFT_BIT = 1u << 0u, //
    FACE_RIGHT = 1, FACE_RIGHT_BIT = 1u << 1u,             //
    FACE_FRONT = 2, FACE_FRONT_BIT = 1u << 2u,             //
    FACE_BACK = 3, FACE_BACK_BIT = 1u << 3u,               //
    FACE_BOTTOM = 4, FACE_BOTTOM_BIT = 1u << 4u,           //
    FACE_TOP = 5, FACE_TOP_BIT = 1u << 5u;

enum BlockType : uint16_t
{
    grass_block = 1,
    dirt_block  = 2,
    stone_block = 3,
    water_block = 4,
    grass       = 5,
    flower      = 6,
};

struct BlockConfig
{
    bool               is_opaque;
    bool               has_six_faces;
    array<uint32_t, 6> tex;
};

constexpr array<BlockConfig, 7> block_config { {
    {},
    { true, true, { { 1, 1, 1, 1, 2, 0 } } },
    { true, true, { { 2, 2, 2, 2, 2, 2 } } },
    { true, true, { { 3, 3, 3, 3, 3, 3 } } },
    { false, true, { { 4, 4, 4, 4, 4, 4 } } },
    { false, false, { { 5 } } },
    { false, false, { { 6 } } },
} };

#endif
