#ifndef _OBJLOAD_
#define _OBJLOAD_

#include <vector>
#include "tiny_obj_loader.h"
#include "Viewer.h"
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "anObj.h"

class loadObj
{
	public:
		loadObj(std::string input);
		
		std::string inputfile;
		std::string filepath;

		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		void load_obj();
		void load_materials();
		void load_shapes();
};
#endif
