#ifndef _TEXLOAD_
#define _TEXLOAD_

#include <vector>
#include "tiny_obj_loader.h"
#include "Viewer.h"
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "stb_image.h"

#define NUM_TEXTURES 100
#define NUM_TEXTURES_OBJ 1
#define TEX_SIZE 64  // resolution of our constructed texture image

class loadTex
{
	public:
		loadTex(std::string f, std::vector<tinyobj::shape_t> s, std::vector<tinyobj::material_t> m);
		loadTex(std::string f, std::vector<tinyobj::shape_t> s, std::vector<tinyobj::material_t> m, std::string n);
		loadTex(std::string f, std::vector<tinyobj::shape_t> s, std::vector<tinyobj::material_t> m, std::string n, std::string d);

		std::string filepath, filepath_bump, filepath_displacement;
		std::vector<tinyobj::material_t> materials;
		std::vector<tinyobj::shape_t> shapes;
		// texture image data
		GLubyte myimage[TEX_SIZE][TEX_SIZE][4];
		
		GLenum CurrentTexSample;
		char   CurrentTexString[32];

		GLenum CurrentTexMode;
		char   CurrentTexModeString[32];

		GLuint TexID[NUM_TEXTURES];
		GLuint TexIDObj[1];
		GLuint CurrentTex;

		void createTexture();
		void loadTexture(std::string path, int index);
};
#endif
