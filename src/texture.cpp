#include <array>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "texture.hpp"
#include "util.hpp"

array<vector<uint8_t>, N_MIP_LEVEL> load_texture(string tex_folder_path) {
    array<vector<uint8_t>, N_MIP_LEVEL> data;

    if (tex_folder_path.back() != '/') {
        tex_folder_path.append("/");
    }
    for (uint8_t i = 0; i < N_MIP_LEVEL; i++) {
        string tex_path;
        {
            stringstream ss;
            ss << tex_folder_path
               << static_cast<unsigned short>(i)
               << "/mip.ppm";
            tex_path = ss.str();
        }

        ifstream tex_stream(tex_path);
        if (!tex_stream.is_open()) {
            cerr << "Cannot open " << tex_path << endl;
            _exit(1);
        }

        string line;

        tex_stream >> line;
        if (line != "P6") {
            cerr << "Incorrect ppm file" << endl;
            _exit(1);
        }
        tex_stream.ignore(numeric_limits<streamsize>::max(), '\n');

        while (getline(tex_stream, line) && line[0] == '#') {}

        uint32_t w, h;
        {
            stringstream wh(line);
            wh >> w >> h;
        }

        while (getline(tex_stream, line) && line[0] == '#') {}

        for (uint32_t _i = 0; _i < h; _i++) {
            for (uint32_t _j = 0; _j < w; _j++) {
                data[i].push_back(static_cast<uint8_t>(tex_stream.get()));
                data[i].push_back(static_cast<uint8_t>(tex_stream.get()));
                data[i].push_back(static_cast<uint8_t>(tex_stream.get()));
            }
        }
    }

    return data;
}
