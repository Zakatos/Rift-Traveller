float4x4 gWorld : WORLD;
float4x4 gLightViewProj : LIGHTVIEWPROJECTION;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION; 
float3 gLightDirection = float3(-0.577f, -0.577f, 0.577f);
float shadowMapBias = 0.0001f;
//To do fix peter panning
Texture2D gDiffuseMap;
Texture2D shadowMap;

SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;// or Mirror or Clamp or Border
    AddressV = Wrap;// or Mirror or Clamp or Border
};
SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
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
};
struct VS_OUTPUT{
	float4 pos : SV_POSITION;
	float4 lpos : TEXCOORD1;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};

DepthStencilState EnableDepth
{
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
	CullMode = NONE;
};

//--------------------------------------------------------------------------------------
// Vertex Shaders
//--------------------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input){

	VS_OUTPUT output;
	// Step 1:	convert position into float4 and multiply with matWorldViewProj
	output.pos = mul ( float4(input.pos,1.0f), gWorldViewProj );
	// Step 2:	rotate the normal: NO TRANSLATION
	//			this is achieved by clipping the 4x4 to a 3x3 matrix, 
	//			thus removing the postion row of the matrix
	output.normal = normalize(mul(input.normal, (float3x3)gWorld));
	output.texCoord = input.texCoord;
	output.lpos = mul(float4(input.pos, 1), mul(gWorld, gLightViewProj));
	return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shaders
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

    //float shadowFactor = shadowMap.SampleCmpLevelZero(cmpSampler, input.lpos.xy, input.lpos.z);
 
    //sample shadow map - point sampler
    //float shadowMapDepth = shadowMap.Sample(samPoint, input.lpos.xy).r;
 
    //float shadowFactor = input.lpos.z <= shadowMapDepth;


    //if clip space z value greater than shadow map value then pixel is in shadow
    //if (shadowMapDepth < input.lpos.z)
        //return float4(0,0,0,0);

    //if (shadowMapDepth == 1.0f)
    //    return float4(1, 0, 0, 0);

    ////re-homogenize position after interpolation
    //float3 lpos = input.lpos.xyz;

    //lpos /= input.lpos.w;;
    ////input.lpos.xyz /= input.lpos.w;
 
    ////if position is not visible to the light - dont illuminate it
    ////results in hard light frustum
    //if (lpos.x < -1.0f || lpos.x > 1.0f ||
    //    lpos.y < -1.0f || lpos.y > 1.0f ||
    //    lpos.z < 0.0f || lpos.z > 1.0f)
    //    return 0;
 
    ////transform clip space coords to texture space coords (-1:1 to 0:1)
    //lpos.x = lpos.x / 2 + 0.5;
    //lpos.y = lpos.y / -2 + 0.5;
 
    ////apply shadow map bias
    //lpos.z -= shadowMapBias;

    ////PCF sampling for shadow map
    //float sum = 0;
    //float x, y;

    ////perform PCF filtering on a 4 x 4 texel neighborhood
    //for (y = -1.5; y <= 1.5; y += 1)
    //{
    //    for (x = -1.5; x <= 1.5; x += 1)
    //    {
    //        sum += shadowMap.SampleCmpLevelZero(cmpSampler, lpos.xy + texOffset(x, y), lpos.z);
    //    }
    //}
 
    //float shadowFactor = sum / 16.0;

    //HalfLambert Diffuse :)
    float diffuseStrength = dot(input.normal, -gLightDirection);
    diffuseStrength = diffuseStrength * 0.5 + 0.5;
    diffuseStrength = saturate(diffuseStrength);
    color_rgb = color_rgb * diffuseStrength;
   
    shadowFactor = clamp(shadowFactor, 0.5f, 1.0f);
    //shadowFactor *= 0.5f + 0.5f;

    //return float4(color_rgb * shadowFactor, color_a);
    return float4(color_rgb * shadowFactor, color_a);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 Default
{
    pass P0
    {
		SetRasterizerState(NoCulling);
		SetDepthStencilState(EnableDepth, 0);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

