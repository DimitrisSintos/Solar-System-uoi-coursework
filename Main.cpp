#define _CRT_SECURE_NO_WARNINGS
// Include standard headers
#include <stdio.h>
#include <stdlib.h>


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include <random>
#include <iomanip>
using std::cout;
using std::endl;
using std::setprecision;




// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"
using namespace glm;
using namespace std;


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

// Global flags to escape main loop and change triangle buffers 
int closeWindow = 0;


//camera flags
bool moveRight = false;
bool moveLeft = false;
bool moveUp = false;
bool moveDown = false;
bool moveIn = false;
bool moveOut = false;
bool firstMet = true;
bool crash = false;
bool spawnMet = false;

float planetSpeed = 0;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	if (key == GLFW_KEY_Q && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT )
		closeWindow = 1;

	if (key == GLFW_KEY_W && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
		moveRight = true;
	if (key == GLFW_KEY_W && action == GLFW_REPEAT && mods != GLFW_MOD_SHIFT)
		moveRight = true;
	if (key == GLFW_KEY_W && action == GLFW_RELEASE && mods != GLFW_MOD_SHIFT)
		moveRight = false;

	if (key == GLFW_KEY_X && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
		moveLeft = true;
	if (key == GLFW_KEY_X && action == GLFW_REPEAT && mods != GLFW_MOD_SHIFT)
		moveLeft = true;
	if (key == GLFW_KEY_X && action == GLFW_RELEASE && mods != GLFW_MOD_SHIFT)
		moveLeft = false;

	if (key == GLFW_KEY_A && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
		moveUp= true;
	if (key == GLFW_KEY_A && action == GLFW_REPEAT && mods != GLFW_MOD_SHIFT)
		moveUp = true;
	if (key == GLFW_KEY_A && action == GLFW_RELEASE && mods != GLFW_MOD_SHIFT)
		moveUp = false;

	if (key == GLFW_KEY_D && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
		moveDown = true;
	if (key == GLFW_KEY_D && action == GLFW_REPEAT && mods != GLFW_MOD_SHIFT)
		moveDown = true;
	if (key == GLFW_KEY_D && action == GLFW_RELEASE && mods != GLFW_MOD_SHIFT)
		moveDown = false;

	if (key == GLFW_KEY_EQUAL && action == GLFW_PRESS && mods == GLFW_MOD_SHIFT)
		moveIn= true;
	if (key == GLFW_KEY_EQUAL && action == GLFW_REPEAT && mods == GLFW_MOD_SHIFT)
		moveIn = true;
	if (key == GLFW_KEY_EQUAL && action == GLFW_RELEASE && mods == GLFW_MOD_SHIFT)
		moveIn = false;

	if (key == GLFW_KEY_MINUS && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
		moveOut = true;
	if (key == GLFW_KEY_MINUS && action == GLFW_REPEAT && mods != GLFW_MOD_SHIFT)
		moveOut = true;
	if (key == GLFW_KEY_MINUS && action == GLFW_RELEASE && mods != GLFW_MOD_SHIFT)
		moveOut = false;

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		spawnMet = true ;   
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT)
		planetSpeed += 0.00025;
	if (key == GLFW_KEY_P && action == GLFW_REPEAT && mods != GLFW_MOD_SHIFT)
		planetSpeed += 0.00025;
	if (key == GLFW_KEY_P && action == GLFW_RELEASE && mods != GLFW_MOD_SHIFT)
		planetSpeed -= 0.00025;

	if (key == GLFW_KEY_U && action == GLFW_PRESS && mods != GLFW_MOD_SHIFT) {
		if (planetSpeed >= 0) {
			planetSpeed -= 0.00025;
		}
	}
	if (key == GLFW_KEY_U && action == GLFW_REPEAT && mods != GLFW_MOD_SHIFT) {
		if (planetSpeed >= 0) {
			planetSpeed -= 0.00025;
		}
	}
	if (key == GLFW_KEY_U && action == GLFW_RELEASE && mods != GLFW_MOD_SHIFT)
		planetSpeed += 0.00025;

}


bool loadOBJ(
	const char* path,
	std::vector<glm::vec3>& out_vertices,
	std::vector<glm::vec2>& out_uvs,
	std::vector<glm::vec3>& out_normals
) {
	printf("Loading OBJ file %s...\n", path);

	std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
	std::vector<glm::vec3> temp_vertices;
	std::vector<glm::vec2> temp_uvs;
	std::vector<glm::vec3> temp_normals;


	FILE* file = fopen(path, "r");
	if (file == NULL) {
		printf("Impossible to open the file ! Are you in the right path ? See Tutorial 1 for details\n");
		getchar();
		return false;
	}

	while (1) {

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader

		if (strcmp(lineHeader, "v") == 0) {
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			temp_vertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			glm::vec2 uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uv.y = -uv.y; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
			temp_uvs.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			temp_normals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
			if (matches != 9) {
				printf("File can't be read by our simple parser :-( Try exporting with other options\n");
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			uvIndices.push_back(uvIndex[0]);
			uvIndices.push_back(uvIndex[1]);
			uvIndices.push_back(uvIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
		else {
			// Probably a comment, eat up the rest of the line
			char stupidBuffer[1000];
			fgets(stupidBuffer, 1000, file);
		}

	}

	// For each vertex of each triangle
	for (unsigned int i = 0; i < vertexIndices.size(); i++) {

		// Get the indices of its attributes
		unsigned int vertexIndex = vertexIndices[i];
		unsigned int uvIndex = uvIndices[i];
		unsigned int normalIndex = normalIndices[i];

		// Get the attributes thanks to the index
		glm::vec3 vertex = temp_vertices[vertexIndex - 1];
		glm::vec2 uv = temp_uvs[uvIndex - 1];
		glm::vec3 normal = temp_normals[normalIndex - 1];

		// Put the attributes in buffers
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);

	}
	fclose(file);
	return true;
}

glm::mat4 Projection = glm::perspective(glm::radians(30.0f), 4.0f / 3.0f, 0.1f, 100.0f);

// Camera matrix
float x = 0, y = 10, k = 100;
float xAngle = 0, yAngle = 0;
float sc = 1;
glm::vec3 position = glm::vec3(x, y, k);
glm::vec3 metPos = position;
glm::vec3 lookatPosition = glm::vec3(0, 0, 0);

glm::mat4 View = glm::lookAt(
	position, // Camera is at (10,10,30), in World Space
	lookatPosition, // and looks at the origin
	glm::vec3(0, 0, 1)  // up Vector
);



// Model matrix : an identity matrix (model will be at the origin)
glm::mat4 Model = glm::mat4(1.0f);

// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 MVP = Projection * View * Model;

glm::mat4 ModelMatrix = glm::translate(Model, glm::vec3(25, 0, 0));
glm::mat4 MVP1 = Projection * View * ModelMatrix;



glm::mat4 getViewMatrix() {
	if (moveRight == true) {
		//printf("RIGHT\n");
		moveRight = false;
		xAngle += 2.0f;
		

	}
	if (moveLeft == true) {
		//printf("LEFT\n");
		moveLeft = false;
		xAngle -= 2.0f;
		

	}
	if (moveUp == true) {
		//printf("Up\n");
		moveUp = false;
		yAngle += 2.0f;
		

	}
	if (moveDown == true) {
		//printf("DOWN\n");
		moveDown = false;
		yAngle -= 2.0f;
		

	}
	if (moveOut == true) {
		//printf("OUT\n");
		moveOut = false;
		sc -= 0.05;


	}

	if (moveIn == true) {
		//printf("IN\n");
		moveIn = false;
		sc += 0.05;
	}


	View = glm::lookAt(
		position,
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, 1)
	);

	View = glm::rotate(View, glm::radians(xAngle), glm::vec3(1, 0, 0));
	View = glm::rotate(View, glm::radians(yAngle), glm::vec3(0, 1, 0));

	View = glm::scale(View, glm::vec3(sc, sc, sc));

	return View;
}

vec3 ExtractCameraPos(const mat4& a_modelView)
{
	// Get the 3 basis vector planes at the camera origin and transform them into model space.
	//  
	// NOTE: Planes have to be transformed by the inverse transpose of a matrix
	//       Nice reference here: http://www.opengl.org/discussion_boards/showthread.php/159564-Clever-way-to-transform-plane-by-matrix
	//
	//       So for a transform to model space we need to do:
	//            inverse(transpose(inverse(MV)))
	//       This equals : transpose(MV) - see Lemma 5 in http://mathrefresher.blogspot.com.au/2007/06/transpose-of-matrix.html
	//
	// As each plane is simply (1,0,0,0), (0,1,0,0), (0,0,1,0) we can pull the data directly from the transpose matrix.
	//  
	mat4 modelViewT = transpose(a_modelView);

	// Get plane normals 
	vec3 n1(modelViewT[0]);
	vec3 n2(modelViewT[1]);
	vec3 n3(modelViewT[2]);

	// Get plane distances
	float d1(modelViewT[0].w);
	float d2(modelViewT[1].w);
	float d3(modelViewT[2].w);

	// Get the intersection of these 3 planes
	// http://paulbourke.net/geometry/3planes/
	vec3 n2n3 = cross(n2, n3);
	vec3 n3n1 = cross(n3, n1);
	vec3 n1n2 = cross(n1, n2);

	vec3 top = (n2n3 * d1) + (n3n1 * d2) + (n1n2 * d3);
	float denom = dot(n1, n2n3);

	return top / -denom;
}




int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(800, 800, "Ηλιακό Σύστημα", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
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


	// Ensure we can capture the n and c keys
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);


	//Black background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);



	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);



	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	 // Remember, matrix multiplication is the other way around


	int width, height, nrChannels;
	unsigned char* data[3];
	unsigned int textureID[3];
	data[0] = stbi_load("sun.jpg", &width, &height, &nrChannels, 0);
	data[1] = stbi_load("planet.jpg", &width, &height, &nrChannels, 0);
	data[2] = stbi_load("meteor.jpg", &width, &height, &nrChannels, 0);

	if (data)
	{

		glGenTextures(1, &textureID[0]);
		//sun
		// "Bind" the newly created texture : all future texture functions will modify this texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID[0]);

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[0]);
		glGenerateMipmap(GL_TEXTURE_2D);

		glDisable(GL_TEXTURE0);

		//planet
		glGenTextures(1, &textureID[1]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID[1]);

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[1]);
		glGenerateMipmap(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE1);

		//meteor
		glGenTextures(1, &textureID[2]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureID[2]);

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data[2]);
		glGenerateMipmap(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE2);

	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	
	

	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");


	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;


	bool sun = loadOBJ("sun.obj", vertices, uvs, normals);


	

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);


	std::vector<glm::vec3> vertices1;
	std::vector<glm::vec3> normals1;
	std::vector<glm::vec2> uvs1;


	bool planet = loadOBJ("planet.obj", vertices1, uvs1, normals1);




	GLuint vertexbuffer1;
	glGenBuffers(1, &vertexbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
	glBufferData(GL_ARRAY_BUFFER, vertices1.size() * sizeof(glm::vec3), &vertices1[0], GL_STATIC_DRAW);

	GLuint uvbuffer1;
	glGenBuffers(1, &uvbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
	glBufferData(GL_ARRAY_BUFFER, uvs1.size() * sizeof(glm::vec2), &uvs1[0], GL_STATIC_DRAW);

	//meteor
	std::vector<glm::vec3> vertices2;
	std::vector<glm::vec3> normals2;
	std::vector<glm::vec2> uvs2;


	bool meteor = loadOBJ("meteor.obj", vertices2, uvs2, normals2);




	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, vertices2.size() * sizeof(glm::vec3), &vertices2[0], GL_STATIC_DRAW);

	GLuint uvbuffer2;
	glGenBuffers(1, &uvbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
	glBufferData(GL_ARRAY_BUFFER, uvs2.size() * sizeof(glm::vec2), &uvs2[0], GL_STATIC_DRAW);

	
	float delta = 0.1;
	float theta = 0;
	float u = 25, n = 0, angle = 0;
	
	vec3 localcameraPos;//inisilze camera pos
	vec3 cameraPos = ExtractCameraPos(View);
	float metX = cameraPos[0];
	float metY = cameraPos[0];
	float metZ = cameraPos[0];
	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		

		// Use our shader
		glUseProgram(programID);

		View = getViewMatrix();

		MVP = Projection * View * Model;
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID[0]);

		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		if (crash==false) {
			angle += 0.00025+planetSpeed;
			u = cos(angle) * 25;
			n = sin(angle) * 25;
			ModelMatrix = glm::translate(Model, glm::vec3(u, n, 0));

			delta += 0.1;
			ModelMatrix = glm::rotate(ModelMatrix, glm::radians(delta), glm::vec3(0, 1, 0));


			MVP1 = Projection * View * ModelMatrix;
			// Send our transformation to the currently bound shader, 
			// in the "MVP" uniform
			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, textureID[1]);

			// Set our "myTextureSampler" sampler to use Texture Unit 0
			glUniform1i(TextureID, 1);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer1);
			glVertexAttribPointer(
				0,                  // attribute
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			// 2nd attribute buffer : UVs
			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
			glVertexAttribPointer(
				1,                                // attribute
				2,                                // size
				GL_FLOAT,                         // type
				GL_FALSE,                         // normalized?
				0,                                // stride
				(void*)0                          // array buffer offset
			);


			glDrawArrays(GL_TRIANGLES, 0, vertices1.size());

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);


		}
		if (spawnMet == true && crash == false) {
			if (firstMet) {
				firstMet = false;
				localcameraPos = ExtractCameraPos(View); //local extract camera pos
				metX = localcameraPos[0];
				metY = localcameraPos[1];
				metZ = localcameraPos[2];
				printf("metX '%f , metY=%f, metZ=%f \n", metX, metY, metZ);
			}
			float met_sun_dist = glm::distance(vec3(0, 0, 0),vec3(metX, metY, metZ));
			//printf("dist=%f\n", met_sun_dist);
			//std::cout << glm::to_string(met_sun_dist) << std::endl;
			//printf("metX '%f , metY=%f, metZ=%f \n", metX, metY, metZ);
			if (met_sun_dist >17 ){
			    //printf("metX '%f , metY=%f, metZ=%f \n", metX, metY, metZ);
				float minusPer = 0.0025;
				glm::mat4 meteorModel = glm::mat4(1.0f);
				if(metX<0){ 
					metX += -metX * minusPer;
				}
				else {
					metX -= metX * minusPer;
				}
				if (metY < 0) {
					metY += -metY * minusPer;
				}
				else {
					metY -= metY * minusPer;
				}if (metZ < 0) {
					metZ += -metZ * minusPer;
				}
				else {
					metZ -= metZ * minusPer;
				}
				
				
				float met_planet_dist = glm::distance(vec3(metX, metY, metZ), vec3(u, n, 0));
				if (met_planet_dist < 7) {
					crash = true;
				}
				meteorModel = glm::translate(meteorModel, glm::vec3(metX, metY, metZ));

				glm::mat4 MVP2 = Projection * View * meteorModel;
				// Send our transformation to the currently bound shader, 
				// in the "MVP" uniform
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);

				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, textureID[2]);

				// Set our "myTextureSampler" sampler to use Texture Unit 0
				glUniform1i(TextureID, 2);

				glEnableVertexAttribArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
				glVertexAttribPointer(
					0,                  // attribute
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
				);

				// 2nd attribute buffer : UVs
				glEnableVertexAttribArray(1);
				glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
				glVertexAttribPointer(
					1,                                // attribute
					2,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);


				glDrawArrays(GL_TRIANGLES, 0, vertices2.size());

				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
			}
			else {
				firstMet = true;
				spawnMet = false;
			}
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} 
	while (closeWindow == 0 && glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}





