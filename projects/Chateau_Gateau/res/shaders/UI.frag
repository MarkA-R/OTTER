/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

texturedlit.frag
Fragment shader.
Uses a fixed directional light grey light with only diffuse and ambient lighting.
You'll learn a lot about lighting in graphics - this shader just gives us a simple
way to make sure that everything looks right with our normals, etc.
*/

#version 420 core

uniform float transparency;

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 3) in vec2 inUV;
//layout(location = 3) in float transparency;


layout(location = 0) out vec4 outColor;

uniform vec3 camPos;


uniform vec3 matColor = vec3(1.0f, 1.0f, 1.0f);
uniform sampler2D albedo;

uniform vec3 lightColorX = vec3(0.9f, 0.9f, 0.9f);
uniform vec3 lightDirX = normalize(vec3(-1.0f, -1.0f, -1.0f));
uniform vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f);
uniform float ambientPower = 0.4f;

void main()
{
   const mat4x4 thresholds = mat4x4(1,9,3,11,
   13,5,15,7,
   4,12,12,10,
   16,8,14,6);

   ivec2 coord = ivec2(gl_FragCoord.xy - 0.5);
   float thresh = thresholds[int(mod(coord.x,4))][int(mod(coord.y,4))] / 17;
   if(thresh < transparency){
   discard;
   }

    vec3 norm = normalize(inNorm); 

    vec3 eye = normalize(camPos - inPos.xyz);
    vec3 toLight = -lightDirX;

    vec3 avg = normalize(eye + toLight);

    float diffPower = max(dot(norm, toLight), 0.0f);
    float angle = dot(toLight, norm);
   
    
    vec3 diff =  lightColorX;

    vec3 ambient = ambientPower * ambientColor;

    

    vec4 texCol = texture(albedo, inUV);
    vec3 result = ((ambient + diff)) * matColor * texCol.rgb  *0.77;

    outColor = vec4(result, texCol.a );
   
}