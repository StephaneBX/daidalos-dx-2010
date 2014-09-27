struct VS_INPUT
{
	float3 position 	: POSITION0;
	float2 texcoord0	: TEXCOORD0;
	float3 viewDistance	: NORMAL0;
};

struct VS_OUTPUT
{
	float4 position 	: POSITION;
	float2 texcoord0	: TEXCOORD0;
	float3 viewDistance	: TEXCOORD1;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT;
	OUT.position = float4(IN.position.xyz, 1);
	OUT.texcoord0 = IN.texcoord0;
	OUT.viewDistance = IN.viewDistance;
	return OUT;
}
