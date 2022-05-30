#include "Vertex.h"
#include "glm/gtc/matrix_transform.hpp"
#include <math.h>
#include <iostream>
#include "tiny_obj_loader.h"

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

void Mesh::InitializeBuffers()
{
	// VAO
	glGenVertexArrays(1, &VAO);

	// VBO
	glGenBuffers(1, &VBO);

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

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	///////////////////////////////////////////////////////
	// VAO
	glGenVertexArrays(1, &NormalVAO);

	// VBO
	glGenBuffers(1, &NormalVBO);

	glBindVertexArray(NormalVAO);
	glBindBuffer(GL_ARRAY_BUFFER, NormalVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * Normals.size(), Normals.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return;
}

void SceneObjs::ReoadMeshes()
{
	int Slices = Camera::slices;

	if (Slices < 4)
	{
		return;
	}

	for (int i = 0; i < objects.size() - 1; i++)
	{
		if (objects[i].transform.mesh == "CONE")
		{
			objects[i].mesh = CreateCone(Slices);
			objects[i].Normals = GetNormalVec(objects[i]);
			objects[i].InitializeBuffers();
		}
		else if (objects[i].transform.mesh == "CYLINDER")
		{
			objects[i].mesh = CreateCylinder(Slices);
			objects[i].Normals = GetNormalVec(objects[i]);
			objects[i].InitializeBuffers();
		}
		else if (objects[i].transform.mesh == "SPHERE")
		{
			objects[i].mesh = CreateSphere(Slices);
			objects[i].Normals = GetNormalVec(objects[i]);
			objects[i].InitializeBuffers();
		}
	
	}
	
}

void SceneObjs::LoadObjects(CS300Parser parser)
{
	for (int i = 0; i < parser.objects.size(); i++)
	{
		Mesh newobj;

		newobj.transform.mesh = parser.objects[i].mesh;
		newobj.transform.name = parser.objects[i].name;
		newobj.transform.pos = parser.objects[i].pos;
		newobj.transform.rot = parser.objects[i].rot;
		newobj.transform.sca = parser.objects[i].sca;

		if (newobj.transform.mesh == "PLANE")
		{
			newobj.mesh = CreatePlane();
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "CUBE")
		{
			newobj.mesh = CreateCube();
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "CONE")
		{
			newobj.mesh = CreateCone(Camera::slices);
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "CYLINDER")
		{
			newobj.mesh = CreateCylinder(Camera::slices);
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "SPHERE")
		{
			newobj.mesh = CreateSphere(Camera::slices);
		}
		if (newobj.transform.mesh == "data/meshes/plane.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/plane.obj");
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "data/meshes/cube_face.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/cube_face.obj");
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "data/meshes/cone_20_face.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/cone_20_face.obj");
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "data/meshes/cylinder_20_face.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/cylinder_20_face.obj");
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "data/meshes/sphere_20_face.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/sphere_20_face.obj");
			newobj.Normals = GetNormalVec(newobj);
		}
		else if (newobj.transform.mesh == "data/meshes/suzanne.obj")
		{
			newobj.mesh = LoadTinyObj("data/meshes/suzanne.obj");
			newobj.Normals = GetNormalVec(newobj);
		}

		newobj.InitializeBuffers();

		newobj.handle = i;

		objects.push_back(newobj);

	}

}

MyVertex::MyVertex()
{
	Normal = glm::vec3();
	UV = glm::vec2();
	Position = glm::vec4();
}

MyVertex::MyVertex(glm::vec4 Pos, glm::vec2 uv, glm::vec3 N)
{
	Position = Pos;

	Normal = N;

	UV = uv;
}


void Camera::GetCameraValues(CS300Parser parser)
{
	fovy = parser.fovy;
	width = parser.width;
	height = parser.height;
	nearPlane = parser.nearPlane;
	farPlane = parser.farPlane;
	camPos = parser.camPos;
	camTarget = parser.camTarget;
	camUp = parser.camUp;

	slices = 4;

	glm::vec3 dist(camPos.x - camTarget.x, camPos.y - camTarget.y, camPos.z - camTarget.z);

	r = glm::sqrt((dist.x * dist.x) + (dist.y * dist.y) + (dist.z * dist.z));
	theta = glm::pi<float>() / 2;
	phi = 0;

	return;
}

glm::vec3 ComputeNormal(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3)
{
	glm::vec3 V1 = pos1 - pos2;

	glm::vec3 V2 = pos1 - pos3;

	return glm::cross(V1, V2);


}

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

std::vector<MyVertex> CreatePlane()
{
	std::vector<MyVertex> plane;

	//Vert 1
	glm::vec4 pos1(-0.5f, -0.5f, 0.0f, 1.0f);

	//Vert 2
	glm::vec4 pos2(0.5f, -0.5f, 0.0f, 1.0f);

	//Vert 3
	glm::vec4 pos3(0.5f, 0.5f, 0.0f, 1.0f);

	//Vert 4
	glm::vec4 pos4(-0.5f, 0.5f, 0.0f, 1.0f);


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

void CalculateCamPosition()
{
	Camera::camPos.x = Camera::r * sin(Camera::theta) * sin(Camera::phi);
	Camera::camPos.y = Camera::r * cos(Camera::theta);
	Camera::camPos.z = Camera::r * sin(Camera::theta) * cos(Camera::phi);

	Camera::camPos += Camera::camTarget;

	return;

}

std::vector<MyVertex> CreateCone(int slices)
{
	std::vector<MyVertex> cone;

	glm::vec4 top(0.0f, 0.5f, 0.0f, 1.0f);

	glm::vec4 bot(0.0f, -0.5f, 0.0f, 1.0f);

	glm::vec4 prevpos(0.5f, -0.5f, 0.0f, 1.0f);

	MyVertex Top;
	MyVertex Bot;
	MyVertex Prev;
	MyVertex last;
	MyVertex NewV;

	Top.Position = top;
	Prev.Position = prevpos;
	Bot.Position = bot;

	float pi = 2.0f * glm::pi<float>();

	float XstepUV = 1/ static_cast<float>(slices);
	float actualUV = 0.0f;

	for (float angle = pi / slices; angle <= pi + 0.05; angle += pi / slices)
	{

		NewV.Position = glm::vec4(cos(angle) * 0.5,  -0.5f, sin(angle) * 0.5, 1.0f);

		NewV.UV = glm::vec2(actualUV + XstepUV ,0.0f);
		Top.UV = glm::vec2(0.5f, 1.0f);
		Prev.UV = glm::vec2(actualUV, 0.0f);

		NewV.Normal = ComputeNormal(NewV.Position, Prev.Position,Top.Position );
		Prev.Normal = ComputeNormal(Prev.Position, Top.Position,NewV.Position );
		Top.Normal = ComputeNormal(Top.Position, NewV.Position,Prev.Position );

		NewV.Normal = glm::normalize(NewV.Normal);
		Prev.Normal = glm::normalize(Prev.Normal);
		Top.Normal = glm::normalize(Top.Normal);
		

		MyVertex botPrev;
		MyVertex botNew;
		botPrev.Position = Prev.Position;
		botNew.Position = NewV.Position;

		botNew.Normal =  ComputeNormal(botNew.Position, Bot.Position, botPrev.Position);
		botPrev.Normal = ComputeNormal(botPrev.Position, botNew.Position, Bot.Position);
		Bot.Normal =     ComputeNormal(Bot.Position, botPrev.Position, botNew.Position);

		botNew.Normal = glm::normalize(botNew.Normal);
		botPrev.Normal = glm::normalize(botPrev.Normal);
		Bot.Normal = glm::normalize(Bot.Normal);


		Bot.UV = glm::vec2(0.5f, 0.0f);
		botNew.UV = glm::vec2(actualUV + XstepUV, 0.0f);
		botPrev.UV = glm::vec2(actualUV, 0.0f);

		cone.push_back(Top);
		cone.push_back(NewV);
		cone.push_back(Prev);

		cone.push_back(Bot);
		cone.push_back(botPrev);
		cone.push_back(botNew);

		Prev = NewV;

		actualUV += XstepUV;
	}


	return cone;
}

std::vector<MyVertex> CreateCylinder(int slices)
{
	std::vector<MyVertex> cylinder;

	glm::vec4 topCenter(0.0f, 0.5f, 0.0f, 1.0f);
	glm::vec4 botCenter(0.0f, -0.5f, 0.0f, 1.0f);

	glm::vec4 prevposBot(0.5f, -0.5f, 0.0f, 1.0f);
	glm::vec4 prevposTop(0.5f, 0.5f, 0.0f, 1.0f);

	MyVertex CenterTop;
	MyVertex CenterBot;
	MyVertex PrevBot;
	MyVertex PrevTop;
	MyVertex NextBot;
	MyVertex NextTop;

	CenterTop.Position = topCenter;
	CenterBot.Position = botCenter;
	PrevBot.Position = prevposBot;
	PrevTop.Position = prevposTop;

	float pi = 2.0f * glm::pi<float>();

	float XstepUV = 1 / static_cast<float>(slices);
	float actualUV = 0.0f;

	for (float angle = pi / slices; angle <= pi +0.05; angle += pi / slices)
	{
		NextBot.Position = glm::vec4(cos(angle) * 0.5, -0.5f,sin(angle) * 0.5 , 1.0f);
		NextTop.Position = glm::vec4(cos(angle) * 0.5, +0.5f,sin(angle) * 0.5 , 1.0f);

		CenterTop.UV = glm::vec2(0.5f, 1.0f);
		CenterBot.UV = glm::vec2(0.5f, 0.0f);
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
		NextBot.Normal = ComputeNormal(NextBot.Position, NextBot.Position,PrevTop.Position );

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

std::vector<MyVertex> CreateSphere(int slices)
{
	float pi = glm::pi<float>();

	float verticalSub = slices / 2;
	float vertAngleStep = pi / verticalSub; 
	float horizAngleStep = 2.0 * pi / slices;
	float vertAngle = 0;
	float horizAngle = 0;

	std::vector<MyVertex> Finalcylinder;

	glm::vec4 topPos(0.0f, 0.5f, 0.0f, 1.0f);
	glm::vec4 botPos(0.0f, -0.5f, 0.0f, 1.0f);

	glm::vec4 prevposBot(0.5f, -0.5f, 0.0f, 1.0f);
	glm::vec4 prevposTop(0.5f, 0.5f, 0.0f, 1.0f);

	MyVertex Top;
	MyVertex Bot;
	MyVertex PrevBot;
	MyVertex PrevTop;
	MyVertex NextBot;
	MyVertex NextTop;
	MyVertex FirstTopPrev;
	MyVertex FirstTopNext;
	MyVertex FirstBotPrev;
	MyVertex FirstBotNext;


	for (int i = 0; i <= slices; i++)
	{
		//get the top triangle
		Top.Position = topPos;
		PrevTop.Position = glm::vec4(sin(vertAngle) * cos(horizAngle) * 0.5, cos(vertAngle) * 0.5, sin(vertAngle) * sin(horizAngle) * 0.5, 1.0f);
		NextTop.Position = glm::vec4(sin(vertAngle) * cos(horizAngle + ( 2.0 * pi / slices)) * 0.5, cos(vertAngle) * 0.5, sin(vertAngle) * sin(horizAngle + (2.0 * pi / slices)) * 0.5, 1.0f);

		Finalcylinder.push_back(Top);
		Finalcylinder.push_back(NextTop);
		Finalcylinder.push_back(PrevTop);

		Bot.Position = botPos;
		PrevBot.Position = PrevTop.Position;
		PrevBot.Position.y *= -1;
		NextBot.Position = NextTop.Position;
		NextBot.Position.y *= -1;

		Finalcylinder.push_back(Bot);
		Finalcylinder.push_back(PrevBot);
		Finalcylinder.push_back(NextBot);

		FirstBotPrev = PrevBot;
		FirstBotNext = NextBot;
		FirstTopPrev = PrevTop;
		FirstTopNext = NextTop;

		//for (int i = 0; i <= verticalSub; i++)
		//{
		//	PrevTop.Position = glm::vec4(sin(vertAngle) * cos(horizAngle) * 0.5, cos(vertAngle) * 0.5, sin(vertAngle) * sin(horizAngle) * 0.5, 1.0f);
		//	PrevTop.Position = glm::vec4(sin(vertAngle) * cos(horizAngle + (2.0 * pi / slices)) * 0.5, cos(vertAngle) * 0.5, sin(vertAngle) * sin(horizAngle + (2.0 * pi / slices)) * 0.5, 1.0f);
		//	
		//	PrevBot.Position = glm::vec4(sin(vertAngle + (pi / verticalSub)) * cos(horizAngle) * 0.5, cos(vertAngle + (pi / verticalSub)) * 0.5, sin(vertAngle + (pi / verticalSub)) * sin(horizAngle) * 0.5, 1.0f);
		//	NextBot.Position = glm::vec4(sin(vertAngle + (pi / verticalSub)) * cos(horizAngle + (2.0 * pi / slices)) * 0.5, cos(vertAngle) * 0.5, sin(vertAngle + (pi / verticalSub)) * sin(horizAngle + (2.0 * pi / slices)) * 0.5, 1.0f);
		//
		//
		//	Finalcylinder.push_back(PrevTop);
		//	Finalcylinder.push_back(PrevBot);
		//	Finalcylinder.push_back(NextBot);
		//
		//	Finalcylinder.push_back(PrevTop);
		//	Finalcylinder.push_back(NextBot);
		//	Finalcylinder.push_back(NextTop);
		//
		//	PrevBot.Position = NextBot.Position;
		//	PrevTop.Position = NextTop.Position;
		//
		//	vertAngle += vertAngleStep;
		//}

		horizAngle += horizAngleStep;

	}
	return Finalcylinder;
}

std::vector<glm::vec4> GetNormalVec(Mesh obj)
{
	std::vector<glm::vec4> normals;

	for (unsigned int i = 0; i < obj.mesh.size(); i++)
	{
		glm::vec4 position = obj.mesh[i].Position;
		glm::vec3 normal = obj.mesh[i].Normal;

		normals.push_back(position);

		glm::vec4 normalEnd(position.x + (normal.x / 2.0), position.y + (normal.y / 2.0), position.z + (normal.z / 2.0), 1.0f);
		normalEnd = position + normalEnd;
		normalEnd = glm::normalize(normalEnd); 


		normals.push_back(( normalEnd / 4.0f));

	}

	return normals;
}

std::vector<MyVertex> SceneObjs::LoadTinyObj(const char* filename)
{
	std::vector<MyVertex> mesh;

	tinyobj::attrib_t atributes;
	std::vector<tinyobj::shape_t> shapes;
	std::string warning;
	std::string error;
	
	bool works = tinyobj::LoadObj(&atributes, &shapes, nullptr,&warning, &error, filename);

	if (!works)
	{
		return std::vector<MyVertex>{};
	}

	tinyobj::shape_t myshape = shapes[0];

	for (unsigned int i = 0; i <= myshape.mesh.indices.size() - 1; i++)
	{
		MyVertex newVert;
		tinyobj::index_t indice = myshape.mesh.indices[i];

		newVert.Position = glm::vec4(atributes.vertices[3 * indice.vertex_index], atributes.vertices[3 * indice.vertex_index + 1], atributes.vertices[3 * indice.vertex_index + 2], 1.0f);
		newVert.Normal = glm::vec3(atributes.normals[3 * indice.normal_index], atributes.normals[3 * indice.normal_index + 1], atributes.normals[3 * indice.normal_index + 2]);
		newVert.UV = glm::vec2(atributes.texcoords[2 * indice.texcoord_index], atributes.texcoords[2 * indice.texcoord_index + 1]);
		mesh.push_back(newVert);
	}

	return mesh;
}

Texture::Texture()
{
	texture = new unsigned[3 * 6 * 6];

	unsigned Blue[3] = {0,0,255};
	unsigned Cyan[3] = { 0,255,255 };
	unsigned Green[3] = { 0,255,0 };
	unsigned Yellow[3] = { 255,255,0 };
	unsigned Red[3] = { 255,0,0 };
	unsigned Purple[3] = { 255,0,255 };

	texture[0] = 0;			texture[3] = 0;         texture[6] = 0;        texture[9] = 225;		texture[12] = 255;		texture[15] = 255;
	texture[1] = 0;			texture[4] = 225;       texture[7] = 225;	   texture[10] = 255;		texture[13] = 0;		texture[16] = 0;
	texture[2] = 225;		texture[5] = 225;       texture[8] = 0;        texture[11] = 0;		    texture[14] = 0;		texture[17] = 255;

	texture[18] = 255;      texture[21] = 0;        texture[24] = 0;       texture[27] = 0;			texture[30] = 225;		texture[33] = 255;
	texture[19] = 0;		texture[22] = 0;        texture[25] = 225;	   texture[28] = 225;		texture[31] = 255;		texture[34] = 0;
	texture[20] = 255;	    texture[23] = 225;      texture[26] = 225;     texture[29] = 0;			texture[32] = 0;		texture[35] = 0;

	texture[36] = 0;        texture[39] = 255;      texture[42] = 0;        texture[45] = 0;		texture[48] = 0;		texture[51] = 225;
	texture[37] = 255;      texture[40] = 0;        texture[43] = 0;		texture[46] = 225;		texture[49] = 0;		texture[52] = 225;
	texture[38] = 0;	    texture[41] = 225;      texture[44] = 225;      texture[47] = 225;		texture[50] = 0;		texture[53] = 0;

	texture[54] = 225;      texture[57] = 0;        texture[60] = 225;      texture[63] = 0;		texture[66] = 0;		texture[69] = 0;
	texture[55] = 225;      texture[58] = 255;      texture[61] = 0;		texture[64] = 0;		texture[67] = 225;		texture[70] = 0;
	texture[56] = 0;		texture[59] = 0;        texture[62] = 225;      texture[65] = 225;		texture[68] = 225;		texture[71] = 0;

	texture[72] = 255;      texture[75] = 225;      texture[78] = 0;        texture[81] = 225;		texture[84] = 0;		texture[87] = 0;
	texture[73] = 0;        texture[76] = 225;      texture[79] = 225;		texture[82] = 0;		texture[85] = 0;		texture[88] = 225;
	texture[74] = 0;	    texture[77] = 0;		texture[80] = 0;        texture[83] = 225;		texture[86] = 225;		texture[89] = 225;

	texture[90] = 255;      texture[93] = 225;      texture[96] = 225;      texture[99] = 0;		texture[102] = 225;		texture[105] = 0;
	texture[91] = 0;        texture[94] = 0;        texture[97] = 225;		texture[100] = 225;		texture[103] = 0;		texture[106] = 0;
	texture[92] = 225;	    texture[95] = 0;        texture[98] = 0;		texture[101] = 0;		texture[104] = 225;		texture[107] = 225;


}

Texture::~Texture()
{
	delete[] texture;
}