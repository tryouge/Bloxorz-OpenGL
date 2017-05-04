#include <iostream>
#include <cmath>
#include <fstream>
#include <vector>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>


using namespace std;
//const int font=(int)GLUT_BITMAP_9_BY_15;
char s[30];
struct colors {
	float r;
	float g;
	float b;
};

struct colors color, color1;

  int grid1[9][9]={
        {1,1,1,1,1,1,3,1,0},
        {0,0,0,0,2,1,3,0,1},
        {0,0,0,0,1,1,1,0,3},
        {0,0,1,1,1,1,3,1,1},
        {0,0,1,0,1,1,1,1,0},
        {0,0,1,0,0,0,0,1,1},
        {0,0,0,0,0,0,1,1,1},
        {0,0,0,0,0,0,1,3,0},
        {0,0,0,0,0,0,1,1,0}};
   int grid2[9][9]={
        {0,0,0,0,0,1,3,1,0},
        {3,3,0,0,2,1,3,0,1},
        {0,3,0,0,1,1,1,0,3},
        {0,0,1,1,1,1,3,1,1},
        {0,0,1,0,1,1,1,1,0},
        {0,0,1,0,0,0,0,1,1},
        {0,3,1,1,0,0,1,1,1},
        {0,1,3,1,0,0,1,3,0},
        {0,1,1,1,0,0,1,1,0}};
   int grid3[9][9]={
        {0,0,0,0,0,1,3,1,1},
        {1,0,0,0,2,1,3,0,1},
        {0,1,0,0,1,1,1,0,1},
        {1,1,0,1,1,1,3,1,1},
        {1,1,0,0,1,4,1,1,1},
        {0,0,0,0,0,0,0,1,1},
        {0,1,1,1,0,0,1,1,1},
        {0,1,1,1,0,0,1,3,0},
        {0,1,1,1,0,0,1,1,0}};
int v=0;
int b=0;
int width = 800;
int height = 800;
GLfloat fov = 90.0f;
struct VAO {
    GLuint VertexArrayID;
    GLuint VertexBuffer;
    GLuint ColorBuffer;

    GLenum PrimitiveMode;
    GLenum FillMode;
    int NumVertices;
};
typedef struct VAO VAO;

struct GLMatrices {
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 view;
	GLuint MatrixID;
} Matrices;

struct GLBlock {
  struct VAO* cube1;
  struct VAO* cube2;
  glm::mat4 transvector1;
  glm::mat4 transvector2;
  glm::mat4 rotvector;
  float block_rotation;
};

struct GLGrid {
  struct VAO* grid[10][10];
  glm::mat4 transvector;
  glm::mat4 rotvector;
  int gridconfig[9][9]={
        {0,0,0,0,0,1,3,1,1},
        {0,0,0,0,2,1,3,0,1},
        {0,0,0,0,1,1,1,0,1},
        {0,0,0,1,1,1,3,1,1},
        {0,0,0,0,1,4,1,1,1},
        {0,0,0,0,0,0,0,1,1},
        {0,1,1,1,0,0,1,1,1},
        {0,1,1,1,0,0,1,3,0},
        {0,1,1,1,0,0,1,1,0}};
};
int l=0;
struct GLTile {
  struct VAO* tileimg;
  glm::mat4 transvector;
};

void draw();

struct GLTile tile[500];
int tilecount=0;
struct GLBlock block;
int xalign=0;
int yalign=1;
int zalign=0;
float x1=0;
float x2=0;
float yc1=0.3;
float y2=-0.3;
float z1=0;
float z2=0;
struct GLGrid map;
GLuint programID;
float cx=1,cy=2,cz=3;
float tx=0,ty=0,tz=0;

/* Function to load Shaders - Use it as it is */
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
	{
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);

	// Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	std::vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

/* Generate VAO, VBOs and return VAO handle */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat* color_buffer_data, GLenum fill_mode=GL_FILL)
{
    struct VAO* vao = new struct VAO;
    vao->PrimitiveMode = primitive_mode;
    vao->NumVertices = numVertices;
    vao->FillMode = fill_mode;

    // Create Vertex Array Object
    glGenVertexArrays(1, &(vao->VertexArrayID)); // VAO
    glGenBuffers (1, &(vao->VertexBuffer)); // VBO - vertices
    glGenBuffers (1, &(vao->ColorBuffer));  // VBO - colors

    glBindVertexArray (vao->VertexArrayID); // Bind the VAO 
    glBindBuffer (GL_ARRAY_BUFFER, vao->VertexBuffer); // Bind the VBO vertices 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW); // Copy the vertices into VBO
    glVertexAttribPointer(
                          0,                  // attribute 0. Vertices
                          3,                  // size (x,y,z)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    glBindBuffer (GL_ARRAY_BUFFER, vao->ColorBuffer); // Bind the VBO colors 
    glBufferData (GL_ARRAY_BUFFER, 3*numVertices*sizeof(GLfloat), color_buffer_data, GL_STATIC_DRAW);  // Copy the vertex colors
    glVertexAttribPointer(
                          1,                  // attribute 1. Color
                          3,                  // size (r,g,b)
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );

    return vao;
}

/* Generate VAO, VBOs and return VAO handle - Common Color for all vertices */
struct VAO* create3DObject (GLenum primitive_mode, int numVertices, const GLfloat* vertex_buffer_data, const GLfloat red, const GLfloat green, const GLfloat blue, GLenum fill_mode=GL_FILL)
{
    GLfloat* color_buffer_data = new GLfloat [3*numVertices];
    for (int i=0; i<numVertices; i++) {
        color_buffer_data [3*i] = red;
        color_buffer_data [3*i + 1] = green;
        color_buffer_data [3*i + 2] = blue;
    }

    return create3DObject(primitive_mode, numVertices, vertex_buffer_data, color_buffer_data, fill_mode);
}

/* Render the VBOs handled by VAO */
void draw3DObject (struct VAO* vao)
{
    // Change the Fill Mode for this object
    glPolygonMode (GL_FRONT_AND_BACK, vao->FillMode);

    // Bind the VAO to use
    glBindVertexArray (vao->VertexArrayID);

    // Enable Vertex Attribute 0 - 3d Vertices
    glEnableVertexAttribArray(0);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->VertexBuffer);

    // Enable Vertex Attribute 1 - Color
    glEnableVertexAttribArray(1);
    // Bind the VBO to use
    glBindBuffer(GL_ARRAY_BUFFER, vao->ColorBuffer);

    // Draw the geometry !
    glDrawArrays(vao->PrimitiveMode, 0, vao->NumVertices); // Starting from vertex 0; 3 vertices total -> 1 triangle
}

/**************************
 * Customizable functions *
 **************************/



float triangle_rot_dir = 1;
float rectangle_rot_dir = 1;
bool triangle_rot_status = true;
bool rectangle_rot_status = true;
glm::mat4 abcd,efgh;

/* Executed when a regular key is pressed */
void keyboardDown (unsigned char key, int x, int y)
{
	if (key=='v')
	{
		v=(v%5+1)%5;
	}
	if (key=='b')
	{
		b=(b%4+1)%4;
	}
	if (key=='l')
	{
		l=(l%4+1)%3;
	}
	float i;
    if (key=='a')
    {
      if (yalign==1)
      {
	      float temp=block.block_rotation;
	      while (block.block_rotation<=temp+90)
	      {
	      	abcd = glm::translate (glm::vec3(-0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.3,0));
	      	efgh = glm::translate (glm::vec3(-0.3,-0.9,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.9,0));
	      	block.block_rotation=block.block_rotation+3;
	      	draw();
	      }
	      abcd=glm::mat4(1.0f);
	      efgh=glm::mat4(1.0f);
	      yalign=0;
		  xalign=1;
	      x1=x1-1.2;
	      x2=x2-0.6;
	      yc1=yc1-0.6;
	      if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
		      	i=i-0.05;
		      	abcd = glm::translate(glm::vec3(0,i,0))*glm::translate (glm::vec3(-0.9,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.9,0.3,0));
		      	efgh = glm::translate(glm::vec3(0,i,0))*glm::translate (glm::vec3(-0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.3,0));
		      	block.block_rotation=block.block_rotation+3;
		      	draw();
	      	}
	      	abcd=glm::mat4(1.0f);
		      efgh=glm::mat4(1.0f);
		      block.block_rotation=0;
		      xalign=0;
			  yalign=1;
			  zalign=0;
	      }
		  block.block_rotation=0;	      
  	  }
  	  else if (xalign==1)
      {
      float temp=block.block_rotation;
      while (block.block_rotation<=temp+90)
      {
      	abcd = glm::translate (glm::vec3(-0.9,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.9,0.3,0));
      	efgh = glm::translate (glm::vec3(-0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.3,0));
      	block.block_rotation=block.block_rotation+3;
      	draw();
      }
      	block.block_rotation=0;
      	abcd=glm::mat4(1.0f);
      	efgh=glm::mat4(1.0f);
      	xalign=0;
      	yalign=1;
      	x2=x2-1.2;
      	x1=x1-0.6;
      	yc1=yc1+0.6;
      	//printf("%d %d\n", x1, x2);
      	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall2 %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate(glm::vec3(0,i,0))*glm::translate (glm::vec3(-0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.3,0));
	      		efgh = glm::translate(glm::vec3(0,i,0))*glm::translate (glm::vec3(-0.3,-0.9,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.9,0));
	      		block.block_rotation=block.block_rotation+3;
	      		draw();
	      	}
	      	abcd=glm::mat4(1.0f);
	      efgh=glm::mat4(1.0f);
	      block.block_rotation=0;
	      xalign=0;
		  yalign=1;
		  zalign=0;
	      }
  	  }
  	  else if (zalign==1)
      {
      float temp=block.block_rotation;
      while (block.block_rotation<=temp+90)
      {
      	abcd = glm::translate (glm::vec3(-0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.3,0));
      	efgh = glm::translate (glm::vec3(-0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.3,0));
      	block.block_rotation=block.block_rotation+3;
      	draw();
      }
      	block.block_rotation=0;
      	abcd=glm::mat4(1.0f);
      	efgh=glm::mat4(1.0f);
      	x2=x2-0.6;
      	x1=x1-0.6;
      	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(-0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.3,0));
		      	efgh = glm::translate (glm::vec3(-0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(0.3,0.3,0));
		      	block.block_rotation=block.block_rotation+3;
		      	draw();
		     }
		     abcd=glm::mat4(1.0f);
		     efgh=glm::mat4(1.0f);
		     block.block_rotation=0;
		     xalign=0;
			  yalign=1;
			  zalign=0;
		  }
  	  }		
  	}
      //abcd=glm::mat4(1.0f);
    if (key=='d')
    {
      if (yalign==1)
      {
	      float temp=block.block_rotation;
	      while (block.block_rotation>=temp-90)
	      {
	      	abcd = glm::translate (glm::vec3(0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3,0.3,0));
	      	efgh = glm::translate (glm::vec3(0.3,-0.9,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3,0.9,0));
	      	block.block_rotation=block.block_rotation-3;
	      	draw();
	      }
	      abcd=glm::mat4(1.0f);
	      efgh=glm::mat4(1.0f);
	      x1=x1+0.6;
	      x2=x2+1.2;
	      yc1=yc1-0.6;
	      yalign=0;
	      xalign=1;
	      block.block_rotation=0;
	      if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3
      		,0.3,0));
		      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0.9,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.9,0.3,0));
		      	block.block_rotation=block.block_rotation-3;
		      	draw();

		     }
		     abcd=glm::mat4(1.0f);
	      	 efgh=glm::mat4(1.0f);
	      	 block.block_rotation=0;
	      	 xalign=0;
			  yalign=1;
			  zalign=0;
      	}
  	  }
  	  else if (xalign==1)
      {
      float temp=block.block_rotation;
      while (block.block_rotation>=temp-90)
      {
      	abcd = glm::translate (glm::vec3(0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3
      		,0.3,0));
      	efgh = glm::translate (glm::vec3(0.9,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.9,0.3,0));
      	block.block_rotation=block.block_rotation-3;
      	draw();
      }
      	block.block_rotation=0;
      	abcd=glm::mat4(1.0f);
      	efgh=glm::mat4(1.0f);
      	xalign=0;
      	yalign=1;
      	x2=x2+0.6;
      	x1=x1+1.2;
      	yc1=yc1+0.6;
      	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3,0.3,0));
	      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0.3,-0.9,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3,0.9,0));
	      	block.block_rotation=block.block_rotation-3;
	      	draw();
	      	}
	      	block.block_rotation=0;
	      	abcd=glm::mat4(1.0f);
	      	efgh=glm::mat4(1.0f);
	      	xalign=0;
			  yalign=1;
			  zalign=0;
	      }
	      
  	  }
  	  else if (zalign==1)
      {
      float temp=block.block_rotation;
      while (block.block_rotation>=temp-90)
      {
      	abcd = glm::translate (glm::vec3(0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3,0.3,0));
      	efgh = glm::translate (glm::vec3(0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3,0.3,0));
      	block.block_rotation=block.block_rotation-3;
      	draw();
      }
      	block.block_rotation=0;
      	abcd=glm::mat4(1.0f);
      	efgh=glm::mat4(1.0f);
      	x2=x2+0.6;
      	x1=x1+0.6;
      	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3,0.3,0));
		      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0.3,-0.3,0))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(0,0,1))*glm::translate (glm::vec3(-0.3,0.3,0));
		      	block.block_rotation=block.block_rotation-3;
		      	draw();
		      	block.block_rotation=0;
	      	abcd=glm::mat4(1.0f);
	      	efgh=glm::mat4(1.0f);
	      	xalign=0;
		  yalign=1;
		  zalign=0;
	      	}
	      }      
  	  }	
    }
    if (key=='w')
    {
      if (yalign==1)
      {
	      float temp=block.block_rotation;
	      while (block.block_rotation>=temp-90)
	      {
	      	abcd = glm::translate (glm::vec3(0,-0.3,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,0.3));
	      	efgh = glm::translate (glm::vec3(0,-0.9,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.9,0.3));
	      	block.block_rotation=block.block_rotation-3;
	      	draw();
	      }
	      abcd=glm::mat4(1.0f);
	      efgh=glm::mat4(1.0f);
	      z1=z1-1.2;
	      z2=z2-0.6;
	      yc1=yc1-0.6;
	      yalign=0;
	      zalign=1;
	      block.block_rotation=0;
	      if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,-0.9))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0
      		,0.3,0.9));
		      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,0.3));
		      	block.block_rotation=block.block_rotation-3;
		      	draw();
	      	}
	      	 block.block_rotation=0;
	      	abcd=glm::mat4(1.0f);
	      	efgh=glm::mat4(1.0f);
	      	xalign=0;
			  yalign=1;
			  zalign=0;
	      }
	     
  	  }
  	  else if (zalign==1)
      {
      float temp=block.block_rotation;
      while (block.block_rotation>=temp-90)
      {
      	abcd = glm::translate (glm::vec3(0,-0.3,-0.9))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0
      		,0.3,0.9));
      	efgh = glm::translate (glm::vec3(0,-0.3,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,0.3));
      	block.block_rotation=block.block_rotation-3;
      	draw();
      }
      	block.block_rotation=0;
      	abcd=glm::mat4(1.0f);
      	efgh=glm::mat4(1.0f);
      	zalign=0;
      	yalign=1;
      	z2=z2-1.2;
      	z1=z1-0.6;
      	yc1=yc1+0.6;
      	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,0.3));
		      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.9,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.9,0.3));
		      	block.block_rotation=block.block_rotation-3;
		      	draw();
	      	}
	      	block.block_rotation=0;
	      	abcd=glm::mat4(1.0f);
	      	efgh=glm::mat4(1.0f);
	      	xalign=0;
			  yalign=1;
			  zalign=0;
	      }
	      
  	  }
  	  else if (xalign==1)
      {
      	//printf("-----\n");
      float temp=block.block_rotation;
      while (block.block_rotation>=temp-90)
      {
      	abcd = glm::translate (glm::vec3(0,-0.3,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,0.3));
      	efgh = glm::translate (glm::vec3(0,-0.3,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,0.3));
      	block.block_rotation=block.block_rotation-3;
      	draw();
      }
      	block.block_rotation=0;
      	abcd=glm::mat4(1.0f);
      	efgh=glm::mat4(1.0f);
      	z2=z2-0.6;
      	z1=z1-0.6;
      	yc1=-0.3;
  	  }
  	  if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,0.3));
		      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,-0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,0.3));
		      	block.block_rotation=block.block_rotation-3;
		      	draw();
	      	}
	      	block.block_rotation=0;
	      	abcd=glm::mat4(1.0f);
	      	efgh=glm::mat4(1.0f);
	      	xalign=0;
			  yalign=1;
			  zalign=0;
	      }
	      
    }
    if (key=='s')
    {
      if (yalign==1)
      {
      	  //printf("hello\n");
	      float temp=block.block_rotation;
	      while (block.block_rotation<=temp+90)
	      {
	      	abcd = glm::translate (glm::vec3(0,-0.3,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.3));
	      	efgh = glm::translate (glm::vec3(0,-0.9,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.9,-0.3));
	      	block.block_rotation=block.block_rotation+3;
	      	draw();
	      }
	      abcd=glm::mat4(1.0f);
	      efgh=glm::mat4(1.0f);
	      z1=z1+0.6;
	      z2=z2+1.2;
	      yc1=yc1-0.6;
	      yalign=0;
	      zalign=1;
	      block.block_rotation=0;
	      printf("%f", z1);
	      printf("%d %d\n",int((z1/0.6f)+4), (int)(x1/0.6f)+4);   
	      if (map.gridconfig[int((z1/0.6f)+4)][int((x1/0.6f)+4)]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6f)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	//printf("-----------------------------------\n\n\n");
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.3));
		      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,0.9))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.9));
		      	block.block_rotation=block.block_rotation+3;
		      	draw();
	      	}
	      	block.block_rotation=0;
	      	abcd=glm::mat4(1.0f);
	      	efgh=glm::mat4(1.0f);
	      	xalign=0;
		  yalign=1;
		  zalign=0;
	      }
  	  }
  	  else if (zalign==1)
      {
      float temp=block.block_rotation;
      while (block.block_rotation<=temp+90)
      {
      	abcd = glm::translate (glm::vec3(0,-0.3,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.3));
      	efgh = glm::translate (glm::vec3(0,-0.3,0.9))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.9));
      	block.block_rotation=block.block_rotation+3;
      	draw();
      }
      	block.block_rotation=0;
      	abcd=glm::mat4(1.0f);
      	efgh=glm::mat4(1.0f);
      	zalign=0;
      	yalign=1;
      	z2=z2+0.6;
      	z1=z1+1.2;
      	yc1=yc1+0.6;

      	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.3));
		      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.9,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.9,-0.3));
		      	block.block_rotation=block.block_rotation+3;
		      	draw();
	      	}
	      	block.block_rotation=0;
	      	abcd=glm::mat4(1.0f);
	      	efgh=glm::mat4(1.0f);
	      	xalign=0;
			  yalign=1;
			  zalign=0;
	      }
  	  }
  	  else if (xalign==1)
      {
      float temp=block.block_rotation;
      while (block.block_rotation<=temp+90)
      {
      	abcd = glm::translate (glm::vec3(0,-0.3,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.3));
      	efgh = glm::translate (glm::vec3(0,-0.3,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.3));
      	block.block_rotation=block.block_rotation+3;
      	draw();
      }
      	block.block_rotation=0;
      	abcd=glm::mat4(1.0f);
      	efgh=glm::mat4(1.0f);
      	z2=z2+0.6;
      	z1=z1+0.6;
      	 if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	      {
	      	i=0;
	      	//printf("fall %d %d\n",(int)(x1/0.6f)+4, (int)(x2/0.6)+4);
	      	while (1)
	      	{
	      		if (i<=-5)
	      		{
	      			x1=0;
	      			yc1=0.3;
	      			z1=0;
	      			x2=0;
	      			y2=-0.3;
	      			z2=0;
	      			break;
	      		}
	      		i=i-0.05;
	      		abcd = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.3));
		      	efgh = glm::translate (glm::vec3(0,i,0))*glm::translate (glm::vec3(0,-0.3,0.3))*glm::rotate((float)(block.block_rotation*M_PI/180.0f), glm::vec3(1,0,0))*glm::translate (glm::vec3(0,0.3,-0.3));
		      	block.block_rotation=block.block_rotation+3;
		      	draw();
	      	}
	      	block.block_rotation=0;
	      	abcd=glm::mat4(1.0f);
	      	efgh=glm::mat4(1.0f);
	      	xalign=0;
			  yalign=1;
			  zalign=0;
	      }
  	  }
    }
    if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==2 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==2)
	{
		system("aplay -q level.wav &");
		while (yc1>=-7)
		{
			yc1=yc1-0.05;
			y2=y2-0.05;
			draw();
		}

	}
	if (yc1<=-7)
	{
		x1=0;
		x2=0;
		z1=0;
		z2=0;
		yc1=0.3;
		y2=-0.3;
		l=(l%3+1)%3;
	}
	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==3 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==3)
	{
		system("aplay -q glass.wav &");
		if (l==0)
		{
			grid1[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]=0;
			grid1[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]=0;
		}
		if (l==1)
		{
			grid2[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]=0;
			grid2[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]=0;
		}
		if (l==2)
		{
			grid3[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]=0;
			grid3[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]=0;
		}
		while (yc1>=-7)
		{
			yc1=yc1-0.05;
			y2=y2-0.05;
			draw();
		}

	}
	if (yc1<=-7)
	{
		x1=0;
		x2=0;
		z1=0;
		z2=0;
		yc1=0.3;
		y2=-0.3;
	}
	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==4 || map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==4)
	{
		if (l==0)
		{	
			grid1[5][5]=1;
			grid1[6][5]=1;
			grid1[6][4]=1;
		}
		if (l==1)
		{	
			grid2[5][5]=1;
			grid2[6][5]=1;
			grid2[6][4]=1;
		}
		if (l==2)
		{	
			grid3[5][5]=1;
			grid3[6][5]=1;
			grid3[6][4]=1;
		}
		system("aplay -q switch.wav &");
	}
}

/* Executed when a regular key is released */
void keyboardUp (unsigned char key, int x, int y)
{
    switch (key) {
        case 'c':
        case 'C':
            rectangle_rot_status = !rectangle_rot_status;
            break;
        case 'p':
        case 'P':
            triangle_rot_status = !triangle_rot_status;
            break;
        case 'x':
            // do something
            break;
        default:
            break;
    }
}

/* Executed when a special key is pressed */
void keyboardSpecialDown (int key, int x, int y)
{
	if (key==GLUT_KEY_UP)
	{
		z1=z1-1.2;
		z2=z2-1.2;
	}
	if (key==GLUT_KEY_DOWN)
	{
		z1=z1+1.2;
		z2=z2+1.2;
	}

	if (key==GLUT_KEY_LEFT)
	{
		x1=x1-1.2;
		x2=x2-1.2;
	}
	if (key==GLUT_KEY_RIGHT)
	{
		x1=x1+1.2;
		x2=x2+1.2;
	}
	if (map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4]==0 && map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]==0)
	{
		while (yc1>=-7)
		{
			yc1=yc1-0.05;
			y2=y2-0.05;
			draw();
		}

	}
	if (yc1<=-7)
	{
		x1=0;
		x2=0;
		z1=0;
		z2=0;
		yc1=0.3;
		y2=-0.3;
	}
}

/* Executed when a special key is released */
void keyboardSpecialUp (int key, int x, int y)
{
}

/* Executed when a mouse button 'button' is put into state 'state'
 at screen position ('x', 'y')
 */
float zoom;
double rx, ry;
double azimuthal;
double elevation;
double k=0.5;
double r=5;
double px=400, py=400;
void mouseClick (int button, int state, int x, int y)
{
	double xx=x, yy=y;
	if (button==3)
	{
		//printf("hello\n");
		r--;
		if (v==4)
		{
			rx=(xx-px)*(M_PI/180)*k;
			ry=((yy-py)*(M_PI/180))*k;
			azimuthal-=rx;elevation-=ry;
			cx=(cos(elevation)*sin(azimuthal))*r;
			cz=(cos(elevation)*cos(azimuthal))*r;
			cy=(sin(elevation))*r;
			tx=0;ty=0;tz=0;
			px=xx;py=yy;
		}
	}
	if (button==4)
	{
		r++;
		if (v==4)
		{
			rx=(xx-px)*(M_PI/180)*k;
			ry=((yy-py)*(M_PI/180))*k;
			azimuthal-=rx;elevation-=ry;
			cx=(cos(elevation)*sin(azimuthal))*r;
			cz=(cos(elevation)*cos(azimuthal))*r;
			cy=(sin(elevation))*r;
			tx=0;ty=0;tz=0;
			px=xx;py=yy;
		}
	}
}

/* Executed when the mouse moves to position ('x', 'y') */
void mouseMotion (int x, int y)
{
	double xx=x, yy=y;

	if (v==4)
	{
		rx=(xx-px)*(M_PI/180)*k;
		ry=((yy-py)*(M_PI/180))*k;
		azimuthal-=rx;elevation-=ry;
		cx=(cos(elevation)*sin(azimuthal))*r;
		cz=(cos(elevation)*cos(azimuthal))*r;
		cy=(sin(elevation))*r;
		tx=0;ty=0;tz=0;
		px=xx;py=yy;
	}
}


/* Executed when window is resized to 'width' and 'height' */
/* Modify the bounds of the screen here in glm::ortho or Field of View in glm::Perspective */
void reshapeWindow (int width, int height)
{


	// sets the viewport of openGL renderer
	glViewport (0, 0, (GLsizei) width, (GLsizei) height);

	// set the projection matrix as perspective/ortho
	// Store the projection matrix in a variable for future use

     //Perspective projection for 3D views
     //Matrices.projection = glm::perspective (fov, (GLfloat) width / (GLfloat) height, 0.1f, 500.0f);

    // Ortho projection for 2D views
    Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
}

VAO *triangle, *rectangle;

// Creates the triangle object used in this sample code
void createTriangle ()
{
  /* ONLY vertices between the bounds specified in glm::ortho will be visible on screen */

  /* Define vertex array as used in glBegin (GL_TRIANGLES) */
  static const GLfloat vertex_buffer_data [] = {
    0, 1,0, // vertex 0
    -1,-1,0, // vertex 1
    1,-1,0, // vertex 2
  };

  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 0
    0,1,0, // color 1
    0,0,1, // color 2
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  triangle = create3DObject(GL_TRIANGLES, 3, vertex_buffer_data, color_buffer_data, GL_LINE);
}

void createRectangle ()
{
  // GL3 accepts only Triangles. Quads are not supported static
  const GLfloat vertex_buffer_data [] = {
    -1.2,-1,0, // vertex 1
    1.2,-1,0, // vertex 2
    1.2, 1,0, // vertex 3

    1.2, 1,0, // vertex 3
    -1.2, 1,0, // vertex 4
    -1.2,-1,0  // vertex 1
  };

  static const GLfloat color_buffer_data [] = {
    1,0,0, // color 1
    0,0,1, // color 2
    0,1,0, // color 3

    0,1,0, // color 3
    0.3,0.3,0.3, // color 4
    1,0,0  // color 1
  };

  // create3DObject creates and returns a handle to a VAO that can be used later
  rectangle = create3DObject(GL_TRIANGLES, 6, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createCube1()
{
	const GLfloat vertex_buffer_data [] = {

	//Front face
    -0.3,-0.3,0.3, // vertex 0.3
    -0.3,0.3,0.3, // vertex 2
    0.3, -0.3,0.3, // vertex 3

    0.3, -0.3,0.3, // vertex 3
    0.3, 0.3,0.3, // vertex 4
    -0.3,0.3,0.3,  // vertex 0.3

    //Back face
    -0.3,-0.3,-0.3, // vertex 0.3
    -0.3,0.3,-0.3, // vertex 2
    0.3, -0.3,-0.3, // vertex 3

    0.3, -0.3,-0.3, // vertex 3
    0.3, 0.3,-0.3, // vertex 4
    -0.3,0.3,-0.3,  // vertex 0.3

    //Right face
    0.3, -0.3,0.3, // vertex 3
    0.3, 0.3,0.3, // vertex 4
    0.3,-0.3,-0.3,  // vertex 0.3

    0.3, -0.3,-0.3, // vertex 3
    0.3, 0.3,-0.3, // vertex 4
    0.3,0.3,0.3,  // vertex 0.3

    //Left face
    -0.3, -0.3,0.3, // vertex 3
    -0.3, 0.3,0.3, // vertex 4
    -0.3,-0.3,-0.3,  // vertex 0.3

    -0.3, -0.3,-0.3, // vertex 3
    -0.3, 0.3,-0.3, // vertex 4
    -0.3,0.3,0.3,  // vertex 0.3

    //Top face
    -0.3, 0.3,0.3, // vertex 3
    -0.3, 0.3,-0.3, // vertex 4
    0.3,0.3,0.3,  // vertex 0.3

    0.3, 0.3,0.3, // vertex 3
    0.3, 0.3,-0.3, // vertex 4
    -0.3,0.3,-0.3,  // vertex 0.3

    //Bottom face
    -0.3, -0.3,0.3, // vertex 3
    -0.3, -0.3,-0.3, // vertex 4
    0.3,-0.3,0.3,  // vertex 0.3

    0.3, -0.3,0.3, // vertex 3
    0.3, -0.3,-0.3, // vertex 4
    -0.3,-0.3,-0.3  // vertex 0.3
  };
  static const GLfloat color_buffer_data [] = {

  	color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1
          color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1
  };
  block.cube1 = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
  block.cube2 = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createTile1(int i)
{
  const GLfloat vertex_buffer_data [] = {

  //Front face
    -0.3,-0.05,0.3, // vertex 0
    -0.3,0,0.3, // vertex 2
    0.3, -0.05,0.3, // vertex 3

    0.3, -0.05,0.3, // vertex 3
    0.3, 0,0.3, // vertex 4
    -0.3,0,0.3,  // vertex 0

    //Back face
    -0.3,-0.05,-0.3, // vertex 0
    -0.3,0,-0.3, // vertex 2
    0.3, -0.05,-0.3, // vertex 3

    0.3, -0.05,-0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    -0.3,0,-0.3,  // vertex 0

    //Right face
    0.3, -0.05,0.3, // vertex 3
    0.3, 0,0.3, // vertex 4
    0.3,-0.05,-0.3,  // vertex 0

    0.3, -0.05,-0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    0.3,0,0.3,  // vertex 0

    //Left face
    -0.3, -0.05,0.3, // vertex 3
    -0.3, 0,0.3, // vertex 4
    -0.3,-0.05,-0.3,  // vertex 0

    -0.3, -0.05,-0.3, // vertex 3
    -0.3, 0,-0.3, // vertex 4
    -0.3,0,0.3,  // vertex 0

    //Top face
    -0.3, 0,0.3, // vertex 3
    -0.3, 0,-0.3, // vertex 4
    0.3,0,0.3,  // vertex 0

    0.3, 0,0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    -0.3,0,-0.3,  // vertex 0

    //Bottom face
    -0.3, -0.05,0.3, // vertex 3
    -0.3, -0.05,-0.3, // vertex 4
    0.3,-0.05,0.3,  // vertex 0

    0.3, -0.05,0.3, // vertex 3
    0.3, -0.05,-0.3, // vertex 4
    -0.3,-0.05,-0.3  // vertex 0
  };
  static const GLfloat color_buffer_data [] = {

    //front and back
color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1
          color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1
  };
  tile[i].tileimg = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createTile2(int i)
{
  const GLfloat vertex_buffer_data [] = {

  //Front face
    -0.3,-0.05,0.3, // vertex 0
    -0.3,0,0.3, // vertex 2
    0.3, -0.05,0.3, // vertex 3

    0.3, -0.05,0.3, // vertex 3
    0.3, 0,0.3, // vertex 4
    -0.3,0,0.3,  // vertex 0

    //Back face
    -0.3,-0.05,-0.3, // vertex 0
    -0.3,0,-0.3, // vertex 2
    0.3, -0.05,-0.3, // vertex 3

    0.3, -0.05,-0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    -0.3,0,-0.3,  // vertex 0

    //Right face
    0.3, -0.05,0.3, // vertex 3
    0.3, 0,0.3, // vertex 4
    0.3,-0.05,-0.3,  // vertex 0

    0.3, -0.05,-0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    0.3,0,0.3,  // vertex 0

    //Left face
    -0.3, -0.05,0.3, // vertex 3
    -0.3, 0,0.3, // vertex 4
    -0.3,-0.05,-0.3,  // vertex 0

    -0.3, -0.05,-0.3, // vertex 3
    -0.3, 0,-0.3, // vertex 4
    -0.3,0,0.3,  // vertex 0

    //Top face
    -0.3, 0,0.3, // vertex 3
    -0.3, 0,-0.3, // vertex 4
    0.3,0,0.3,  // vertex 0

    0.3, 0,0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    -0.3,0,-0.3,  // vertex 0

    //Bottom face
    -0.3, -0.05,0.3, // vertex 3
    -0.3, -0.05,-0.3, // vertex 4
    0.3,-0.05,0.3,  // vertex 0

    0.3, -0.05,0.3, // vertex 3
    0.3, -0.05,-0.3, // vertex 4
    -0.3,-0.05,-0.3  // vertex 0
  };
  static const GLfloat color_buffer_data [] = {

    color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1
          color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1
	};
  tile[i].tileimg = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createTile3(int i)
{
  const GLfloat vertex_buffer_data [] = {

  //Front face
    -0.3,-0.05,0.3, // vertex 0
    -0.3,0,0.3, // vertex 2
    0.3, -0.05,0.3, // vertex 3

    0.3, -0.05,0.3, // vertex 3
    0.3, 0,0.3, // vertex 4
    -0.3,0,0.3,  // vertex 0

    //Back face
    -0.3,-0.05,-0.3, // vertex 0
    -0.3,0,-0.3, // vertex 2
    0.3, -0.05,-0.3, // vertex 3

    0.3, -0.05,-0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    -0.3,0,-0.3,  // vertex 0

    //Right face
    0.3, -0.05,0.3, // vertex 3
    0.3, 0,0.3, // vertex 4
    0.3,-0.05,-0.3,  // vertex 0

    0.3, -0.05,-0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    0.3,0,0.3,  // vertex 0

    //Left face
    -0.3, -0.05,0.3, // vertex 3
    -0.3, 0,0.3, // vertex 4
    -0.3,-0.05,-0.3,  // vertex 0

    -0.3, -0.05,-0.3, // vertex 3
    -0.3, 0,-0.3, // vertex 4
    -0.3,0,0.3,  // vertex 0

    //Top face
    -0.3, 0,0.3, // vertex 3
    -0.3, 0,-0.3, // vertex 4
    0.3,0,0.3,  // vertex 0

    0.3, 0,0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    -0.3,0,-0.3,  // vertex 0

    //Bottom face
    -0.3, -0.05,0.3, // vertex 3
    -0.3, -0.05,-0.3, // vertex 4
    0.3,-0.05,0.3,  // vertex 0

    0.3, -0.05,0.3, // vertex 3
    0.3, -0.05,-0.3, // vertex 4
    -0.3,-0.05,-0.3  // vertex 0
  };
  static const GLfloat color_buffer_data [] = {

    color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1
          color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1
	};
  tile[i].tileimg = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}

void createTile4(int i)
{
  const GLfloat vertex_buffer_data [] = {

  //Front face
    -0.3,-0.05,0.3, // vertex 0
    -0.3,0,0.3, // vertex 2
    0.3, -0.05,0.3, // vertex 3

    0.3, -0.05,0.3, // vertex 3
    0.3, 0,0.3, // vertex 4
    -0.3,0,0.3,  // vertex 0

    //Back face
    -0.3,-0.05,-0.3, // vertex 0
    -0.3,0,-0.3, // vertex 2
    0.3, -0.05,-0.3, // vertex 3

    0.3, -0.05,-0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    -0.3,0,-0.3,  // vertex 0

    //Right face
    0.3, -0.05,0.3, // vertex 3
    0.3, 0,0.3, // vertex 4
    0.3,-0.05,-0.3,  // vertex 0

    0.3, -0.05,-0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    0.3,0,0.3,  // vertex 0

    //Left face
    -0.3, -0.05,0.3, // vertex 3
    -0.3, 0,0.3, // vertex 4
    -0.3,-0.05,-0.3,  // vertex 0

    -0.3, -0.05,-0.3, // vertex 3
    -0.3, 0,-0.3, // vertex 4
    -0.3,0,0.3,  // vertex 0

    //Top face
    -0.3, 0,0.3, // vertex 3
    -0.3, 0,-0.3, // vertex 4
    0.3,0,0.3,  // vertex 0

    0.3, 0,0.3, // vertex 3
    0.3, 0,-0.3, // vertex 4
    -0.3,0,-0.3,  // vertex 0

    //Bottom face
    -0.3, -0.05,0.3, // vertex 3
    -0.3, -0.05,-0.3, // vertex 4
    0.3,-0.05,0.3,  // vertex 0

    0.3, -0.05,0.3, // vertex 3
    0.3, -0.05,-0.3, // vertex 4
    -0.3,-0.05,-0.3  // vertex 0
  };
  static const GLfloat color_buffer_data [] = {

    //front and back
    color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1
          color.r, color.g, color.b,       // color 1
          color.r, color.g, color.b + 0.2, // color 2
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.4, // color 3
          color.r, color.g, color.b + 0.6, // color 4
          color.r, color.g, color.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1

          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color1 1
          color1.r, color1.g, color1.b,       // color1 1
          color1.r, color1.g, color1.b + 0.2, // color1 2
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.4, // color1 3
          color1.r, color1.g, color1.b + 0.6, // color1 4
          color1.r, color1.g, color1.b + 0.8, // color 1
	};
  tile[i].tileimg = create3DObject(GL_TRIANGLES, 36, vertex_buffer_data, color_buffer_data, GL_FILL);
}



float camera_rotation_angle = 90;
float rectangle_rotation = 0;
float triangle_rotation = 0;

void renderBitmapString(float x, float y, void *font,const char *string){
    const char *c;
    glRasterPos2f(x, y);
    for (c=string; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'a');
    }
} 

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw ()
{
		if (v==0)
		{
			Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
			cx=1;
			cy=2;
			cz=3;
			tx=0;
			ty=0;
			tz=0;
		}
		if (v==1)
		{
			Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
			//printf("hello\n");
			cx=2;
			cz=0;
			cy=3;
			tx=0;
			ty=0;
			tz=0;
		}
		if (v==2)
		{
			Matrices.projection = glm::ortho(-4.0f, 4.0f, -4.0f, 4.0f, 0.1f, 500.0f);
			cx=x1-1;
			cz=z1-1;
			cy=3;
			tx=x1;
			ty=yc1;
			tz=z1;
		}
		if (v==3)
		{
			//Perspective projection for 3D views
     		Matrices.projection = glm::perspective (fov, (GLfloat) width / (GLfloat) height, 0.1f, 500.0f);

    // Ortho projection for 2D views
			if (b==0)
			{
				cx=x2+0.3;
				cy=y2;
				cz=z2;
				tx=x2+4;
				ty=y2;
				tz=0;
			}
			if (b==1)
			{
				cx=x2;
				cy=y2;
				cz=z2+0.3;
				tx=x2;
				ty=y2-0.3;
				tz=z2+4;
			}
			if (b==2)
			{
				cx=x2-0.3;
				cy=y2;
				cz=z2;
				tx=x2-4;
				ty=y2-0.3;
				tz=0;
			}
			if (b==3)
			{
				cx=x2;
				cy=y2;
				cz=z2-0.3;
				tx=x2;
				ty=y2-0.3;
				tz=z2-4;
			}

		}
		if (v==4)
		{

		}
  // clear the color and depth in the frame buffer
	//printf("%d %d %d\n", xalign, yalign, zalign);
  	//printf("%.1f %.1f %.1f %.1f %.1f %.1f\n", x1, yc1, z1, x2, y2, z2);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3d(1.0, 0.0, 0.0);

  //printf("%f %f %f %f %d %d\n", x1, z1, x2, z2, map.gridconfig[(int)(z1/0.6f)+4][(int)(x1/0.6f)+4], map.gridconfig[(int)(z2/0.6f)+4][(int)(x2/0.6)+4]);
  //printf("%d %d\n", (int)(z1/0.6f)+4, (int)(x1/0.6f)+4);
  // use the loaded shader program
  // Don't change unless you know what you are doing
  glUseProgram (programID);

  // Eye - Location of camera. Don't change unless you are sure!!
  glm::vec3 eye (cx,cy,cz);
  // Target - Where is the camera looking at.  Don't change unless you are sure!!
  glm::vec3 target (tx, ty, tz);
  // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
  glm::vec3 up (0, 1, 0);

  // Compute Camera matrix (view)
  Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
  //  Don't change unless you are sure!!
  //Matrices.view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0)); // Fixed camera for 2D (ortho) in XY plane

  // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
  //  Don't change unless you are sure!!
  glm::mat4 VP = Matrices.projection * Matrices.view;

  // Send our transformation to the currently bound shader, in the "MVP" uniform
  // For each model you render, since the MVP will be different (at least the M part)
  //  Don't change unless you are sure!!
  glm::mat4 MVP;	// MVP = Projection * View * Model

  // Load identity to model matrix
  Matrices.model = glm::mat4(1.0f);
  glDisable(GL_TEXTURE_2D);
  glLoadIdentity();
  glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,'3');
  renderBitmapString(600,200,GLUT_BITMAP_TIMES_ROMAN_24,"Font Rendering - Programming Techniques");
  renderBitmapString(600,220, GLUT_BITMAP_TIMES_ROMAN_24, s);
  renderBitmapString(600,240,GLUT_BITMAP_TIMES_ROMAN_24,"Esc - Quit");
  glEnable(GL_TEXTURE_2D);
  /* Render your scene */
  //  Don't change unless you are sure!!
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix

  Matrices.model = glm::mat4(1.0f);

  //glm::mat4 translateRectangle = glm::translate (glm::vec3(0, 0, 0));        // glTranslatef
  //glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  //block.transvector=glm::translate(glm::vec3(0,0,zblock));
  block.transvector1=glm::translate(glm::vec3(x1,yc1,z1));
  block.transvector2=glm::translate(glm::vec3(x2,y2,z2));
  Matrices.model *= (block.transvector1*efgh);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(block.cube1);
  Matrices.model = glm::mat4(1.0f);

  //glm::mat4 translateRectangle = glm::translate (glm::vec3(0, 0, 0));        // glTranslatef
  //glm::mat4 rotateRectangle = glm::rotate((float)(rectangle_rotation*M_PI/180.0f), glm::vec3(0,0,1)); // rotate about vector (-1,1,1)
  //block.transvector=glm::translate(glm::vec3(0,0,zblock));
  Matrices.model *= (block.transvector2*abcd);
  MVP = VP * Matrices.model;
  glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);

  // draw3DObject draws the VAO given to it using current MVP matrix
  draw3DObject(block.cube2);
  Matrices.model = glm::mat4(1.0f);
  int i,j,c=0;

  if (l==0)
  {
  	for (i=0;i<9;i++)
  	{
  		for (j=0;j<9;j++)
  		{
  			//printf(%)
  			map.gridconfig[i][j]=grid1[i][j];  			
  		}
  	}
  }
  else if (l==1)
  {
  	for (i=0;i<9;i++)
  	{
  		for (j=0;j<9;j++)
  		{
  			map.gridconfig[i][j]=grid2[i][j];  			
  		}
  	}
  }
  else if (l==2)
  {
  	for (i=0;i<9;i++)
  	{
  		for (j=0;j<9;j++)
  		{
  			map.gridconfig[i][j]=grid3[i][j];  			
  		}
  	}
  }


  for (i=0;i<9;i++)
  {
    for (j=0;j<9;j++)
    {
        if (map.gridconfig[i][j]==1)
        {
        	color.r=0;
        	color.g=0;
        	color.b=0.2;
        	color1.r=0;
        	color1.g=0;
        	color1.b=0.2;
            createTile1(c);
            tile[c].transvector = glm::translate (glm::vec3((j-4)*0.6,-0.6,(i-4)*0.6));
            Matrices.model *= tile[c].transvector;
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DObject(tile[c].tileimg);
            Matrices.model = glm::mat4(1.0f);
            c++;
        }
        if (map.gridconfig[i][j]==2)
        {
        	color.r=0.5;
        	color.g=0.5;
        	color.b=0.2;
        	color1.r=0.5;
        	color1.g=0.5;
        	color1.b=0.2;
            createTile2(c);
            tile[c].transvector = glm::translate (glm::vec3((j-4)*0.6,-0.6,(i-4)*0.6));
            Matrices.model *= tile[c].transvector;
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DObject(tile[c].tileimg);
            Matrices.model = glm::mat4(1.0f);
            c++;
        }
        if (map.gridconfig[i][j]==3)
        {
        	color.r=0.8;
        	color.g=0;
        	color.b=0.2;
        	color1.r=0.8;
        	color1.g=0;
        	color1.b=0.2;
            createTile3(c);
            tile[c].transvector = glm::translate (glm::vec3((j-4)*0.6,-0.6,(i-4)*0.6));
            Matrices.model *= tile[c].transvector;
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DObject(tile[c].tileimg);
            Matrices.model = glm::mat4(1.0f);
            c++;
        }
        if (map.gridconfig[i][j]==4)
        {
        	color.r=0;
        	color.g=0.8;
        	color.b=0.2;
        	color1.r=0;
        	color1.g=0.8;
        	color1.b=0.2;
            createTile4(c);
            tile[c].transvector = glm::translate (glm::vec3((j-4)*0.6,-0.6,(i-4)*0.6));
            Matrices.model *= tile[c].transvector;
            MVP = VP * Matrices.model;
            glUniformMatrix4fv(Matrices.MatrixID, 1, GL_FALSE, &MVP[0][0]);
            draw3DObject(tile[c].tileimg);
            Matrices.model = glm::mat4(1.0f);
            c++;
        }
    }
  }
  // Swap the frame buffers
  glutSwapBuffers ();

  // Increment angles
  float increments = 1;

  //camera_rotation_angle++; // Simulating camera rotation
}

/* Executed when the program is idle (no I/O activity) */
void idle () {
    // OpenGL should never stop drawing
    // can draw the same scene or a modified scene
    draw (); // drawing same scene
}


/* Initialise glut window, I/O callbacks and the renderer to use */
/* Nothing to Edit here */
void initGLUT (int& argc, char** argv, int width, int height)
{
    // Init glut
    glutInit (&argc, argv);

    // Init glut window
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitContextVersion (3, 3); // Init GL 3.3
    glutInitContextFlags (GLUT_CORE_PROFILE); // Use Core profile - older functions are deprecated
    glutInitWindowSize (width, height);
    glutCreateWindow ("Sample OpenGL3.3 Application");

    // Initialize GLEW, Needed in Core profile
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cout << "Error: Failed to initialise GLEW : "<< glewGetErrorString(err) << endl;
        exit (1);
    }

    // register glut callbacks
    glutKeyboardFunc (keyboardDown);
    glutKeyboardUpFunc (keyboardUp);

    glutSpecialFunc (keyboardSpecialDown);
    glutSpecialUpFunc (keyboardSpecialUp);

    glutMouseFunc (mouseClick);
    glutMotionFunc (mouseMotion);

    glutReshapeFunc (reshapeWindow);

    glutDisplayFunc (draw); // function to draw when active
    glutIdleFunc (idle); // function to draw when idle (no I/O activity)
    
    glutIgnoreKeyRepeat (true); // Ignore keys held down
}

/* Process menu option 'op' */
void menu(int op)
{
    switch(op)
    {
        case 'Q':
        case 'q':
            exit(0);
    }
}

void addGLUTMenus ()
{
    // create sub menus
    int subMenu = glutCreateMenu (menu);
    glutAddMenuEntry ("Do Nothing", 0);
    glutAddMenuEntry ("Really Quit", 'q');

    // create main "middle click" menu
    glutCreateMenu (menu);
    glutAddSubMenu ("Sub Menu", subMenu);
    glutAddMenuEntry ("Quit", 'q');
    glutAttachMenu (GLUT_MIDDLE_BUTTON);
}


/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL (int width, int height)
{
	// Create the models
	createTriangle (); // Generate the VAO, VBOs, vertices data & copy into the array buffer

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders( "Sample_GL.vert", "Sample_GL.frag" );
	// Get a handle for our "MVP" uniform
	Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


	reshapeWindow (width, height);

	// Background color of the scene
	glClearColor (0.79f, 0.79f, 0.79f, 1.0f); // R, G, B, A
	glClearDepth (1.0f);

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LEQUAL);

	createRectangle ();
	color.r=0;
	color.g=0.9;
	color.b=0;
	color1.r=0;
	color1.g=0.9;
	color1.b=0;
	createCube1();
	cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
	cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
	cout << "VERSION: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

int main (int argc, char** argv)
{
	
  //printf("%d\n", map.gridconfig[0][5]);

    initGLUT (argc, argv, width, height);

    addGLUTMenus ();

	initGL (width, height);

    glutMainLoop ();

    return 0;
}
