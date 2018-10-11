
float4x4 gWorld : WORLD;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float4x4 gLightViewProj : LIGHTVIEWPROJECTION;
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float4x4 gBones[70];
float shadowMapBias = 0.f;

Texture2D gDiffuseMap;
Texture2D shadowMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};

SamplerComparisonState cmpSampler
{
   // sampler state
    Filter = COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV = MIRROR;
 
   // sampler comparison state
    ComparisonFunc = LESS_EQUAL;
};

RasterizerState Solid
{
	FillMode = SOLID;
	CullMode = FRONT;
};

struct VS_INPUT{
	float3 pos : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
    float4 blendWeight : BLENDWEIGHTS;
    float4 blendIndex : BLENDINDICES;
};

struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
    float4 lpos : TEXCOORD1;
};

DepthStencilState enableDepth
{
    DepthEnable = TRUE;
    
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;

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
	
	//Don't forget to change the output.pos & output.normal variables...
    output.pos = mul(transformedPosition, gWorldViewProj); //Non skinned position
    output.normal = normalize(mul(transformedNormal, (float3x3) gWorld)); //Non skinned normal
    output.lpos = mul(transformedPosition, mul(gWorld, gLightViewProj));

	output.texCoord = input.texCoord;
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float2 texOffset(int u, int v)
{
    uint miplevel = 0;
    uint width, height, levels;

    shadowMap.GetDimensions(miplevel, width, height, levels);
    return float2(u * 1.0f / float(width), v * 1.0f / float(height));
}

float4 PS(VS_OUTPUT input) : SV_TARGET{

	float4 diffuseColor = gDiffuseMap.Sample( samLinear,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	float color_a = diffuseColor.a;

     //re-homogenize position after interpolation
    input.lpos.xyz /= input.lpos.w;
 
    //if position is not visible to the light - dont illuminate it
    //results in hard light frustum
    if (input.lpos.x < -1.0f || input.lpos.x > 1.0f ||
        input.lpos.y < -1.0f || input.lpos.y > 1.0f ||
        input.lpos.z < 0.0f || input.lpos.z > 1.0f)
        return float4(0, 0, 0, 0);
 
    //transform clip space coords to texture space coords (-1:1 to 0:1)
    input.lpos.x = input.lpos.x / 2.0f + 0.5;
    input.lpos.y = input.lpos.y / -2.0f + 0.5;

    input.lpos.z -= shadowMapBias;

    //PCF sampling for shadow map
    float sum = 0;
    float x, y;

    //perform PCF filtering on a 4 x 4 texel neighborhood
    for (y = -1.5; y <= 1.5; y += 1)
    {
        for (x = -1.5; x <= 1.5; x += 1)
        {
            sum += shadowMap.SampleCmpLevelZero(cmpSampler, input.lpos.xy + texOffset(x, y), input.lpos.z);
        }
    }
 
    float shadowFactor = sum / 16.0;
	
	//HalfLambert Diffuse :)
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	color_rgb = color_rgb * diffuseStrength;

    shadowFactor = clamp(shadowFactor, 0.5f, 1.0f);

	//return float4( color_rgb , color_a );
    return float4(color_rgb * shadowFactor, color_a);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
        SetDepthStencilState(enableDepth, 0);
		SetRasterizerState(Solid);
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

