#h

#version 330 core


#vs

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

uniform mat4 MVP;

out vec3 fragmentColor;

//out vec2 uv;

void main()
{                                                   
  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);    
  fragmentColor = vertexColor;
  //uv = vertexUV;
}


#fs

//in vec2 uv;
in vec3 fragmentColor;

out vec3 color;  

//uniform sampler2D textureSampler;

void main()
{                           
  color = fragmentColor;  
  //color = texture(textureSampler, uv).rgb;
}
