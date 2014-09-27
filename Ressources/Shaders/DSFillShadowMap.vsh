uniform float4x4 viewProj;

struct VS_INPUT
{
	float3 position : POSITION0;
};

struct VS_OUTPUT
{
	float4 position : POSITION;
	float2 depth	: TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT;
	float4 v = float4( IN.position.xyz, 1.0f );
    OUT.position = mul( v, viewProj );
	OUT.depth = OUT.position.zw;
	return OUT;
}
