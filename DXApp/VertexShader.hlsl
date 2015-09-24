cbuffer ConstantBuffer
{
	float redLevel;
	float blueLevel;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

VOut main( float4 pos : POSITION, float4 col : COLOR ) 
{
	VOut output;
	output.position = pos;
	output.position.xy *= 0.5f;
	output.colour = col;
	output.colour.r *= redLevel;
	output.colour.b *= blueLevel;
	return output;
}