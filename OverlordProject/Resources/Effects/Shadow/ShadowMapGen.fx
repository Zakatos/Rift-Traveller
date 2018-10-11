float4x4 gWorld : WORLD;
float4x4 gLightViewProj : LIGHTVIEWPROJECTION;

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
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

    output.pos = mul(float4(input.pos, 1), mul(gWorld, gLightViewProj));
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

