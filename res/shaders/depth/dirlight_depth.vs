#version 330

layout (location = 0) in vec3 aPos;
layout (location = 7) in vec3 aOffset;
layout (location = 8) in vec3 aSize;

layout (location = 9) in vec4 aRotation_1;
layout (location = 10) in vec4 aRotation_2;
layout (location = 11) in vec4 aRotation_3;
layout (location = 12) in vec4 aRotation_4;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main()
{
	mat4 rotation = mat4(aRotation_1, aRotation_2, aRotation_3, aRotation_4);
	vec3 rotatedPos = vec3(rotation * vec4(aPos * aSize, 1.0));
	gl_Position = lightSpaceMatrix * vec4(rotatedPos + aOffset, 1.0);
}