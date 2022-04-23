#h

#version 450 core


#vs

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 MVP;

out vec2 UV;


void main()
{                                                   
  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);    
  //fragmentColor = vertexColor;
  UV = vertexUV;
}


#fs

//in vec2 uv;
in vec2 UV;

out vec3 color;  

uniform sampler2D textureSampler;

void main()
{                           
  //color = fragmentColor;  
  color = texture(textureSampler, UV).rgb;
}
