#version 410 core

in vec3 v_vertexColors;

out vec4 color;

uniform float u_Offset;

void main()
{
   color = vec4(v_vertexColors.r - u_Offset,
		v_vertexColors.g - u_Offset,
		v_vertexColors.b - u_Offset,
		1.0f);
}