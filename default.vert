#version 330 core

layout (location = 0) in vec3 aPos;

out vec2 UV;

void main(){
	gl_Position = vec4(aPos, 1.0);
	UV = (aPos.xy + 1.0) * 0.5;
}