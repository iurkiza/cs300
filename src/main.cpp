#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
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
static GLuint enviromentMapProgram;
static GLuint texture;
static GLuint cubeMap;
static GLuint enviromentMapTexture;
static GLuint cubeMapFBO[6];
static GLuint skyBoxprogram;

static bool wireframeMode;
static int textureMap = 0;
static bool drawNormals;
static bool normalColor;
static bool tangentColor;
static bool bitangentColor;
static bool normalVec;
static bool itsALight;
static bool light;

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

	time /= 1000.0f;

	light.pos = light.Origpos;
	for (size_t u = 0; u < light.anims.size(); u++)
	{
		light.pos = light.anims[u].Update(light.pos, time);
	}

}

void UpdatePositions(Mesh& obj)
{
	float ticks = static_cast<float>(SDL_GetTicks());
	
	obj.transform.pos = obj.transform.OrigPos;
	for (size_t u = 0; u < obj.transform.anims.size(); u++)
	{
		obj.transform.pos = obj.transform.anims[u].Update(obj.transform.pos, ticks / 1000.0f);
	}

}

void fillEnviromentMap(SDL_Window* window, SceneObjs& scene, Skybox skybox)
{
	glViewport(0, 0, 512, 512);
	//glCullFace(GL_FRONT);
	for (unsigned int i = 0; i < 6; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, cubeMapFBO[i]);


		glUseProgram(0);
		glCullFace(GL_FRONT);
		glDepthMask(GL_FALSE);

		//get the transformations
		glm::mat4 enviromentMapPrespective = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 500.0f);

		glm::vec3 directionVec;
		glm::vec3 upVec;

		switch (i)
		{
		case 0:
			directionVec = glm::vec3(1.0f, 0.0f, 0.0f);
			upVec = glm::vec3(0.0f, -1.0f, 0.0f);
			break;
		case 1:
			directionVec = glm::vec3(-1.0f, 0.0f, 0.0f);
			upVec = glm::vec3(0.0f, -1.0f, 0.0f);
			break;
		case 2:
			directionVec = glm::vec3(0.0f, 1.0f, 0.0f);
			upVec = glm::vec3(0.0f, 0.0f, 1.0f);
			break;
		case 3:
			directionVec = glm::vec3(0.0f, -1.0f, 0.0f);
			upVec = glm::vec3(0.0f, 0.0f, -1.0f);
			break;
		case 4:
			directionVec = glm::vec3(0.0f, 0.0f, 1.0f);
			upVec = glm::vec3(0.0f, -1.0f, 0.0f);
			break;
		case 5:
			directionVec = glm::vec3(0.0f, 0.0f, -1.0f);
			upVec = glm::vec3(0.0f, -1.0f, 0.0f);
			break;
		}
		glm::mat4 enviromentMapView = glm::mat4(glm::mat3(glm::lookAt(scene.reflective->transform.pos, scene.reflective->transform.pos + directionVec, upVec)));



		glUseProgram(skyBoxprogram);
		glBindVertexArray(skybox.VAO);

		//uniform of M2W
		int ViewLoc = glGetUniformLocation(skyBoxprogram, "u_projection");
		glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, &enviromentMapPrespective[0][0]);

		//uniform of M2W
		int M2Wloc = glGetUniformLocation(skyBoxprogram, "u_View");
		glUniformMatrix4fv(M2Wloc, 1, GL_FALSE, &enviromentMapView[0][0]);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(skybox.mesh.size()));

		glCullFace(GL_BACK);
		glDepthMask(GL_TRUE);

		for (unsigned j = 0; j < scene.objects.size(); j++)
		{
			//get the transformations
			glm::mat4 enviromentMapPrespective = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 500.0f);

			glm::vec3 directionVec;
			glm::vec3 upVec;

			switch (i)
			{
			case 0:
				directionVec = glm::vec3(1.0f, 0.0f, 0.0f);
				upVec = glm::vec3(0.0f, -1.0f, 0.0f);
				break;
			case 1:
				directionVec = glm::vec3(-1.0f, 0.0f, 0.0f);
				upVec = glm::vec3(0.0f, -1.0f, 0.0f);
				break;
			case 2:
				directionVec = glm::vec3(0.0f, 1.0f, 0.0f);
				upVec = glm::vec3(0.0f, 0.0f, 1.0f);
				break;
			case 3:
				directionVec = glm::vec3(0.0f, -1.0f, 0.0f);
				upVec = glm::vec3(0.0f, 0.0f, -1.0f);
				break;
			case 4:
				directionVec = glm::vec3(0.0f, 0.0f, 1.0f);
				upVec = glm::vec3(0.0f, -1.0f, 0.0f);
				break;
			case 5:
				directionVec = glm::vec3(0.0f, 0.0f, -1.0f);
				upVec = glm::vec3(0.0f, -1.0f, 0.0f);
				break;
			}


			glm::mat4 enviromentMapView = glm::lookAt(scene.reflective->transform.pos, scene.reflective->transform.pos + directionVec, upVec);

			glm::mat4 enviromentMapModel = glm::translate(glm::mat4(1.0f), scene.objects[j].transform.pos) *
				(glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[j].transform.rot.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
					glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[j].transform.rot.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
					glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[j].transform.rot.x), glm::vec3(1.0f, 0.0f, 0.0f))) *
				glm::scale(glm::mat4(1.0f), scene.objects[j].transform.sca);


			//get the MVP
			glm::mat4 enviromentMapMVP = enviromentMapPrespective * enviromentMapView * enviromentMapModel;

			glUseProgram(theProgram);
			glBindVertexArray(scene.objects[j].VAO);

			//uniform of M2W
			int ViewLoc = glGetUniformLocation(theProgram, "u_View");
			glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, &enviromentMapView[0][0]);

			//uniform of M2W
			int M2Wloc = glGetUniformLocation(theProgram, "u_M2W");
			glUniformMatrix4fv(M2Wloc, 1, GL_FALSE, &enviromentMapModel[0][0]);

			//uniform of MVP
			int location = glGetUniformLocation(theProgram, "u_MVP");
			glUniformMatrix4fv(location, 1, GL_FALSE, &enviromentMapMVP[0][0]);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(scene.objects[j].mesh.size()));


			glUseProgram(0);


		}
		//skybox

		//glViewport(0, 0, WIDTH, HEIGHT)

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void displaySkyBox(Skybox& skybox)
{
	glViewport(0, 0, WIDTH, HEIGHT);
	glUseProgram(0);
	glCullFace(GL_FRONT);
	glDepthMask(GL_FALSE);

	glm::mat4 projection = glm::perspective(glm::radians(Camera::fovy), Camera::width / Camera::height, Camera::nearPlane, Camera::farPlane);

	glm::mat4 View = glm::mat4(glm::mat3(glm::lookAt(Camera::camPos, Camera::camTarget, Camera::camUp)));



	glUseProgram(skyBoxprogram);
	glBindVertexArray(skybox.VAO);

	//uniform of M2W
	int ViewLoc = glGetUniformLocation(skyBoxprogram, "u_projection");
	glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, &projection[0][0]);

	//uniform of M2W
	int M2Wloc = glGetUniformLocation(skyBoxprogram, "u_View");
	glUniformMatrix4fv(M2Wloc, 1, GL_FALSE, &View[0][0]);


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(skybox.mesh.size()));

	glCullFace(GL_BACK);
	glDepthMask(GL_TRUE);
}

//Called to update the display.
//You should call SDL_GL_SwapWindow after all of your rendering to display what you rendered.
void display(SDL_Window* window, SceneObjs& scene)
{
	
	itsALight = false;
	light = false;
	
	glViewport(0, 0, WIDTH, HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);

	for (unsigned i = 0; i < scene.objects.size(); i++)
	{
		UpdatePositions(scene.objects[i]);
		//get the transformations
		glm::mat4 projection = glm::perspective(glm::radians(Camera::fovy), Camera::width / Camera::height, Camera::nearPlane, Camera::farPlane);

		glm::mat4 View = glm::lookAt(Camera::camPos, Camera::camTarget, Camera::camUp);

		glm::mat4 Model = glm::translate(glm::mat4(1.0f), scene.objects[i].transform.pos) *
			(glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[i].transform.rot.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[i].transform.rot.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
				glm::rotate(glm::mat4(1.0f), glm::radians(scene.objects[i].transform.rot.x), glm::vec3(1.0f, 0.0f, 0.0f))) *
				glm::scale(glm::mat4(1.0f), scene.objects[i].transform.sca);



		//get the MVP
		glm::mat4 MVP = projection * View * Model;


		// Bind the glsl program and this object's VAO
		glUseProgram(theProgram);
		glBindVertexArray(scene.objects[i].VAO);

		//uniform of M2W
		int ViewLoc = glGetUniformLocation(theProgram, "u_View");
		glUniformMatrix4fv(ViewLoc, 1, GL_FALSE, &View[0][0]);

		//uniform of M2W
		int M2Wloc = glGetUniformLocation(theProgram, "u_M2W");
		glUniformMatrix4fv(M2Wloc, 1, GL_FALSE, &Model[0][0]);

		//uniform of MVP
		int location = glGetUniformLocation(theProgram, "u_MVP");
		glUniformMatrix4fv(location, 1, GL_FALSE, &MVP[0][0]);


		//set the texture and pass it to the shader
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		int textureLocalization = glGetUniformLocation(theProgram, "u_Texture");
		glUniform1i(textureLocalization, 0);

		//set the height map and pass it to the shader
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, enviromentMapTexture);
		int shadowmapLoc = glGetUniformLocation(theProgram, "u_enviromentMap");
		glUniform1i(shadowmapLoc, 1);


		int relractorLoc = glGetUniformLocation(theProgram, "u_refractor");
		glUniform1i(relractorLoc, scene.objects[i].transform.reflector);

		int textureMapLoc = glGetUniformLocation(theProgram, "u_TextureMode");
		glUniform1i(textureMapLoc, textureMap);

		int iorLoc = glGetUniformLocation(theProgram, "u_ior");
		glUniform1f(iorLoc, scene.objects[i].transform.ior);

		int camPos = glGetUniformLocation(theProgram, "u_cameraPos");
		glUniform3fv(camPos, 1, &Camera::camPos.x);


		// Draw
		if (!wireframeMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(scene.objects[i].mesh.size()));
		}
		else if (wireframeMode)
		{
			glPolygonMode(GL_FRONT, GL_LINE); glPolygonMode(GL_BACK, GL_LINE);
			glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(scene.objects[i].mesh.size()));
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
			glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(scene.objects[i].NormalsToDraw.size()));

			// Unbind
			glBindVertexArray(0);
			normalColor = false;
			glUniform1i(glGetUniformLocation(theProgram, "u_DrawNormalCol"), normalColor);

			///////////////////////////////////////// TANGENTS

			tangentColor = true;
			glUniform1i(glGetUniformLocation(theProgram, "u_TangentCol"), tangentColor);
			
			glBindVertexArray(scene.objects[i].TangentVAO);
			glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(scene.objects[i].TangentToDraw.size()));


			// Unbind
			glBindVertexArray(0);
			tangentColor = false;
			glUniform1i(glGetUniformLocation(theProgram, "u_TangentCol"), tangentColor);
			///////////////////////////////////////// BiTANGENTS

			bitangentColor = true;
			glUniform1i(glGetUniformLocation(theProgram, "u_BitangentCol"), bitangentColor);

			glBindVertexArray(scene.objects[i].BitangentVAO);
			glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(scene.objects[i].BiTangentsToDraw.size()));


			// Unbind
			glBindVertexArray(0);
			bitangentColor = false;

			
			
			glUniform1i(glGetUniformLocation(theProgram, "u_BitangentCol"), bitangentColor);
		}
		
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

GLuint LoadTexture(SceneObjs& scene)
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


	//CUBE MAP

	glGenTextures(1, &cubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	int EnvMapWith = 512;
	int EnvMapHeight = 512;
	int filler;

	unsigned char* data;

	for (GLuint i = 0; i < 6; i++)
	{
		data = stbi_load(scene.environmentMap[i].c_str(), &EnvMapWith, &EnvMapHeight, &filler, 4);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, EnvMapWith, EnvMapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);



	//FRAME BUFFERS***********************************************************************

	glGenTextures(1, &enviromentMapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, enviromentMapTexture);

	const unsigned EnvMapSize = 512;
	for (GLuint i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, EnvMapSize, EnvMapSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Create and set up the FBO
	glGenFramebuffers(6, cubeMapFBO);
	for (GLuint i = 0; i < 6; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, cubeMapFBO[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, enviromentMapTexture, 0);
		GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, drawBuffers);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


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

	drawNormals = false;
	wireframeMode = false;

	//get the shaders and set them
	theProgram = ShaderUtils::CreateShaderProgram("data/vertshader.vert", "data/fragShader.frag");
	enviromentMapProgram = ShaderUtils::CreateShaderProgram("data/enviromentMapVertex.vert", "data/enviromentMapFragment.frag");
	skyBoxprogram = ShaderUtils::CreateShaderProgram("data/skyBoxVertexShader.vert", "data/skyBoxFragmentShader.frag");
	
	//Initialize the parser
	CS300Parser parser;
	parser.LoadDataFromFile("data/scene_A4.txt");
	
	//get the camera data
	Camera camera;
	camera.GetCameraValues(parser);

	//Create the meshes and store them in an array
	SceneObjs Scene;
	Scene.LoadObjects(parser);

	

	texture = LoadTexture(Scene);

	normalColor = false;
	tangentColor = false;
	bitangentColor = false;

	Skybox skybox(Scene);

	
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
					Camera::phi -= 0.03f;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_D)
				{
					Camera::phi += 0.03f;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_Q)
				{
					Camera::r -= 0.5f;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_E)
				{
					Camera::r += 0.5f;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_S)
				{
					if(Camera::theta <glm::pi<float>() - 0.02)
					Camera::theta += 0.03f;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_W)
				{
					if (Camera::theta > 0.1)
					Camera::theta -= 0.03f;
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
					textureMap++;
					if (textureMap == 3)
					{
						textureMap = 0;
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
		
		displaySkyBox(skybox);
		fillEnviromentMap(window, Scene, skybox);
		display(window, Scene);


		SDL_GL_SwapWindow(window);

	}

	

	cleanup(theProgram, Scene);

	SDL_GL_DeleteContext(context_);
	SDL_DestroyWindow(window); 
	SDL_Quit();

	return 0;
}