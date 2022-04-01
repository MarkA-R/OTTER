/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

texturedlit.vert
Vertex shader.
Passes world vertex position, transformed normal direction, and UV coordinates
to the fragment shader.
*/

#version 420 core




layout(location = 0) in vec4 inPos;

layout(location = 1) in vec2 inUV;


out vec2 outUV;




void main()
{
   gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    outUV = inUV;
}