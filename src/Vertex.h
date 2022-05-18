#pragma once
#include "glm/vec4.hpp"
#include "glm/vec2.hpp"
#include "Parser/CS300Parser.h"


namespace vertex
{
	class Vertex
	{
	public:
		glm::vec4 Position;

		glm::vec4 Normal;

		glm::vec2 UV;

	};
}
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


