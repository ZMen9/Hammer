// Basic Texture Shader
#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;   
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TextIdx;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;
layout(std140, binding = 0) uniform Camera
{
	mat4 u_View_Projection;
};
struct VertexOutput
{
	vec4 Color;
	vec2 TexCoord;
	float TilingFactor;
};
layout(location = 0) out VertexOutput Output;
layout(location = 3) out flat float v_TexIndex;
layout(location = 4) out flat int v_EntityID;
void main()
{	
	Output.Color = a_Color;
	Output.TexCoord = a_TexCoord;
	v_TexIndex = a_TextIdx;
	Output.TilingFactor = a_TilingFactor;
	v_EntityID = a_EntityID;
	gl_Position = u_View_Projection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 color;
layout(location = 1) out int entity_id;

struct VertexOutput
{
	vec4 Color;
	vec2 TexCood;
	float TilingFactor;
};
layout(location = 0) in VertexOutput Input;
layout(location = 3) in flat float v_TexIdx;
layout(location = 4) in flat int v_EntityId;

layout(binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	// color = texture(u_Textures[(int)v_TextIdx], Input.TexCood * Input.TilingFactor) * v_Color;
	// this does not work on all gpu or drivers
	vec4 texColor = Input.Color;
	switch(int(v_TexIdx))
	{
		case 0: texColor *= texture(u_Textures[0], Input.TexCood * Input.TilingFactor); break;
		case 1: texColor *= texture(u_Textures[1], Input.TexCood * Input.TilingFactor); break;
		case 2: texColor *= texture(u_Textures[2], Input.TexCood * Input.TilingFactor); break;
		case 3: texColor *= texture(u_Textures[3], Input.TexCood * Input.TilingFactor); break;
		case 5: texColor *= texture(u_Textures[5], Input.TexCood * Input.TilingFactor); break;
		case 4: texColor *= texture(u_Textures[5], Input.TexCood * Input.TilingFactor); break;
		case 6: texColor *= texture(u_Textures[6], Input.TexCood * Input.TilingFactor); break;
		case 7: texColor *= texture(u_Textures[7], Input.TexCood * Input.TilingFactor); break;
		case 8: texColor *= texture(u_Textures[8], Input.TexCood * Input.TilingFactor); break;
		case 9: texColor *= texture(u_Textures[9], Input.TexCood * Input.TilingFactor); break;
		case 10: texColor *= texture(u_Textures[10], Input.TexCood * Input.TilingFactor); break;
		case 11: texColor *= texture(u_Textures[11], Input.TexCood * Input.TilingFactor); break;
		case 12: texColor *= texture(u_Textures[12], Input.TexCood * Input.TilingFactor); break;
		case 13: texColor *= texture(u_Textures[13], Input.TexCood * Input.TilingFactor); break;
		case 14: texColor *= texture(u_Textures[14], Input.TexCood * Input.TilingFactor); break;
		case 15: texColor *= texture(u_Textures[15], Input.TexCood * Input.TilingFactor); break;
		case 16: texColor *= texture(u_Textures[16], Input.TexCood * Input.TilingFactor); break;
		case 17: texColor *= texture(u_Textures[17], Input.TexCood * Input.TilingFactor); break;
		case 18: texColor *= texture(u_Textures[18], Input.TexCood * Input.TilingFactor); break;
		case 19: texColor *= texture(u_Textures[19], Input.TexCood * Input.TilingFactor); break;
		case 20: texColor *= texture(u_Textures[20], Input.TexCood * Input.TilingFactor); break;
		case 21: texColor *= texture(u_Textures[21], Input.TexCood * Input.TilingFactor); break;
		case 22: texColor *= texture(u_Textures[22], Input.TexCood * Input.TilingFactor); break;
		case 23: texColor *= texture(u_Textures[23], Input.TexCood * Input.TilingFactor); break;
		case 24: texColor *= texture(u_Textures[24], Input.TexCood * Input.TilingFactor); break;
		case 25: texColor *= texture(u_Textures[25], Input.TexCood * Input.TilingFactor); break;
		case 26: texColor *= texture(u_Textures[26], Input.TexCood * Input.TilingFactor); break;
		case 27: texColor *= texture(u_Textures[27], Input.TexCood * Input.TilingFactor); break;
		case 28: texColor *= texture(u_Textures[28], Input.TexCood * Input.TilingFactor); break;
		case 29: texColor *= texture(u_Textures[29], Input.TexCood * Input.TilingFactor); break;
		case 30: texColor *= texture(u_Textures[30], Input.TexCood * Input.TilingFactor); break;
		case 31: texColor *= texture(u_Textures[31], Input.TexCood * Input.TilingFactor); break;
	}
	color = texColor;
	entity_id = v_EntityId;
}