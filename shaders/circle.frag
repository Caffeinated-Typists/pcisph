#version 430 core
in vec3 outColor;
out vec4 fColor;

void main(){
	fColor = vec4(outColor, 1.0);
}