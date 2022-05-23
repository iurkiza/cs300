#include "Vertex.h"
#include "glm/gtc/matrix_transform.hpp"

float     Camera::fovy;
float     Camera::width;
float     Camera::height;
float     Camera::nearPlane;
float     Camera::farPlane;
glm::vec3 Camera::camPos;
glm::vec3 Camera::camTarget;
glm::vec3 Camera::camUp;


void LoadObjects(CS300Parser parser)
{
	for (int i = 0; i <= parser.objects.size(); i++)
	{




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
	camUp = camUp;

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

		MyVertex newVert24(pos3, UV2, nor6);
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

std::vector<MyVertex> CreateCone(int slices)
{
	std::vector<MyVertex> cone;

	glm::vec4 top(0.0f, 0.0f, 0.5f, 1.0f);

	glm::vec4 bot(0.0f, 0.0f, -0.5f, 1.0f);

	glm::vec4 prevpos(0.5f, 0.0f, -0.5f, 1.0f);

	MyVertex Top;
	MyVertex Bot;
	MyVertex Prev;
	MyVertex last;
	MyVertex NewV;

	Top.Position = top;
	Prev.Position = prevpos;
	Bot.Position = bot;

	float pi = 2.0f * glm::pi<float>();

	for (float angle = pi / slices; angle <= pi + (pi / slices); angle += pi / slices)
	{

		NewV.Position = glm::vec4(cos(angle) * 0.5, sin(angle) * 0.5, -0.5f, 1.0f);

		NewV.UV = glm::vec2(0.0f ,0.0f);
		Top.UV = glm::vec2(0.0f, 0.0f);
		Prev.UV = glm::vec2(0.0f, 0.0f);

		NewV.Normal = ComputeNormal(NewV.Position, Top.Position, Prev.Position);
		Prev.Normal = ComputeNormal(Prev.Position, NewV.Position, Top.Position);
		Top.Normal = ComputeNormal(Top.Position, Prev.Position, NewV.Position);

		MyVertex botPrev;
		MyVertex botNew;
		botPrev.Position = Prev.Position;
		botNew.Position = NewV.Position;

		botNew.Normal = ComputeNormal(botNew.Position, Bot.Position, botPrev.Position);
		botPrev.Normal = ComputeNormal(botPrev.Position, botNew.Position, Bot.Position);
		Bot.Normal = ComputeNormal(Bot.Position, botPrev.Position, botNew.Position);

		cone.push_back(Top);
		cone.push_back(Prev);
		cone.push_back(NewV);

		cone.push_back(Bot);
		cone.push_back(botPrev);
		cone.push_back(botNew);

		Prev = NewV;
	}


	return cone;
}

std::vector<MyVertex> CreateCylinder(int slices)
{
	std::vector<MyVertex> cylinder;

	glm::vec4 topCenter(0.0f, 0.0f, 0.5f, 1.0f);
	glm::vec4 botCenter(0.0f, 0.0f, -0.5f, 1.0f);

	glm::vec4 prevposBot(0.5f, 0.0f, -0.5f, 1.0f);
	glm::vec4 prevposTop(0.5f, 0.0f, 0.5f, 1.0f);

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

	for (float angle = pi / slices; angle <= pi + (pi / slices); angle += pi / slices)
	{
		NextBot.Position = glm::vec4(cos(angle) * 0.5, sin(angle) * 0.5, -0.5f, 1.0f);
		NextTop.Position = glm::vec4(cos(angle) * 0.5, sin(angle) * 0.5, +0.5f, 1.0f);

		CenterTop.UV = glm::vec2(0.0f, 0.0f);
		CenterBot.UV = glm::vec2(0.0f, 0.0f);
		PrevBot.UV = glm::vec2(0.0f, 0.0f);
		PrevTop.UV = glm::vec2(0.0f, 0.0f);
		NextBot.UV = glm::vec2(0.0f, 0.0f);
		NextTop.UV = glm::vec2(0.0f, 0.0f);

		//top triangle
		CenterTop.Normal = ComputeNormal(CenterTop.Position, PrevTop.Position, NextTop.Position);
		PrevTop.Normal = ComputeNormal(PrevTop.Position, NextTop.Position, CenterTop.Position);
		NextTop.Normal = ComputeNormal(NextTop.Position, CenterTop.Position, PrevTop.Position);

		cylinder.push_back(CenterTop);
		cylinder.push_back(PrevTop);
		cylinder.push_back(NextTop);

		//bot Triangle

		CenterBot.Normal = ComputeNormal(CenterBot.Position, PrevBot.Position, NextBot.Position);
		PrevBot.Normal = ComputeNormal(PrevBot.Position, NextBot.Position, CenterBot.Position);
		NextBot.Normal = ComputeNormal(NextBot.Position, CenterBot.Position, PrevBot.Position);

		cylinder.push_back(CenterBot);
		cylinder.push_back(PrevBot);
		cylinder.push_back(NextBot);

		//mid triangle 1

		PrevTop.Normal = ComputeNormal(PrevTop.Position, PrevBot.Position, NextBot.Position);
		PrevBot.Normal = ComputeNormal(PrevBot.Position, NextBot.Position, PrevTop.Position);
		NextBot.Normal = ComputeNormal(NextBot.Position, PrevTop.Position, NextBot.Position);

		cylinder.push_back(PrevTop);
		cylinder.push_back(PrevBot);
		cylinder.push_back(NextBot);

		//mid triangle 2

		NextBot.Normal = ComputeNormal(NextBot.Position, NextTop.Position, PrevTop.Position);
		NextTop.Normal = ComputeNormal(NextTop.Position, PrevTop.Position, NextBot.Position);
		PrevTop.Normal = ComputeNormal(PrevTop.Position, NextBot.Position, NextTop.Position);

		cylinder.push_back(PrevTop);
		cylinder.push_back(PrevBot);
		cylinder.push_back(NextBot);

		//set the next to the previous

		PrevBot = NextBot;
		PrevTop = NextTop;

	}

	return cylinder;

}

std::vector<MyVertex> CreateSphere(int slices)
{
	float pi = glm::pi<float>();

	float verticalSub = slices / 2;

	float vertAngle = pi / verticalSub;





	return std::vector<MyVertex>{};
}