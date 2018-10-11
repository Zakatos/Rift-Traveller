float4x4 gWorld : WORLD;
float4x4 gLightViewProj : LIGHTVIEWPROJECTION;
float4x4 gBones[70];

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
    float4 blendWeight : BLENDWEIGHTS;
    float4 blendIndex : BLENDINDICES;
};
struct SHADOW_VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = BACK;
};

//--------------------------------------------------------------------------------------
// Vertex Shaders
//--------------------------------------------------------------------------------------

SHADOW_VS_OUTPUT ShadowMapVS( VS_INPUT input )
{
    SHADOW_VS_OUTPUT output;
    float4 originalPosition = float4(input.pos, 1);
    float4 transformedPosition = 0;
    float3 transformedNormal = 0;

	//Skinning Magic...
    for (int i = 0; i < 4; ++i)
    {
        float boneIndex = input.blendIndex[i];
        if (boneIndex > -1)
        {
            transformedPosition += mul(float4(input.pos, 1.0) * input.blendWeight[i], (float4x4) gBones[boneIndex]);
            transformedNormal += mul(input.normal * input.blendWeight[i], (float3x3) gBones[boneIndex]);
            transformedPosition.w = 1;

        }
    }

    output.pos = mul(float4(transformedPosition), mul(gWorld, gLightViewProj));
    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shaders
//--------------------------------------------------------------------------------------

void ShadowMapPS(SHADOW_VS_OUTPUT input)
{
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 shadow
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

        SetVertexShader(CompileShader(vs_4_0, ShadowMapVS()));
		SetGeometryShader( NULL );
        SetPixelShader(CompileShader(ps_4_0, ShadowMapPS()));
    }
}

