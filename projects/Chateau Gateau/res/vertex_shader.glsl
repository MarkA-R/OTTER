#version 410
layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec3 inColor;

//lec5

layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;


layout(location = 0) out vec3 outPos;
layout(location = 3) out vec3 outColor;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;


//Lecture 4
uniform mat4 MVP;

//lec 5

uniform mat4 Model;//same names as in c++



void main() {
	// vertex position in clip space
	gl_Position = MVP * vec4(inPosition, 1.0);

	//l5

	outPos = (Model * vec4(inPosition,1.0)).xyz;
	outNormal = (Model * vec4(inNormal,1)).xyz;


	outColor = inColor;
}


	