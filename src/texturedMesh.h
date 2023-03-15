#pragma once
//this was used for error checking in a different file but removed after debbuging
#define GLcall(x) x;

#include "glad.h"

#include <string>
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "happly/happly.h"
#include <iostream>
#include <vector>

//this namespace encompasses code that takes care of model loading
//the textured mesh is declared right after it
namespace Models
{
	struct vertexData
	{
		vertexData() {}
		vertexData(glm::vec3 pos, glm::vec3 normal, glm::vec2 UV, glm::vec3 color = glm::vec3(0.0f))
		{
			this->position = pos;
			this->normal = normal;
			this->UV = UV;
			this->color = color;
		}

		glm::vec3 position;
		glm::vec3 normal;
		glm::vec3 color;
		glm::vec2 UV;
	};

	struct triData
	{
		triData() {}
		triData(glm::uvec3 _index)
		{
			index = _index;
		}

		glm::uvec3 index;
	};

	inline void read_ply_file(const std::string& filepath, std::vector<vertexData>& vertices, std::vector<triData>& faces)
	{
		std::cout << "........................................................................\n";
		std::cout << "Now Reading: " << filepath << std::endl;

		// Construct the data object by reading from file
		happly::PLYData plyIn(filepath.c_str());

		// Get mesh-style data from the object
		std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
		std::vector<std::vector<size_t>> fInd = plyIn.getFaceIndices<size_t>();

		//gets each  normal element
		std::vector<float> nx = plyIn.getElement("vertex").getProperty<float>("nx");
		std::vector<float> ny = plyIn.getElement("vertex").getProperty<float>("ny");
		std::vector<float> nz = plyIn.getElement("vertex").getProperty<float>("nz");

		//getts each UV element
		std::vector<float> u = plyIn.getElement("vertex").getProperty<float>("u");
		std::vector<float> v = plyIn.getElement("vertex").getProperty<float>("v");

		//iterate through the vPos and other vectors for each element
		for (int i = 0; i < vPos.size(); i++)
		{
			glm::vec3 pos = glm::vec3(vPos[i][0], vPos[i][1], vPos[i][2]);
			glm::vec3 normal = glm::vec3(nx[i], ny[i], nz[i]);
			glm::vec2 tex_coord = glm::vec2(u[i], v[i]);

			vertices.push_back(vertexData(pos, normal, tex_coord));
		}

		//iterates through the faces cop them into the reference paramaeter
		for (int i = 0; i < fInd.size(); i++)
		{
			glm::uvec3 index = glm::vec3(fInd[i][0], fInd[i][1], fInd[i][2]);
			faces.push_back(triData(index));
		}

	}

}


class TexturedMesh
{
public:
	TexturedMesh();
	TexturedMesh(std::string plyFilePath, std::string bitmapPath);
	~TexturedMesh();

	void draw(glm::mat4 MVP);

private:
	unsigned int vao_id;
	unsigned int vertices_vbo_id;
	unsigned int texture_vbo_id;
	unsigned int vertex_faces_id;
	unsigned int bmp_id;
	unsigned int shader_id;

	unsigned int face_count;
};
