#ifndef _Shader_H
#define _Shader_H

#include <string>
#include <fstream>
#include <GL/glew.h>
#include <vector>

class Shader {
public:

	Shader() {
	}

	~Shader() {
		destroy();
	}

	std::string loadShaderFile(std::string strFile);

	void initialize(std::string strVertexFile, std::string strFragmentFile);

	GLint getUniformLocation(std::string strVariable);

	void setMatrix4(GLint id, GLsizei count, GLboolean transpose,
			const GLfloat *value) {
		glUniformMatrix4fv(id, count, transpose, value);
	}
	//Este metodo sirve para activar los shaders que dessemos utilizar
	void turnOn() {
		glUseProgram(ShaderProgramId);
	}
	//Este desactiva el shader
	void turnOff() {
		glUseProgram(0);
	}

	void destroy();

private:

	GLuint VertexShaderId;
	GLuint FragmentShaderId;
	GLuint ShaderProgramId;
};

#endif
