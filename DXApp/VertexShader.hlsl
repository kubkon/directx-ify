struct VOut
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
};

VOut main( float4 pos : POSITION, float4 col : COLOR ) 
{
	VOut output;
	output.position = pos;
	output.colour = col;
	return output;
}