cbuffer ConstantBuffer
{
	float4x4 transform;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

VOut main( float4 pos : POSITION, float4 col : COLOR ) 
{
	VOut output;

	output.position = mul(transform, pos);
	output.colour = col;

	return output;
}