#ifndef _TEXTURE_H
#define _TEXTURE_H

#include <array>
#include <vector>

using namespace std;

constexpr uint32_t SUB_TEX_WIDTH = 16, SUB_TEX_HEIGHT = 16, N_TILES = 4;
constexpr uint8_t N_MIP_LEVEL = 5;

array<vector<uint8_t>, N_MIP_LEVEL> load_texture(string tex_folder_path);

#endif
