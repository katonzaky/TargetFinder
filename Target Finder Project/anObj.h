#ifndef _OBJ01_
#define _OBJ01_

#include <vector>
#include "tiny_obj_loader.h"
#include "Viewer.h"
#include "loadTex.h"
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class anObj
{
	private:


	public:
		float maxVert;
		float maxX, minX, maxY, minY, maxZ, minZ;

		float floorW, floorH;
		float tex_multiplier;
		std::vector<int> vaoBuffer;

		std::vector<std::vector<float> > vertices;
		std::vector<std::vector<int> > indices;
		std::vector<std::vector<float> > normals;
		std::vector<std::vector<float> > texcoords;
		std::vector<std::vector<float> > normal_colors;
		std::vector<std::vector<float> > diffuse_colors;
		std::vector<std::vector<glm::vec3> > tangents;
		std::vector<std::vector<glm::vec3> > bitangents;

		std::vector<tinyobj::material_t> materials;
		std::vector<tinyobj::shape_t> shapes;

		int createVao(int programID, int index);
		void createTangents();
		void createData();
		anObj(std::vector<tinyobj::shape_t> s, std::vector<tinyobj::material_t> m, int programID);
		anObj(std::vector<tinyobj::shape_t> s, std::vector<tinyobj::material_t> m, int programID, float mp);
		int render(int programID, int depthID, Viewer *Camera, float flag, float dt, loadTex *Texture, int i);
};
#endif
