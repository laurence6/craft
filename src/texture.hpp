#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <array>
#include <vector>

#include "config.hpp"

using namespace std;

array<vector<uint8_t>, N_MIP_LEVEL> load_texture(string tex_folder_path, int n_channels);

#endif
