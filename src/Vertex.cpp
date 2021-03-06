/**
 * @file Vertex.cpp
 * @author i.urkiza
 * @brief this function contains the main loop to render the assignment and some other 
 * 		  important functions such us the display and clean
 * @version 0.1
 * @date 2022-05-31
 * 
 */
#include "stb/stb_image.h"
#include "Vertex.h"
#include "glm/gtc/matrix_transform.hpp"
#include <math.h>
#include <iostream>
#include "tiny_obj_loader.h"
#include <algorithm>


static bool avgNormal;

float     Camera::fovy;
float     Camera::width;
float     Camera::height;
float     Camera::nearPlane;
float     Camera::farPlane;
glm::vec3 Camera::camPos;
glm::vec3 Camera::camTarget;
glm::vec3 Camera::camUp;
float	  Camera::r;
float	  Camera::theta;
float	  Camera::phi;
int       Camera::slices;


void Light::InitializeBuffers()
{
	// VAO
	glGenVertexArrays(1, &VAO);

	// VBO
	glGenBuffers(1, &VBO);

	//bind VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * mesh.size(), mesh.data(), GL_STATIC_DRAW);

	// Insert the VBO into the VAO
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, UV)));

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return;
}

/**
 * @brief Initializes the VAO and VBO of the mesh and the normals
 * 
 */
void Mesh::InitializeBuffers(bool avgTangents)
{
	// VAO
	glGenVertexArrays(1, &VAO);

	// VBO
	glGenBuffers(1, &VBO);

	//bind VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * mesh.size(), mesh.data(), GL_STATIC_DRAW);

	// Insert the VBO into the VAO
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), 0);

	if (avgTangents)
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, AvgNormal)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, UV)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, AvgTangent)));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, AvgBitangent)));
	}
	else
	{
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, Normal)));
		
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, UV)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, Tangent)));

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, Bitangent)));
	}

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	///////////////////////////////////////////////////////
	// VAO
	glGenVertexArrays(1, &NormalVAO);

	// VBO
	glGenBuffers(1, &NormalVBO);

	glBindVertexArray(NormalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, NormalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * NormalsToDraw.size(), NormalsToDraw.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	///////////////////////////////////////////////////////

	// VAO
	glGenVertexArrays(1, &TangentVAO);

	// VBO
	glGenBuffers(1, &TangentVBO);

	glBindVertexArray(TangentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, TangentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * TangentToDraw.size(), TangentToDraw.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	///////////////////////////////////////////////////////

	// VAO
	glGenVertexArrays(1, &BitangentVAO);

	// VBO
	glGenBuffers(1, &BitangentVBO);

	glBindVertexArray(BitangentVAO);
	glBindBuffer(GL_ARRAY_BUFFER, BitangentVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * BiTangentsToDraw.size(), BiTangentsToDraw.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return;
}

void Mesh::InitializeHeightMap()
{
	int height;
	int with;
	int req;

	auto tex = stbi_load(transform.normalMap.c_str(), &with, &height, &req, 4);

	// Create texture
	glGenTextures(1, &normalMap);
	glBindTexture(GL_TEXTURE_2D, normalMap);

	// Give pixel data to opengl
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, with, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	stbi_image_free(tex);

}

/**
 * @brief reloads the meshes to update the slices
 * 
 */
void SceneObjs::ReoadMeshes()
{
	int Slices = Camera::slices;

	//minimum of slices
	if (Slices < 4)
	{
		return;
	}

	//loop through all the objects 
	for (int i = 0; i < objects.size(); i++)
	{
		//If it??s the shape we want reload the normals, the mesh and 
		//initialize the buffers again
		if (objects[i].transform.mesh == "PLANE")
		{
			if (avgNormals)
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], true);
				TangetBitangentToDraw(objects[i], true);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(true);
			}
			else
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], false);
				TangetBitangentToDraw(objects[i], false);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(false);
			}
		}
		if (objects[i].transform.mesh == "CUBE")
		{
			if (avgNormals)
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], true);
				TangetBitangentToDraw(objects[i], true);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(true);
			}
			else
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], false);
				TangetBitangentToDraw(objects[i], false);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(false);
			}
		}
		if (objects[i].transform.mesh == "CONE")
		{
			objects[i].mesh = CreateCone(Slices);
			if (avgNormals)
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], true);
				TangetBitangentToDraw(objects[i], true);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(true);
			}
			else
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], false);
				TangetBitangentToDraw(objects[i], false);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(false);
			}
		}
		else if (objects[i].transform.mesh == "CYLINDER")
		{
			objects[i].mesh = CreateCylinder(Slices);
			if (avgNormals)
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], true);
				TangetBitangentToDraw(objects[i], true);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(true);
			}
			else
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], false);
				TangetBitangentToDraw(objects[i], false);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(false);
			}
		}
		else if (objects[i].transform.mesh == "SPHERE")
		{
			objects[i].mesh = CreateSphere(Slices);
			if (avgNormals)
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], true);
				TangetBitangentToDraw(objects[i], true);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(true);

			}
			else
			{
				ComputeTangentBitangent(objects[i].mesh);
				GetAvgNormal(objects[i]);
				GetNormalVec(objects[i], false);
				TangetBitangentToDraw(objects[i], false);
				//Initialize VAO and VBO
				objects[i].InitializeBuffers(false);
			}

		}
	
	}
	
}

/**
 * @brief gets all the meshes of the parser, 
 * creates them and initializes the VAO and VBO
 * 
 * @param parser Parser to take the objects
 */
void SceneObjs::LoadObjects(CS300Parser parser)
{
	//go through all the objects
	for (int i = 0; i < parser.objects.size(); i++)
	{
		//new mesh
		Mesh newobj;

		//get the transform of the mesh
		newobj.transform.mesh = parser.objects[i].mesh;
		newobj.transform.name = parser.objects[i].name;
		newobj.transform.OrigPos = parser.objects[i].pos;
		newobj.transform.pos = parser.objects[i].pos;
		newobj.transform.rot = parser.objects[i].rot;
		newobj.transform.sca = parser.objects[i].sca;
		newobj.transform.anims = parser.objects[i].anims;
		newobj.transform.normalMap = parser.objects[i].normalMap;
		newobj.transform.ns = parser.objects[i].ns;
		newobj.transform.ior = parser.objects[i].ior;
		newobj.transform.reflector = parser.objects[i].reflector;


		//depending which object is, load its mesh and normals
		if (newobj.transform.mesh == "PLANE")
		{
			newobj.mesh = CreatePlane();
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, true);
			TangetBitangentToDraw(newobj, true);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(true);

			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "CUBE")
		{
			newobj.mesh = CreateCube();
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, true);
			TangetBitangentToDraw(newobj, true);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(true);

			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "CONE")
		{
			newobj.mesh = CreateCone(Camera::slices);
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, true);
			TangetBitangentToDraw(newobj, true);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(true);
			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "CYLINDER")
		{
			newobj.mesh = CreateCylinder(Camera::slices);
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, true);
			TangetBitangentToDraw(newobj, true);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(true);
			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "SPHERE")
		{
			newobj.mesh = CreateSphere(Camera::slices);
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, true);
			TangetBitangentToDraw(newobj, true);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(true);
			newobj.InitializeHeightMap();
		}
		if (newobj.transform.mesh == "data/meshes/plane.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/plane.obj");
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, false);
			TangetBitangentToDraw(newobj, false);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(false);
			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "data/meshes/cube_face.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/cube_face.obj");
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, false);
			TangetBitangentToDraw(newobj, false);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(false);
			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "data/meshes/cone_20_face.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/cone_20_face.obj");
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, false);
			TangetBitangentToDraw(newobj, false);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(false);
			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "data/meshes/cylinder_20_face.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/cylinder_20_face.obj");
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, false);
			TangetBitangentToDraw(newobj, false);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(false);
			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "data/meshes/sphere_20_face.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/sphere_20_face.obj");
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, false);
			TangetBitangentToDraw(newobj, false);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(false);
			newobj.InitializeHeightMap();
		}
		else if (newobj.transform.mesh == "data/meshes/suzanne.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/suzanne.obj");
			ComputeTangentBitangent(newobj.mesh);
			GetAvgNormal(newobj);
			GetNormalVec(newobj, false);
			TangetBitangentToDraw(newobj, false);
			//Initialize VAO and VBO
			newobj.InitializeBuffers(false);
			newobj.InitializeHeightMap();
		}

		//push back into the objects array
		objects.push_back(newobj);

	}

	for (int i = 0; i < parser.lights.size(); i++)
	{
		Light newLight;

		newLight.Origpos = parser.lights[i].pos;
		newLight.pos = parser.lights[i].pos;
		newLight.amb = parser.lights[i].amb;
		newLight.anims = parser.lights[i].anims;
		newLight.att = parser.lights[i].att;
		newLight.col = parser.lights[i].col;
		newLight.dir = parser.lights[i].dir;
		newLight.falloff = parser.lights[i].falloff;
		newLight.inner = parser.lights[i].inner;
		newLight.outer = parser.lights[i].outer;
		newLight.type = parser.lights[i].type;
		newLight.bias = parser.lights[i].bias;
		newLight.pcf = parser.lights[i].pcf;

		newLight.mesh = LoadTinyObj("data/meshes/sphere_20_face.obj");

		newLight.InitializeBuffers();

		lights.push_back(newLight);
	}

	environmentMap = parser.environmentMap;

	reflective = &objects[0];

}


/**
 * @brief Construct a new My Vertex:: My Vertex object
 * 
 */
MyVertex::MyVertex()
{
	Normal = glm::vec3();
	UV = glm::vec2();
	Position = glm::vec4();
}

/**
 * @brief Construct a new My Vertex:: My Vertex object
 * 
 * @param Pos Position 
 * @param uv UV
 * @param N Normal
 */
MyVertex::MyVertex(glm::vec4 Pos, glm::vec2 uv, glm::vec3 N)
{
	Position = Pos;

	Normal = N;

	UV = uv;
}

/**
 * @brief gets the camera values
 * 
 * @param parser parser to take the data from
 */
void Camera::GetCameraValues(CS300Parser parser)
{
	//get the data
	fovy = parser.fovy;
	width = parser.width;
	height = parser.height;
	nearPlane = parser.nearPlane;
	farPlane = parser.farPlane;
	camPos = parser.camPos;
	camTarget = parser.camTarget;
	camUp = parser.camUp;

	slices = 4;

	//calculate the r, theta and phi
	glm::vec3 dist(camPos.x - camTarget.x, camPos.y - camTarget.y, camPos.z - camTarget.z);

	r = glm::sqrt((dist.x * dist.x) + (dist.y * dist.y) + (dist.z * dist.z));
	theta = glm::pi<float>() / 2;
	phi = 0;

	return;
}

/**
 * @brief Calculates Camera Position
 * 
 */
void CalculateCamPosition()
{
	//get the camera position
	Camera::camPos.x = Camera::r * sin(Camera::theta) * sin(Camera::phi);
	Camera::camPos.y = Camera::r * cos(Camera::theta);
	Camera::camPos.z = Camera::r * sin(Camera::theta) * cos(Camera::phi);

	Camera::camPos += Camera::camTarget;

	return;

}

/**
 * @brief Cpmutes a normal Given three Points
 * 
 * @param pos1 Point 1
 * @param pos2 Point 2
 * @param pos3 Point 3
 * @return glm::vec3 Normal vector
 */
glm::vec3 ComputeNormal(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3)
{
	//generate two vectors
	glm::vec3 V1 = pos1 - pos2;

	glm::vec3 V2 = pos1 - pos3;

	//get the normal
	return glm::cross(V1, V2);
}


void ComputeTangentBitangent(std::vector<MyVertex>& mesh)
{
	// Reset tangents and bitangents to zero vectors

	for (int i = 0; i < mesh.size(); i++)
	{
		mesh[i].Bitangent = glm::vec3(0.0, 0.0, 0.0);
		mesh[i].Tangent = glm::vec3(0.0, 0.0, 0.0);
	}
	// Loop through the triangles (using the index buffer)
	for (int i = 0; i < mesh.size(); i += 3)
	{
		// Solve equations to find T and B for this triangle

		glm::vec3 uv1 = glm::vec3(mesh[i + 1].UV.x - mesh[i].UV.x, mesh[i + 1].UV.y - mesh[i].UV.y, 1.0);
		glm::vec3 uv2 = glm::vec3(mesh[i + 2].UV.x - mesh[i].UV.x, mesh[i + 2].UV.y - mesh[i].UV.y, 1.0);

		glm::vec3 V1 = glm::vec3(mesh[i + 1].Position - mesh[i].Position);
		glm::vec3 V2 = glm::vec3(mesh[i + 2].Position - mesh[i].Position);

		glm::vec3 T;
		glm::vec3 B;

		if (((uv1.y * uv2.x) - (uv2.y * uv1.x)) >= glm::epsilon<float>())
		{
			T = glm::vec3(((uv1.y * (V2)) - (uv2.y * V1)) / ((uv1.y * uv2.x) - (uv2.y * uv1.x)));
			B = glm::vec3(((uv2.x * V1) - (uv1.x * V2)) / ((uv1.y * uv2.x) - (uv2.y * uv1.x)));

		}
		else
		{
			B = glm::vec3(0.0, 1.0, 0.0);
			T = glm::vec3(1.0, 0.0, 0.0);

		}

		// Accumulate tangent/bitangent for the 3 vertices of the triangle (to average after)

		glm::vec3 N = mesh[i].Normal;

		// Set normal to (0,0,1) when lenght is 0
		if (glm::length(N) <= glm::epsilon<float>())
		{
			N = glm::vec3(0.0, 0.0, 1.0);
		}

		// Gram-Schmidt orthogonalization of tangent respect to normal and normalize tangent
		T = T - (((glm::dot(T, N)) / (glm::length(N) * glm::length(N))) * N);


		// Compute the new perpendicular bitangent maintaining the original handeness of the previously

		if (glm::dot(B, glm::cross(N, T)) > 0)
		{
			B = glm::cross(N, T);
		}
		else
		{
			B = -(glm::cross(N, T));
		}


		// computed one (T,B,N need to be normalized and orthogonal at this point)

		mesh[i].Bitangent = glm::normalize(B);
		mesh[i].Tangent = glm::normalize(T);
		mesh[i].Normal = glm::normalize(N);

		mesh[i + 1].Bitangent = glm::normalize(B);
		mesh[i + 1].Tangent = glm::normalize(T);
		mesh[i + 1].Normal = glm::normalize(N);

		mesh[i + 2].Bitangent = glm::normalize(B);
		mesh[i + 2].Tangent = glm::normalize(T);
		mesh[i + 2].Normal = glm::normalize(N);
	}

	// Loop through every vertex


}

void ComputeNormals(std::vector<MyVertex>& mesh)
{
	for (int i = 0; i < mesh.size(); i += 3)
	{
		mesh[i].Normal = ComputeNormal(mesh[i].Position, mesh[i + 1].Position, mesh[i + 2].Position);
		mesh[i + 1].Normal = -(ComputeNormal(mesh[i + 1].Position, mesh[i].Position, mesh[i + 2].Position));
		mesh[i + 2].Normal = -ComputeNormal(mesh[i + 2].Position, mesh[i + 1].Position, mesh[i].Position);

		mesh[i].Normal = glm::normalize(mesh[i].Normal);
		mesh[i + 1].Normal = glm::normalize(mesh[i + 1].Normal);
		mesh[i + 2].Normal = glm::normalize(mesh[i + 2].Normal);

	}
}

/**
 * @brief Create a Cube object
 * 
 * @return std::vector<MyVertex> Mesh with all the vertexes
 */
std::vector<MyVertex> CreateCube()
{
	std::vector<MyVertex> cube;

	//Vert 1
	glm::vec4 pos1(-0.5f, -0.5f, 0.5f, 1.0f);

	//Vert 2
	glm::vec4 pos2(0.5f, -0.5f, 0.5f, 1.0f);

	//Vert 3
	glm::vec4 pos3(0.5f, 0.5f, 0.5f, 1.0f);

	//Vert 4
	glm::vec4 pos4(-0.5f, 0.5f, 0.5f, 1.0f);

	//Vert 5
	glm::vec4 pos5(-0.5f, -0.5f, -0.5f, 1.0f);

	//Vert 6
	glm::vec4 pos6(0.5f, -0.5f, -0.5f, 1.0f);

	//Vert 7
	glm::vec4 pos7(0.5f, 0.5f, -0.5f, 1.0f);

	//Vert 8
	glm::vec4 pos8(-0.5f, 0.5f, -0.5f, 1.0f);


	//UV 1
	glm::vec2 UV1(0.0f, 0.0f);

	//UV 2
	glm::vec2 UV2(0.0f, 1.0f);

	//UV 3
	glm::vec2 UV3(1.0f, 0.0f);

	//UV 4
	glm::vec2 UV4(1.0f, 1.0f);


	//Normal 1
	glm::vec3 nor1(1.0f, 0.0f, 0.0f);

	//Normal 2
	glm::vec3 nor2(0.0f, 1.0f, 0.0f);
								   
	//Normal 3					   
	glm::vec3 nor3(0.0f, 0.0f, 1.0f);
								   
	//Normal 4					   
	glm::vec3 nor4(-1.0f, 0.0f, 0.0);
								   
	//Normal 5					   
	glm::vec3 nor5(0.0f, -1.0f, 0.0);
								   
	//Normal 6					   
	glm::vec3 nor6(0.0f, 0.0f, -1.0);

	//add the vertexes
	{
		MyVertex newVert1(pos3, UV1, nor2);
		cube.push_back(newVert1);

		MyVertex newVert2(pos7, UV3, nor2);
		cube.push_back(newVert2);

		MyVertex newVert3(pos8, UV4, nor2);
		cube.push_back(newVert3);

		MyVertex newVert4(pos3, UV1, nor2);
		cube.push_back(newVert4);

		MyVertex newVert5(pos8, UV4, nor2);
		cube.push_back(newVert5);

		MyVertex newVert6(pos4, UV2, nor2);
		cube.push_back(newVert6);

		MyVertex newVert7(pos1, UV1, nor3);
		cube.push_back(newVert7);

		MyVertex newVert8(pos2, UV3, nor3);
		cube.push_back(newVert8);

		MyVertex newVert9(pos3, UV4, nor3);
		cube.push_back(newVert9);

		MyVertex newVert10(pos1, UV1, nor3);
		cube.push_back(newVert10);

		MyVertex newVert11(pos3, UV4, nor3);
		cube.push_back(newVert11);

		MyVertex newVert12(pos4, UV2, nor3);
		cube.push_back(newVert12);

		MyVertex newVert13(pos2, UV1, nor1);
		cube.push_back(newVert13);

		MyVertex newVert14(pos6, UV3, nor1);
		cube.push_back(newVert14);

		MyVertex newVert15(pos3, UV2, nor1);
		cube.push_back(newVert15);

		MyVertex newVert16(pos6, UV3, nor1);
		cube.push_back(newVert16);

		MyVertex newVert17(pos7, UV4, nor1);
		cube.push_back(newVert17);

		MyVertex newVert18(pos3, UV2, nor1);
		cube.push_back(newVert18);

		MyVertex newVert19(pos6, UV1, nor6);
		cube.push_back(newVert19);

		MyVertex newVert20(pos5, UV3, nor6);
		cube.push_back(newVert20);

		MyVertex newVert21(pos8, UV4, nor6);
		cube.push_back(newVert21);

		MyVertex newVert22(pos6, UV1, nor6);
		cube.push_back(newVert22);

		MyVertex newVert23(pos8, UV4, nor6);
		cube.push_back(newVert23);

		MyVertex newVert24(pos7, UV2, nor6);
		cube.push_back(newVert24);

		MyVertex newVert25(pos5, UV1, nor4);
		cube.push_back(newVert25);

		MyVertex newVert26(pos1, UV3, nor4);
		cube.push_back(newVert26);

		MyVertex newVert27(pos8, UV2, nor4);
		cube.push_back(newVert27);

		MyVertex newVert28(pos1, UV3, nor4);
		cube.push_back(newVert28);

		MyVertex newVert29(pos4, UV4, nor4);
		cube.push_back(newVert29);

		MyVertex newVert30(pos8, UV2, nor4);
		cube.push_back(newVert30);

		MyVertex newVert31(pos1, UV1, nor5);
		cube.push_back(newVert31);

		MyVertex newVert32(pos5, UV3, nor5);
		cube.push_back(newVert32);

		MyVertex newVert33(pos6, UV4, nor5);
		cube.push_back(newVert33);

		MyVertex newVert34(pos1, UV1, nor5);
		cube.push_back(newVert34);

		MyVertex newVert35(pos6, UV4, nor5);
		cube.push_back(newVert35);

		MyVertex newVert36(pos2, UV2, nor5);
		cube.push_back(newVert36);
	}


	return cube;
}

/**
 * @brief Create a Plane object
 * 
 * @return std::vector<MyVertex> Mesh with all the vertexes
 */
std::vector<MyVertex> CreatePlane()
{
	//Vector to return
	std::vector<MyVertex> plane;

	//Vert 1
	glm::vec4 pos1(-0.5f, -0.5f, 0.0f, 1.0f);

	//Vert 2
	glm::vec4 pos2(0.5f, -0.5f, 0.0f, 1.0f);

	//Vert 3
	glm::vec4 pos3(0.5f, 0.5f, 0.0f, 1.0f);

	//Vert 4
	glm::vec4 pos4(-0.5f, 0.5f, 0.0f, 1.0f);


	//get all the vertexes and store them
	MyVertex newVert1(pos3, glm::vec2(1.0f, 1.0f), ComputeNormal( pos3, pos4, pos2));
	plane.push_back(newVert1);

	MyVertex newVert2(pos4, glm::vec2(0.0f, 1.0f), ComputeNormal(pos4, pos2, pos3));
	plane.push_back(newVert2);

	MyVertex newVert3(pos2, glm::vec2(1.0f, 0.0f), ComputeNormal(pos2, pos3, pos4));
	plane.push_back(newVert3);


	MyVertex newVert4(pos1, glm::vec2(0.0f, 0.0f), ComputeNormal(pos1, pos2, pos4));
	plane.push_back(newVert4);


	MyVertex newVert5(pos2, glm::vec2(1.0f, 0.0f), ComputeNormal(pos2, pos4, pos1));
	plane.push_back(newVert5);

	MyVertex newVert6(pos4, glm::vec2(0.0f, 1.0f), ComputeNormal(pos4, pos1, pos2));
	plane.push_back(newVert6);

	return plane;

}

/**
 * @brief Create a Cone object
 * 
 * @param slices slices of the cone
 * @return std::vector<MyVertex> Mesh with all the vertexes
 */
std::vector<MyVertex> CreateCone(int slices)
{	
	//vetex to return
	std::vector<MyVertex> cone;

	//fixed positions and the initial one
	glm::vec4 top(0.0f, 0.5f, 0.0f, 1.0f);

	glm::vec4 bot(0.0f, -0.5f, 0.0f, 1.0f);

	glm::vec4 prevpos(0.5f, -0.5f, 0.0f, 1.0f);

	//vetexes to sotre data
	MyVertex Top;
	MyVertex Bot;
	MyVertex Prev;
	MyVertex last;
	MyVertex NewV;

	//set the positions
	Top.Position = top;
	Prev.Position = prevpos;
	Bot.Position = bot;

	//2 * PI
	const float pi = (2.0f * glm::pi<float>());

	//uv step
	float XstepUV = 1/ static_cast<float>(slices);
	float actualUV = 0.0f;

	
	for (float angle = pi / slices; angle <= pi + 0.05; angle += pi / slices)
	{
		//get a new position
		NewV.Position = glm::vec4(cos(angle) * 0.5,  -0.5f, sin(angle) * 0.5, 1.0f);

		//get the UVs
		NewV.UV = glm::vec2(actualUV + XstepUV ,0.0f);
		Top.UV = glm::vec2(actualUV + (XstepUV / 2), 1.0f);
		Prev.UV = glm::vec2(actualUV, 0.0f);

		//compute normals
		NewV.Normal = ComputeNormal(NewV.Position, Prev.Position,Top.Position );
		Prev.Normal = ComputeNormal(Prev.Position, Top.Position,NewV.Position );
		Top.Normal = ComputeNormal(Top.Position, NewV.Position,Prev.Position );

		//normalize normals
		NewV.Normal = glm::normalize(NewV.Normal);
		Prev.Normal = glm::normalize(Prev.Normal);
		Top.Normal = glm::normalize(Top.Normal);
		
		//bottom triangle
		MyVertex botPrev;
		MyVertex botNew;
		botPrev.Position = Prev.Position;
		botNew.Position = NewV.Position;

		//compute normals
		botNew.Normal =  ComputeNormal(botNew.Position, Bot.Position, botPrev.Position);
		botPrev.Normal = ComputeNormal(botPrev.Position, botNew.Position, Bot.Position);
		Bot.Normal =     ComputeNormal(Bot.Position, botPrev.Position, botNew.Position);

		//normalize normals
		botNew.Normal = glm::normalize(botNew.Normal);
		botPrev.Normal = glm::normalize(botPrev.Normal);
		Bot.Normal = glm::normalize(Bot.Normal);

		//get the UVs
		Bot.UV = glm::vec2(actualUV + (XstepUV / 2), 0.0f);
		botNew.UV = glm::vec2(actualUV + XstepUV, 0.0f);
		botPrev.UV = glm::vec2(actualUV, 0.0f);

		//push all the verices to the vector
		cone.push_back(Top);
		cone.push_back(NewV);
		cone.push_back(Prev);

		cone.push_back(Bot);
		cone.push_back(botPrev);
		cone.push_back(botNew);

		//update the UV and the position
		Prev = NewV;

		actualUV += XstepUV;
	}


	return cone;
}

/**
 * @brief Create a Cylinder object
 * 
 * @param slices slices of the Cylinder
 * @return std::vector<MyVertex> Mesh with all the vertexes
 */
std::vector<MyVertex> CreateCylinder(int slices)
{
	//vetex to return
	std::vector<MyVertex> cylinder;

	//fixed positions and the initial ones
	glm::vec4 topCenter(0.0f, 0.5f, 0.0f, 1.0f);
	glm::vec4 botCenter(0.0f, -0.5f, 0.0f, 1.0f);

	glm::vec4 prevposBot(0.5f, -0.5f, 0.0f, 1.0f);
	glm::vec4 prevposTop(0.5f, 0.5f, 0.0f, 1.0f);

	//vetexes to sotre data
	MyVertex CenterTop;
	MyVertex CenterBot;
	MyVertex PrevBot;
	MyVertex PrevTop;
	MyVertex NextBot;
	MyVertex NextTop;
	
	//set positions
	CenterTop.Position = topCenter;
	CenterBot.Position = botCenter;
	PrevBot.Position = prevposBot;
	PrevTop.Position = prevposTop;

	//2 * PI
	const float pi = 2.0f * glm::pi<float>();

	//UV step and uv
	float XstepUV = 1 / static_cast<float>(slices);
	float actualUV = 0.0f;


	for (float angle = pi / slices; angle <= pi +0.05; angle += pi / slices)
	{
		//get new positions
		NextBot.Position = glm::vec4(cos(angle) * 0.5, -0.5f,sin(angle) * 0.5 , 1.0f);
		NextTop.Position = glm::vec4(cos(angle) * 0.5, +0.5f,sin(angle) * 0.5 , 1.0f);

		//get the UVs
		CenterTop.UV = glm::vec2(actualUV + (XstepUV / 2), 1.0f);
		CenterBot.UV = glm::vec2(actualUV + (XstepUV / 2), 0.0f);
		PrevBot.UV = glm::vec2(actualUV, 0.0f);
		PrevTop.UV = glm::vec2(actualUV, 1.0f);
		NextBot.UV = glm::vec2(actualUV + XstepUV, 0.0f);
		NextTop.UV = glm::vec2(actualUV + XstepUV, 1.0f);

		//top triangle
		CenterTop.Normal = ComputeNormal(CenterTop.Position, NextTop.Position, PrevTop.Position );
		PrevTop.Normal = ComputeNormal(PrevTop.Position, CenterTop.Position,NextTop.Position );
		NextTop.Normal = ComputeNormal(NextTop.Position, PrevTop.Position,CenterTop.Position );

		CenterTop.Normal = glm::normalize(CenterTop.Normal);
		PrevTop.Normal = glm::normalize(PrevTop.Normal);
		NextTop.Normal = glm::normalize(NextTop.Normal);

		cylinder.push_back(CenterTop);
		cylinder.push_back(NextTop);
		cylinder.push_back(PrevTop);

		//bot Triangle

		CenterBot.Normal = ComputeNormal(CenterBot.Position, PrevBot.Position, NextBot.Position);
		PrevBot.Normal = ComputeNormal(PrevBot.Position, NextBot.Position, CenterBot.Position);
		NextBot.Normal = ComputeNormal(NextBot.Position, CenterBot.Position, PrevBot.Position);

		CenterBot.Normal = glm::normalize(CenterBot.Normal);
		PrevBot.Normal = glm::normalize(PrevBot.Normal);
		NextBot.Normal = glm::normalize(NextBot.Normal);

		cylinder.push_back(CenterBot);
		cylinder.push_back(PrevBot);
		cylinder.push_back(NextBot);

		//mid triangle 1

		PrevTop.Normal = ComputeNormal(PrevTop.Position, NextBot.Position,PrevBot.Position );
		PrevBot.Normal = ComputeNormal(PrevBot.Position, PrevTop.Position,NextBot.Position );
		NextBot.Normal = ComputeNormal(NextBot.Position, PrevBot.Position,PrevTop.Position );

		PrevTop.Normal = glm::normalize(PrevTop.Normal);
		PrevBot.Normal = glm::normalize(PrevBot.Normal);
		NextBot.Normal = glm::normalize(NextBot.Normal);

		cylinder.push_back(NextBot);
		cylinder.push_back(PrevBot);
		cylinder.push_back(PrevTop);

		//mid triangle 2

		NextBot.Normal = ComputeNormal(NextBot.Position, PrevTop.Position, NextTop.Position);
		NextTop.Normal = ComputeNormal(NextTop.Position,NextBot.Position , PrevTop.Position);
		PrevTop.Normal = ComputeNormal(PrevTop.Position,NextTop.Position , NextBot.Position);

		NextBot.Normal = glm::normalize(NextBot.Normal);
		NextTop.Normal = glm::normalize(NextTop.Normal);
		PrevTop.Normal = glm::normalize(PrevTop.Normal);

		cylinder.push_back(PrevTop);
		cylinder.push_back(NextTop);
		cylinder.push_back(NextBot);

		//set the next to the previous

		PrevBot = NextBot;
		PrevTop = NextTop;

		actualUV += XstepUV;

	}

	return cylinder;

}

/**
 * @brief Create a Sphere object
 * 
 * @param slices slices of the Sphere
 * @return std::vector<MyVertex> Mesh with all the vertexes
 */
std::vector<MyVertex> CreateSphere(int slices)
{
	//PI
	float pi = glm::pi<float>();

	//calculate subdivisions and angle steps
	int verticalSub = static_cast<int>(floor(slices / 2.0f));
	float vertAngleStep = pi / verticalSub; 
	float horizAngleStep = static_cast<float>((2.0 * pi) / slices);
	float vertAngle = 0;
	float horizAngle = 0;

	//vector to store data
	std::vector<MyVertex> Finalcylinder;

	//vertexes to store the data
	MyVertex PrevBot;
	MyVertex PrevTop;
	MyVertex NextBot;
	MyVertex NextTop;




	// iterate through all the horizontal slices and trough all the vertical slices 
	for (int i = 0; i < slices ; i++)
	{
		vertAngle = 0;

		for (int j = 0; j < verticalSub ; j++)
		{
			
			//get the position with spherical coordinates
			PrevBot.Position = glm::vec4( 0.5 * glm::sin(vertAngle) * glm::sin( horizAngle), 0.5 * glm::cos(vertAngle) , 0.5 * glm::sin(vertAngle) * glm::cos(horizAngle), 1.0 );
			PrevTop.Position = glm::vec4(0.5 * glm::sin(vertAngle + vertAngleStep) * glm::sin(horizAngle), 0.5 * glm::cos(vertAngle + vertAngleStep), 0.5 * glm::sin(vertAngle + vertAngleStep) * glm::cos(horizAngle), 1.0);
			
			NextBot.Position = glm::vec4(0.5 * glm::sin(vertAngle) * glm::sin(horizAngle + horizAngleStep), 0.5 * glm::cos(vertAngle), 0.5 * glm::sin(vertAngle) * glm::cos(horizAngle + horizAngleStep), 1.0);
			NextTop.Position = glm::vec4(0.5 * glm::sin(vertAngle + vertAngleStep) * glm::sin(horizAngle + horizAngleStep), 0.5 * glm::cos(vertAngle + vertAngleStep), 0.5 * glm::sin(vertAngle + vertAngleStep) * glm::cos(horizAngle + horizAngleStep), 1.0);


			//get the UVs
			NextBot.UV = glm::vec2((horizAngle + horizAngleStep) / (2.0 * pi),  vertAngle/ pi);
			NextTop.UV = glm::vec2((horizAngle + horizAngleStep) / (2.0 * pi),  (vertAngle + vertAngleStep)/ pi);

			PrevTop.UV = glm::vec2(horizAngle / (2.0 * pi),  (vertAngle + vertAngleStep)/ pi);
			PrevBot.UV = glm::vec2(horizAngle / (2.0 * pi),  vertAngle/ pi);

			//get the Normals
			PrevBot.Normal = ComputeNormal(PrevBot.Position, PrevTop.Position, NextBot.Position);
			PrevTop.Normal = ComputeNormal(PrevTop.Position, NextBot.Position, PrevBot.Position);
			NextBot.Normal = ComputeNormal(NextBot.Position, PrevBot.Position, PrevTop.Position);

			PrevBot.Normal = glm::normalize(PrevBot.Normal);
			PrevTop.Normal = glm::normalize(PrevTop.Normal);
			NextBot.Normal = glm::normalize(NextBot.Normal);

			//push back all the vertices
			Finalcylinder.push_back(PrevTop);
			Finalcylinder.push_back(NextBot);
			Finalcylinder.push_back(PrevBot);


			NextBot.Normal = ComputeNormal(NextBot.Position, PrevTop.Position, NextTop.Position);
			PrevTop.Normal = ComputeNormal(PrevTop.Position, NextTop.Position, NextBot.Position);
			NextTop.Normal = ComputeNormal(NextTop.Position, NextBot.Position, PrevTop.Position);

			NextBot.Normal = glm::normalize(NextBot.Normal);
			PrevTop.Normal = glm::normalize(PrevTop.Normal);
			NextTop.Normal = glm::normalize(NextTop.Normal);

			Finalcylinder.push_back(NextBot);
			Finalcylinder.push_back(PrevTop);
			Finalcylinder.push_back(NextTop);


			
			
			//update vertical angle
			vertAngle += (vertAngleStep);
		}

		//update horizontal Angle
		horizAngle += horizAngleStep;

	}
	return Finalcylinder;
}

/**
 * @brief gets the average normals of the cube
 * 
 * @param obj cube to get the normals from
 * @return std::vector<glm::vec4> vector of the normals
 */
std::vector<glm::vec4> CubeAvgNormals(Mesh& obj)
{
	//vector for positions
	std::vector<glm::vec4> normals;

	//Vert 1
	glm::vec4 pos1(-0.5f, -0.5f, 0.5f, 1.0f);

	//Vert 2
	glm::vec4 pos2(0.5f, -0.5f, 0.5f, 1.0f);

	//Vert 3
	glm::vec4 pos3(0.5f, 0.5f, 0.5f, 1.0f);

	//Vert 4
	glm::vec4 pos4(-0.5f, 0.5f, 0.5f, 1.0f);

	//Vert 5
	glm::vec4 pos5(-0.5f, -0.5f, -0.5f, 1.0f);

	//Vert 6
	glm::vec4 pos6(0.5f, -0.5f, -0.5f, 1.0f);

	//Vert 7
	glm::vec4 pos7(0.5f, 0.5f, -0.5f, 1.0f);

	//Vert 8
	glm::vec4 pos8(-0.5f, 0.5f, -0.5f, 1.0f);

	// get the normals
	glm::vec4 nor1 = pos1 - glm::vec4(0.0, 0.0, 0.0, 1.0);
	nor1 = glm::normalize(nor1);
	glm::vec4 nor2 = pos2 - glm::vec4(0.0, 0.0, 0.0, 1.0);
	nor2 = glm::normalize(nor2);
	glm::vec4 nor3 = pos3 - glm::vec4(0.0, 0.0, 0.0, 1.0);
	nor3 = glm::normalize(nor3);
	glm::vec4 nor4 = pos4 - glm::vec4(0.0, 0.0, 0.0, 1.0);
	nor4 = glm::normalize(nor4);
	glm::vec4 nor5 = pos5 - glm::vec4(0.0, 0.0, 0.0, 1.0);
	nor5 = glm::normalize(nor5);
	glm::vec4 nor6 = pos6 - glm::vec4(0.0, 0.0, 0.0, 1.0);
	nor6 = glm::normalize(nor6);
	glm::vec4 nor7 = pos7 - glm::vec4(0.0, 0.0, 0.0, 1.0);
	nor7 = glm::normalize(nor7);
	glm::vec4 nor8 = pos8 - glm::vec4(0.0, 0.0, 0.0, 1.0);
	nor8 = glm::normalize(nor8);

	//push back the positions
	normals.push_back(pos1);
	normals.push_back(pos1 + (nor1 / 4.0f));

	normals.push_back(pos2);
	normals.push_back(pos2 + (nor2 / 4.0f));

	normals.push_back(pos3);
	normals.push_back(pos3 + (nor3 / 4.0f));

	normals.push_back(pos4);
	normals.push_back(pos4 + (nor4 / 4.0f));

	normals.push_back(pos5);
	normals.push_back(pos5 + (nor5 / 4.0f));

	normals.push_back(pos6);
	normals.push_back(pos6 + (nor6 / 4.0f));

	normals.push_back(pos7);
	normals.push_back(pos7 + (nor7 / 4.0f));

	normals.push_back(pos8);
	normals.push_back(pos8 + (nor8 / 4.0f));

	return normals;

}

/**
 * @brief Get the sphere average normals
 * 
 * @param obj sphere to get the normals from
 * @return std::vector<glm::vec4> vector of normals
 */
void ShereAvgNormals(Mesh& obj)
{
	
	
	//iterate through all the points and get the normals
	for (unsigned int i = 0; i < obj.mesh.size(); i++)
	{
		glm::vec4 nor1 = obj.mesh[i].Position - glm::vec4(0.0, 0.0, 0.0, 1.0);
		nor1 = glm::normalize(nor1);

		//push back the positions
		obj.mesh[i].AvgNormal = nor1;
	}

	return;
}

/**
 * @brief Get the Avg Normals of object
 * 
 * @param obj obj to get the normals from
 * @return std::vector<glm::vec4> vector of positions
 */
void GetAvgNormal(Mesh& obj)
{
	obj.BiTangentsToDraw.clear();
	obj.TangentToDraw.clear();
	obj.NormalsToDraw.clear();

	if (obj.transform.mesh == "SPHERE")
	{
		ShereAvgNormals(obj);
		return;
	}

	//go through all the Positions 
	for (unsigned int i = 0; i < obj.mesh.size(); i++)
	{
		//store the position
		glm::vec4 pos = obj.mesh[i].Position;
		std::vector<glm::vec3> Normals;
		std::vector<glm::vec3> Tangents;
		std::vector<glm::vec3> BiTangents;

		Normals.push_back(obj.mesh[i].Normal);
		Tangents.push_back(obj.mesh[i].Tangent);
		BiTangents.push_back(obj.mesh[i].Bitangent);

		//iterate through the mesh again
		for (unsigned int j = 0; j < obj.mesh.size(); j++)
		{
			//same vertex
			if (obj.mesh[j].Position.x - pos.x <= glm::epsilon<float>() && obj.mesh[j].Position.y - pos.y <= glm::epsilon<float>() && obj.mesh[j].Position.z - pos.z <= glm::epsilon<float>())
			{
				bool newNormal = true; 
				//check if same normal is stored
				//for (int k = 0; k < Normals.size(); k++)
				//{
				//	if ((obj.mesh[j].Normal) == Normals[k])
				//	{
				//		newNormal = false;
				//	}
				//}
				if (newNormal)
				{
					Normals.push_back(glm::uvec4(obj.mesh[j].Normal, 1.0));
					Tangents.push_back(glm::uvec4(obj.mesh[j].Tangent, 1.0));
					BiTangents.push_back(glm::uvec4(obj.mesh[j].Bitangent, 1.0));
				}
			}
		}

		glm::vec3 AvgNorm(0.0, 0.0, 0.0);
		glm::vec3 AvgTan(0.0, 0.0, 0.0);
		glm::vec3 AvgBiTan(0.0, 0.0, 0.0);

		//get the average normal and store it
		for (auto it = Normals.begin(); it != Normals.end(); it++)
		{
			AvgNorm += *it;
		}
		for (auto it = Tangents.begin(); it != Tangents.end(); it++)
		{
			AvgTan += *it;
		}
		for (auto it = BiTangents.begin(); it != BiTangents.end(); it++)
		{
			AvgBiTan += *it;
		}

		AvgNorm /= Normals.size();
		AvgNorm = glm::normalize(AvgNorm);
		//AvgNorm = -AvgNorm;
		obj.mesh[i].AvgNormal = AvgNorm;
		
		AvgTan /= Tangents.size();
		AvgTan = glm::normalize(AvgTan);
		//AvgNorm = -AvgNorm;
		obj.mesh[i].AvgTangent = AvgTan;
		
		AvgBiTan /= BiTangents.size();
		AvgBiTan = glm::normalize(AvgBiTan);
		//AvgNorm = -AvgNorm;
		obj.mesh[i].AvgBitangent = AvgBiTan;

	}
}

void TangetBitangentToDraw(Mesh& obj, bool drawAverage)
{
	obj.BiTangentsToDraw.clear();
	obj.TangentToDraw.clear();

	for (int i = 0; i < obj.mesh.size(); i++)
	{
		glm::vec4 position;
		glm::vec3 bitangent;
		glm::vec3 tangent;

		if (drawAverage)
		{
			position = obj.mesh[i].Position;
			bitangent = obj.mesh[i].AvgBitangent;
			tangent = obj.mesh[i].AvgTangent;
		}
		else
		{
			position = obj.mesh[i].Position;
			bitangent = obj.mesh[i].Bitangent;
			tangent = obj.mesh[i].Tangent;
		}
		

		glm::vec4 tangentEnd(position.x + (tangent.x / 2.0), position.y + (tangent.y / 2.0), position.z + (tangent.z / 2.0), 1.0f);
		glm::vec4 bitangentEnd(position.x + (bitangent.x / 2.0), position.y + (bitangent.y / 2.0), position.z + (bitangent.z / 2.0), 1.0f);

		tangentEnd = position + tangentEnd;
		tangentEnd = glm::normalize(tangentEnd);

		bitangentEnd = position + bitangentEnd;
		bitangentEnd = glm::normalize(bitangentEnd);


		obj.BiTangentsToDraw.push_back(position);
		obj.BiTangentsToDraw.push_back(bitangentEnd);


		obj.TangentToDraw.push_back(position);
		obj.TangentToDraw.push_back(tangentEnd);
	}
	


}

/**
 * @brief Get the Normal Vectors of object
 * 
 * @param obj object to take the positions from
 * @return std::vector<glm::vec4> vector of positions
 */
void GetNormalVec(Mesh& obj, bool drawAverage)
{
	//vector of positions
	std::vector<glm::vec4> normals;

	obj.NormalsToDraw.clear();

	//go through all the mesh
	for (unsigned int i = 0; i < obj.mesh.size(); i++)
	{
		//get the position of the vertex and its normal
		glm::vec4 position;
		glm::vec3 normal;

		if (drawAverage)
		{
			position = obj.mesh[i].Position;
			normal = obj.mesh[i].AvgNormal;
		}
		else
		{
			position = obj.mesh[i].Position;
			normal = obj.mesh[i].Normal;
		}

		//push back the position
		obj.NormalsToDraw.push_back(position);

		//push back position + normal
		glm::vec4 normalEnd(position.x + (normal.x / 2.0), position.y + (normal.y / 2.0), position.z + (normal.z / 2.0), 1.0f);
		normalEnd = position + normalEnd;
		normalEnd = glm::normalize(normalEnd); 


		obj.NormalsToDraw.push_back(( normalEnd / 4.0f));
	}
}

/**
 * @brief load the meshes from the files
 * 
 * @param filename file to load the data from
 * @return std::vector<MyVertex> mesh
 */
std::vector<MyVertex> SceneObjs::LoadTinyObj(const char* filename)
{
	//mesh
	std::vector<MyVertex> mesh;

	//variables needed to load the mesh with tiny obj
	tinyobj::attrib_t atributes;
	std::vector<tinyobj::shape_t> shapes;
	std::string warning;
	std::string error;
	
	//load the atributes and the shapes
	bool works = tinyobj::LoadObj(&atributes, &shapes, nullptr,&warning, &error, filename);

	//check if it works
	if (!works)
	{
		return std::vector<MyVertex>{};
	}

	//get the mesh
	tinyobj::shape_t myshape = shapes[0];

	//go through all the indices getting the data 
	for (unsigned int i = 0; i <= myshape.mesh.indices.size() - 1; i++)
	{
		//get the data and push it back
		MyVertex newVert;
		tinyobj::index_t indice = myshape.mesh.indices[i];

		newVert.Position = glm::vec4(atributes.vertices[(3 * static_cast<int>(indice.vertex_index))], atributes.vertices[(3 * indice.vertex_index + 1)], atributes.vertices[3 * indice.vertex_index + 2], 1.0f);
		newVert.Normal = glm::vec3(atributes.normals[(3 * static_cast<int>(indice.normal_index))], atributes.normals[3 * indice.normal_index + 1], atributes.normals[3 * indice.normal_index + 2]);
		newVert.UV = glm::vec2(atributes.texcoords[static_cast<unsigned int>(2 * indice.texcoord_index)], atributes.texcoords[2 * indice.texcoord_index + 1]);
		mesh.push_back(newVert);
	}

	return mesh;
}

/**
 * @brief Construct a new Texture:: Texture object
 * 
 */
Texture::Texture()
{
	texture = new unsigned char[4 * 6 * 6];

	//Create the texture
	texture[120] = 0;        texture[124] = 0;        texture[128] = 0;       texture[132] = 255;	 texture[136] = 225;	texture[140] = 255;
	texture[121] = 0;        texture[125] = 255;      texture[129] = 225;	  texture[133] = 225;	 texture[137] = 0;		texture[141] = 0;
	texture[122] = 225;	     texture[126] = 255;      texture[130] = 0;		  texture[134] = 0;		 texture[138] = 0;	    texture[142] = 225;
	texture[123] = 255;		 texture[127] = 225;      texture[131] = 255;     texture[135] = 255;	 texture[139] = 255;	texture[143] = 255;

	texture[96] = 0;		texture[100] = 0;        texture[104] = 255;      texture[108] = 225;	texture[112] = 255;		texture[116] = 0;
	texture[97] = 255;      texture[101] = 225;      texture[105] = 225;	 texture[109] = 0;		texture[113] = 0;		texture[117] = 0;
	texture[98] = 255;	    texture[102] = 0;		 texture[106] = 0;       texture[110] = 0;		texture[114] = 225;		texture[118] = 225;
	texture[99] = 255;		texture[103] = 225;      texture[107] = 255;     texture[111] = 255;	texture[115] = 255;		texture[119] = 255;

	texture[72] = 0;        texture[76] = 255;      texture[80] = 225;      texture[84] = 255;		texture[88] = 0;		texture[92] = 0;
	texture[73] = 225;      texture[77] = 255;      texture[81] = 0;		texture[85] = 0;		texture[89] = 0;		texture[93] = 255;
	texture[74] = 0;		texture[78] = 0;        texture[82] = 0;        texture[86] = 225;		texture[90] = 225;		texture[94] = 255;
	texture[75] = 255;		texture[79] = 225;      texture[83] = 255;      texture[87] = 255;		texture[91] = 255;		texture[95] = 255;

	texture[48] = 255;      texture[52] = 255;      texture[56] = 255;        texture[60] = 0;		texture[64] = 0;		texture[68] = 0;
	texture[49] = 255;      texture[53] = 0;        texture[57] = 0;		texture[61] = 0;		texture[65] = 255;		texture[69] = 225;
	texture[50] = 0;	    texture[54] = 0;        texture[58] = 225;      texture[62] = 225;		texture[66] = 255;		texture[70] = 0;
	texture[51] = 255;		texture[55] = 225;      texture[59] = 255;       texture[63] = 255;		texture[67] = 255;		texture[71] = 255;

	texture[24] = 255;      texture[28] = 255;      texture[32] = 0;         texture[36] = 0;	    texture[40] = 0;		texture[44] = 255;
	texture[25] = 0;		texture[29] = 0;        texture[33] = 0;	     texture[37] = 225;		texture[41] = 255;		texture[45] = 255;
	texture[26] = 0;	    texture[30] = 225;      texture[34] = 225;       texture[38] = 255;		texture[42] = 0;		texture[46] = 0;
	texture[27] = 255;		texture[31] = 225;      texture[35] = 255;       texture[39] = 255;	    texture[43] = 255;		texture[47] = 255;

	texture[0] = 255;		texture[4] = 0;         texture[8] = 0;         texture[12] = 0;		texture[16] = 255;		texture[20] = 255;
	texture[1] = 0;		    texture[5] = 0;         texture[9] = 225;	    texture[13] = 255;		texture[17] = 255;		texture[21] = 0;
	texture[2] = 255;		texture[6] = 225;       texture[10] =255;        texture[14] = 0;		texture[18] = 0;		texture[22] = 0;
	texture[3] = 255;		texture[7] = 225;       texture[11] = 255;      texture[15] = 255;		texture[19] =255;		texture[23] = 255;

}

/**
 * @brief Destroy the Texture:: Texture object
 * 
 */
Texture::~Texture()
{
	delete[] texture;
}

Skybox::Skybox(SceneObjs& scene)
{
	mesh = CreateCube();

	// VAO
	glGenVertexArrays(1, &VAO);

	// VBO
	glGenBuffers(1, &VBO);

	//bind VAO
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * mesh.size(), mesh.data(), GL_STATIC_DRAW);

	// Insert the VBO into the VAO
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, UV)));

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return;

}