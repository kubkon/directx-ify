cbuffer ConstantBuffer
{
	float3 offset;
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
	output.position.x += offset.x;
	output.position.y += offset.y;
	output.position.xy += offset.z;
	output.colour = col;
	return output;
}