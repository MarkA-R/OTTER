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

uniform vec3 lightDir2;// = normalize(vec3(-0.0f, -0.0f, -1.0f));
uniform vec3 lightColor2;// = vec3(0.9f, 0.0f, 0.0f);
uniform vec3 lightPos2;
uniform float strength;

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 3) in vec2 inUV;
layout(location = 4) in mat3 TBN;
//layout(location = 3) in float transparency;


layout(location = 0) out vec4 outColor;

uniform vec3 camPos;

uniform vec3 matColor = vec3(1.0f, 1.0f, 1.0f);
layout(binding = 0)uniform sampler2D albedo;
//layout(binding = 1)uniform sampler2D Nmap;
uniform sampler2D ramp;
uniform sampler2D LUT1;
uniform sampler2D LUT2;
uniform sampler2D LUT3;


uniform vec3 lightColor = vec3(0.9f, 0.9f, 0.9f);
uniform vec3 lightDir = normalize(vec3(-1.0f, -1.0f, -1.0f));
uniform vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f);
uniform float ambientPower = 0.4f;

uniform int mode;
uniform int diffuseRamp;
uniform int specularRamp;
uniform int usingLUT1;
uniform int usingLUT2;
uniform int usingLUT3;

void main()
{
    //vec3 lightDir = normalize(vec3(0f, -1.0f, 1.0f));
    //normalize(vec3(-1.0f, -1.0f, -1.0f));
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

    /*
     if(texture(Nmap,inUV).r > 0.001){
     vec3 newN = texture(Nmap,inUV).rgb;
     newN = newN * 2.0 - 1.0;
     norm = normalize(TBN * norm);
   
     }
     */
    vec3 eye = normalize(camPos - inPos.xyz);
    vec3 toLight = -lightDir;
    vec3 toLight2 = -lightDir2;

    vec3 avg = normalize(eye + toLight);

    float diffPower = max(dot(norm, toLight), 0.0f);//value from 0-1
     float diffPower2 = max(dot(norm, toLight2), 0.0f);
    float angle = dot(toLight, norm);

    
    if(mode == 5){//if in toon shading mode
     float first = 0.20f;
    float second = 0.4f;
    if(diffPower < first){
    diffPower = 0.20;
    }
    if(diffPower >= first && diffPower <= second){
    diffPower = 0.4;
    }
    if(diffPower > second){
    diffPower = 0.6;
    }

    if(diffPower2 < 0.6){
    //diffPower2 = 0.0;
    }
    if(diffPower2 >= 0.6){
    diffPower2 = 0.6;
    }
    }
    if(diffuseRamp == 1){
    diffPower = texture(ramp,vec2(diffPower,0.5)).r;
    }
   
   
    vec3 diff = diffPower * lightColor;
     vec3 diff2 = (diffPower2* strength) * lightColor2;

    vec3 ambient = ambientPower * ambientColor;
     vec4 texCol = texture(albedo, inUV);

    vec3 camDir = normalize(camPos - inPos.xyz);
	vec3 reflectedRay = reflect(-lightDir, norm); // light direction to the point
	float spec = pow(max(dot(camDir, reflectedRay), 0.0), 128); // shininess coeficient




    if(specularRamp == 1){
    spec = texture(ramp,vec2(spec,0.5)).r;
    }

    //get the amount of blue in the texture colour
	 float blueAmount = texCol.b * 63;

     vec2 quad1;//find the closest blue tile thats <= the blue value in the LUT
    quad1.y = floor(floor(blueAmount) / 8.0);
    quad1.x = floor(blueAmount) - (quad1.y * 8.0);

    vec2 quad2;//find the closest blue tile thats >= the blue value in the LUT
    quad2.y = floor(ceil(blueAmount) / 8.0);
    quad2.x = ceil(blueAmount) - (quad2.y * 8.0);

	//find the R and G values within the blue squares
     vec2 texPos1;
    texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * texCol.r);
    texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * texCol.g);

   

     vec2 texPos2;
    texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * texCol.r);
    texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * texCol.g);
   
   

   //make variables for the 2 colours to mix together
   //defaulted to the texture colour in case no LUT is applied
   vec4 newColour1 = texCol;
   vec4 newColour2 = texCol;

   float str = 1.0;
   if(usingLUT1 == 1){//if using first LUT, sample using LUT1
    newColour1 = texture2D(LUT1, texPos1);
     newColour2 = texture2D(LUT1, texPos2);
	 str = 0.3;
   }
   else if(usingLUT2 == 1){//if using second LUT, sample using LUT2
    newColour1 = texture2D(LUT2, texPos1);
     newColour2 = texture2D(LUT2, texPos2);
	 str = 0.3;
   }
    else if(usingLUT3 == 1){//if using third LUT, sample using LUT3
    newColour1 = texture2D(LUT3, texPos1);
     newColour2 = texture2D(LUT3, texPos2);
	 str = 1.0;
   }
    
	//mix the two colours
     vec4 LUTCol = mix(texCol, newColour2, str);

   vec3 specular = spec * lightColor;
   vec4 lighting = vec4(ambient + diff + diff2 + specular, 1.0);

	if(mode == 1){//if in no lighting mode, replace the calculated light with no light
	lighting = vec4(1,1,1,1);
	}
	if(mode == 2){//if in ambient lighting mode, replace the calculated light with the ambient only
		lighting = vec4(ambient,1);
	}
	if(mode == 3){//if in specular lighting mode, replace the calculated light with the specular only
		lighting = vec4(specular,1);
	}
	if(mode == 4){//if in ambient + specular lighting mode, replace the calculated light with ambient + specular
		lighting = vec4(ambient + specular,1);
	}


    //vec3 result = ((ambient + diff + diff2 + spec)) * matColor * LUTCol.rgb;

   // outColor = vec4(result, texCol.a);

   outColor = LUTCol * lighting;
   
}