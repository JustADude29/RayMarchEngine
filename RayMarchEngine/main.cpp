#include "libs.h"

Vertex vertices[] =
{
	//pos							//color						//texcoord
	glm::vec3(-1.f, 1.f, 0.f),		glm::vec3(1.0f,0.f,0.f),	glm::vec2(0.f, 1.f),
	glm::vec3(-1.f, -1.f, 0.f),		glm::vec3(0.f,1.0f,0.f),	glm::vec2(0.f, 0.f),
	glm::vec3(1.f, -1.f, 0.f),		glm::vec3(0.f,0.f,1.0f),	glm::vec2(1.f, 0.f),
	glm::vec3(1.f, 1.f, 0.f),		glm::vec3(1.f,1.0f,0.f),	glm::vec2(0.f, 0.f)
};
unsigned noOfVertices = sizeof(vertices) / sizeof(Vertex);

GLuint indices[]
{
	0,1,2,
	0,2,3
};
unsigned noOfIndices = sizeof(indices) / sizeof(GLuint);

void framebuffer_resize_callback(GLFWwindow* window,int fbW,int fbH) {
	glViewport(0, 0, fbW, fbH);

}

void updateInput(GLFWwindow* wind) {
	if (glfwGetKey(wind, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(wind, GLFW_TRUE);
	}
}

bool loadShaders(GLuint& program) {
	bool loadSuccess = true;
	char infoLog[512];
	GLint success;

	std::string temp = "";
	std::string src = "";

	std::ifstream in_file;

	//Vertex
	in_file.open("vert.glsl");
	if (in_file.is_open()) {
		while (std::getline(in_file, temp)) {
			src += temp + "\n";
		}
	}
	else {
		loadSuccess = false;
		std::cout << "ERROR::LOADSHADERS::COULD_NOT_READ_VERTEX_FILE" << std::endl;
	}

	in_file.close();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vertSrc = src.c_str();
	glShaderSource(vertexShader, 1, &vertSrc, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		loadSuccess = false;
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::LOADSHADERS::COULD_NOT_COMPILE_VERTEX_SHADERS" << std::endl;
		std::cout << infoLog << std::endl;
	}

	temp = "";
	src = "";

	//Fragment
	in_file.open("frag.glsl");
	if (in_file.is_open()) {
		while (std::getline(in_file, temp)) {
			src += temp + "\n";
		}
	}
	else {
		loadSuccess = false;
		std::cout << "ERROR::LOADSHADERS::COULD_NOT_READ_FRAGMENT_FILE" << std::endl;
	}

	in_file.close();

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fragSrc = src.c_str();
	glShaderSource(fragmentShader, 1, &fragSrc, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		loadSuccess = false;
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::LOADSHADERS::COULD_NOT_COMPILE_FRAGMENT_SHADER" << std::endl;
		std::cout << infoLog << std::endl;
	}

	//Program
	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		loadSuccess = false;
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "ERROR::LOADSHADERS::COULD_NOT_LINK_PROGRAM" << std::endl;
		std::cout << infoLog << std::endl;
	}

	//END
	glUseProgram(0);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return loadSuccess;
}

int main()
{
	//initialize glfw
	glfwInit();

	//create window
	const int WINDOW_WIDTH = 1440;
	const int WINDOW_HEIGHT = 900;
	int framebufferWidth = 0;
	int framebufferHeight = 0;

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "RayMarch", NULL, NULL);

	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	//glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
	//glViewport(0, 0, framebufferWidth, framebufferHeight);

	glfwMakeContextCurrent(window);

	//initialize glew
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cout << "ERROR::MAIN>CPP::GLEW_INIT_FAILED" << std::endl;
		glfwTerminate();
	}

	//GL options
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Shaders init
	GLuint core_program;
	if (!loadShaders(core_program))
		glfwTerminate();

	//Model

	//VAO(Vertex Array Object), VBO(Vertex Buffer Object), EBO(Element Buffer Object)
	
	//Gen VAO and bind
	GLuint VAO;
	glCreateVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Gen VBO and bind and send data
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Gen EBO and bind and send data
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Set Vertex Attributes and enable (input assembly)
	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	//Color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
	glEnableVertexAttribArray(1);

	//Texcoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
	glEnableVertexAttribArray(2);

	//Bind VAO 0
	glBindVertexArray(0);

	//Main Loop
	while (!glfwWindowShouldClose(window)) {
		//inout
		glfwPollEvents();

		//update
		updateInput(window);

		//draw

		//clear
		glClearColor(0.1f, 0.3f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		//use program
		glUseProgram(core_program);
		GLint u_res_loc = glGetUniformLocation(core_program, "u_resolution");
		glUniform2f(u_res_loc, WINDOW_WIDTH, WINDOW_HEIGHT);

		//bind vertex array object
		glBindVertexArray(VAO);
		
		//draw
		//glDrawArrays(GL_TRIANGLES, 0, noOfVertices);
		glDrawElements(GL_TRIANGLES, noOfIndices, GL_UNSIGNED_INT, 0);

		//end draw
		glfwSwapBuffers(window);
		glFlush();
	}




	//End of program
	glfwDestroyWindow(window);
	glfwTerminate();

	//Delete program
	glDeleteProgram(core_program);

	return 0;
}