#version 410 core

in vec3 v_vertexColors;
in vec2 v_texCoord;

//uniform mat4 u_ModelMatrix;
//uniform float u_Time;
uniform sampler2D uTexture;

out vec4 color;

void main()
{
	//float stx = gl_FragCoord.x/1024.0f;
	//float sty = gl_FragCoord.y/768.0f;
	//float stz = gl_FragCoord.z/20.0f;

	//color = vec4(v_vertexColors.r, v_vertexColors.g, v_vertexColors.b, 1.0f);
	color = texture(uTexture, v_texCoord);

	//stz*v_vertexColors.r,stz*v_vertexColors.g,
	//0.5f+0.5f*cos(u_Time/40.0f + 40*sqrt(pow(stx,2) + pow(sty,2)))*v_vertexColors.r,
	//0.5f+0.5f*cos(u_Time/40.0f + 40*sqrt(pow(stx,2) + pow(sty,2)))*v_vertexColors.g,
	//0.5f+0.5f*cos(-u_Time/40.0f + 40*sqrt(pow(stx,2) + pow(sty,2)))*v_vertexColors.b,
	//1.0f);
	//0.5f+0.5f*sin(u_Time/10.0f)*v_vertexColors.b,
}