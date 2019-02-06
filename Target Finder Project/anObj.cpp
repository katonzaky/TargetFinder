
#include "anObj.h"
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

anObj::anObj(vector<tinyobj::shape_t> s, vector<tinyobj::material_t> m, int programID)
{
    materials = m;
    shapes = s;
    tex_multiplier = 1.0f;
    maxVert = 0.0f;
    floorW = 100.0f;
    floorH = 1.0f;

    createData();
    createTangents();

    for (int i = 0; i < shapes.size(); i++)
    {
        vaoBuffer.push_back(createVao(programID, i));   
    }
}  

anObj::anObj(vector<tinyobj::shape_t> s, vector<tinyobj::material_t> m, int programID, float mp)
{
    materials = m;
    shapes = s;
    tex_multiplier = mp;
    maxVert = 0.0f;
    floorW = 100.0f;
    floorH = 1.0f;

    createData();
    createTangents();

    for (int i = 0; i < shapes.size(); i++)
    {
        vaoBuffer.push_back(createVao(programID, i));   
    }
}  

void anObj::createTangents()
{
    int n = 0;

	for (int i = 0; i < vertices.size(); i++)
	{
		vector<glm::vec3> vert;
		vector<glm::vec2> uv;
		vector<glm::vec3> temp_tangent;
		vector<glm::vec3> temp_bitangent;
		
		for (int j = 0; j < vertices[i].size(); j+=3)
		{
			glm::vec3 v(vertices[i][j+0], vertices[i][j+1], vertices[i][j+2]);
			vert.push_back(v);
		}

		for (int j = 0; j < texcoords[i].size(); j+=2)
		{
			glm::vec2 u(texcoords[i][j+0], texcoords[i][j+1]);
			uv.push_back(u);
		}


		for (int j = 0; j < vert.size(); j+=3)
		{
			glm::vec3 tangent, bitangent;

			glm::vec3 pos1 = vert[j+0];
			glm::vec3 pos2 = vert[j+1];
			glm::vec3 pos3 = vert[j+2];
			
			glm::vec3 edge1 = pos2 - pos1;
			glm::vec3 edge2 = pos3 - pos1;
			
			glm::vec2 uv1 = uv[j+0];
			glm::vec2 uv2 = uv[j+1];
			glm::vec2 uv3 = uv[j+2];
			
			glm::vec2 deltaUV1 = uv2 - uv1;
			glm::vec2 deltaUV2 = uv3 - uv1;
			
			//cout << i << ": " << j << ": " << edge2.x << " " << edge2.y << " " << edge2.z << endl;

			float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			//cout << deltaUV1.x << " " << deltaUV2.y << " " << deltaUV1.y << " " << deltaUV2.x << endl;
		    tangent = (edge1 * deltaUV2.y   - edge2 * deltaUV1.y)*r;
		    bitangent = (edge2 * deltaUV1.x   - edge1 * deltaUV2.x)*r;

			temp_tangent.push_back(tangent);
			temp_tangent.push_back(tangent);
			temp_tangent.push_back(tangent);
			
			temp_bitangent.push_back(bitangent);
			temp_bitangent.push_back(bitangent);
			temp_bitangent.push_back(bitangent);

		}
		
		tangents.push_back(temp_tangent);
		bitangents.push_back(temp_bitangent);
		
    }
    cout << "**NEW OBJ**" << endl;

}
void anObj::createData()
{
    maxX = 0;
    minX = 0;
    maxY = 0;
    minY = 0;
    maxZ = 0;
    minZ = 0;
    for (size_t i = 0; i < shapes.size(); i++)
    {
        vector<float> tempVert;
        vector<int> tempInd;
        vector<float> tempTex;
        vector<float> tempNorm;
        vector<float> tempNormC;

        for (size_t f = 0; f < shapes[i].mesh.indices.size(); f++) 
        {
            tempInd.push_back(shapes[i].mesh.indices[f]);
        }
        indices.push_back(tempInd);

        for(int p = 0; p < shapes[i].mesh.positions.size(); p+=3){
            if(shapes[i].mesh.positions[p] < minX){
                    minX = shapes[i].mesh.positions[p];
            }
            if(shapes[i].mesh.positions[p] > maxX){
                    maxX = shapes[i].mesh.positions[p];
            }
        }
        for(int p = 1; p < shapes[i].mesh.positions.size(); p+=3){
            if(shapes[i].mesh.positions[p] < minY){
                    minY = shapes[i].mesh.positions[p];
            }
            if(shapes[i].mesh.positions[p] > maxY){
                    maxY = shapes[i].mesh.positions[p];
            }

                
        }
        for(int p = 2; p < shapes[i].mesh.positions.size(); p+=3){
            if(shapes[i].mesh.positions[p] < minZ){
                    minZ = shapes[i].mesh.positions[p];
            }
            if(shapes[i].mesh.positions[p] > maxZ){
                    maxZ = shapes[i].mesh.positions[p];
            }

                
        }

        for (size_t v = 0; v < shapes[i].mesh.positions.size(); v++) 
        {
            tempVert.push_back(shapes[i].mesh.positions[v]);

            if (abs(shapes[i].mesh.positions[v]) > maxVert)
            {
                maxVert = abs(shapes[i].mesh.positions[v]);
            }
        }
        vertices.push_back(tempVert);

        if (shapes[i].mesh.texcoords.size() <= 0)
        {
            for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) 
            {
                tempTex.push_back(3*shapes[i].mesh.positions[v]+0 * tex_multiplier);
                tempTex.push_back(3*shapes[i].mesh.positions[v]+1 * tex_multiplier);
            }
            texcoords.push_back(tempTex);   
        }
        else
        {
            for (size_t v = 0; v < shapes[i].mesh.texcoords.size(); v++) 
            {
                tempTex.push_back(shapes[i].mesh.texcoords[v] * tex_multiplier);
            }
            texcoords.push_back(tempTex);   
        }

        
        if (shapes[i].mesh.normals.size() <= 0)
        {
            for (size_t v = 0; v < shapes[i].mesh.positions.size(); v++) 
            {
                tempNorm.push_back(0.0f);
            }
            normals.push_back(tempNorm);
        }
        else
        {
            for (size_t v = 0; v < shapes[i].mesh.positions.size(); v++) 
            {
                tempNorm.push_back(shapes[i].mesh.normals[v]);
            }
            normals.push_back(tempNorm);
        }
        
    }
}

int anObj::createVao(int programID, int index)
{
    unsigned int tempVao;

    glUseProgram(programID);
    glGenVertexArrays(1, &tempVao);
    glBindVertexArray(tempVao);

    unsigned int buffer[6];
    glGenBuffers(6, buffer);

    // Set vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices[index].size() * sizeof(float), &vertices[index][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, normals[index].size() * sizeof(float), &normals[index][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

    // Texture attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, texcoords[index].size() * sizeof(float), &texcoords[index][0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

   	// Tangent attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ARRAY_BUFFER, tangents[index].size() * sizeof(glm::vec3), &tangents[index][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

   	// BiTangent attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[4]);
	glBufferData(GL_ARRAY_BUFFER, bitangents[index].size() * sizeof(glm::vec3), &bitangents[index][0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[5]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[index].size() * sizeof(int), &indices[index][0], GL_STATIC_DRAW);
    
    // Un-bind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return tempVao;
}



int anObj::render(int programID, int depthID, Viewer *Camera, float flag, float dt, loadTex *Texture, int i)
{
}
