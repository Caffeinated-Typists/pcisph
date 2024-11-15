#version 430 core
layout (location = 0) in vec2 aPos;
uniform mat4 projection;
out vec3 outColor;

void main(){
    gl_Position = projection * vec4(aPos, 0.0, 1.0);
    // gl_Position = vec4(aPos, 0.0, 1.0);
    gl_PointSize = 100.0;
    outColor = vec3(0.0, 0.64, 1.0);
}