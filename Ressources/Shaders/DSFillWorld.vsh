#define MAX_SHADOW_SOURCE 4
uniform float4x4 viewMatrix;
uniform float4x4 viewProjMatrix;
uniform float4x4 lightViewProj[MAX_SHADOW_SOURCE];
uniform int shadowCaster[MAX_SHADOW_SOURCE];

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
	float4 posToLight0 	: TEXCOORD5;
	float4 posToLight1 	: TEXCOORD6;
	float4 posToLight2 	: TEXCOORD7;
	float4 posToLight3 	: TEXCOORD8;
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
	
	if(shadowCaster[0] > 0)
		OUT.posToLight0 = mul(v, lightViewProj[0]);
	else
		OUT.posToLight0 = float4(0,0,0,0);
	
	if(shadowCaster[1] > 0)
		OUT.posToLight1 = mul(v, lightViewProj[1]);
	else
		OUT.posToLight1 = float4(0,0,0,0);
		
	if(shadowCaster[2] > 0)
		OUT.posToLight2 = mul(v, lightViewProj[2]);
	else
		OUT.posToLight2 = float4(0,0,0,0);
		
	if(shadowCaster[3] > 0)
		OUT.posToLight3 = mul(v, lightViewProj[3]);
	else
		OUT.posToLight3 = float4(0,0,0,0);
		
    return OUT;
}
