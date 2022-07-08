//
// Created by MXY on 7/8/2022.
//

#ifndef CG_MODEL_H
#define CG_MODEL_H

#include "mesh.h"
#include "shader_m.h"
#include "shader_s.h"
#include "assimp/scene.h"
#include <algorithm>
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "stb_image.h"

unsigned int TextureFromFile(const char *path, const std::string& directory, bool gamma = false);

class model {
public:
    explicit model(const std::string& path, bool gamma = false);
    void draw(const Shader& shader) const;
private:
    bool gamma_correction;
    std::vector<texture> textures_loaded;
    std::vector<mesh> meshes;
    std::string directory;
    void load_model(const std::string& path);
    void process_node(aiNode *node, const aiScene *scene);
    mesh process_mesh(aiMesh *aiMesh, const aiScene *scene);
    std::vector<texture> load_material_textures(aiMaterial *mat, aiTextureType type,
                                         std::string typeName);
};


#endif //CG_MODEL_H
