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

struct Transform
{
	std::string name;

	std::string mesh;

	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 sca;
};

struct Texture
{
	Texture();
	~Texture();

	unsigned* texture;
};

class Mesh
{
public:

	void InitializeBuffers();

	Transform transform;

	GLuint VAO;
	GLuint VBO;

	GLuint NormalVAO;
	GLuint NormalVBO;

	int handle;

	std::vector<MyVertex> mesh;
	std::vector<glm::vec4> Normals;
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

	static float r;
	static float theta;
	static float phi;

	static int slices;

	static void GetCameraValues(CS300Parser parser);
	static void CalculateCamPosition();
};

class SceneObjs
{
public:
	void LoadObjects(CS300Parser parser);
	std::vector<MyVertex> LoadTinyObj(const char* filename);
	void ReoadMeshes();

	std::vector<Mesh> objects;
};

std::vector<MyVertex> CreateCube();

std::vector<MyVertex> CreatePlane();

std::vector<MyVertex> CreateCone(int slices = 4);

std::vector<MyVertex> CreateCylinder(int slices = 4);

std::vector<MyVertex> CreateSphere(int slices = 4);

glm::vec3 ComputeNormal(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3);

void CalculateCamPosition();

std::vector<glm::vec4> GetNormalVec(Mesh obj);
