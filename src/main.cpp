#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>
#include "Parser/CS300Parser.h"
#include "glm/gtc/matrix_transform.hpp"
#include "OGLDebug.h"
#include "ShaderUtils.h"
#include "tiny_obj_loader.h"
#include "Vertex.h"


static int     winID;
static GLsizei WIDTH = 1280;
static GLsizei HEIGHT = 720;
static GLuint theProgram;
static GLuint texture;
static bool wireframeMode;
static bool textureMap;
static bool drawNormals;

struct Vertex
{
	float pos[4];
	float color[3];
};

const Vertex vertexData[3] = {
	{{0.75f, 0.75f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.75f, -0.75f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
	{{-0.75f, -0.75f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}} };


GLuint InitializeProgram()
{
	return ShaderUtils::CreateShaderProgram("data//vertshader.vert", "data//fragShader.frag");
	

}

//Called to update the display.
//You should call SDL_GL_SwapWindow after all of your rendering to display what you rendered.
void display(SDL_Window* window, Mesh& obj)
{

	glm::mat4 projection = glm::perspective(glm::radians(Camera::fovy), Camera::width / Camera::height, Camera::nearPlane, Camera::farPlane);

	glm::mat4 View = glm::lookAt(Camera::camPos, Camera::camTarget, Camera::camUp);

	glm::mat4 Model = glm::translate(glm::mat4(1.0f), obj.transform.pos) *
					 (glm::rotate(glm::mat4(1.0f), glm::radians(obj.transform.rot.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
					  glm::rotate(glm::mat4(1.0f), glm::radians(obj.transform.rot.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
					  glm::rotate(glm::mat4(1.0f), glm::radians(obj.transform.rot.x), glm::vec3(1.0f, 0.0f, 0.0f)))*
					  glm::scale(glm::mat4(1.0f), obj.transform.sca);
	

	glm::mat4 MVP = projection * View * Model;

	// Bind the glsl program and this object's VAO
	glUseProgram(theProgram);
	glBindVertexArray(obj.VAO);

	int location = glGetUniformLocation(theProgram, "u_MVP");

	glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);

	int textureLoc = glGetUniformLocation(theProgram, "u_TextureBool");
	int TexValue = textureMap;
	glUniform1i(textureLoc, textureMap);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	int textureLocalization = glGetUniformLocation(theProgram, "u_Texture");
	glUniform1i(textureLocalization, 0);

	// Draw
	if (!wireframeMode)
	{
		//glPolygonMode(GL_FRONT, GL_LINE); glPolygonMode(GL_BACK, GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, obj.mesh.size());
	}
	else if (wireframeMode)
	{
		glPolygonMode(GL_FRONT, GL_LINE); glPolygonMode(GL_BACK, GL_LINE);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLES, 0, obj.mesh.size());
	}

	// Unbind
	glBindVertexArray(0);

	if (drawNormals)
	{
		glBindVertexArray(obj.NormalVAO);
		glDrawArrays(GL_LINES, 0, obj.Normals.size());

		// Unbind
		glBindVertexArray(0);
	}
	
	glUseProgram(0);

}

void cleanup(GLuint theProgram, SceneObjs scene)
{
	// Delete the program
	glDeleteProgram(theProgram);

	for (unsigned i = 0; i < scene.objects.size(); i++)
	{
		// Delete the VBOs
		glDeleteBuffers(1, &scene.objects[i].VBO);
		// Delete the VAO
		glDeleteVertexArrays(1, &scene.objects[i].VAO);
	}
	
}

GLuint LoadTexture()
{
	int w = 6;
	int h = 6;


	GLuint texture;

	Texture mytexture;

	// Create texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Give pixel data to opengl
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, mytexture.texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return texture;
}

#undef main
int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_Window* window = SDL_CreateWindow("CS300", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GLContext context_ = SDL_GL_CreateContext(window);
	if (context_ == nullptr)
	{
		SDL_DestroyWindow(window);
		std::cout << "SDL_GL_CreateContext Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		exit(1);
	}

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		SDL_GL_DeleteContext(context_);
		SDL_DestroyWindow(window);
		std::cout << "GLEW Error: Failed to init" << std::endl;
		SDL_Quit();
		exit(1);
	}

#if _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif


	// print GPU data
	std::cout << "GL_VENDOR: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;

	GLint totalMemKb = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &totalMemKb);
	std::cout << "GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX: " << totalMemKb << std::endl;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalMemKb);
	std::cout << "GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX: " << totalMemKb << std::endl;

	std::cout << std::endl
		<< "Extensions: "
		<< std::endl;
	int numExtensions;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
	for (int i = 0; i < numExtensions; i++)
	{
		std::cout << glGetStringi(GL_EXTENSIONS, i) << std::endl;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.f, 1.f);
	glEnable(GL_CLAMP);

	textureMap = false;
	drawNormals = false;
	wireframeMode = false;

	//get the shaders and set them
	theProgram = ShaderUtils::CreateShaderProgram("data/vertshader.vert", "data/fragShader.frag");
	//glUseProgram(theProgram);
	
	//Initialize the parser
	CS300Parser parser;
	parser.LoadDataFromFile("data/scene_A0.txt");
	
	//get the camera data
	Camera camera;
	camera.GetCameraValues(parser);

	//Create the meshes and store them in an array
	SceneObjs Scene;
	Scene.LoadObjects(parser);

	texture = LoadTexture();


	
	SDL_Event event;
	bool      quit = false;
	while (!quit)
	{
		
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
					quit = true;
				if (event.key.keysym.scancode == SDL_SCANCODE_A)
				{
					Camera::phi -= 0.03;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_D)
				{
					Camera::phi += 0.03;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
				{
					Camera::r -= 0.5;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_E)
				{
					Camera::r += 0.5;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_S)
				{
					if(Camera::theta <glm::pi<float>() - 0.02)
					Camera::theta += 0.03;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_W)
				{
					if (Camera::theta > 0.1)
					Camera::theta -= 0.03;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_P)
				{
					Camera::slices++;
					Scene.ReoadMeshes();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_O)
				{
					if(Camera::slices > 4)
					Camera::slices--;
					Scene.ReoadMeshes();
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_M)
				{
					if (wireframeMode)
					{
						wireframeMode = false;
					}
					else if (!wireframeMode)
					{
						wireframeMode = true;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_T)
				{
					if (textureMap)
					{
						textureMap = false;
					}
					else if (!textureMap)
					{
						textureMap = true;
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_N)
				{
					if (drawNormals)
					{
						drawNormals = false;
					}
					else if (!drawNormals)
					{
						drawNormals = true;
					}
				}
				break;
			}
		}

		//UpdateCamera();
		CalculateCamPosition();

		glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		for (unsigned i = 0; i < Scene.objects.size(); i++)
		{
			display(window, Scene.objects[i]);
		}

		SDL_GL_SwapWindow(window);

	}

	cleanup(theProgram, Scene);

	SDL_GL_DeleteContext(context_);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}