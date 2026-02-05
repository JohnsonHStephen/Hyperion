#version 450

layout(push_constant) uniform Push {
	mat4 modelMatrix;
	mat4 normalMatrix;
} push;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColour;

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = push.modelMatrix * vec4(inPosition, 0.0, 1.0);
	fragColor = inColour;
}