#h

#version 330 core


#vs

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

uniform mat4 MVP;

out vec3 fragmentColor;

void main()
{                                                   
  gl_Position = MVP * vec4(vertexPosition_modelspace, 1);    
  fragmentColor = vertexColor;
}


#fs

in vec3 fragmentColor;
out vec3 color;  

void main()
{                           
  color = fragmentColor;  
 // color = vec3(0,0,1);                          
}
