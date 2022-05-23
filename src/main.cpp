#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <GL/glew.h>
#include <GL/GL.h>
#include <SDL2/SDL.h>
#include "Parser/CS300Parser.h"
#include "OGLDebug.h"
#include "ShaderUtils.h"

#include "Vertex.h"

static int     winID;
static GLsizei WIDTH = 1280;
static GLsizei HEIGHT = 720;

struct Vertex
{
	float pos[4];
	float color[3];
};

const Vertex vertexData[3] = {
	{{0.75f, 0.75f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.75f, -0.75f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
	{{-0.75f, -0.75f, 0.0f, 1.0f}, {1.0f, 1.0f, 0.0f}} };

namespace
{
	GLuint theProgram;
	GLuint vertexBufferObject;
	GLuint vao;
}

void InitializeProgram()
{
	theProgram = ShaderUtils::CreateShaderProgram("data//vertshader.vert", "data//fragShader.frag");

}


void InitializeBuffers()
{
	std::vector<MyVertex> cube = CreateCylinder(20);

	glDisable(GL_CULL_FACE);

	// VAO
	glGenVertexArrays(1, &vao);

	// VBO
	glGenBuffers(1, &vertexBufferObject);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * cube.size(), cube.data(), GL_STATIC_DRAW);

	// Insert the VBO into the VAO
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(MyVertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, Normal)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MyVertex), reinterpret_cast<void*>(offsetof(MyVertex, UV)));

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//Called after the window and OpenGL are initialized. Called exactly once, before the main loop.
void init()
{
	InitializeProgram();
	InitializeBuffers();
}

//Called to update the display.
//You should call SDL_GL_SwapWindow after all of your rendering to display what you rendered.
void display(SDL_Window* window)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Bind the glsl program and this object's VAO
	glUseProgram(theProgram);
	glBindVertexArray(vao);

	std::vector<MyVertex> cube = CreateCylinder(20);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, cube.size());

	// Unbind
	glBindVertexArray(0);
	glUseProgram(0);

	SDL_GL_SwapWindow(window);
}

void cleanup()
{
	// Delete the program
	glDeleteProgram(theProgram);
	// Delete the VBOs
	glDeleteBuffers(1, &vertexBufferObject);
	// Delete the VAO
	glDeleteVertexArrays(1, &vao);
}

#undef main
int main(int argc, char* args[])
{

	std::vector<MyVertex> cube = CreatePlane();


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

	init();	

	
	//Initialize the parser
	CS300Parser parser;
	parser.LoadDataFromFile("data//scene_A0.txt");
	
	//get the camera data
	Camera camera;
	camera.GetCameraValues(parser);
	
	
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
				break;
			}
		}

		display(window);
	}

	cleanup();

	SDL_GL_DeleteContext(context_);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}