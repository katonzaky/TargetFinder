/**
ASSIGNMENT 3 PART 2
Rafiqi Rosli - a1682431
Zaky Katon
Jamie 

Things done:
- Load multiple .obj
- Load multiple textures
- Anti-aliasing 
- Enable alpha mode (no proper blending yet)
- Shadow mapping
- Cube Mapping
- Depth Mapping
- Directional Light (We need 3 types of lighting)
- Multiple Shaders
- Multiple Cameras

*/
#include <iterator>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#define STB_IMAGE_IMPLEMENTATION
#include "loadTex.h"
#include "loadObj.h"
#include "InputState.h"
#include "Viewer.h"
#include "anObj.h"
#include "plane.h"

#include "glm/glm.hpp"
#include "Shader.hpp"


#define DEG2RAD(x) ((x)*M_PI/180.0) 
#define RAD2DEG(x) ((x)*180.0/M_PI) 
#define VALS_PER_TEX 2

using namespace std;

int winX = 800;
int winY = 600;

WorldObjectViewer *WorldCam;
ObjectViewer *ObjCam;
FirstPerson *FPScam;
Viewer *Camera;
loadTex *Texture1, *Texture2, *Texture3;
loadObj *Object;
plane *plane1;

vector<float> starting;
vector<float> timez;
vector<float> timez2;
vector<plane> smokez;
vector<plane> firez;
vector<anObj> objectz;
vector<loadTex> texturez;

anObj* objectArray[100];

glm::vec3 cameraInit(0.0f, 0.0f, -5.0f);

//Shadow Stuff
GLuint depthMapFBO;
GLuint depthCubeMapFBO;
GLuint depthMap;
GLuint SHADOW_WIDTH = 512;
GLuint SHADOW_HEIGHT = 512;
GLuint depthCubemap;

GLuint skyboxVAO;
GLuint cubemapTexture;
// Data structure storing mouse input info
InputState Input;

float flagBump = 0.0f;
float flagDisplace = 0.0f;
glm::mat4 viewMatrix;
glm::vec3 aimVec;
glm::vec3 aimVec2;

// Our shader program
unsigned int Shader1;
unsigned int Shader2;
unsigned int Shader3;
unsigned int Shader4;
unsigned int Shader5;
unsigned int programID;
unsigned int depthID;
unsigned int depthCubeID;
unsigned int cubemapID;
unsigned int waterID;

unsigned int planeVAO;

float lightDiffuse[3] = {1.0f, 1.0f, 1.0f};
float lightSpecular[3] = {1.0f, 1.0f, 1.0f};

float maxVertG = 0.0f;
float maxX = 0.0f;
float minX = 999.0f;
float maxY = 0.0f;
float minY = 999.0f;
float maxZ = 0.0f;
float minZ = 999.0f;

float flagCamera = 0; 
GLboolean shadows = true;

float dt = 0.0f;
float lf = 0.0f;
bool firstMouse = true;

bool timeFlag = false;
bool childFlag = true;
time_t start;
float timer = 0.0f;
float timer2 = 0.0f;
bool fireCheck = false;
bool smokeCheck = false;
float moveSpeed = 0.0f;
float cameraSpeed = 10.0 * moveSpeed;
bool bulletCollisionFlag = false;
clock_t timingFire;
float duration = 0;

float yaw   = -90.0f;   // yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  winX / 2.0;
float lastY =  winY / 2.0;
float fov   =  45.0f;
vector<vector<float> > objectExtremities (5,vector<float> (4,0));


//
// Callbacks
//


void objectTranslations(){

    
    for(int b = 0; b <3; b++){
        anObj *tempObject = objectArray[b];
        objectExtremities[b][0] = tempObject->maxX;
        objectExtremities[b][1] = tempObject->minX;
        objectExtremities[b][2] = tempObject->maxZ;
        objectExtremities[b][3] = tempObject->minZ;
    }
    
	/*
    for(int a = 0; a < objectExtremities.size(); a++){
            cout << objectExtremities[a][0] << endl;
            cout << objectExtremities[a][1] << endl;
            cout << objectExtremities[a][2] << endl;
            cout << objectExtremities[a][3] << endl;
            cout << " Next Object" << endl << endl;
    }
    */
}
/*
void didBulletHit(){

    float xpos = timer*(Camera->cameraPos + Camera->cameraFront).x;
    float ypos = 0.5;
    float zpos = timer*(Camera->cameraPos + Camera->cameraFront).z;


    if((xpos > 50.0 && xpos < 55.0) || (xpos < -50.0 && xpos > -55.0)){ //collision positions, fill as required for the objects

        if(bulletCollisionFlag != true){
            bulletCollisionFlag = true;
            pid_t pid = fork();
            if(pid == 0){
                system("canberra-gtk-play -f sound/Frying-Pan-Impact-SoundBible.com-786709826.wav");
                exit(0);
                sleep(10);       
            }

        }
        

    }

    //cout << "Bullet position is : (" << xpos << "," << ypos << "," << zpos << ")" << endl;

}
*/

void fireProjectile(){

    time_t starttimer = time(0);
    fireCheck = true;
}

void smokie()
{
	time_t starttimer = time(0);
    smokeCheck = true;
}

bool didICollide(int direction){

    if(direction == 1){ 
        //cout << "(" << Camera->cameraPos.x << " , " << Camera->cameraPos.z << ")" << endl;
        if(Camera->cameraPos.x + cameraSpeed * Camera->cameraFront.x > 30.0){
           Camera->cameraPos += cameraSpeed * Camera->cameraFront;
           Camera->cameraPos.x = 30.0;
        }
        else if(Camera->cameraPos.x + cameraSpeed * Camera->cameraFront.x < -30.0){
           Camera->cameraPos += cameraSpeed * Camera->cameraFront;
           Camera->cameraPos.x = -30.0;
        }
        else if(Camera->cameraPos.z + cameraSpeed * Camera->cameraFront.z > 30.0){
           Camera->cameraPos += cameraSpeed * Camera->cameraFront;
           Camera->cameraPos.z = 30.0;
        }
        else if(Camera->cameraPos.z + cameraSpeed * Camera->cameraFront.z < -30.0){
           Camera->cameraPos += cameraSpeed * Camera->cameraFront;
           Camera->cameraPos.z = -30.0;
        }
        bool hitflag = false;
        for(int gat = 0; gat < 3; gat++){
            if((Camera->cameraPos.x + cameraSpeed * Camera->cameraFront.x) < objectExtremities[gat][0] &&
            (Camera->cameraPos.x + cameraSpeed * Camera->cameraFront.x) > objectExtremities[gat][1] && 
            (Camera->cameraPos.z + cameraSpeed * Camera->cameraFront.z) < objectExtremities[gat][2] && 
            (Camera->cameraPos.z + cameraSpeed * Camera->cameraFront.z) > objectExtremities[gat][3]){

                //cout << "Setting hitflag to true at collision with " << objectExtremities[gat][0] << "," << objectExtremities[gat][1] << "," << objectExtremities[gat][2] << "," << objectExtremities[gat][3] << endl;
                hitflag = true;
                float mindist = 9999;
                int temp = -1;
                /*
                cout << abs(Camera->cameraPos.x - objectExtremities[gat][0]) << " difference between x and max" << endl;
                cout << abs(Camera->cameraPos.x - objectExtremities[gat][1]) << " difference between x and min" << endl;
                cout << abs(Camera->cameraPos.z - objectExtremities[gat][2]) << " difference between z and max" << endl;
                cout << abs(Camera->cameraPos.z - objectExtremities[gat][3]) << " difference between z and min" << endl;
                */
                for(int fo = 0; fo < 4; fo++){
                    if(fo == 0 || fo ==1){
                         if(abs(Camera->cameraPos.x - objectExtremities[gat][fo]) < mindist){
                            mindist = abs(Camera->cameraPos.x - objectExtremities[gat][fo]); 
                            //cout << mindist << endl;
                            temp = fo;
                            //cout << mindist << endl;
                        }
                    }
                    if(fo == 2 || fo == 3){
                        if(abs(Camera->cameraPos.z - objectExtremities[gat][fo]) < mindist){
                            mindist = abs(Camera->cameraPos.z - objectExtremities[gat][fo]); 
                            //cout << mindist << endl;
                            temp = fo;
                        }
                    }
                }

               // cout << mindist << endl;
                //cout << temp << " is temp" << endl;
                if(temp == 0 || temp == 1){
                    Camera->cameraPos += cameraSpeed * Camera->cameraFront;
                    Camera->cameraPos.x = objectExtremities[gat][temp];

                }
                else if(temp == 2 || temp == 3){
                    Camera->cameraPos += cameraSpeed * Camera->cameraFront;
                    Camera->cameraPos.z = objectExtremities[gat][temp];
                }
                

            }
        }
        if(hitflag == true){
            return true;
        }

        else{
            return false;
        }
    }
    if(direction == 2){ 
        if(Camera->cameraPos.x - cameraSpeed * Camera->cameraFront.x > 30.0){
           Camera->cameraPos -= cameraSpeed * Camera->cameraFront;
           Camera->cameraPos.x = 30.0;
        }
        else if(Camera->cameraPos.x - cameraSpeed * Camera->cameraFront.x < -30.0){
           Camera->cameraPos -= cameraSpeed * Camera->cameraFront;
           Camera->cameraPos.x = -30.0;
        }
        else if(Camera->cameraPos.z - cameraSpeed * Camera->cameraFront.z > 30.0){
           Camera->cameraPos -= cameraSpeed * Camera->cameraFront;
           Camera->cameraPos.z = 30.0;
        }
        else if(Camera->cameraPos.z - cameraSpeed * Camera->cameraFront.z < -30.0){
           Camera->cameraPos -= cameraSpeed * Camera->cameraFront;
           Camera->cameraPos.z = -30.0;
        }
        else{
            return false;
        }

    
    }

    
    if(direction != 1 || direction != 2){

        return false;
    }


}

float flashlight = 0.0f;

void key_callback(GLFWwindow* window,
                  int key, int scancode, int action, int mods)
{
    float cameraSpeed = 10.0 * moveSpeed;
    if (action == GLFW_PRESS) {
        switch(key) 
            {
            case GLFW_KEY_ESCAPE: // escape key pressed
            	system("xrandr -s 1920x1080");
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case '1':
                cout << "First-person mode" << endl;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                Camera = FPScam;
                flagCamera = 0;
                break;
            case '2':
                cout << "Object view mode" << endl;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                Camera->reset();
                Camera = ObjCam;
                flagCamera = 1;  
                break;
            case '3':
                cout << "World view mode" << endl;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                Camera->reset();
                Camera = WorldCam;
                flagCamera = 2;  
                break;
            case GLFW_KEY_F:
            	if (flashlight == 1.0f) {
                	cout << "FLASHLIGHT OFF" << endl;
					flashlight = 0.0f;     
					glUseProgram(programID);
					glUniform1fv(glGetUniformLocation(programID, "flashlight"), 1, &flashlight);
 	
                }
				else {
					cout << "FLASHLIGHT ON" << endl;
					flashlight = 1.0f;
					glUseProgram(programID);
					glUniform1fv(glGetUniformLocation(programID, "flashlight"), 1, &flashlight);

				}
                break;
            default:
                break;
            }
    }

    if (flagCamera == 0)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            if(didICollide(1) == false){
                Camera->cameraPos += cameraSpeed * Camera->cameraFront;
            }
            //cout << "x : " << Camera->cameraPos.x << endl;
            //cout << "z : " << Camera->cameraPos.z << endl;

            if(timeFlag == true){  
                timeFlag = false;
                pid_t pid = fork();
                if(pid == 0){
                    system("canberra-gtk-play -f sound/footsteps.wav");
                    exit(0);
                    sleep(10);
                    
                }

            }

        }
                
             
            
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            if(didICollide(2) == false){
                Camera->cameraPos -= cameraSpeed * Camera->cameraFront;
            }
            if(timeFlag == true){  
                timeFlag = false;
                pid_t pid = fork();
                if(pid == 0){
                    system("canberra-gtk-play -f sound/footsteps.wav");
                    exit(0);
                    sleep(10);
                    
                }

            }
        }
            
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            if(didICollide(3) == false){
                Camera->cameraPos -= glm::normalize(glm::cross(Camera->cameraFront, Camera->cameraUp)) * cameraSpeed;
            }
            if(timeFlag == true){  
                timeFlag = false;
                pid_t pid = fork();
                if(pid == 0){
                    system("canberra-gtk-play -f sound/footsteps.wav");
                    exit(0);
                    sleep(10);
                    
                }

            }
        }
            
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            if(didICollide(4) == false){
                Camera->cameraPos += glm::normalize(glm::cross(Camera->cameraFront, Camera->cameraUp)) * cameraSpeed;
            } 
            if(timeFlag == true){  
                timeFlag = false;
                pid_t pid = fork();
                if(pid == 0){
                    system("canberra-gtk-play -f sound/footsteps.wav");
                    exit(0);
                    sleep(10);
                    
                }

            } 
        }

        Camera->cameraPos.y = 2.0f;
    }
}   

int setupMaterials(float ma[], float md[], float ms[], float  shine)
{
    // Uniform variables defining material colours
    // These can be changed for each sphere, to compare effects
    int mtlambientHandle = glGetUniformLocation(programID, "mtl_ambient");
	int mtldiffuseHandle = glGetUniformLocation(programID, "mtl_diffuse");
	int mtlspecularHandle = glGetUniformLocation(programID, "mtl_specular");
	if ( mtlambientHandle == -1 ||
         mtldiffuseHandle == -1 ||
         mtlspecularHandle == -1) {
        //fprintf(stderr, "Error: can't find material uniform variables\n");
		//return 1;
    }
    
    float *mtlambient, *mtldiffuse, *mtlspecular;
    mtlambient = ma;
    mtldiffuse = md;
    mtlspecular = ms;

	glUniform3fv(mtlambientHandle, 1, mtlambient);
	glUniform3fv(mtldiffuseHandle, 1, mtldiffuse);
	glUniform3fv(mtlspecularHandle, 1, mtlspecular);    

    int mtlshininessHandle = glGetUniformLocation(programID, "mtl_shininess");
    if (mtlshininessHandle == -1)
    {
        //fprintf(stderr, "Error: can't find shininess uniform variables\n");
    }

    float shininess;
    shininess = shine;

    glUniform1fv(mtlshininessHandle, 1, &shininess);
    
	return 0;	// return success
}

// Set the projection matrix. Takes into account window aspect ratio, so called
// when the window is resized.
void setProjection()
{

    glm::mat4 projection;
    
    projection = glm::perspective( (float)M_PI/3.0f, (float) winX / winY, 0.1f, 100.0f );

	// Load it to the shader program
	int projHandle1 = glGetUniformLocation(Shader1, "projection_matrix");
	int projHandle2 = glGetUniformLocation(Shader2, "projection_matrix");
	int projHandle3 = glGetUniformLocation(Shader3, "projection_matrix");
	int projHandle4 = glGetUniformLocation(Shader4, "projection_matrix");
	int projHandle5 = glGetUniformLocation(Shader5, "projection_matrix");
	glUseProgram(Shader1);
	glUniformMatrix4fv( projHandle1, 1, false, glm::value_ptr(projection) );
   	glUseProgram(Shader2);
    glUniformMatrix4fv( projHandle2, 1, false, glm::value_ptr(projection) );
    glUseProgram(Shader3);
    glUniformMatrix4fv( projHandle3, 1, false, glm::value_ptr(projection) );
    glUseProgram(Shader4);
    glUniformMatrix4fv( projHandle4, 1, false, glm::value_ptr(projection) );
    glUseProgram(Shader5);
    glUniformMatrix4fv( projHandle5, 1, false, glm::value_ptr(projection) );

}    

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y ) 
{
    winX = x;
    winY = y;
    setProjection();
    glViewport( 0, 0, x, y );
}

void mouse_pos_callback(GLFWwindow* window, double x, double y)
{
    // Use a global data structure to store mouse info
    // We can then use it however we want
    Input.update((float)x, (float)y);

    if (flagCamera == 0)
    {
        if (firstMouse)
        {
            lastX = x;
            lastY = y;
            firstMouse = false;
        }

        float xoffset = x - lastX;
        float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top
        lastX = x;
        lastY = y;

        float sensitivity = 0.1f; // change this value to your liking
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        Camera->cameraFront = glm::normalize(front); 
    }
}    

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            Input.rMousePressed = true;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            Input.rMousePressed = false;
        }
        else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
            Input.lMousePressed = true;
			fireProjectile();
        }
        else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
            Input.lMousePressed = false;
        }        
}

void renderScene(int id)
{
    glUseProgram(id);
    glActiveTexture(GL_TEXTURE0); //diffuse texture
    glBindTexture(GL_TEXTURE_2D, Texture1->TexID[0]);
    glActiveTexture(GL_TEXTURE3); //normal texture
    glBindTexture(GL_TEXTURE_2D, Texture1->TexID[1]);
    glActiveTexture(GL_TEXTURE4); //displacement texture
    glBindTexture(GL_TEXTURE_2D, Texture1->TexID[2]);
    
    flagBump = 1.0f;
    glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);
    flagDisplace = 1.0f;
	glUniform1fv(glGetUniformLocation(id, "displaceFlag"), 1, &flagDisplace);    

    glBindVertexArray(plane1->vaoBuffer);

	glm::mat4 mvMatrix;
	glm::mat3 normMatrix;
	mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, 19.5f, 0.0f));
    glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
	normMatrix = glm::mat3(mvMatrix);
    glUniformMatrix3fv(glGetUniformLocation(id, "normal_matrix"), 1, false, glm::value_ptr(normMatrix) );
    glBindVertexArray(plane1->vaoBuffer);    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

	//Object 0 - Hammock
	for (size_t i = 0; i < objectz[0].shapes.size(); i++)
    {
    	int index = objectz[0].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[0].materials[index].ambient[0], objectz[0].materials[index].ambient[0], objectz[0].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[0].materials[index].diffuse[0], objectz[0].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[0].materials[index].specular[0], objectz[0].materials[index].specular[0], objectz[0].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[0].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[0].TexID[index]);

	    glBindVertexArray(objectz[0].vaoBuffer[i]);


    	glm::mat4 mvMatrix;
		mvMatrix = glm::rotate(mvMatrix,1.0f, glm::vec3(0.0f,1.0f,0.0f));
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.06f, 0.06f, 0.06f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f, 15.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[0].indices[i].size(), GL_UNSIGNED_INT, 0); 


        glBindVertexArray(0);  
    }

	//Object 1 - Boat
	for (size_t i = 0; i < objectz[1].shapes.size(); i++)
    {
    	int index = objectz[1].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[1].materials[index].ambient[0], objectz[1].materials[index].ambient[1], objectz[1].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[1].materials[index].diffuse[1], objectz[1].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[1].materials[index].specular[0], objectz[1].materials[index].specular[1], objectz[1].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[1].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[1].TexID[index]);

	    glBindVertexArray(objectz[1].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        mvMatrix = glm::scale(mvMatrix, glm::vec3(1.4f, 1.4f, 1.4f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-18.0f,0.0f, 0.0f));
		mvMatrix = glm::rotate(mvMatrix,0.5f, glm::vec3(0.0f,0.0f,1.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[1].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 2 - Sandbag
	for (size_t i = 0; i < objectz[2].shapes.size(); i++)
    {

    	int index = objectz[2].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[2].materials[index].ambient[0], objectz[2].materials[index].ambient[1], objectz[2].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[2].materials[index].diffuse[1], objectz[2].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[2].materials[index].specular[0], objectz[2].materials[index].specular[1], objectz[2].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[2].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[2].TexID[index]);

	    glBindVertexArray(objectz[2].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);

		mvMatrix = glm::rotate(mvMatrix,(float)M_PI/2.0f, glm::vec3(0.0f, 1.0f, 0.0f));          
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.2f,0.2f,0.2f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(90.0f,-02.0f,-116.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[2].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 3 - Hut and fireplace
	for (size_t i = 0; i < objectz[3].shapes.size(); i++)
    {

    	int index = objectz[3].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[3].materials[index].ambient[0], objectz[3].materials[index].ambient[1], objectz[3].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[3].materials[index].diffuse[1], objectz[3].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[3].materials[index].specular[0], objectz[3].materials[index].specular[1], objectz[3].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[3].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[3].TexID[index]);

	    glBindVertexArray(objectz[3].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.3f, 0.3f, 0.3f));
		mvMatrix = glm::translate(mvMatrix, glm::vec3(40.0f,-1.5f,-40.0f));
		mvMatrix = glm::rotate(mvMatrix,-1.5f, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[3].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 4 - Barrel1
	for (size_t i = 0; i < objectz[4].shapes.size(); i++)
    {

    	int index = objectz[4].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[4].materials[index].ambient[0], objectz[4].materials[index].ambient[1], objectz[4].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[4].materials[index].diffuse[1], objectz[4].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[4].materials[index].specular[0], objectz[4].materials[index].specular[1], objectz[4].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[4].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[4].TexID[index]);

	    glBindVertexArray(objectz[4].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(50.0f,50.0f,50.0f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-0.35f,-0.005f,-0.4f));

        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[4].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 5 - Barrel2
	for (size_t i = 0; i < objectz[5].shapes.size(); i++)
    {

    	int index = objectz[5].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[5].materials[index].ambient[0], objectz[5].materials[index].ambient[1], objectz[5].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[5].materials[index].diffuse[1], objectz[5].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[5].materials[index].specular[0], objectz[5].materials[index].specular[1], objectz[5].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[5].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[5].TexID[index]);

	    glBindVertexArray(objectz[5].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(50.0f,50.0f,50.0f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-0.3f,-0.005f,-0.35f));

        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[5].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 6 - Rock1
	for (size_t i = 0; i < objectz[6].shapes.size(); i++)
    {

    	int index = objectz[6].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[6].materials[index].ambient[0], objectz[6].materials[index].ambient[1], objectz[6].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[6].materials[index].diffuse[1], objectz[6].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[6].materials[index].specular[0], objectz[6].materials[index].specular[1], objectz[6].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[6].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[6].TexID[index]);

	    glBindVertexArray(objectz[6].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(1.5f,1.5f,1.5f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(15.0f,0.0f,10.0f));
		mvMatrix = glm::rotate(mvMatrix,(float)M_PI/2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[6].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 7 - Rock 2
	for (size_t i = 0; i < objectz[7].shapes.size(); i++)
    {

    	int index = objectz[7].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[7].materials[index].ambient[0], objectz[7].materials[index].ambient[1], objectz[7].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[7].materials[index].diffuse[1], objectz[7].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[7].materials[index].specular[0], objectz[7].materials[index].specular[1], objectz[7].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[7].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[7].TexID[index]);

	    glBindVertexArray(objectz[7].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.03f, 0.03f, 0.03f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(400.0f, -10.0f, 400.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[7].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 8 - Rock3
	for (size_t i = 0; i < objectz[8].shapes.size(); i++)
    {

    	int index = objectz[8].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[8].materials[index].ambient[0], objectz[8].materials[index].ambient[1], objectz[8].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[8].materials[index].diffuse[1], objectz[8].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[8].materials[index].specular[0], objectz[8].materials[index].specular[1], objectz[8].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[8].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[8].TexID[index]);

	    glBindVertexArray(objectz[8].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(1.5f,1.5f,1.5f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(12.0f,0.0f,8.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[8].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 9 - Target1
	for (size_t i = 0; i < objectz[9].shapes.size(); i++)
    {

    	int index = objectz[9].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[9].materials[index].ambient[0], objectz[9].materials[index].ambient[1], objectz[9].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[9].materials[index].diffuse[1], objectz[9].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[9].materials[index].specular[0], objectz[9].materials[index].specular[1], objectz[9].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[9].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[9].TexID[index]);

	    glBindVertexArray(objectz[9].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);

		mvMatrix = glm::rotate(mvMatrix,(float)M_PI*1.25f, glm::vec3(0.0f, 1.0f, 0.0f));          
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.3f, 0.3f, 0.1f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-10.0f,0.0f, 250.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[9].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 10 - Target2
	for (size_t i = 0; i < objectz[10].shapes.size(); i++)
    {

    	int index = objectz[10].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[10].materials[index].ambient[0], objectz[10].materials[index].ambient[1], objectz[10].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[10].materials[index].diffuse[1], objectz[10].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[10].materials[index].specular[0], objectz[10].materials[index].specular[1], objectz[10].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[10].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[10].TexID[index]);

	    glBindVertexArray(objectz[10].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
		mvMatrix = glm::rotate(mvMatrix,(float)M_PI/1.45f, glm::vec3(0.0f, 1.0f, 0.0f));
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.3f, 0.3f, 0.1f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(05.0f,0.0f,230.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[10].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 11 - Target3
	for (size_t i = 0; i < objectz[11].shapes.size(); i++)
    {

    	int index = objectz[11].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[11].materials[index].ambient[0], objectz[11].materials[index].ambient[1], objectz[11].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[11].materials[index].diffuse[1], objectz[11].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[11].materials[index].specular[0], objectz[11].materials[index].specular[1], objectz[11].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[11].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[11].TexID[index]);

	    glBindVertexArray(objectz[11].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);

		mvMatrix = glm::rotate(mvMatrix,(float)M_PI/2.0f, glm::vec3(0.0f, 1.0f, 0.0f));        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.3f, 0.3f, 0.1f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-55.0f,0.0f,125.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[11].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 12 - Box 1
	for (size_t i = 0; i < objectz[12].shapes.size(); i++)
    {

    	int index = objectz[12].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[12].materials[index].ambient[0], objectz[12].materials[index].ambient[1], objectz[12].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[12].materials[index].diffuse[1], objectz[12].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[12].materials[index].specular[0], objectz[12].materials[index].specular[1], objectz[12].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[12].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[12].TexID[index]);

	    glBindVertexArray(objectz[12].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(1.0f,1.0f,1.0f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-21.0f,0.5f,-19.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[12].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 13 - Box2
	for (size_t i = 0; i < objectz[13].shapes.size(); i++)
    {

    	int index = objectz[13].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[13].materials[index].ambient[0], objectz[13].materials[index].ambient[1], objectz[13].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[13].materials[index].diffuse[1], objectz[13].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[13].materials[index].specular[0], objectz[13].materials[index].specular[1], objectz[13].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[13].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[13].TexID[index]);

	    glBindVertexArray(objectz[13].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(1.0f,1.0f,1.0f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-18.0f,0.5f,-17.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[13].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 14 - Box3
	for (size_t i = 0; i < objectz[14].shapes.size(); i++)
    {

    	int index = objectz[14].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[14].materials[index].ambient[0], objectz[14].materials[index].ambient[1], objectz[14].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[14].materials[index].diffuse[1], objectz[14].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[14].materials[index].specular[0], objectz[14].materials[index].specular[1], objectz[14].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[14].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[14].TexID[index]);

	    glBindVertexArray(objectz[14].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
		mvMatrix = glm::rotate(mvMatrix,(float)M_PI/4.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.5f,0.5f,1.5f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(34.0f,0.0f,-4.5f));

        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[14].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object15 - Box4
	for (size_t i = 0; i < objectz[15].shapes.size(); i++)
    {

    	int index = objectz[15].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[15].materials[index].ambient[0], objectz[15].materials[index].ambient[1], objectz[15].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[15].materials[index].diffuse[1], objectz[15].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[15].materials[index].specular[0], objectz[15].materials[index].specular[1], objectz[15].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[15].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[15].TexID[index]);

	    glBindVertexArray(objectz[15].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.5f,0.5f,0.5f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-31.0f,0.0f,-29.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[13].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 16 - Box5
	for (size_t i = 0; i < objectz[16].shapes.size(); i++)
    {

    	int index = objectz[16].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[16].materials[index].ambient[0], objectz[16].materials[index].ambient[1], objectz[16].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[16].materials[index].diffuse[1], objectz[16].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[16].materials[index].specular[0], objectz[16].materials[index].specular[1], objectz[16].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[16].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[16].TexID[index]);

	    glBindVertexArray(objectz[16].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.5f,0.5f,0.5f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-35.0f,0.0f,-30.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[13].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 17 - Box6
	for (size_t i = 0; i < objectz[17].shapes.size(); i++)
    {

    	int index = objectz[17].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[17].materials[index].ambient[0], objectz[17].materials[index].ambient[1], objectz[17].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[17].materials[index].diffuse[1], objectz[17].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[17].materials[index].specular[0], objectz[17].materials[index].specular[1], objectz[17].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[17].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[17].TexID[index]);

	    glBindVertexArray(objectz[17].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.5f,0.5f,0.5f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-28.0f,0.0f,-41.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[17].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 18 - Tree1
	for (size_t i = 0; i < objectz[18].shapes.size(); i++)
    {

    	int index = objectz[18].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[18].materials[index].ambient[0], objectz[18].materials[index].ambient[1], objectz[18].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[18].materials[index].diffuse[1], objectz[18].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[18].materials[index].specular[0], objectz[18].materials[index].specular[1], objectz[18].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[18].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[18].TexID[index]);

	    glBindVertexArray(objectz[18].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(420.0f, -10.0f, -300.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[18].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object19 - Tree2
	for (size_t i = 0; i < objectz[19].shapes.size(); i++)
    {

    	int index = objectz[19].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[19].materials[index].ambient[0], objectz[19].materials[index].ambient[1], objectz[19].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[19].materials[index].diffuse[1], objectz[19].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[19].materials[index].specular[0], objectz[19].materials[index].specular[1], objectz[19].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[19].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[19].TexID[index]);

	    glBindVertexArray(objectz[19].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(53.5f, -10.0f, 34.5f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[19].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 20 - Tree3
	for (size_t i = 0; i < objectz[20].shapes.size(); i++)
    {

    	int index = objectz[20].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[20].materials[index].ambient[0], objectz[20].materials[index].ambient[1], objectz[20].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[20].materials[index].diffuse[1], objectz[20].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[20].materials[index].specular[0], objectz[20].materials[index].specular[1], objectz[20].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[20].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[20].TexID[index]);

	    glBindVertexArray(objectz[20].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.05f, 0.05f, 0.05f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-51.0f, -10.0f, -32.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[20].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 21 Bush1
	for (size_t i = 0; i < objectz[21].shapes.size(); i++)
    {

    	int index = objectz[21].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[21].materials[index].ambient[0], objectz[21].materials[index].ambient[1], objectz[21].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[21].materials[index].diffuse[1], objectz[21].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[21].materials[index].specular[0], objectz[21].materials[index].specular[1], objectz[21].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[21].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[21].TexID[index]);

	    glBindVertexArray(objectz[21].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(1.5f,1.5f,1.5f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(0.0f,-0.4f,0.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[21].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 22 Tree5
	for (size_t i = 0; i < objectz[22].shapes.size(); i++)
    {

    	int index = objectz[22].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[22].materials[index].ambient[0], objectz[22].materials[index].ambient[1], objectz[22].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[22].materials[index].diffuse[1], objectz[22].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[22].materials[index].specular[0], objectz[22].materials[index].specular[1], objectz[22].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[22].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[22].TexID[index]);

	    glBindVertexArray(objectz[22].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.1f, 0.1f, 0.1f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-200.0f, -05.0f, -220.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[22].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 23 - Tree6
	for (size_t i = 0; i < objectz[23].shapes.size(); i++)
    {

    	int index = objectz[23].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[23].materials[index].ambient[0], objectz[23].materials[index].ambient[1], objectz[23].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[23].materials[index].diffuse[1], objectz[23].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[23].materials[index].specular[0], objectz[23].materials[index].specular[1], objectz[23].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[23].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[23].TexID[index]);

	    glBindVertexArray(objectz[23].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.15f, 0.09f, 0.15f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(110.0f, -10.0f, 135.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[23].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 24 - Wall
    for (size_t i = 0; i < objectz[24].shapes.size(); i++)
    {
    	int index = objectz[24].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;
    		
        float mambient[3] = {objectz[24].materials[index].ambient[0], objectz[24].materials[index].ambient[1], objectz[24].materials[index].ambient[2]};
        float mdiffuse[3] = {objectz[24].materials[index].diffuse[0], objectz[24].materials[index].diffuse[1], objectz[24].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[24].materials[index].specular[0], objectz[24].materials[index].specular[1], objectz[24].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[24].materials[index].shininess);

        flagBump = 1.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);
		flagDisplace = 1.0f;
		glUniform1fv(glGetUniformLocation(id, "displaceFlag"), 1, &flagDisplace);  
		
        glBindVertexArray(objectz[24].vaoBuffer[index]);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texturez[24].TexID[0]);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, texturez[24].TexID[1]);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, texturez[24].TexID[2]);

        mvMatrix = glm::mat4(1.0f);
        mvMatrix = glm::rotate(mvMatrix, (float)M_PI * 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        mvMatrix = glm::rotate(mvMatrix, (float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.15f, 5.5f, 3.5f));
        mvMatrix = glm::translate(mvMatrix, glm::vec3(-200.0f,-3.5f,-0.7f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[24].indices[index].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 25 - Fence1
	for (size_t i = 0; i < objectz[25].shapes.size(); i++)
    {

    	int index = objectz[25].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[25].materials[index].ambient[0], objectz[25].materials[index].ambient[1], objectz[25].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[25].materials[index].diffuse[1], objectz[25].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[25].materials[index].specular[0], objectz[25].materials[index].specular[1], objectz[23].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[25].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[25].TexID[index]);

	    glBindVertexArray(objectz[25].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);

        mvMatrix = glm::rotate(mvMatrix, (float)M_PI / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));          
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.002,0.002,0.020));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(15500.0f,-250.0f,0.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[25].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }	

	//Object 26 - Fence2
	for (size_t i = 0; i < objectz[26].shapes.size(); i++)
    {

    	int index = objectz[26].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[26].materials[index].ambient[0], objectz[26].materials[index].ambient[1], objectz[26].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[26].materials[index].diffuse[1], objectz[26].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[26].materials[index].specular[0], objectz[26].materials[index].specular[1], objectz[23].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[26].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[26].TexID[index]);

	    glBindVertexArray(objectz[26].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        mvMatrix = glm::rotate(mvMatrix, (float)M_PI / 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));     
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.002,0.002,0.020));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-15500.0f,-250.0f,0.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[26].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }	

	//Object 27 - Fence3
	for (size_t i = 0; i < objectz[27].shapes.size(); i++)
    {

    	int index = objectz[27].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[27].materials[index].ambient[0], objectz[27].materials[index].ambient[1], objectz[27].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[27].materials[index].diffuse[1], objectz[27].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[27].materials[index].specular[0], objectz[27].materials[index].specular[1], objectz[23].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[27].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[27].TexID[index]);

	    glBindVertexArray(objectz[27].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.002,0.002,0.020));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(15500.0f,-250.0f,0.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[27].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 28 - Fence4
	for (size_t i = 0; i < objectz[28].shapes.size(); i++)
    {

    	int index = objectz[28].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[28].materials[index].ambient[0], objectz[28].materials[index].ambient[1], objectz[28].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[28].materials[index].diffuse[1], objectz[28].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[28].materials[index].specular[0], objectz[28].materials[index].specular[1], objectz[23].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[28].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[28].TexID[index]);

	    glBindVertexArray(objectz[28].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(0.002,0.002,0.020));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(-15500.0f,-250.0f,0.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[28].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Object 29 Bush2
	for (size_t i = 0; i < objectz[29].shapes.size(); i++)
    {

    	int index = objectz[29].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[29].materials[index].ambient[0], objectz[29].materials[index].ambient[1], objectz[29].materials[index].ambient[2]};
        float mdiffuse[3] = {1.0f, objectz[29].materials[index].diffuse[1], objectz[29].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[29].materials[index].specular[0], objectz[29].materials[index].specular[1], objectz[29].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[29].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);

	    glActiveTexture(GL_TEXTURE0);
	    glBindTexture(GL_TEXTURE_2D, texturez[29].TexID[index]);

	    glBindVertexArray(objectz[29].vaoBuffer[i]);

    	glm::mat4 mvMatrix; 

    	mvMatrix = glm::mat4(1.0f);
        
        mvMatrix = glm::scale(mvMatrix, glm::vec3(1.5f,1.5f,1.5f));
	    mvMatrix = glm::translate(mvMatrix, glm::vec3(8.0f,-0.4f,7.0f));
        glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[29].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

	//Smoke Object
	for(int i = 0; i < smokez.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0); //diffuse texture
		glBindTexture(GL_TEXTURE_2D, Texture2->TexID[0]);
		
		flagBump = 0.0f;
		glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);
		flagDisplace = 0.0f;
		glUniform1fv(glGetUniformLocation(id, "displaceFlag"), 1, &flagDisplace);    

		glBindVertexArray(smokez[i].vaoBuffer);

		mvMatrix = glm::mat4(1.0f);
		mvMatrix = glm::rotate(mvMatrix, (float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));    
		mvMatrix = glm::translate(mvMatrix, glm::vec3(11.8f+starting[i],-8.6f+starting[i],-1.0f));
		mvMatrix = glm::translate(mvMatrix, glm::vec3(timez2[i]/20.0f,timez2[i]/11.0f, (i/150.0f)*(-timez[i]*(Camera->cameraPos + Camera->cameraFront).y)));

		glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
		glBindVertexArray(smokez[i].vaoBuffer);    
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	//Fire Object
	for(int i = 0; i < firez.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0); //diffuse texture
		glBindTexture(GL_TEXTURE_2D, Texture3->TexID[0]);
		
		flagBump = 0.0f;
		glUniform1fv(glGetUniformLocation(id, "bumpFlag"), 1, &flagBump);
		flagDisplace = 0.0f;
		glUniform1fv(glGetUniformLocation(id, "displaceFlag"), 1, &flagDisplace);    

		glBindVertexArray(firez[i].vaoBuffer);

		mvMatrix = glm::mat4(1.0f);
		mvMatrix = glm::rotate(mvMatrix, (float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));    
		mvMatrix = glm::translate(mvMatrix, glm::vec3(11.8f+starting[i],-8.6f+starting[i],0.2f));
		mvMatrix = glm::translate(mvMatrix, glm::vec3(timez2[i]/6.0f,timez2[i]/12.0f, (i/220.0f)*(-timez[i]*(Camera->cameraPos + Camera->cameraFront).y)));

		glUniformMatrix4fv(glGetUniformLocation(id, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
		glBindVertexArray(firez[i].vaoBuffer);    
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}


}
float at = 0.0f;
void render() 
{
	/***CAMERA MATRIX/POSITION**/
    Camera->update( Input );  

    if (flagCamera == 0)
    {
        viewMatrix = glm::lookAt(Camera->cameraPos, Camera->cameraPos + Camera->cameraFront, Camera->cameraUp);
        glm::vec3 viewDir = glm::vec3(viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2]);
        glUseProgram(programID);
        glUniform3fv( glGetUniformLocation(programID, "viewPos"), 1, &Camera->cameraPos[0]);  
        glUniform3fv( glGetUniformLocation(programID, "viewDir"), 1, &viewDir[0]); 
        glUseProgram(waterID);
        glUniform3fv( glGetUniformLocation(waterID, "viewPos"), 1, &Camera->cameraPos[0]);  
    }
    else
    {
        viewMatrix = Camera->getViewMtx();
		//Get Camera Position
		glm::mat3 rotmat(viewMatrix);
		glm::vec3 rotvec(viewMatrix[3]);
		glm::vec3 retvec = -rotvec * rotmat;
        glm::vec3 viewDir = glm::vec3(viewMatrix[2][0], viewMatrix[2][1], viewMatrix[2][2]);
		glUseProgram(programID); 
        glUniform3fv( glGetUniformLocation(programID, "viewPos"), 1, &retvec[0]);
        glUniform3fv( glGetUniformLocation(programID, "viewDir"), 1, &viewDir[0]);  
		glUseProgram(waterID); 
        glUniform3fv( glGetUniformLocation(waterID, "viewPos"), 1, &retvec[0]);
    }
 
	/***TIMEFLAG***/
    if(difftime(time(0),start) > 0.7){
        //cout << "resetting timeflag to true" << endl;
        timeFlag=true;
        start = time(0);
    }
	
    /***LIGHT 1***/
    glm::vec3 light1_pos = glm::vec3(-5.0f, 10.0f, 5.0f);
    glm::vec3 light1_at = glm::vec3(0.0f, 0.0f, 0.0f);
    glUseProgram(programID);
    glUniform3fv(glGetUniformLocation(programID, "light1_pos"), 1, &light1_pos[0]);
    
	
    /***LIGHT 2***/
    glm::mat4 lightMatrix;
    lightMatrix = glm::rotate(lightMatrix, (float)dt*-0.5f, glm::vec3(0.0, 1.0, 0.0));
    glm::vec4 vec = glm::vec4(0.0f, 5.0f, 10.0f, 1.0f);
    vec = glm::vec4(lightMatrix * vec);
    glm::vec3 light2_pos = glm::vec3(vec[0], vec[1], vec[2]);
	glUseProgram(programID);
	glUniform3fv(glGetUniformLocation(programID, "light2_pos"), 1, &light2_pos[0]);
	
    /***LIGHT 3***/
	glm::mat3 rotmat(viewMatrix);
	glm::vec3 rotvec(viewMatrix[3]);
	glm::vec3 retvec = -rotvec * rotmat;
    glm::vec3 light3_pos = glm::vec3(retvec[0]+2.0f, 5.0f, retvec[2]+2.0f);
   	glm::mat4 camDir = glm::transpose(viewMatrix);
    glm::vec3 light3_at = glm::vec3(-camDir[2][0], -camDir[2][1], -camDir[2][2]);
    glUseProgram(programID);
    glUniform3fv(glGetUniformLocation(programID, "light3_pos"), 1, &light3_pos[0]);

	/***POINT SHADOW***/
    GLfloat aspect = (GLfloat)SHADOW_WIDTH/(GLfloat)SHADOW_HEIGHT;
    GLfloat near = 1.0f;
    GLfloat far = 20.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far); 

    std::vector<glm::mat4> shadowTransforms;
    shadowTransforms.push_back(shadowProj * 
                     glm::lookAt(light2_pos, light2_pos + glm::vec3(1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)));
    shadowTransforms.push_back(shadowProj * 
                     glm::lookAt(light2_pos, light2_pos + glm::vec3(-1.0,0.0,0.0), glm::vec3(0.0,-1.0,0.0)));
    shadowTransforms.push_back(shadowProj * 
                     glm::lookAt(light2_pos, light2_pos + glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,0.0,1.0)));
    shadowTransforms.push_back(shadowProj * 
                     glm::lookAt(light2_pos, light2_pos + glm::vec3(0.0,-1.0,0.0), glm::vec3(0.0,0.0,-1.0)));
    shadowTransforms.push_back(shadowProj * 
                     glm::lookAt(light2_pos, light2_pos + glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,-1.0,0.0)));
    shadowTransforms.push_back(shadowProj * 
                     glm::lookAt(light2_pos, light2_pos + glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,-1.0,0.0)));

    // 1. Render scene to depth cubemap
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(depthCubeID);
        for (GLuint i = 0; i < 6; ++i)
        {
            stringstream ss;
            ss << i;
            string num = ss.str();
            glUniformMatrix4fv(glGetUniformLocation(depthCubeID, ("shadowMatrices[" + num + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
        }
        glUniform1f(glGetUniformLocation(depthCubeID, "far_plane"), far);
        glUniform3fv(glGetUniformLocation(depthCubeID, "light2_pos"), 1, &light2_pos[0]);
        renderScene(depthCubeID);
     glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /**DIRECTIONAL LIGHT SHADOWS**/
    // - Get light projection/view matrix.
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;

	GLfloat near_plane = -100.0f, far_plane = 100.0f;
    lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
    lightView = glm::lookAt(light1_pos, light1_at, glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
    glm::mat4 lightT = glm::transpose(lightSpaceMatrix);
	glm::vec3 light1_dir = glm::vec3(-lightT[2][0], -lightT[2][1], -lightT[2][2]);
	
	glUseProgram(programID);
	glUniformMatrix4fv(glGetUniformLocation(programID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glUniform3fv(glGetUniformLocation(programID, "light1_dir"), 1, &light1_dir[0]);
    glUseProgram(depthID);
    glUniformMatrix4fv(glGetUniformLocation(depthID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

	/*SPOT LIGHT*/
	glm::vec3 light3_dir = glm::vec3(-camDir[2][0], -camDir[2][1], -camDir[2][2]);
	glUseProgram(programID);
	glUniform3fv(glGetUniformLocation(programID, "light3_dir"), 1, &light3_dir[0]);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        renderScene(depthID);
        glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	/***NORMAL RENDERING***/
    glViewport(0, 0, winX, winY);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(programID);
    
    float mambient[3] = {0.3f, 0.3f, 0.3f};
    float mdiffuse[3] = {0.75f, 0.75f, 0.75f};
    float mspecular[3] = {1.0f, 1.0f, 1.0f};
    setupMaterials(mambient, mdiffuse, mspecular, 128.0f);

    glm::mat4 projection = glm::perspective( glm::radians(fov), (float) winX / winY, 0.1f, 100.0f );
    glUniformMatrix4fv( glGetUniformLocation(programID, "projection_matrix"), 1, false, glm::value_ptr(projection) );
    glUniformMatrix4fv( glGetUniformLocation(programID, "view"), 1, false, glm::value_ptr(viewMatrix) );
    glUniform1f( glGetUniformLocation(programID, "far_plane"), far);
    
    //Use Texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);

    renderScene(programID);
	
	
	//Object 30 Projectile
    for (size_t i = 0; i < objectz[30].shapes.size(); i++)
    {
		glUseProgram(waterID); 
		glUniformMatrix4fv( glGetUniformLocation(waterID, "projection_matrix"), 1, false, glm::value_ptr(projection) );
		glUniformMatrix4fv( glGetUniformLocation(waterID, "view"), 1, false, glm::value_ptr(viewMatrix) );
    
    	int index = objectz[30].shapes[i].mesh.material_ids[0];
    	if (index < 0)
    		index = i;

        float mambient[3] = {objectz[30].materials[index].ambient[0], objectz[30].materials[index].ambient[1], objectz[30].materials[index].ambient[2]};
        float mdiffuse[3] = {objectz[30].materials[index].diffuse[0], objectz[30].materials[index].diffuse[1], objectz[30].materials[index].diffuse[2]};
        float mspecular[3] = {objectz[30].materials[index].specular[0], objectz[30].materials[index].specular[1], objectz[30].materials[index].specular[2]};
        setupMaterials(mambient, mdiffuse, mspecular, objectz[30].materials[index].shininess);

        flagBump = 0.0f;
        glUniform1fv(glGetUniformLocation(programID, "bumpFlag"), 1, &flagBump);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

        glBindVertexArray(objectz[30].vaoBuffer[i]);

        glm::mat4 mvMatrix; 

        mvMatrix = glm::mat4(1.0f);
        mvMatrix = glm::translate(mvMatrix, glm::vec3(Camera->cameraPos.x, 1.0f, Camera->cameraPos.z));
        if(fireCheck == true && bulletCollisionFlag == false){
            mvMatrix = glm::translate(mvMatrix, glm::vec3(timer*10.0f*aimVec[0], timer*10.0f*aimVec[1], timer*10.0f*aimVec[2]));
        }
        else{
			mvMatrix = glm::translate(mvMatrix, glm::vec3(Camera->cameraPos.x, Camera->cameraPos.y-2.0f, Camera->cameraPos.z));

        }
        
        glUniformMatrix4fv(glGetUniformLocation(waterID, "modelview_matrix"), 1, false, glm::value_ptr(mvMatrix) );
        glDrawElements(GL_TRIANGLES, objectz[30].indices[i].size(), GL_UNSIGNED_INT, 0); 

        glBindVertexArray(0);  
    }

    /*https://learnopengl.com/#!Advanced-OpenGL/Cubemaps*/
    // Draw skybox as last
    glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content    
    glUseProgram(cubemapID); 
    
    viewMatrix = glm::mat4(glm::mat3(viewMatrix));    // Remove any translation component of the view matrix
    glUniformMatrix4fv(glGetUniformLocation(cubemapID, "view"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
    // skybox cube
    glBindVertexArray(skyboxVAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // Set depth function back to default

    glFlush();
}

GLuint loadCubemap(vector<const GLchar*> faces)
{
    GLuint skyboxID;
    glGenTextures(1, &skyboxID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxID);

    for (GLuint i = 0; i < faces.size(); i++)
    {
        int x, y, n;
        unsigned char *data = stbi_load(
                                        faces[i], /*char* filepath */
                                        &x, /*The address to store the width of the image*/
                                        &y, /*The address to store the height of the image*/
                                        &n  /*Number of channels in the image*/,
                                        0   /*Force number of channels if > 0*/
        );

        glBindTexture( GL_TEXTURE_2D,  skyboxID);
        
        if (n == 3) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else {
            fprintf(stderr, "Image pixels are not RGB. Texture may not load correctly.");
        }
        stbi_image_free(data);
    }
    
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return skyboxID;
}

/**
 * Error callback for GLFW. Simply prints error message to stderr.
 */
static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

int main (int argc, char **argv)
{

    start = time(0);
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit()) {
        exit(1);
    }

    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);



    // Create the window and OpenGL context
    window = glfwCreateWindow(winX, winY, "Overwatch 0.1",  NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}
	
    // Set OpenGL state we need for this application.
    glClearColor(0.5F, 0.5F, 0.5F, 0.0F);

    Shader1 = LoadShaders("depth.vert", "depth.frag");
    Shader2 = LoadShadersExtended("depthCube.vert", "depthCube.frag", "depthCube.gs"); // custom function inside Shader.cpp/hpp
    Shader3 = LoadShaders("debug.vert", "debug.frag");
   	Shader4 = LoadShaders("cubemap.vert", "cubemap.frag");
   	Shader5 = LoadShaders("water.vert", "water.frag");

    depthID = Shader1;
    depthCubeID = Shader2;
    programID = Shader3;
    cubemapID = Shader4;
	waterID = Shader5;

	if (Shader1 == 0 || Shader2 == 0 || Shader3 == 0 || Shader4 == 0 || Shader5 == 0) {
        fprintf(stderr, "Can't compile shaders!\n");
		exit(1);
    }


    
    // Set up the scene and the cameras
    setProjection();
    

    WorldCam = new WorldObjectViewer( cameraInit );
    ObjCam = new ObjectViewer( cameraInit );
    FPScam = new FirstPerson( cameraInit );
    Camera = FPScam;

    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup some OpenGL options
    glEnable(GL_DEPTH_TEST);
    //MSAA x4
    glEnable(GL_MULTISAMPLE); 
    //ENABLE FRAME BUFFER
    glEnable(GL_FRAMEBUFFER_SRGB);
	//ALPHA BLENDING
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//CULLING FACE
	//glEnable(GL_CULL_FACE);

    /*INIT TEXTURES*/
    vector<tinyobj::material_t> temp_mat; //send empty vector for non-obj
    vector<tinyobj::shape_t> temp_shp; //send empty vector for non-obj
    Texture1 = new loadTex("texture/sand.jpg", temp_shp, temp_mat, "texture/NormalSand.png", "texture/displacementsand.png");
	Texture2 = new loadTex("objects/smoke/smoke.png", temp_shp, temp_mat);
	Texture3 = new loadTex("objects/fire/fire.png", temp_shp, temp_mat);

    string input;

    //Object 0 - Hammock
    input = "objects/hammock/3d-model.obj";
    Object = new loadObj(input);
	texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

    //Object 1 - Boat
    input = "objects/boat/Wood_Boat.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

  	//Object 2 - Sandbag
    input = "objects/bags/bags.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

  	//Object 3 - Hut and fireplace
    input = "objects/hut/kk1.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

  	//Object 4 - Barrel1
    input = "objects/barrel/Barrel02.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 5 - Barrel2
    input = "objects/barrel/Barrel02.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

  	//Object 6 - Rock1
    input = "objects/rocks/Rock.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

  	//Object 7 - Rock2
    input = "objects/rocks/Rock-3.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

  	//Object 8 - Rock3
    input = "objects/rocks/Rock.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

  	//Object 9 - Target1
    input = "objects/target/shtfrtr.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 10 - Target2
    input = "objects/target/shtfrtr.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 11 - Target3
    input = "objects/target/shtfrtr.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object12 - Box1
    input = "objects/cube2/cube2.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object13 - Box2
    input = "objects/cube2/cube2.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object14 - Box3
    input = "objects/cube2/cube2.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object15 - Box4
    input = "objects/cube2/cube2.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object16 - Box5
    input = "objects/cube2/cube2.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object17 - Box6
    input = "objects/cube2/cube2.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object18 - Tree1
    input = "objects/tree/3d-model.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 19 - Tree2
    input = "objects/tree/3d-model.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 20 - Tree3
    input = "objects/tree/3d-model.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 21 - Bush1
    input = "objects/bush/Ferns.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 22 - Tree5
    input = "objects/tree/3d-model.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 23 - Tree6
    input = "objects/tree/3d-model.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 24 - Wall
    input = "objects/cube2/cube2.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex("texture/bricks2.jpg", temp_shp, temp_mat, "texture/bricks2_normal.jpg", "texture/bricks2_disp.jpg")));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID, 6.0f)));

	//Object 25 - Fence1
    input = "objects/fence/fence.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 26 - Fence2
    input = "objects/fence/fence.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 27 - Fence3
    input = "objects/fence/fence.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 28 - Fence4
    input = "objects/fence/fence.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

	//Object 29 - Bush2
    input = "objects/bush/Ferns.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));

    //Object30 - Sphere
    input = "objects/sphere/sphere.obj";
    Object = new loadObj(input);
    texturez.push_back(*(new loadTex(Object->filepath, Object->shapes, Object->materials)));
	objectz.push_back(*(new anObj(Object->shapes, Object->materials, programID)));
	
	/*
	objectArray[0] = &objectz[1]; //sandbag
	objectArray[1] = &objectz[2]; //hut and fireplace
	objectArray[2] = &objectz[5]; // rock1
	objectTranslations();
	*/

    /*plane 1*/
    plane1 = new plane(programID, 40.0f, 40.0f);

	//Smoke
	for(int x = 0; x < 100; x++)
	{
		smokez.push_back(*(new plane(programID,0.1f,1.0f)));
		starting.push_back(((float)rand()/(float)(RAND_MAX)) * 1.0);
	}

	//Fire
	for(int x = 0; x < 100; x++)
	{
		firez.push_back(*(new plane(programID,0.1f,1.0f)));
	}
	
    //Depth Mapping
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  
        
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    //Depth Cube Mapping
    glGenFramebuffers(1, &depthCubeMapFBO);
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (GLuint i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
                         SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); 
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Cube Mapping (Skybox)

    GLfloat skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // Setup cube VAO
    GLuint skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    vector<const GLchar*> faces;
    faces.push_back("texture/sky2/right.JPG");
    faces.push_back("texture/sky2/left.JPG");
    faces.push_back("texture/sky2/top.JPG");
    faces.push_back("texture/sky2/bottom.JPG");
    faces.push_back("texture/sky2/back.JPG");
    faces.push_back("texture/sky2/front.JPG");
    cubemapTexture = loadCubemap(faces);  



	glUseProgram(cubemapID);
    int cubemapHandle = glGetUniformLocation(cubemapID, "skybox");
    if (cubemapHandle == -1)
    {
        cout << "Cubemap Handle not being used" << endl;
    }

	glUseProgram(waterID);
    int waterHandle = glGetUniformLocation(waterID, "skybox");
    if (waterHandle == -1)
    {
        cout << "Water Handle not being used" << endl;
    }

	glUseProgram(programID);
    int textureHandle = glGetUniformLocation(programID, "texMap");
    if (textureHandle == -1)
    {
    	cout << "Texture Handle not being used" << endl;
    }

    int bumpHandle = glGetUniformLocation(programID, "normalMap");
    if (textureHandle == -1)
    {
        cout << "Bump Handle not being used" << endl;
    }

    int shadowHandle = glGetUniformLocation(programID, "shadowMap");
    if (shadowHandle == -1)
    {
    	cout << "Shadow Handle not being used" << endl;
    }

    int pointShadowHandle = glGetUniformLocation(programID, "pointShadowMap");
    if (pointShadowHandle == -1)
    {
        cout << "Point Shadow Handle not being used" << endl;
    }

    int displaceHandle = glGetUniformLocation(programID, "displaceMap");
    if (displaceHandle == -1)
    {
        cout << "Displacement Handle not being used" << endl;
    }

    glUniform1i(textureHandle, 0);
    glUniform1i(bumpHandle, 3);
    glUniform1i(shadowHandle, 1);
    glUniform1i(cubemapHandle, 0);
    glUniform1i(pointShadowHandle, 2);
    glUniform1i(displaceHandle, 4);
    glUniform1i(waterHandle, 0);

    // GLFW records the time since the program started (in seconds)
    float start = glfwGetTime();
    float now;

	for(int i = 0; i < 100; i++)
	{
		timez.push_back((((float)rand()/(float)(RAND_MAX)) * 1.4));
		if( i % 2 == 0)
		{
			timez2.push_back((((float)rand()/(float)(RAND_MAX)) * 1.0));
		}
		else
		{
			timez2.push_back(-(((float)rand()/(float)(RAND_MAX)) * 1.0));
		}
	}
	
    while (!glfwWindowShouldClose(window))
    {
    	now = glfwGetTime();

        dt = now - start;
        moveSpeed = now - lf;
        lf = now;
        glm::mat4 aimMat2 = glm::transpose(viewMatrix);
        aimVec2 = glm::vec3(-aimMat2[2][0], -aimMat2[2][1], -aimMat2[2][2]);
        if(fireCheck == true){
            timer+=0.1;
            //didBulletHit();
            if(timer>15){
            	glm::mat4 aimMat = glm::transpose(viewMatrix);
            	aimVec = glm::vec3(-aimMat[2][0], -aimMat[2][1], -aimMat[2][2]);
                bulletCollisionFlag = false;
                timer = 0;
                fireCheck = false;
            }
        }
		if(smokeCheck == true)
		{
			for(int i = 0; i < timez.size();i++)
			{
				timez[i]+= 0.01;
				if(timez2[i] >= 0.0)
				{
					timez2[i]+= 0.01;
				}
				if(timez2[i] < 0.0)
				{
					timez2[i]-= 0.01;
				}
				if(timez2[i] > 1.7 || timez2[i] < -1.7)
				{
					if( i % 2 == 0)
					{
						timez2[i]= (((float)rand()/(float)(RAND_MAX)) * 1.0);
					}
					else
					{
						timez2[i]= -(((float)rand()/(float)(RAND_MAX)) * 1.0);
					}
				}
				if(timez[i] > 1.7)
				{
					timez[i] = (((float)rand()/(float)(RAND_MAX)) * 1.4);
				}
			}
		}
		smokie();
        render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    delete Camera;
    delete Object;
    delete Texture1, Texture2, Texture3;
    delete plane1;

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(0);
    
    return 0;
}
