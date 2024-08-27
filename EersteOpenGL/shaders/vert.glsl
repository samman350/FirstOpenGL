#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 vertexColors;

uniform mat4 u_ModelMatrix;
uniform mat4 u_View;
uniform mat4 u_Projection;
//uniform float u_Time;

out vec3 v_vertexColors;

void main()
{
	v_vertexColors = vertexColors;

	//vec4 newPos = u_Projection * u_View * u_ModelMatrix * vec4((2.f+0.2f*cos(u_Time/300.0f))*position.x,
	//				(2.f + 0.1*sin(u_Time/300.0f))*position.y,
	//				position.z,1.0f);
	vec4 newPos = u_Projection * u_View * u_ModelMatrix * vec4(2.0f*position.x,2.0f*position.y,2.0f*position.z,1.0f);
	
    gl_Position = newPos;
}