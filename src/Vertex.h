#pragma once
#include <GL/glew.h>
#include <GL/GL.h>
#include "glm/vec4.hpp"
#include "glm/vec2.hpp"
#include "Parser/CS300Parser.h"


class MyVertex
{
public:
	MyVertex(glm::vec4 Pos, glm::vec2 uv, glm::vec3 N);

	MyVertex();

	glm::vec4 Position;

	glm::vec3 Normal;

	glm::vec2 UV;

};

class Mesh
{
public:

	struct Transform
	{
		std::string name;

		std::string mesh;

		glm::vec3 pos;
		glm::vec3 rot;
		glm::vec3 sca;
	};

	GLuint VAO;
	GLuint VBO;

	std::vector<MyVertex> mesh;
};


class Camera
{
public:

	static float     fovy;
	static float     width;
	static float     height;
	static float     nearPlane;
	static float     farPlane;
	static glm::vec3 camPos;
	static glm::vec3 camTarget;
	static glm::vec3 camUp;

	static void GetCameraValues(CS300Parser parser);
};

class SceneObjs
{
	void LoadObjects(CS300Parser parser);
	std::vector<Mesh> objects;
};

std::vector<MyVertex> CreateCube();

std::vector<MyVertex> CreatePlane();

std::vector<MyVertex> CreateCone(int slices = 4);

std::vector<MyVertex> CreateCylinder(int slices = 4);

std::vector<MyVertex> CreateSphere(int slices = 4);

glm::vec3 ComputeNormal(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3);
