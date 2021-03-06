#include <array>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

#include "texture.hpp"

array<vector<uint8_t>, N_MIP_LEVEL> load_texture(string tex_folder_path, int n_channels)
{
    array<vector<uint8_t>, N_MIP_LEVEL> data {};

    if (tex_folder_path.back() != '/')
    {
        tex_folder_path.append("/");
    }
    for (int i = 0; i < N_MIP_LEVEL; i++)
    {
        string tex_path;
        {
            stringstream ss;
            ss << tex_folder_path << static_cast<unsigned short>(i) << "/mip.png";
            tex_path = ss.str();
        }

        int                  x, y, n, desired_channels = n_channels;
        unsigned char const* tex_data = stbi_load(tex_path.c_str(), &x, &y, &n, desired_channels);
        if (tex_data == nullptr)
        {
            throw runtime_error(stbi_failure_reason());
        }

        data[i].insert(data[i].end(), tex_data, tex_data + x * y * desired_channels);
        free((void*) tex_data);
    }

    return data;
}
