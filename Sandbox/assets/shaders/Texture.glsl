// Basic Texture Shader
#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;   
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TextIdx;
layout(location = 4) in float a_TilingFactor;

out vec4 v_Color;
out vec2 v_textcoord;
out flat float v_TextIdx;
out float v_TilingFactor;

uniform mat4 u_ViewProjection;


void main()
{	
	v_Color = a_Color;
	v_textcoord = a_TexCoord;
	v_TextIdx = a_TextIdx;
	v_TilingFactor = a_TilingFactor;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_textcoord;
in flat float v_TextIdx;
in float v_TilingFactor;

uniform sampler2D u_Textures[32];

void main()
{
	vec4 texColor = v_Color;
	switch(int(v_TextIdx))
	{
		case 0: texColor *= texture(u_Textures[0], v_textcoord * v_TilingFactor); break;
		case 1: texColor *= texture(u_Textures[1], v_textcoord * v_TilingFactor); break;
		case 2: texColor *= texture(u_Textures[2], v_textcoord * v_TilingFactor); break;
		case 3: texColor *= texture(u_Textures[3], v_textcoord * v_TilingFactor); break;
		case 5: texColor *= texture(u_Textures[5], v_textcoord * v_TilingFactor); break;
		case 4: texColor *= texture(u_Textures[5], v_textcoord * v_TilingFactor); break;
		case 6: texColor *= texture(u_Textures[6], v_textcoord * v_TilingFactor); break;
		case 7: texColor *= texture(u_Textures[7], v_textcoord * v_TilingFactor); break;
		case 8: texColor *= texture(u_Textures[8], v_textcoord * v_TilingFactor); break;
		case 9: texColor *= texture(u_Textures[9], v_textcoord * v_TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], v_textcoord * v_TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], v_textcoord * v_TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], v_textcoord * v_TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], v_textcoord * v_TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], v_textcoord * v_TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], v_textcoord * v_TilingFactor); break;
		case 16: texColor *= texture(u_Textures[16], v_textcoord * v_TilingFactor); break;
		case 17: texColor *= texture(u_Textures[17], v_textcoord * v_TilingFactor); break;
		case 18: texColor *= texture(u_Textures[18], v_textcoord * v_TilingFactor); break;
		case 19: texColor *= texture(u_Textures[19], v_textcoord * v_TilingFactor); break;
		case 20: texColor *= texture(u_Textures[20], v_textcoord * v_TilingFactor); break;
		case 21: texColor *= texture(u_Textures[21], v_textcoord * v_TilingFactor); break;
		case 22: texColor *= texture(u_Textures[22], v_textcoord * v_TilingFactor); break;
		case 23: texColor *= texture(u_Textures[23], v_textcoord * v_TilingFactor); break;
		case 24: texColor *= texture(u_Textures[24], v_textcoord * v_TilingFactor); break;
		case 25: texColor *= texture(u_Textures[25], v_textcoord * v_TilingFactor); break;
		case 26: texColor *= texture(u_Textures[26], v_textcoord * v_TilingFactor); break;
		case 27: texColor *= texture(u_Textures[27], v_textcoord * v_TilingFactor); break;
		case 28: texColor *= texture(u_Textures[28], v_textcoord * v_TilingFactor); break;
		case 29: texColor *= texture(u_Textures[29], v_textcoord * v_TilingFactor); break;
		case 30: texColor *= texture(u_Textures[30], v_textcoord * v_TilingFactor); break;
		case 31: texColor *= texture(u_Textures[31], v_textcoord * v_TilingFactor); break;
	}
	color = texColor;
	// color = texture(u_Textures[(int)v_TextIdx], v_textcoord * v_TilingFactor) * v_Color;
	// this does not work on all gpu or drivers
}