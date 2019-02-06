#include "loadTex.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2
#define VALS_PER_COLOUR 4

using namespace std;

loadTex::loadTex(string f, vector<tinyobj::shape_t> s, vector<tinyobj::material_t> m)
{
	filepath = f;
	materials = m;
	shapes = s;
	filepath_bump = "";
	filepath_displacement = "";

	CurrentTexSample = GL_NEAREST;
	strcpy(CurrentTexString, "GL_NEAREST");
	
	CurrentTexMode = GL_REPLACE;
	strcpy(CurrentTexModeString, "GL_REPLACE");
	
	CurrentTex = 0;	
	createTexture();
}

loadTex::loadTex(std::string f, vector<tinyobj::shape_t> s, std::vector<tinyobj::material_t> m, std::string n)
{
	filepath = f;
	materials = m;
	shapes = s;
	filepath_bump = n;
	filepath_displacement = "";
	
	CurrentTexSample = GL_NEAREST;
	strcpy(CurrentTexString, "GL_NEAREST");
	
	CurrentTexMode = GL_REPLACE;
	strcpy(CurrentTexModeString, "GL_REPLACE");
	
	CurrentTex = 0;	
	createTexture();
}

loadTex::loadTex(std::string f, vector<tinyobj::shape_t> s, std::vector<tinyobj::material_t> m, std::string n, std::string d)
{
	filepath = f;
	materials = m;
	shapes = s;
	filepath_bump = n;
	filepath_displacement = d;

	CurrentTexSample = GL_NEAREST;
	strcpy(CurrentTexString, "GL_NEAREST");
	
	CurrentTexMode = GL_REPLACE;
	strcpy(CurrentTexModeString, "GL_REPLACE");
	
	CurrentTex = 0;	
	createTexture();
}

void loadTex::loadTexture(string path, int index)
{
    // load an image from file as texture 1
    int x, y, n;
    unsigned char *data = stbi_load(
                                    path.c_str(), /*char* filepath */
                                    &x, /*The address to store the width of the image*/
                                    &y, /*The address to store the height of the image*/
                                    &n  /*Number of channels in the image*/,
                                    0   /*Force number of channels if > 0*/
    );

    glBindTexture( GL_TEXTURE_2D, TexID[index] );
    //cout << "n: " << n << endl;
    if (n == 3) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if (n == 4) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        fprintf(stderr, "Image pixels are not RGB. Texture may not load correctly.");
    }
    stbi_image_free(data);
    
    glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void loadTex::createTexture()
{
    glGenTextures( NUM_TEXTURES, TexID ); 

	if (shapes.size() <= 0 && materials.size() <= 0)
	{
	    loadTexture(filepath, 0);

	    //for bump map textures
	    if (filepath_bump.size() > 1 && filepath_bump != "")
	    {
		    loadTexture(filepath_bump, 1);
	    }
	    //for displacement map textures
	    if (filepath_displacement.size() > 1 && filepath_displacement != "")
	    {
		    loadTexture(filepath_displacement, 2);
	    }
	    
	    return;
	}
    
    for (size_t i = 0; i < shapes.size(); i++)
    {
    	int index = shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;
    	//cout << "index: " << index << endl;
    	if (materials[index].diffuse_texname.size() == 0 || materials[index].diffuse_texname == "")
    	{
			// create checkerboard image
			// We don't really need RGB channels for this image, but use them anyway.
			//cout << "IN HERE?" << endl;
			for(int j=0; j < TEX_SIZE; j++){
				for(int k=0; k < TEX_SIZE; k++)
				{
				    myimage[j][k][0]  = 155;
				    myimage[j][k][1]  = 155;
				    myimage[j][k][2]  = 155;
				}                
			}

			// load checkerboard image as texture 0
			glBindTexture( GL_TEXTURE_2D, TexID[index] );
			glTexImage2D(GL_TEXTURE_2D, 
				         0,                 // level
				         GL_RGB,            // colour components
				         TEX_SIZE,          // width
				         TEX_SIZE,          // height
				         0,                 // #define NUM_TEXTURES 2border
				         GL_RGB,            // pixel format
				         GL_UNSIGNED_BYTE,  // pixel type
				         myimage);          // texture data
		
			// generate mipmaps for the texture 
			glGenerateMipmap(GL_TEXTURE_2D); 
    	}
    	else
    	{
		    // load an image from file as texture 1
		    string texfile = filepath + materials[index].diffuse_texname;
		    
		    loadTexture(texfile, index);
    	}
	    
	    //for bump map textures
	    if (filepath_bump.size() > 1 && filepath_bump != "")
	    {
	    	string texfile = filepath_bump + materials[index].bump_texname;
		    
		    loadTexture(texfile, shapes.size());
	    }
	    else if (filepath_bump.size() == 0)
	    {
			for(int j=0; j < TEX_SIZE; j++){
				for(int k=0; k < TEX_SIZE; k++)
				{
				    myimage[j][k][0]  = 255;
				    myimage[j][k][1]  = 255;
				    myimage[j][k][2]  = 255;
				}                
			}

			// load checkerboard image as texture 0
			glBindTexture( GL_TEXTURE_2D, TexID[shapes.size()] );
			glTexImage2D(GL_TEXTURE_2D, 
				         0,                 // level
				         GL_RGB,            // colour components
				         TEX_SIZE,          // width
				         TEX_SIZE,          // height
				         0,                 // #define NUM_TEXTURES 2border
				         GL_RGB,            // pixel format
				         GL_UNSIGNED_BYTE,  // pixel type
				         myimage);          // texture data
		
			// generate mipmaps for the texture 
			glGenerateMipmap(GL_TEXTURE_2D); 
	    }

	    //for displacement map textures
	    if (filepath_displacement.size() > 1 && filepath_displacement != "")
	    {
	    	string texfile = filepath_displacement + materials[index].displacement_texname;
		    
		    loadTexture(texfile, shapes.size()+1);
	    }
    }
}
