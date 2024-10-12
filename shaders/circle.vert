#version 430 core
layout (location = 0) in vec3 aPos;
out vec3 outColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main(){
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    gl_PointSize = 100.0;
    outColor = vec3(0.0, 0.64, 1.0);
}