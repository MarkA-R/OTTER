/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

texturedlit.vert
Vertex shader.
Passes world vertex position, transformed normal direction, and UV coordinates
to the fragment shader.
*/

#version 420 core

uniform float inTrans;

uniform mat4 model;
uniform mat3 normal;
uniform mat4 viewproj;


layout(location = 0) in vec4 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec2 inUV;
layout(location = 5) in vec3 inTangent;
layout(location = 6) in vec3 inBitangent;


layout(location = 0) out vec4 outPos;
layout(location = 1) out vec3 outNorm;
layout(location = 3) out vec2 outUV;
layout(location = 4) out mat3 TBN;
layout(location = 2) out float transparency;

void main()
{
    transparency = inTrans;
    outNorm = normal * inNorm;
    outPos = model * inPos;
    outUV = inUV;
    TBN = mat3(1,0,0,0,1,0,0,0,1);
    if(inTangent.x + inTangent.y + inTangent.z > 0.001){
   
    vec3 T = normalize(vec3(model * vec4(inTangent,   1.0)));
   vec3 B = normalize(vec3(model * vec4(inBitangent, 1.0)));
   vec3 N = normalize(vec3(model * vec4(inNorm,    1.0)));
    TBN = mat3(T, B, N) * -1;
    
    }
   
    gl_Position = viewproj * outPos;
}