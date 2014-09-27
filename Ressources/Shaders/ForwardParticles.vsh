#define PI 3.14159265f
#define MAX_LIGHTS 6
#define MAX_SHADOW_SOURCE 4

uniform int spritesPerLine;
uniform float4x4 viewProj;
uniform float4x4 billboard;
uniform float4x4 lightViewProj[MAX_LIGHTS];
uniform int lightShadowCasterIndexes[MAX_SHADOW_SOURCE];

struct VS_INPUT
{
	float3 position 				: POSITION0; // Vertex position (non translated)
	float2 texcoord 				: TEXCOORD0; // Texture coordinate (non translated)
	float3 instPosition 			: POSITION1; // Translation factor
	float4 color    				: COLOR0; // Color
	float4 rotationSizeLifeTexture	: POSITION2; // Rotation value, Size modification, Lifetime [0 - 1] and textureId
};

struct VS_OUTPUT
{
	float4 position 		: POSITION; // Screen space coordinate
	float2 texcoord 		: TEXCOORD0; // Texture coordinate
	float3 normal			: TEXCOORD1;
	float3 tangent			: TEXCOORD2;
	float3 bitangent		: TEXCOORD3;
	float4 pos		  		: TEXCOORD4;
	//float4 posToLight0 		: TEXCOORD5;
	//float4 posToLight1 		: TEXCOORD6;
	//float4 posToLight2 		: TEXCOORD7;
	//float4 posToLight3 		: TEXCOORD8;
	float4 color	  		: COLOR0; // Color
	float3 sinCosLife		: COLOR1; // Sin, Cos, Life
};

VS_OUTPUT main(VS_INPUT IN)
{
	VS_OUTPUT OUT;
	
	// Position data
	float4 v = float4( IN.position.x,
		               IN.position.y,
					   IN.position.z,
					   1.0f );
					   
	// Rotation data
	float4x4 rotMat = float4x4(	cos(IN.rotationSizeLifeTexture.x * PI / 180.0f),  sin(IN.rotationSizeLifeTexture.x * PI / 180.0f),0,0,
								-sin(IN.rotationSizeLifeTexture.x * PI / 180.0f), cos(IN.rotationSizeLifeTexture.x * PI / 180.0f),0,0,
								0,0,1,0,
								0,0,0,1);
	
	// Modify the position by assigning the rotation
	v = mul( v, rotMat );
	
	// Assign the size
    v.xy *= IN.rotationSizeLifeTexture.y;
	
	// Billboard
	v = mul( v, billboard );
	
	// Translation
	v.xyz += IN.instPosition.xyz;
	
	OUT.position = mul(v, viewProj);
	
	// Texture coordinate translation
	OUT.texcoord.x = IN.texcoord.x + IN.texcoord.x * (IN.rotationSizeLifeTexture.w - ((IN.rotationSizeLifeTexture.w % (int)spritesPerLine) * spritesPerLine));
	OUT.texcoord.y = IN.texcoord.y + IN.texcoord.y * (IN.rotationSizeLifeTexture.w % (int)spritesPerLine);
	
	OUT.normal = mul(float4(0,0,-1,1), billboard);
	OUT.tangent = mul(float4(1,0,0,1), billboard);
	OUT.bitangent = mul(float4(0,1,0,1), billboard);
	OUT.pos = v;
	
	/*if(lightShadowCasterIndexes[0] >= 0)
		OUT.posToLight0 = mul(v, lightViewProj[lightShadowCasterIndexes[0]]);
	else
		OUT.posToLight0 = float4(0,0,0,0);
	
	if(lightShadowCasterIndexes[1] >= 0)
		OUT.posToLight1 = mul(v, lightViewProj[lightShadowCasterIndexes[1]]);
	else
		OUT.posToLight1 = float4(0,0,0,0);
		
	if(lightShadowCasterIndexes[2] >= 0)
		OUT.posToLight2 = mul(v, lightViewProj[lightShadowCasterIndexes[2]]);
	else
		OUT.posToLight2 = float4(0,0,0,0);
		
	if(lightShadowCasterIndexes[3] >= 0)
		OUT.posToLight3 = mul(v, lightViewProj[lightShadowCasterIndexes[3]]);
	else
		OUT.posToLight3 = float4(0,0,0,0);*/
	
	OUT.color = IN.color;
	
	// Sin, Cos, Life
	float fRot = IN.rotationSizeLifeTexture.x * PI / 180.0f;
    OUT.sinCosLife.x = sin(fRot);
    OUT.sinCosLife.y = cos(fRot);
    OUT.sinCosLife.z = IN.rotationSizeLifeTexture.z;
	
	return OUT;
}
