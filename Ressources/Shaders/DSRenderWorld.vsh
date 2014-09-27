struct VS_INPUT
{
	float3 position 	: POSITION0;
	float2 texcoord0	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position 	: POSITION;
	float2 texcoord0	: TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT;
	OUT.position = float4(IN.position.xyz, 1);
	OUT.texcoord0 = IN.texcoord0;
	return OUT;
}
