#version 330 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_color;

out vec3 our_color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

<<<<<<< HEAD

void main(){

	gl_Position = projection * view * model *  vec4(in_position, 1.0);
	our_color = in_color;

}
=======
void main(){

	gl_Position = projection * view * model * vec4(in_position, 1.0);
	our_color = in_color;

}

>>>>>>> e24aebae62b3431ea597760cd924bfc50197c85c
