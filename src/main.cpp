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
static bool normalColor;
static bool normalVec;

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

void UpdatePositions(Light& light)
{
	auto ticks = SDL_GetTicks();

	float time = static_cast<float>(ticks);

	time /= 1000000;

	for (size_t u = 0; u < light.anims.size(); u++)
	{
		light.pos = light.anims[u].Update(light.pos, time);
	}

}

void UpdatePositions(Mesh& obj)
{
	float ticks = SDL_GetTicks();

	for (size_t u = 0; u < obj.transform.anims.size(); u++)
	{
		obj.transform.pos = obj.transform.anims[u].Update(obj.transform.pos, ticks / 100000.0f);
	}

}

//Called to update the display.
//You should call SDL_GL_SwapWindow after all of your rendering to display what you rendered.
void display(SDL_Window* window, SceneObjs& scene)
{
	for (unsigned i = 0; i < scene.objects.size(); i++)
	{
		UpdatePositions(scene.objects[i]);
		//get the transformations
		glm::mat4 projection = glm::perspective(glm::radians(Camera::fovy), Camera::width / Camera::height, Camera::nearPlane, Camera::farPlane);

		glm::mat4 View = glm::lookAt(Camera::camPos, Camera::camTarget, Camera::camUp);

		glm::mat4 Model = glm::translate(glm::mat4(1.0f), scene.objects[i].transform.pos) *
						 (glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[i].transform.rot.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
						  glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[i].transform.rot.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
						  glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[i].transform.rot.x), glm::vec3(1.0f, 0.0f, 0.0f)))*
						  glm::scale(glm::mat4(1.0f), scene.objects[i].transform.sca);
		
		//get the MVP
		glm::mat4 MVP = projection * View * Model;

		// Bind the glsl program and this object's VAO
		glUseProgram(theProgram);
		glBindVertexArray(scene.objects[i].VAO);

		//uniform of M2W
		int M2Wloc = glGetUniformLocation(theProgram, "u_M2W");
		glUniformMatrix4fv(M2Wloc, 1, GL_FALSE, &Model[0][0]);


		//uniform of MVP
		int location = glGetUniformLocation(theProgram, "u_MVP");
		glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);

		//uniform of texture bool
		int textureLoc = glGetUniformLocation(theProgram, "u_TextureBool");
		int TexValue = textureMap;
		glUniform1i(textureLoc, textureMap);


		//PASS THE LIGHT TO THE SHADER

		//number of lights
		int numoflights = glGetUniformLocation(theProgram, "uLightNum");
		glUniform1i(numoflights, 1);

		//pass the light
		
		int type = glGetUniformLocation(theProgram, "uLight[0].type");
		if (scene.lights[0].type == "POINT")
		{
			glUniform1i(type, 1);
		}
		else
		{
			glUniform1i(type, 2);
		}

		int Pos = glGetUniformLocation(theProgram, "uLight[0].uPos");
		glUniform3fv(Pos, 1, &scene.lights[0].pos.x);

		int Dir = glGetUniformLocation(theProgram, "uLight[0].uDir");
		glUniform3fv(Pos, 1, &scene.lights[0].dir.x);

		int Col = glGetUniformLocation(theProgram, "uLight[0].uCol");
		glUniform3fv(Pos, 1, &scene.lights[0].col.x);

		int Att = glGetUniformLocation(theProgram, "uLight[0].uAtt");
		glUniform3fv(Pos, 1, &scene.lights[0].att.x);

		int amb = glGetUniformLocation(theProgram, "uLight[0].uAmb");
		glUniform1i(amb, scene.lights[0].amb);

		int Inner = glGetUniformLocation(theProgram, "uLight[0].uInner");
		glUniform1i(Inner, scene.lights[0].inner);

		int Outter = glGetUniformLocation(theProgram, "uLight[0].uOuter");
		glUniform1i(Outter, scene.lights[0].outer);
		
		int falloff = glGetUniformLocation(theProgram, "uLight[0].falloff");
		glUniform1i(falloff, scene.lights[0].falloff);



		//set the texture and pass it to the shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		int textureLocalization = glGetUniformLocation(theProgram, "u_Texture");
		glUniform1i(textureLocalization, 0);

		// Draw
		if (!wireframeMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, scene.objects[i].mesh.size());
		}
		else if (wireframeMode)
		{
			glPolygonMode(GL_FRONT, GL_LINE); glPolygonMode(GL_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, scene.objects[i].mesh.size());
		}

		// Unbind
		glBindVertexArray(0);

		int DrawNorm = glGetUniformLocation(theProgram, "u_DrawNormalCol");
		glUniform1i(DrawNorm, normalColor);

		//normal drawing
		if (drawNormals)
		{
			normalColor = true;
			int DrawNorm = glGetUniformLocation(theProgram, "u_DrawNormalCol");
			glUniform1i(DrawNorm, normalColor);
			

			glBindVertexArray(scene.objects[i].NormalVAO);
			glDrawArrays(GL_LINES, 0, scene.objects[i].Normals.size());

			// Unbind
			glBindVertexArray(0);
			normalColor = false;
			
			glUniform1i(glGetUniformLocation(theProgram, "u_DrawNormalCol"), normalColor);
		}
		
		glUseProgram(0);
	}

	for (unsigned int i = 0; i < scene.lights.size(); i++)
	{
		UpdatePositions(scene.lights[i]);

		//get the transformations
		glm::mat4 projection = glm::perspective(glm::radians(Camera::fovy), Camera::width / Camera::height, Camera::nearPlane, Camera::farPlane);

		glm::mat4 View = glm::lookAt(Camera::camPos, Camera::camTarget, Camera::camUp);

		glm::mat4 Model = glm::translate(glm::mat4(1.0f), scene.lights[i].pos) *
			glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

		//get the MVP
		glm::mat4 MVP = projection * View * Model;

		// Bind the glsl program and this object's VAO
		glUseProgram(theProgram);
		glBindVertexArray(scene.lights[i].VAO);

		//uniform of MVP
		int location = glGetUniformLocation(theProgram, "u_MVP");
		glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);

		//uniform of texture bool
		int textureLoc = glGetUniformLocation(theProgram, "u_TextureBool");
		int TexValue = textureMap;
		glUniform1i(textureLoc, textureMap);



		// Draw
		if (!wireframeMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, scene.lights[i].mesh.size());
		}
		else if (wireframeMode)
		{
			glPolygonMode(GL_FRONT, GL_LINE); glPolygonMode(GL_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, scene.lights[i].mesh.size());
		}

		// Unbind
		glBindVertexArray(0);

		int DrawNorm = glGetUniformLocation(theProgram, "u_DrawNormalCol");
		glUniform1i(DrawNorm, normalColor);

		//normal drawing

		glUseProgram(0);

	}


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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, mytexture.texture);

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
	parser.LoadDataFromFile("data/scene_A1.txt");
	
	//get the camera data
	Camera camera;
	camera.GetCameraValues(parser);

	//Create the meshes and store them in an array
	SceneObjs Scene;
	Scene.LoadObjects(parser);

	texture = LoadTexture();

	normalColor = false;


	
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
				if (event.key.keysym.scancode == SDL_SCANCODE_F)
				{
					if (Scene.avgNormals)
					{
						Scene.avgNormals = false;
						Scene.ReoadMeshes();
					}
					else if (!Scene.avgNormals)
					{
						Scene.avgNormals = true;
						Scene.ReoadMeshes();
					}
				}
				break;
			}
		}

		CalculateCamPosition();

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		display(window, Scene);

		SDL_GL_SwapWindow(window);

	}

	cleanup(theProgram, Scene);

	SDL_GL_DeleteContext(context_);
	SDL_DestroyWindow(window); 
	SDL_Quit();

	return 0;
}