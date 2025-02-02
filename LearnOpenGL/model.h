#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

#include <assimp/scene.h>
#include <glm/glm.hpp>

#include "shader_program.h"
#include "texture.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

class Mesh {
   public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	void draw(Shader_Program& shader);

   private:
	GLuint m_vao, m_vbo, m_ebo;

	void setup_mesh();
};

class Model {
   public:
	std::vector<Mesh> meshes;

	Model(const std::filesystem::path& path) { load_model(path); }
	void draw(Shader_Program& shader);

   private:
	std::unordered_map<std::filesystem::path, Texture> textures_loaded;
	std::filesystem::path m_directory;

	void load_model(const std::filesystem::path& path);
	void process_node(aiNode* node, const aiScene* scene);
	Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name);
};