#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <array>
#include <vector>

using namespace std;

constexpr uint32_t SUB_TEX_WIDTH = 64, SUB_TEX_HEIGHT = 64, N_TILES = 4;
constexpr uint8_t N_MIP_LEVEL = 7;

array<vector<uint8_t>, N_MIP_LEVEL> load_texture(string tex_folder_path, int n_channels);

#endif
