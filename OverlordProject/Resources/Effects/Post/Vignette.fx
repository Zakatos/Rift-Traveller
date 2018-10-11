
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

Texture2D gTexture;

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
    float4 color = float4(1, 1, 1, 1);
    float4 initColor = gTexture.Sample(samPoint, input.TexCoord);
    float2 distance = input.TexCoord - 0.5f;//0.5 being the middle
    distance = abs(distance)*1.2f;//radius multiplier
    distance.x = dot(distance, distance);
    color.a = distance;
    initColor.a = 1 + color.a;
    return initColor - float4((color.xyz * color.a), 0.0f);
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