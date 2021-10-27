#include "Shader.h"
std::string returnShaderContents(std::string fileName) {
	bool couldLoad = false;
	std::string shaderStr;
	std::ifstream shaderStream((fileName), std::ios::in);
	if (shaderStream.is_open()) {
		std::string Line = "";
		while (getline(shaderStream, Line))
		{
			shaderStr += "\n" + Line;
		}

		shaderStream.close();
		couldLoad = true;
	}
	else {
		std::string errorMessage = "Could not open shader: " + fileName + "\n";
		printf(errorMessage.c_str());

	}
	if (couldLoad) {
		return shaderStr;
	}
	return "";
}
Shader::Shader() {

}

Shader::Shader(std::string vertex, std::string fragment) {
	std::string vertContents = returnShaderContents(vertex);
	std::string fragContents = returnShaderContents(fragment);

	const char* vertToString = vertContents.c_str();
	const char* fragToString = fragContents.c_str();

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertToString, NULL);
	glCompileShader(vs);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragToString, NULL);
	glCompileShader(fs);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, fs);
	glAttachShader(shaderProgram, vs);
	glLinkProgram(shaderProgram);
}
Shader::Shader(std::string fileName, GLenum shaderType)
{
	bool couldLoad = false;
	std::string shaderStr = returnShaderContents(fileName);
	
	
		shaderName = fileName;

		const char* shaderToString = shaderStr.c_str();

		GLuint s = glCreateShader(shaderType);
		glShaderSource(s, 1, &shaderToString, NULL);
		glCompileShader(s);

		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, s);
		//glLinkProgram(shaderProgram);
	
	
	
}



GLuint Shader::getShaderProgram()
{
	return shaderProgram;
}

std::string Shader::getName()
{
	return shaderName;
}




