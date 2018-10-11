
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;
Texture2D gNoiseTexture;

float3 gDOffset = float3(-0.002f,0.005f,0.01f);
float gTime = 0.0f;
float gDFrequency = 1.0f;

DepthStencilState enableDepth
{
    DepthEnable = TRUE;
};

RasterizerState BackCulling
{
    CullMode = BACK;
};

BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

//IN/OUT STRUCTS
//--------------
struct VS_INPUT
{
    float3 Position : POSITION;
	float2 TexCoord : TEXCOORD0;

};

struct PS_INPUT
{
    float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD1;
};


//VERTEX SHADER
//-------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
    output.Position = float4(input.Position, 1);
    output.TexCoord = input.TexCoord;
	return output;
}


//PIXEL SHADER
//------------
float4 PS(PS_INPUT input): SV_Target
{
    float distortion = gNoiseTexture.Sample(samPoint, float2((input.TexCoord.y + gTime) * gDFrequency, 0.5)).r;
    float3 offset = distortion * gDOffset;

    float3 color = float3(
    gTexture.Sample(samPoint, float2(input.TexCoord.x + offset.r, input.TexCoord.y)).r,
    gTexture.Sample(samPoint, float2(input.TexCoord.x + offset.g, input.TexCoord.y)).g,
    gTexture.Sample(samPoint, float2(input.TexCoord.x + offset.b, input.TexCoord.y)).b
);

    return float4( color, 1.0f);
}


//TECHNIQUE
//---------
technique11 Vignette
{
    pass P0
    {
        SetDepthStencilState(enableDepth, 0);
        SetBlendState(EnableBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(BackCulling);
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}