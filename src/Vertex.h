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
	float     ns = 10.0f;

	std::vector<Animations::Anim> anims;
};

struct Texture
{
	Texture();
	~Texture();

	unsigned char* texture;
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

class Light
{
public:
	glm::vec3 pos;
	glm::vec3 dir;
	glm::vec3 col;
	glm::vec3 att;
	float     amb = 0.0f;
	float     inner = 0.0f;
	float     outer = 30.0f;
	float     falloff = 1.0f;

	std::string type = "POINT";

	GLuint VAO;
	GLuint VBO;

	void InitializeBuffers();

	std::vector<Animations::Anim> anims;
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

	bool avgNormals = true;

	std::vector<Mesh> objects;
	std::vector<Light> lights;
};

std::vector<MyVertex> CreateCube();

std::vector<MyVertex> CreatePlane();

std::vector<MyVertex> CreateCone(int slices = 4);

std::vector<MyVertex> CreateCylinder(int slices = 4);

std::vector<MyVertex> CreateSphere(int slices = 4);

glm::vec3 ComputeNormal(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3);

void CalculateCamPosition();

std::vector<glm::vec4> GetNormalVec(Mesh obj);

std::vector<glm::vec4> GetAvgNormal(Mesh obj);

std::vector<glm::vec4> CubeAvgNormals(Mesh obj);