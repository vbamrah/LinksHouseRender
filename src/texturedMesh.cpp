#pragma warning(disable : 4996)

#include "texturedMesh.h"

#include <vector>
using namespace Models;

#include <stdio.h>
#include <stdlib.h>

void loadARGB_BMP(const char* imagepath, unsigned char** data, unsigned int* width, unsigned int* height) {


	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	// Actual RGBA data

	// Open the file
	FILE* file = fopen(imagepath, "rb");
	if (!file) {
		printf("%s could not be opened. Are you in the right directory?\n", imagepath);
		getchar();
		return;
	}

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file1\n");
		fclose(file);
		return;
	}

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	*width = *(int*)&(header[0x12]);
	*height = *(int*)&(header[0x16]);
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file2\n");
		fclose(file);
		return;
	}
	// Make sure this is a 32bpp file
	if (*(int*)&(header[0x1E]) != 3) {
		printf("Not a correct BMP file3\n");
		fclose(file);
		return;
	}
	// fprintf(stderr, "header[0x1c]: %d\n", *(int*)&(header[0x1c]));
	// if ( *(int*)&(header[0x1C])!=32 ) {
	//     printf("Not a correct BMP file4\n");
	//     fclose(file);
	//     return;
	// }

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = (*width) * (*height) * 4; // 4 : one byte for each Red, Green, Blue, Alpha component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

	// Create a buffer
	*data = new unsigned char[imageSize];

	if (dataPos != 54) {
		fread(header, 1, dataPos - 54, file);
	}

	// Read the actual data from the file into the buffer
	fread(*data, 1, imageSize, file);

	// Everything is in memory now, the file can be closed.
	fclose(file);
}

unsigned int compileShader(std::string& shader, unsigned int shaderType)
{
	GLcall(unsigned shader_obj = glCreateShader(shaderType));
	const char* src = shader.c_str();
	GLcall(glShaderSource(shader_obj, 1, &src, nullptr));
	GLcall(glCompileShader(shader_obj));

	int status = 0;
	GLcall(glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &status));

	if (status == GL_FALSE)
	{
		int length;
		GLcall(glGetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &length));

		char* message = (char*)alloca(sizeof(char) * length);
		GLcall(glGetShaderInfoLog(shader_obj, length, &length, message));

		std::cout << "shader error -> " << (shaderType == GL_VERTEX_SHADER ? "[vertex shader error: ]" : (shaderType == GL_GEOMETRY_SHADER ? "[geometry shader error: ]" : "[fragment shader error: ]")) << message << std::endl;
	}

	return shader_obj;
}

unsigned int createShader(std::string& vs, std::string& fs)
{
	unsigned int program = glCreateProgram();
	unsigned int m_vs = compileShader(vs, GL_VERTEX_SHADER);
	unsigned int m_fs = compileShader(fs, GL_FRAGMENT_SHADER);

	GLcall(glAttachShader(program, m_vs));
	GLcall(glAttachShader(program, m_fs));

	GLcall(glLinkProgram(program));

	int linkStatus;
	GLcall(glGetProgramiv(program, GL_LINK_STATUS, &linkStatus));

	if (linkStatus == GL_FALSE)
	{
		int length;
		GLcall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(int));
		GLcall(glGetProgramInfoLog(program, length, &length, message));
		std::cout <<  "program error -> linker error" << message << std::endl;

		GLcall(glDeleteShader(m_vs));
		GLcall(glDeleteShader(m_fs));

		GLcall(glDetachShader(program, m_vs));
		GLcall(glDetachShader(program, m_fs));
	}

	GLcall(glValidateProgram(program));

	return program;
}

TexturedMesh::TexturedMesh()
{
}

TexturedMesh::TexturedMesh(std::string plyFilePath, std::string bitmapPath)
{
	std::vector<vertexData> vertices; 
	std::vector<triData> faces;
	
	//reads ply files into the vectors
	Models::read_ply_file(plyFilePath, vertices, faces);

	//copies and seprates the positions data from the other struct variables
	std::vector<float> vertex_data;
	for (auto var : vertices)
	{
		vertex_data.push_back(var.position.x);
		vertex_data.push_back(var.position.y);
		vertex_data.push_back(var.position.z);
	}

	//copies and seprates the UV data from other struct variables
	std::vector<float> texcoord_data;
	for (auto var : vertices)
	{
		texcoord_data.push_back(var.UV.s);
		texcoord_data.push_back(var.UV.t);
	}

	face_count = faces.size();

	//create vertex array
	GLcall(glGenVertexArrays(1, &vao_id));
	GLcall(glBindVertexArray(vao_id));

	//creates VBO objects //VERTEXES
	GLcall(glGenBuffers(1, &vertices_vbo_id));
	GLcall(glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo_id));
	GLcall(glBufferData(GL_ARRAY_BUFFER, vertex_data.size() * sizeof(float), vertex_data.data(), GL_STATIC_DRAW));

    //UV
	GLcall(glGenBuffers(1, &texture_vbo_id));
	GLcall(glBindBuffer(GL_ARRAY_BUFFER, texture_vbo_id));
	GLcall(glBufferData(GL_ARRAY_BUFFER, texcoord_data.size() * sizeof(float), texcoord_data.data(), GL_STATIC_DRAW));

    //FACES
	GLcall(glGenBuffers(1, &vertex_faces_id));
	GLcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_faces_id));
	GLcall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(triData), faces.data(), GL_STATIC_DRAW));


    //setting vertex attributes
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo_id));
    GLcall(glEnableVertexAttribArray(0));
    GLcall(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0));

    //setting UV atrtributes
    GLcall(glBindBuffer(GL_ARRAY_BUFFER, texture_vbo_id));
    GLcall(glEnableVertexAttribArray(1));
    GLcall(glVertexAttribPointer(1, 2, GL_FLOAT, false, 0, 0));

    //loading the bmp image
    unsigned char* data = nullptr;
    unsigned int width;
    unsigned int height;
    loadARGB_BMP(bitmapPath.c_str(), &data, &width, &height);

    //creating the texture
    GLcall(glGenTextures(1, &bmp_id));
    GLcall(glActiveTexture(GL_TEXTURE0));
    GLcall(glBindTexture(GL_TEXTURE_2D, bmp_id));

	//sets texture parameters
	GLcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLcall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    GLcall(glBindTexture(GL_TEXTURE_2D, 0));

    //shader code
    std::string VertexShaderCode = R"(
    	#version 330 core

		// Input vertex data, different for all executions of this shader.
		layout(location = 0) in vec3 vertexPosition;
		layout(location = 1) in vec2 uv;

		// Output data ; will be interpolated for each fragment.
		out vec2 uv_out;

		// Values that stay constant for the whole mesh.
		uniform mat4 MVP;

		void main()
        { 
			// Output position of the vertex, in clip space : MVP * position
			gl_Position =  MVP * vec4(vertexPosition, 1);

			// The color will be interpolated to produce the color of each fragment
			uv_out = uv;
		})";

    std::string FragmentShaderCode = R"(
		#version 330 core

		in vec2 uv_out; 
		uniform sampler2D tex;

		void main()
        {
			gl_FragColor = texture(tex, uv_out );
		})";

	//calls create shader function which is defined above
	shader_id = createShader(VertexShaderCode, FragmentShaderCode);
	GLcall(glUseProgram(0));
}

TexturedMesh::~TexturedMesh()
{
}

void TexturedMesh::draw(glm::mat4 MVP)
{
	//binde vao
	GLcall(glBindVertexArray(vao_id));
	//bind program shader
	GLcall(glUseProgram(shader_id));
	//bind texture
	int slot = 1;
	GLcall(glActiveTexture((GL_TEXTURE0 + slot)));
	GLcall(glBindTexture(GL_TEXTURE_2D, bmp_id));
	//bind EBO
	GLcall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_faces_id));

	
	//set MVP  uniform
	GLcall(int loc = glGetUniformLocation(shader_id, "MVP"));
	GLcall(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(MVP) ));

	//set tex  uniform
	GLcall(loc = glGetUniformLocation(shader_id, "tex"));
	GLcall(glUniform1iv(loc, 1, &slot));


	//renders the object using  vertex face buffer 
	GLcall(glDrawElements(GL_TRIANGLES, face_count * 3, GL_UNSIGNED_INT, nullptr));
}
