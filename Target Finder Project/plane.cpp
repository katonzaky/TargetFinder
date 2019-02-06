
#include "plane.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "loadTex.h"
//#include "tiny_obj_loader.h"
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2
#define VALS_PER_COLOUR 4

using namespace std;

plane::plane(int programID)
{
	size_multiplier = 1.0f;
    tex_multiplier = 1.0f;

    createData();
    createTangents();

    vaoBuffer = createVao(programID);
}  

plane::plane(int programID, float sz_m, float tx_m)
{
	size_multiplier = sz_m;
    tex_multiplier = tx_m;

    createData();
    createTangents();

    vaoBuffer = createVao(programID);
}  

void plane::createTangents()
{
	int n = 0;

	vector<glm::vec3> vert;
	vector<glm::vec2> uv;

	for (int i = 0; i < vertices.size(); i+=3)
	{
		glm::vec3 v(vertices[i+0], vertices[i+1], vertices[i+2]);
		//cout << v[i+0] << " " << v[i+1] << " " << v[i+2] << endl;
		vert.push_back(v);
	}

	for (int i = 0; i < texcoords.size(); i+=2)
	{
		glm::vec2 u(texcoords[i+0], texcoords[i+1]);
		//cout << texcoords[i+0] << " " << texcoords[i+1] << endl;
		uv.push_back(u);
	}


    for (int i = 0; i < vert.size(); i+=3)
    {
    	glm::vec3 tangent, bitangent;

    	glm::vec3 pos1 = vert[i+0];
    	glm::vec3 pos2 = vert[i+1];
    	glm::vec3 pos3 = vert[i+2];
    	
    	glm::vec3 edge1 = pos2 - pos1;
    	//cout << "edge1: " << edge1[0] << " " << edge1[1] << " " << edge1[2] << endl;
    	glm::vec3 edge2 = pos3 - pos1;
		//cout << "edge2: " << edge2[0] << " " << edge2[1] << " " << edge2[2] << endl;

    	glm::vec2 uv1 = uv[i+0];
    	glm::vec2 uv2 = uv[i+1];
    	glm::vec2 uv3 = uv[i+2];
    	
    	glm::vec2 deltaUV1 = uv2 - uv1;
    	glm::vec2 deltaUV2 = uv3 - uv1;

        float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
        tangent = (edge1 * deltaUV2.y   - edge2 * deltaUV1.y)*r;
        bitangent = (edge2 * deltaUV1.x   - edge1 * deltaUV2.x)*r;

        tangents.push_back(tangent);
        tangents.push_back(tangent);
        tangents.push_back(tangent);

        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
        bitangents.push_back(bitangent);
    }

}

void plane::createData()
{
    GLfloat planeVertices[] = {
        // Positions          // Normals         // Texture Coords
        1.0f, -0.5f, 1.0f,
        -1.0f, -0.5f, -1.0f,
        -1.0f, -0.5f, 1.0f,

        1.0f, -0.5f, 1.0f,
        1.0f, -0.5f, -1.0f,
        - 1.0f, -0.5f, -1.0f
    };
    
    GLfloat planeNormals[] = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    
    for (int i = 0; i < 6*3; i++)
    {
    	vertices.push_back(planeVertices[i] * size_multiplier);
    	normals.push_back(planeNormals[i] * size_multiplier);
    }
    
    GLfloat planeTex[] = {
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    
    for (int i = 0; i < 6*2; i++)
    {
    	texcoords.push_back(planeTex[i] * tex_multiplier);
    }
}

int plane::createVao(int programID)
{
    unsigned int tempVao;

    glUseProgram(programID);
    glGenVertexArrays(1, &tempVao);
    glBindVertexArray(tempVao);

    unsigned int buffer[5];
    glGenBuffers(5, buffer);

    // Set vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), &normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

    // Texture attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, texcoords.size() * sizeof(float), &texcoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

   	// Tangent attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

   	// BiTangent attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
	glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), &bitangents[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Un-bind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return tempVao;
}
