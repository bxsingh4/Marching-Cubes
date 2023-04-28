// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <cmath>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>

#include "TriTable.hpp"

#define FRONT_TOP_LEFT     128;
#define FRONT_TOP_RIGHT     64;
#define BACK_TOP_RIGHT      32;
#define BACK_TOP_LEFT       16;
#define FRONT_BOTTOM_LEFT    8;
#define FRONT_BOTTOM_RIGHT   4;
#define BACK_BOTTOM_RIGHT    2;
#define BACK_BOTTOM_LEFT     1;

// Draw cube function

void drawCube(float min, float max) {

// Draw the cube 
glPointSize(2.0f);
glColor3f(1.0f,1.0f,1.0f);
glBegin(GL_LINES);
    //Draw edges of the cube
    glVertex3f(min, min, min);
    glVertex3f(max, min, min);
    glVertex3f(min, min, min);
    glVertex3f(min, max, min);
    glVertex3f(min, min, min);
    glVertex3f(min, min, max);
    glVertex3f(max, min, min);
    glVertex3f(max, max, min);
    glVertex3f(max, min, min);
    glVertex3f(max, min, max);
    glVertex3f(max, max, max);
    glVertex3f(max, min, max);
    glVertex3f(max, max, max);
    glVertex3f(max, max, max);
    glVertex3f(min, max, max);
    glVertex3f(min, max, max);
    glVertex3f(min, min, max);
    glVertex3f(min, max, max);
    glVertex3f(min, max, min);
    glVertex3f(max, max, min);
    // Draw the top face
    glVertex3f(min, max, min);
    glVertex3f(max, max, min);
    glVertex3f(max, max, min);
    glVertex3f(max, max, max);
    glVertex3f(max, max, max);
    glVertex3f(min, max, max);
    glVertex3f(min, max, max);
    glVertex3f(min, max, min);
    // Draw the bottom face
    glVertex3f(min, min, max);
    glVertex3f(max, min, max);
    glVertex3f(max, min, max);
    glVertex3f(max, max, max);
    glVertex3f(max, max, max);
    glVertex3f(min, max, max);
    glVertex3f(min, max, max);
    glVertex3f(min, min, max);
glEnd();
}

// Draw axes function
void drawAxes(float min, float max) {
  // Set the color of the x-axis to red
  glPointSize(4.0f);
  glColor3f(1.0f, 0.0f, 0.0f);
   // Draw the x-axis
  glBegin(GL_LINES);
      glVertex3f(min, min, min);
      glVertex3f(max, min, min);
  glEnd();
   // Set the color of the y-axis to green
  glPointSize(4.0f);
  glColor3f(0.0f, 1.0f, 0.0f);
   // Draw the y-axis
  glBegin(GL_LINES);
      glVertex3f(min, min, min);
      glVertex3f(min, max, min);
  glEnd();
   // Set the color of the z-axis to blue
  glPointSize(4.0f);
  glColor3f(0.0f, 0.0f, 1.0f);

   // Draw the z-axis
  glBegin(GL_LINES);
      glVertex3f(min, min, min);
      glVertex3f(min, min, max);
  glEnd();

}

float f1(float x, float y, float z){
	
	return y-sin(x)*cos(z);
}

float f2(float x, float y, float z){
	
	return (x*x) - (y*y) - (z*z)-z;
}

// Function for calculating normals based on the vertices
void compute_normals(const std::vector<float>& positions, std::vector<float>& normals)
{
    // Clear the output vector
    normals.clear();
    
    // Initialize the output vector with zero vectors
    normals.resize(positions.size(), 0.0f);
    
    // Iterate over each triangle in the mesh
    for (int i = 0; i < positions.size() / 9; i++)
    {
        // Get the positions of the three vertices of the triangle
        glm::vec3 v0(positions[i * 9], positions[i * 9 + 1], positions[i * 9 + 2]);
        glm::vec3 v1(positions[i * 9 + 3], positions[i * 9 + 4], positions[i * 9 + 5]);
        glm::vec3 v2(positions[i * 9 + 6], positions[i * 9 + 7], positions[i * 9 + 8]);
        
        // Calculate the normal of the triangle
        glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
        
        // Add the normal to the normals of the three vertices
        normals[i * 9] += normal.x;
        normals[i * 9 + 1] += normal.y;
        normals[i * 9 + 2] += normal.z;
        normals[i * 9 + 3] += normal.x;
        normals[i * 9 + 4] += normal.y;
        normals[i * 9 + 5] += normal.z;
        normals[i * 9 + 6] += normal.x;
        normals[i * 9 + 7] += normal.y;
        normals[i * 9 + 8] += normal.z;
    }
   
}




// Function for marching cubes algorithm
vector<float> marching_cubes(function <float(float,float,float)>f, float isoval, float min, float max,float stepsize ) {
    // empty vector to store vertices
    vector<float> vertices;
    // set x,y,z to min
    float x = min;
    float y = min;
    float z = min;
    // set the max value to be max-stepsize, ensures we don't go outside confined area
    float maxX = max-stepsize;
    // declare variables
    int* verts;
    float bbl,bbr,fbr,fbl,btl,btr,ftr,ftl;
    // Begin Loop for Algorithm
    for (z=min; z < maxX; z += stepsize) {
        for (y = min ; y < maxX; y += stepsize) {
            for (x = min ; x < maxX; x += stepsize) {
                // Declare which and set to 0
				int which = 0;
                //test the cube
                // float values[8];
                bbl = f(x, y, z);                    // Back bottom left
                bbr = f(x + stepsize, y, z);         // Back bottom right
                fbr = f(x + stepsize, y, z + stepsize);   // Front top right
                fbl = f(x, y, z + stepsize);         // Front top left
                btl = f(x, y + stepsize, z);         // Back top left
                btr = f(x + stepsize, y + stepsize, z);  // Back top right
                ftr = f(x + stepsize, y + stepsize, z + stepsize); // Front top right
                ftl = f(x, y + stepsize, z + stepsize);    // Front top left

                // determine the cube configuration
    
                if (bbl > isoval) which |= BACK_BOTTOM_LEFT;
                if (bbr > isoval) which |= BACK_BOTTOM_RIGHT;
                if (fbr > isoval) which |= FRONT_BOTTOM_RIGHT;
                if (fbl > isoval) which |= FRONT_BOTTOM_LEFT;
                if (btl > isoval) which |= BACK_TOP_LEFT;
                if (btr > isoval) which |= BACK_TOP_RIGHT;
                if (ftr > isoval) which |= FRONT_TOP_RIGHT;
                if (ftl > isoval) which |= FRONT_TOP_LEFT;
                // pushing positions into the vector 
                verts = marching_cubes_lut[which];
                for (int i = 0; verts[i] >=0; i += 3) {
                    // add triangle vertices
                    for (int j = 0; j < 3; j++) {
                        vertices.push_back(x + stepsize * vertTable[verts[i+j]][0]);
                        vertices.push_back(y + stepsize * vertTable[verts[i+j]][1]);
                        vertices.push_back(z + stepsize * vertTable[verts[i+j]][2]);

                       
                    }
                }	

            }
        }
    }

    return vertices;
}

void render(glm::mat4 MVP, glm::mat4 V, vector<float> normals, vector<float> vertices, glm::vec3 lightDir, int i){

// Create and Bind buffers for Positions and Normals
GLuint posVBO, normalVBO, VAO;
glGenBuffers(1, &posVBO);
glBindBuffer(GL_ARRAY_BUFFER, posVBO);
glBufferData(GL_ARRAY_BUFFER, i * sizeof(float), &vertices[0], GL_DYNAMIC_DRAW);

// Normals Buffer binding
glGenBuffers(1, &normalVBO);
glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
glBufferData(GL_ARRAY_BUFFER, i * sizeof(float), &normals[0], GL_DYNAMIC_DRAW);

// Create a VAO object and add pointers for positions and normals in the buffers
glGenVertexArrays(1, &VAO);
glBindVertexArray(VAO);
glBindBuffer(GL_ARRAY_BUFFER, posVBO);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
glEnableVertexAttribArray(0);
glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
glEnableVertexAttribArray(1);
glBindVertexArray(0);

// Vertex shader
string VertexShaderSource = "\
#version 330 core\n\
uniform mat4 MVP;\n\
uniform mat4 V;\n\
uniform vec3 LightDir;\n\
layout (location = 0) in vec3 position;\n\
layout (location = 1) in vec3 normal;\n\
out vec3 fragPos;\n\
out vec3 fragNormal;\n\
void main() {\n\
   gl_Position = MVP * vec4(position, 1.0f);\n\
   fragPos = vec3(V * vec4(position, 1.0f));\n\
   fragNormal = normalize(mat3(transpose(inverse(V))) * normal);\n\
}\n";
// fragment shader
string FragmentShaderSource = "\
#version 330 core\n\
// Interpolated values from the vertex shaders\n\
in vec3 fragPos;\n\
in vec3 fragNormal;\n\
uniform vec3 LightDir;\n\
uniform vec3 modelColor;\n\
const vec3 ambientColor = vec3(0.2f, 0.2f, 0.2f);\n\
const vec3 specularColor = vec3(1.0f, 1.0f, 1.0f);\n\
const float shininess = 64.0f;\n\
\n\
out vec4 fragColor;\n\
\n\
void main() {\n\
vec3 ambient = ambientColor * modelColor;\n\
   vec3 norm = normalize(fragNormal);\n\
   vec3 lightDir = normalize(-LightDir);\n\
   float diff = max(dot(norm, lightDir), 0.0f);\n\
   vec3 diffuse = diff * modelColor;\n\
   vec3 viewDir = normalize(-fragPos);\n\
   vec3 reflectDir = reflect(-lightDir, norm);\n\
   float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);\n\
   vec3 specular = specularColor * spec;\n\
   vec3 result = ambient + diffuse + specular;\n\
   fragColor = vec4(result, 1.0f);\n\
}\n";

// Convert shader code 
char const * VertexSourcePointer = VertexShaderSource.c_str();
char const * FragmentSourcePointer = FragmentShaderSource.c_str();

// Compile Vertex Shader
GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
glCompileShader(VertexShaderID);

// Compile Fragment Shader
GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
glCompileShader(FragmentShaderID);
 

// Link the program
GLuint ProgramID = glCreateProgram();
glAttachShader(ProgramID, VertexShaderID);
glAttachShader(ProgramID, FragmentShaderID);
glLinkProgram(ProgramID);

// Delete Shaders
glDeleteShader(VertexShaderID);
glDeleteShader(FragmentShaderID);
 
// Set shader uniform variables
glUseProgram(ProgramID);
GLint MVPUniformLocation = glGetUniformLocation(ProgramID, "MVP");
GLint VUniformLocation = glGetUniformLocation(ProgramID, "V");
GLint LightDirUniformLocation = glGetUniformLocation(ProgramID, "LightDir");
GLint modelColorUniformLocation = glGetUniformLocation(ProgramID, "modelColor");
glUniformMatrix4fv(MVPUniformLocation, 1, GL_FALSE, &MVP[0][0]);
glUniformMatrix4fv(VUniformLocation, 1, GL_FALSE, &V[0][0]);
glUniform3f(LightDirUniformLocation, lightDir.x, lightDir.y, lightDir.z);
glUniform3f(modelColorUniformLocation, 0.2f, 1.0f, 1.0f);

// Draw the object
glBindVertexArray(VAO);
// glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);
glDrawArrays(GL_TRIANGLES, 0, i / 3);
glBindVertexArray(0); // unbind vao

glUseProgram(0);// unbind shader program

// Cleanup
glDeleteBuffers(1, &posVBO);
glDeleteBuffers(1, &normalVBO);
glDeleteVertexArrays(1, &VAO);
glDeleteProgram(ProgramID);


}


// Write position and normal  information to file 
void writePLY(vector<float> normal, vector<float> position, string plyname){


ofstream output_file(plyname);

if(!output_file.is_open()){
    cerr<<"Could not open file for writing"<<endl;
    return;
}
// write header
output_file<<"ply"<<endl;
output_file<<"format ascii 1.0"<<endl;
output_file<<"element vertex "<< position.size()/3<<endl;
output_file<<"property float x"<< endl;
output_file<<"property float y"<< endl;
output_file<<"property float z"<< endl;
output_file<<"property float nx"<<endl;
output_file<<"property float ny"<<endl;
output_file<<"property float nz"<<endl;
output_file<<"element face "<< position.size()/9<<endl;
output_file<<"property list uchar uint vertex_indices"<<endl;
output_file<<"end_header"<<endl;
//end header

// write position and normals
for(int i=0; i<position.size();i+=3){
    output_file<<position[i]<<" "<<position[i+1]<<" "<<position[i+2]<<" " <<normal[i]<<" "<<normal[i+1]<< " "<<normal[i+2]<<endl;
}

for(int i=0;i<position.size()/3;i+=3){
    output_file<<"3 "<< i <<" "<< i+1 << " "<< i+2 <<endl;
}
// close file after reading
output_file.close();

}

//------------------------------------- Camera Controls Start ------------------------------------------------//


// Camera position in spherical coordinates
float r = sqrt(75); // initial pos -> (5,5,5) --> r = sqrt(25+25+25)= sqrt(75)
float phi = acos(5/r); // phi = acos(y/r)
float theta = atan(5/5); // theta = arctan(z/x)


// Camera movement speed
float camera_speed = 0.25f;


// Mouse input state
double last_xpos, last_ypos;
bool mouse_pressed = false;


// Function for mouse button events
void mouse_button_movements(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        last_xpos = xpos;
        last_ypos = ypos;
        mouse_pressed = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        mouse_pressed = false;
    }
}


// Function for mouse movement events
void mouse_cursor_movements(GLFWwindow* window, double xpos, double ypos)
{
    if (mouse_pressed)
    {
        // Adjust thetha and phi if the user clicks and drags the mouse
        theta += (xpos - last_xpos) * 0.009f;
        phi += -(ypos - last_ypos) * 0.009f;
        // Clam phi values
        if (phi<0.1f) phi = 0.1f;
        if(phi > M_PI -0.1f) phi = M_PI-0.1f;
        last_xpos = xpos;
        last_ypos = ypos;
    }
}

// Function to check for key events
void key_movements(GLFWwindow* window){

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        // pressing up key will decrease the r value
        r -= camera_speed;
        // clamp r at 0.1f
        if (r < 0.1f){
            r = 0.1f;
        } 
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        // pressing down key will increase the r value
        r += camera_speed;
    }
}

// Adjust the camera based on the user's adjustments
void cameraControlsGlobe(glm::mat4& view)
{
    // convert spherical coordinates back to cartesian 
    float x = r * sin(phi) * cos(theta);
    float y = r * cos(phi);
    float z = r * sin(phi) * sin(theta);
    glm::vec3 camera_pos = glm::vec3(x, y, z);
    glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
    view = glm::lookAt(camera_pos, camera_target, camera_up);
}

//------------------------------------- Camera Controls End ------------------------------------------------//

//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////




int main( int argc, char* argv[])
{


///////////////////////////////////////////////////////
	float screenW = 1400;
	float screenH = 900;
	float stepsize = 0.05f;

	float min = -3.0f;
	float max = 3.0f;
	float isoval = 0.0f;

	if (argc > 1 ) {
		screenW = atoi(argv[1]);
	}
	if (argc > 2) {
		screenH = atoi(argv[2]);
	}
	if (argc > 3) {
		stepsize = atof(argv[3]);
	}
	if (argc > 4) {
		min = atof(argv[4]);
	}
	if (argc > 5) {
		max = atof(argv[5]);
	}
	if (argc > 6) {
		isoval = atof(argv[6]);
	}

	///////////////////////////////////////////////////////

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	
	// Open a window and create its OpenGL context
	window = glfwCreateWindow( screenW, screenH, "Marching", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.2f, 0.2f, 0.3f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Populate vertices and normals vectors with data
	vector<float> vertices = marching_cubes(f1, isoval,min,max,stepsize);
	vector<float> normals;
    compute_normals(vertices,normals);

    // Create and write data to ply file
    writePLY(normals,vertices,"assignment.ply");
	
    // Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    // Enable blending
    glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Create Mat for MVP
	glm::mat4 MVP;
    // Setup light direction
    glm::vec3 lightDir = glm::vec3(5.0f,5.0f,5.0f);
	

    // Convert spherical coordinates into cartesian coordinates
    float x = r * sin(phi) * cos(theta);
    float y = r * cos(phi);
    float z = r * sin(phi) * sin(theta);

    // set up camera components, intially set at < 5.0, 5.0 , 5.0 > 
    glm::vec3 camera_pos = glm::vec3(x, y, z);
    glm::vec3 camera_target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Callback functions for mouse movement
    glfwSetMouseButtonCallback(window, mouse_button_movements);
    glfwSetCursorPosCallback(window, mouse_cursor_movements);

    // intialize int to 0, used for growing mesh animation
    int i = 0;

  do{
      // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  
    // Set up Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glm::mat4 P = glm::perspective(glm::radians(75.0f), screenW/screenH, 0.001f, 1000.0f);
	glLoadMatrixf(glm::value_ptr(P));

    // Setup Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();

    // View Matrix
    glm::mat4 V;

    // Update Camera as needed
    cameraControlsGlobe(V);

    // Calculate MV and set view
	glm::mat4 M = glm::mat4(1.0f);
	glm::mat4 MV = V * M;
	glLoadMatrixf(glm::value_ptr(V));
    // Calculate MVP 
	MVP = P * V * M;
    
    // Draw Axes and Cube
    drawAxes(min,max);
	drawCube(min,max);

    // Check for key up and down
    key_movements(window);

    // Slice up data to show growing mesh
    if(i<vertices.size()){
        i+=400;
        // i+=(vertices.size()/250);
        if(i>vertices.size()){
            i=vertices.size();
        }
    }

    // Call the render function
    render(MVP,  V, normals, vertices, lightDir,i);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();


  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0 );

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}

