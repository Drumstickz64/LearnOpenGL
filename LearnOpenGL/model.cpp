#include <iostream>

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include "model.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	: vertices(vertices), indices(indices), textures(textures) {
	setup_mesh();
}

void Mesh::draw(Shader_Program& shader) {
	unsigned int curr_diffuse = 1;
	unsigned int curr_specular = 1;

	for (size_t i = 0; i < textures.size(); i++) {
		Texture& texture = textures[i];

		std::string number;
		if (texture.type == "texture_diffuse") {
			number = std::to_string(curr_diffuse++);
		} else if (texture.type == "texture_specular") {
			//number = std::to_string(curr_specular++);
			continue;
		}

		std::string uniform_name = "material." + texture.type + number;
		shader.set_texture(uniform_name, texture, static_cast<GLenum>(i));
	}

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::setup_mesh() {
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

	glBindVertexArray(0);
}

void Model::draw(Shader_Program& shader) {
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].draw(shader);
	}
}

void Model::load_model(const std::filesystem::path& path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
																aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
		std::cerr << "ERROR::ASSIMP\n" << importer.GetErrorString() << std::endl;
		exit(-1);
	}

	m_directory = path.parent_path();
	process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(process_mesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		aiNode* child = node->mChildren[i];
		process_node(child, scene);
	}
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex{};

		glm::vec3 vector{};
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.normal = vector;
		}

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec{};
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.tex_coords = vec;
		} else {
			vertex.tex_coords = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}  


	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuse_maps = load_material_textures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
		std::vector<Texture> specular_maps = load_material_textures(mat, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}

	return std::move(Mesh(vertices, indices, textures));
}

std::vector<Texture> Model::load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name) {
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString tex_path;
		mat->GetTexture(type, i, &tex_path);
		
		auto texture = Texture(m_directory / tex_path.C_Str(), type_name);
		textures.push_back(texture);
	}

	return textures;
}
