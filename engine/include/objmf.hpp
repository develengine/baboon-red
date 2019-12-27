#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

#include "math.hpp"

namespace OBJMF {

struct Section {
    size_t offset;
    size_t length;
};

struct Model {

    float *data = nullptr;
    uint32_t *indices = nullptr;
    size_t dataSize = 0;
    size_t indexCount = 0;

    std::map<std::string, Section> sections;

    Model(float *d, size_t ds, uint32_t *i, size_t ic)
        : dataSize(ds), indexCount(ic)
    {
        data = new float[ds];
        indices = new uint32_t[ic];
        memcpy(data, d, ds * sizeof(float));
        memcpy(indices, i, ic * sizeof(uint32_t));
    }

    void free() {
        if (data != nullptr) {
            delete[] data;
        }
        if (indices != nullptr) {
            delete[] indices;
        }
    }
};

Model readObjFile(std::string path) {

    std::vector<eng::Vec3f> vertices;
    std::vector<eng::Vec2f> texCoords;
    std::vector<eng::Vec3f> normals;

    std::map<std::string, uint32_t> vertexGroups;
    std::vector<uint32_t> indices;
    std::vector<float> data;
    uint32_t nextIndex = 0;

    std::ifstream file(path);

    if (!file.is_open()) {
        std::cerr << "Failed to open obj file '" << path << "'\n";
        exit(-1);
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line == "") {
            continue;
        }
        const char *str = line.c_str();
        if (strncmp(str, "v ", 2) == 0) {
            float values[3];
            size_t last = 1;
            for (int i = 0; i < 3; i++) {
                size_t offset;
                values[i] = std::stof(&(str[last]), &offset);
                last += offset;
            }
            eng::Vec3f vertex = { values[0], values[1], values[2] };
            vertices.push_back(vertex);
        } else if (strncmp(str, "vt ", 3) == 0) {
            float values[2];
            size_t last = 2;
            for (int i = 0; i < 2; i++) {
                size_t offset;
                values[i] = std::stof(&(str[last]), &offset);
                last += offset;
            }
            eng::Vec2f texCoord = { values[0], values[1] };
            texCoords.push_back(texCoord);
        } else if (strncmp(str, "vn ", 3) == 0) {
            float values[3];
            size_t last = 2;
            for (int i = 0; i < 3; i++) {
                size_t offset;
                values[i] = std::stof(&(str[last]), &offset);
                last += offset;
            }
            eng::Vec3f normal = { values[0], values[1], values[2] };
            normals.push_back(normal);
        } else if (strncmp(str, "f ", 2) == 0) {
            uint32_t values[3];
            size_t last = 2;
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    size_t offset;
                    values[j] = std::stoi(&(str[last]), &offset) - 1;
                    last += offset + 1;
                }
                std::string identifier = std::to_string(values[0]) + '/' +
                                         std::to_string(values[1]) + '/' +
                                         std::to_string(values[2]);
                auto search = vertexGroups.find(identifier);
                if (search != vertexGroups.end()) {
                    indices.push_back(search->second);
                } else {
                    data.reserve(8);
                    eng::Vec3f v3 = vertices[values[0]];
                    for (int i = 0; i < 3; i++)
                        data.push_back(v3[i]);
                    eng::Vec2f v2 = texCoords[values[1]];
                    for (int i = 0; i < 2; i++)
                        data.push_back(v2[i]);
                    eng::Vec3f v3b = normals[values[2]];
                    for (int i = 0; i < 3; i++)
                        data.push_back(v3b[i]);
                    indices.push_back(nextIndex);
                    vertexGroups[identifier] = nextIndex++;
                }
            }
        }
    }

    file.close();

    Model model = { data.data(), data.size(), indices.data(), indices.size() };
    return model;
}

}
