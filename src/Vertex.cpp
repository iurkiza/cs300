#include "Vertex.h"

float     Camera::fovy;
float     Camera::width;
float     Camera::height;
float     Camera::nearPlane;
float     Camera::farPlane;
glm::vec3 Camera::camPos;
glm::vec3 Camera::camTarget;
glm::vec3 Camera::camUp;


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