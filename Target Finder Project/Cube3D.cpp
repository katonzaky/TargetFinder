
#include "Cube3D.h"
#include <iostream>
#include <stdio.h>

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define CUBE_NUM_VERTICES 24
#define CUBE_NUM_TRIS 34
#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2

unsigned int vaoHandle;
unsigned int vaoHandle2;

float tLightX = 0.0f;
float tLightY = 5.0f;
float tLightZ = -5.0f;


using namespace std;

/**
 * Creates a new vertex array object for a cube
 * and loads in data into a vertex attribute buffer
 */
int Cube3D::createVAO(int programID, unsigned int vao)
{
    // Cube vertex data
    float Vertices[ CUBE_NUM_VERTICES * VALS_PER_VERT ] = {
                -1.0f, -1.0f, 1.0f, //Vertex 0
                1.0f, -1.0f, 1.0f,  //v1
                -1.0f, 1.0f, 1.0f,  //v2
                1.0f, 1.0f, 1.0f,   //v3

                1.0f, -1.0f, 1.0f,  //...
                1.0f, -1.0f, -1.0f,         
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,

                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,            
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,         
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,

                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,         
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,

                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,           
                -1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, -1.0f
    };

    float normals [ CUBE_NUM_VERTICES * VALS_PER_VERT ] = {
                -1.0f, -1.0f, 1.0f, //Vertex 0
                1.0f, -1.0f, 1.0f,  //v1
                -1.0f, 1.0f, 1.0f,  //v2
                1.0f, 1.0f, 1.0f,   //v3

                1.0f, -1.0f, 1.0f,  //...
                1.0f, -1.0f, -1.0f,         
                1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, -1.0f,

                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,            
                1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f, 1.0f,         
                -1.0f, 1.0f, -1.0f,
                -1.0f, 1.0f, 1.0f,

                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,         
                -1.0f, -1.0f, 1.0f,
                1.0f, -1.0f, 1.0f,

                -1.0f, 1.0f, 1.0f,
                1.0f, 1.0f, 1.0f,           
                -1.0f, 1.0f, -1.0f,
                1.0f, 1.0f, -1.0f
    };

    // 12 triangles - 2 per face of the cube
    unsigned int Indices[CUBE_NUM_TRIS] = {
         0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
         4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
         8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
        12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
        16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
        20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
    };

    // Cube vertex data
    float Textures[ CUBE_NUM_VERTICES * VALS_PER_TEX ] = {
                0.0f, 0.0f, //Vertex 0
                0.33f, 0.0f, //v1
                0.0f, 0.5f,  //v2
                0.33f, 0.5f, //v3

                0.0f, 0.5f, //Vertex 0
                0.33f, 0.5f, //v1
                0.0f, 1.0f,  //v2
                0.33f, 1.0f,

                0.66f, 0.5f,
                1.0f, 0.5f,            
                0.66f, 1.0f,
                1.0f, 1.0f,

                0.66f, 0.0f,
                1.0f, 0.0f,        
                0.66f, 0.5f, 
                1.0f, 0.5f, 

                0.33f, 0.0f, 
                0.66f, 0.0f,       
                0.33f, 0.5f, 
                0.66f, 0.5f, 

                0.33f, 0.5f, 
                0.66f, 0.5f,            
                0.33f, 1.0f, 
                0.66f, 1.0f
    };

    glUseProgram(programID);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int buffer[4];
	glGenBuffers(4, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
                 sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

    // Normal attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Textures), Textures, GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

 	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(Indices), Indices, GL_STATIC_DRAW);   

    // Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
    
	return vao;
}

/**
 Constructor - calculates the table's dimensions based on the
 desired height and width.
*/
Cube3D::Cube3D(int id1)
{
    vaoHandle = createVAO(id1, vaoHandle);
}  

/**
 Draw the table as a set of scaled blocks.
*/  
void Cube3D::renderFloor(int programID, GLuint TexID[], GLuint CurrentTex, GLenum CurrentTexSample)
{
    glUseProgram(programID);
    
	int texHandle = glGetUniformLocation(programID, "texMap");
	int modelUniformHandle = glGetUniformLocation(programID, "modelview_matrix");
    int normHandle = glGetUniformLocation(programID, "normal_matrix");
    int lightposHandle = glGetUniformLocation(programID, "light_pos");
    if (modelUniformHandle == -1 || normHandle==-1 || lightposHandle == -1 || texHandle == -1) {
        fprintf(stderr, "Error: can't find matrix uniforms\n");
        //exit(1);
    }
    
    //cout << "TexID2: " << TexID[CurrentTex] << endl;
    
    glBindTexture(GL_TEXTURE_2D, TexID[CurrentTex]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, CurrentTexSample);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, CurrentTexSample);	
    
    // Update the light position
    float lightPos[4] = { tLightX, tLightY, tLightZ, 1.0f };   
    glUniform4fv(lightposHandle, 1, lightPos); 

 	glBindVertexArray(vaoHandle);   

    // We have separated modelling from viewing.
    // Viewing (i.e. placing the camera relative to the entire table)
    // is handled in the viewer class.
    glm::mat4 model;
    glm::mat3 normMatrix;
    
    // We are using texture unit 0 (the default)
	glUniform1i(texHandle, 0);
        
    // leg 1
    model = glm::mat4(1.0f);   
    normMatrix = glm::mat3(model);
    glUniformMatrix3fv(normHandle, 1, false, glm::value_ptr(normMatrix));
    glUniformMatrix4fv( modelUniformHandle, 1, false, glm::value_ptr(model) );
    glDrawElements(GL_TRIANGLE_STRIP, CUBE_NUM_TRIS, GL_UNSIGNED_INT, 0);	
    
    glBindVertexArray(0);

    glFlush();

}
