//
// Created by MXY on 7/8/2022.
//

#ifndef CG_MESH_H
#define CG_MESH_H

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <learnopengl/shader_s.h>
#include <learnopengl/shader_m.h>

constexpr unsigned int MAX_BONE_INFLUENCE = 4;
struct vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coords;
    glm::vec3 tangent;
    glm::vec3 bi_tangent;
    int m_bone_IDs[MAX_BONE_INFLUENCE];
    float m_weights[MAX_BONE_INFLUENCE];
};

struct texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class mesh {
private:
    std::vector<vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<texture> textures;
    unsigned int vao{};
    unsigned int vbo{};
    unsigned int ebo{};
    void set_up_mesh();
public:
    explicit mesh(std::vector<vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures);
    void draw(const Shader& shader) const;
};


#endif //CG_MESH_H
