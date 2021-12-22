# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <cmath>
# include <GLEW/glew.h>
# include <GLFW/glfw3.h>

//Window Dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformXMove, uniformChangeColour;

bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

bool hue = true;
float hueOffset = 0.0f;
float hueIncrement = 0.0005f;
float hueMaxOffset = 1.0f;

// Vertex Shader
static const char* vShader = "									\n\
#version 330													\n\
																\n\
layout (location = 0) in vec3 pos;								\n\
																\n\
uniform float xMove;											\n\
																\n\
void main()														\n\
{																\n\
	gl_Position = vec4(0.4*pos.x + xMove, 0.4*pos.y, 0.4*pos.z, 1.0);	\n\
}";

// Fragment Shader
static const char* fShader = "			\n\
#version 330							\n\
										\n\
out vec4 colour;						\n\
										\n\
uniform float changeColour;				\n\
										\n\
void main()								\n\
{										\n\
	colour = vec4(1.0, -changeColour, changeColour, 1.0);	\n\
}";

void CreateTriangle()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO); // Generates a vertes array object of size 1(number of VAOS is 1) and assigns the ID to VAO.
	glBindVertexArray(VAO); // Binds the VAO

	glGenBuffers(1, &VBO); // Generates a vertes buffer object of size 1(number of VBOS is 1) and assigns the ID to VBO.
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //Binds the VBO to a target (GL_ARRAY_BUFFER). You can bind to many different targets.
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //Binding the vertices to the VBO. We use sizeof() because it gives us the size of the vertices but we cn also use GLfloat*9 if we have an object stored somewhere else. GL_STATIC_DRAW is used to declare that our values are not changing.
	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 0, 0); // The first value is the location. This is the same asthe value in the shaders (location = 0). The second value is the size of each attribute. It has been set to 3 since we have 3 values (x, y, and z). The third value is the type of value 2 which is GL_FLOAT. The fourth is whether we want to normalize the values, which we don't so we go with GL_FALSE. The fifth is the stride value which is useful when you have colors mixed in with the vertices. It tells the program to skip a certain amount of values before going to the next value. The last value is the offset which is the starting point of drawing large objects.
	glEnableVertexAttribArray(0); // this enables the array to work and connects to the first value (0) in the previous function.

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbinding the VBO

	glBindVertexArray(0); // Unbinding the VAO

}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType); // Will create an empty shader for that type and pass the ID into the shader.

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader); // Compiles the shader code

	GLint result = 0;
	GLchar eLog[1024] = { 0 }; // Logs the errors here


	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result); // Picks up on what program we want to work on and the informationwe want to get from it i.e compile status.The result is stored in result.
	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog); // We get the InfoLog for shader and the size of where we are storing it.
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog); //%s - String . The line of code outputs the error.
		return;
	}

	glAttachShader(theProgram, theShader);



} // Adds the shaders to the shader program. When compiling the shader, we need to tell the compiler what shader it's going to be. That is why we have the GLenum shaderType

void CompileShaders() {
	shader = glCreateProgram(); // Creates the shader program

	if (!shader) {
		printf("Error creating shader program!\n");
		return;
	} // Checkes if the shader has been created and if not, throws an error

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	// Code for error debugging. IntelliSense does not pick up on errors in shader code.
	// AddShader also picks up on some errors
	GLint result = 0; 
	GLchar eLog[1024] = { 0 }; // Logs the errors here

	glLinkProgram(shader); // Create the executables on the graphics card linking all the programs and making sure it's working
	
	glGetProgramiv(shader, GL_LINK_STATUS, &result); // Picks up on what program we want to work on and the informationwe want to get from it i.e link status.The result is stored in result.
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog); // We get the InfoLog for shader and the size of where we are storing it.
		printf("Error linking program: '%s'\n", eLog); //%s - String . The line of code outputs the error.
		return;
	}

	glValidateProgram(shader); // Checks if the shader we've created is valid in the current context that openGL is working in.
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result); // Picks up on what program we want to work on and the informationwe want to get from it i.e validate status.The result is stored in result.
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog); // We get the InfoLog for shader and the size of where we are storing it.
		printf("Error validating program: '%s'\n", eLog); //%s - String . The line of ocde outputs the error.
		return;
	}

	uniformXMove = glGetUniformLocation(shader, "xMove");
	uniformChangeColour = glGetUniformLocation(shader, "changeColour");

}

int main()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		printf("GLFW initialization failed!");
		glfwTerminate();
		return 1; // The standard for returning an error in C++
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No Backward Compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // This indicates that OpenGL will not be backwards compatible.
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Allow Forward Compatibility

	// Setting up properties for GLFW window
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Shaders and First Triangle", NULL, NULL);
	if (!mainWindow)
	{
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	// Get the Buffer Size Information - The area in the middle of the window is the buffer. Holds all the OpenGL data as it is being passed to the window.
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight); // We are using references to bufferWidth and bufferHeight. We are getting the values of the two and putting them in the bufferWidth and bufferHeight.

	//Set the context for GLEW to use
	glfwMakeContextCurrent(mainWindow); // if you have two windows you can switch between the context of the two windows.

	// Allow modern extension features
	glewExperimental = GL_TRUE; // enables you to access extensions for opengl. Setting this to true allows you to do this.
	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialization failed!");
		glfwDestroyWindow(mainWindow); // We destroy the window that was created before terminating glew.
		glfwTerminate();
		return 1;
	}

	//Set up our viewport size
	glViewport(0, 0, bufferWidth, bufferHeight); // We are passing buffer width and buffer height because this is the middle part of the window that we want to draw to.

	CreateTriangle();
	CompileShaders();

	//Loop until window closed
	while (!glfwWindowShouldClose(mainWindow)) // When we press the x button at the top right of thw window, GLFW automatically calls glfwWindowShouldClose(). This sets its vlue to true causing the window to close. You can also customize that button to do other functions.
	{
		// Get and Handle user input events
		glfwPollEvents(); // checks if any events have happened(resizing window, clicking etc.)

		if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		

		if (hue)
		{
			hueOffset += hueIncrement;
		}
		else {
			hueOffset -= hueIncrement;
		}

		if (abs(hueOffset) >= hueMaxOffset) 
		{
			hue = !hue;
		}
		//Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // allows to clear the screen and set it to a certain color. alpha value 0.0f is transparent and 1.0f is opaque.
		glClear(GL_COLOR_BUFFER_BIT); // clearing the color buffer

		glUseProgram(shader);

		//glUniform1f(uniformXMove, triOffset); // This line creates movement from side to side.
		glUniform1f(uniformChangeColour, hueOffset);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3); // 0 is where int hr array we want to start drawing and 3 is the number of points we want.

		glBindVertexArray(0);

		glUseProgram(0); // unassigning the shader

		glfwSwapBuffers(mainWindow); // We have two buffers. One that the user can see and one that they cannot(that we are drawing to). This swaps them so that they can see the window we are drawing to.

	}

	return 0;
}