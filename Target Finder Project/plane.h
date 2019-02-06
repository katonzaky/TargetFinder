#ifndef _PLANE_
#define _PLANE_

#include <vector>
#include "tiny_obj_loader.h"
#include "Viewer.h"
#include "loadTex.h"
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class plane
{
	private:


	public:
		float maxVert;

		float tex_multiplier;
		float size_multiplier;
		unsigned int vaoBuffer;

		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> bitangents;
		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<float> texcoords;

		int createVao(int programID);
		void createTangents();
		void createData();

		plane(int programID);
		plane(int programID, float sz_m, float tx_m);
};
#endif
