uniform float4x4 viewMatrix;
uniform float4x4 viewProjMatrix;

struct VS_INPUT
{
	float4 position  	: POSITION;
	float2 texcoord0	: TEXCOORD0;
	float3 normal		: NORMAL0;
	float3 tangent		: NORMAL1;
	float3 bitangent	: NORMAL2;
	float4 color		: COLOR0;
};

struct VS_OUTPUT
{
	float4 position 	: POSITION;
	float2 texcoord0	: TEXCOORD0;
	float3 normal		: TEXCOORD1;
	float3 tangent		: TEXCOORD2;
	float3 bitangent	: TEXCOORD3;
	float  depth		: TEXCOORD4;
	float4 color	 	: COLOR0;
};

VS_OUTPUT main( VS_INPUT IN )
{
	VS_OUTPUT OUT;

	float4 v = float4( IN.position.xyz, 1.0f );

    OUT.position = mul( v, viewProjMatrix );
	OUT.texcoord0 = IN.texcoord0;
    OUT.color = IN.color;
	OUT.normal = IN.normal;
	OUT.tangent = IN.tangent;
	OUT.bitangent = IN.bitangent;
	OUT.depth = mul( v, viewMatrix ).z;
	
    return OUT;
}
