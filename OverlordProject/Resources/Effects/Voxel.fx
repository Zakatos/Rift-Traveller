
//Digital Arts & Entertainment

//GLOBAL VARIABLES
//****************

float4x4 gWorld : WORLD;
float4x4 gViewProj : VIEWPROJECTION;
float4x4 gWorldViewProj : WORLDVIEWPROJECTION;
float4x4 gMatViewInv : VIEWINVERSE;
float3 gLightDirection = normalize(float3(-.5f, -1.f, 1.5f));

float gVoxelSize = 1.f;
bool gShowOriginalMesh = false;
bool gShowBoundingBoxes = false;

Texture2D gTextureDiffuse<
	string UIName = "Diffuse Texture";
	string ResourceName = "Skulls_Diffusemap.tga";
	string ResourceType = "2D";
>;

bool gUseDiffuseTexture
<
	string UIName = "Diffuse Texture";
	string UIWidget = "Bool";
> = false;

float3 gColorDiffuse
<
	string UIName = "Diffuse Color";
	string UIWidget = "Color";
> = float3(0.3,0.3,0.3);

float3 gColorSpecular
<
	string UIName = "Specular Color";
	string UIWidget = "Color";
> = float3(1,1,1);

int gShininess
<
	string UIName = "Shininess";
	string UIWidget = "Slider";
	float UIMin = 1;
	float UIMax = 20;
	float UIStep = 0.01;
> = 10;



//In/Out Structs
struct VS_DATA
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};
struct GS_DATA
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
};
//STATES
//******
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
};

RasterizerState NoCulling
{
    CullMode = NONE;
};

RasterizerState gRS_BackCulling
{
    CullMode = BACK;
};

BlendState gBS_DisableBlending
{
    BlendEnable[0] = FALSE;
};


BlendState EnableBlending
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
};

SamplerState gDiffuseSamplerState
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

//DIFFUSE FUNCTION
float3 CalculateDiffuse(float3 normal, float2 texCoord)
{
	//Diffuse Logic
	float3 diffuseColor = gColorDiffuse;
	if(gUseDiffuseTexture)
	{diffuseColor = gTextureDiffuse.Sample( gDiffuseSamplerState,texCoord );
	float3 color_rgb= diffuseColor.rgb;
	}
	
	//HalfLambert Diffuse 
	float diffuseStrength = dot(normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	diffuseColor = diffuseColor * diffuseStrength;

	return diffuseColor;
}


float3 CalculateDiffuseIntensity(float3 color, float3 normal)
{
	float diffuseIntensity = saturate(dot(normal, -gLightDirection));
	color *= diffuseIntensity;

	return color;
}

float3 CalculateSpecularBlinn(float3 viewDirection, float3 normal)
{
	float3 specularColor = gColorSpecular;
    float3 halfVector = normalize(viewDirection + gLightDirection);
    float specularStrenght = saturate(dot(normal, - halfVector));
    specularStrenght = pow(specularStrenght, gShininess);
    specularColor *= specularStrenght;

	return specularColor;
}


//MAIN VERTEX SHADER
//******************
VS_DATA VS(VS_DATA input)
{
	// 	convert position into float4 and multiply with matWorldViewProj
    VS_DATA output = input;
    output.pos = mul(float4(input.pos, 1.f), gWorld).xyz;
    return output;
}

//GEOMETRY  SHADER
//******************
void TransformVertex(inout TriangleStream<GS_DATA> triStream, float3 pos, float4 col, float3 normal,float2 texCoord)
{
    GS_DATA data;
    data.pos = mul(float4(pos, 1.f), gViewProj);
    data.color = col;
    data.normal = normal;
	data.texCoord = texCoord; 
    triStream.Append(data);
}

void GetAABB(float3 v0, float3 v1, float3 v2, out float3 primMin, out float3 primMax)
{
    primMin = min(min(v0, v1), v2);
    primMax = max(max(v0, v1), v2);
}

void ResizeBoundingBox(inout float3 primMin, inout float3 primMax)
{
    int3 minTemp = floor(primMin / float3(gVoxelSize, gVoxelSize, gVoxelSize));
    primMin = (float3)minTemp * gVoxelSize;
    int3 maxTemp = ceil(primMax / float3(gVoxelSize, gVoxelSize, gVoxelSize));
    primMax = (float3) maxTemp * gVoxelSize;
}

void GenerateCube(inout TriangleStream<GS_DATA> triStream, float3 primMin, float3 primMax, float4 color,float2 texCoord)
{
	
	
	//Create the front
    float3 point0 = float3(primMin.x, primMax.y, primMin.z);
    float3 point1 = float3(primMin.x, primMin.y, primMin.z);
    float3 point2 = float3(primMax.x, primMax.y, primMin.z);
    float3 point3 = float3(primMax.x, primMin.y, primMin.z);

    TransformVertex(triStream,point0, color, float3(0.f, 0.f, -1.f),float2(0,0));
    TransformVertex(triStream, point1, color, float3(0.f, 0.f, -1.f),float2(1,0));
    TransformVertex(triStream, point2, color, float3(0.f, 0.f, -1.f),float2(0,1));
    TransformVertex(triStream, point3, color, float3(0.f, 0.f, -1.f),float2(1,1));
    triStream.RestartStrip();

	//Create the back
    point0 = float3(primMin.x, primMax.y, primMax.z);
    point1 = float3(primMin.x, primMin.y, primMax.z);
    point2 = float3(primMax.x, primMax.y, primMax.z);
    point3 = float3(primMax.x, primMin.y, primMax.z);

    TransformVertex(triStream, point0, color, float3(0.f, 0.f, 1.f),texCoord);
    TransformVertex(triStream, point1, color, float3(0.f, 0.f, 1.f),texCoord);
    TransformVertex(triStream, point2, color, float3(0.f, 0.f, 1.f),texCoord);
    TransformVertex(triStream, point3, color, float3(0.f, 0.f, 1.f),texCoord);
    triStream.RestartStrip();

	//Create the left
    point0 = float3(primMin.x, primMin.y, primMax.z);
    point1 = float3(primMin.x, primMin.y, primMin.z);
    point2 = float3(primMin.x, primMax.y, primMax.z);
    point3 = float3(primMin.x, primMax.y, primMin.z);

    TransformVertex(triStream, point0, color, float3(-1.f, 0.f, 0.f),texCoord);
    TransformVertex(triStream, point1, color, float3(-1.f, 0.f, 0.f),texCoord);
    TransformVertex(triStream, point2, color, float3(-1.f, 0.f, 0.f),texCoord);
    TransformVertex(triStream, point3, color, float3(-1.f, 0.f, 0.f),texCoord);
    triStream.RestartStrip();

	//Create the right
    point0 = float3(primMax.x, primMin.y, primMax.z);
    point1 = float3(primMax.x, primMin.y, primMin.z);
    point2 = float3(primMax.x, primMax.y, primMax.z);
    point3 = float3(primMax.x, primMax.y, primMin.z);

    TransformVertex(triStream, point0, color, float3(1.f, 0.f, 0.f),texCoord);
    TransformVertex(triStream, point1, color, float3(1.f, 0.f, 0.f),texCoord);
    TransformVertex(triStream, point2, color, float3(1.f, 0.f, 0.f),texCoord);
    TransformVertex(triStream, point3, color, float3(1.f, 0.f, 0.f),texCoord);
    triStream.RestartStrip();

	//Create the bottom
    point0 = float3(primMin.x, primMin.y, primMax.z);
    point1 = float3(primMin.x, primMin.y, primMin.z);
    point2 = float3(primMax.x, primMin.y, primMax.z);
    point3 = float3(primMax.x, primMin.y, primMin.z);

    TransformVertex(triStream, point0, color, float3(0.f, -1.f, 0.f),texCoord);
    TransformVertex(triStream, point1, color, float3(0.f, -1.f, 0.f),texCoord);
    TransformVertex(triStream, point2, color, float3(0.f, -1.f, 0.f),texCoord);
    TransformVertex(triStream, point3, color, float3(0.f, -1.f, 0.f),texCoord);
    triStream.RestartStrip();

	//Create the top
    point0 = float3(primMin.x, primMax.y, primMax.z);
    point1 = float3(primMin.x, primMax.y, primMin.z);
    point2 = float3(primMax.x, primMax.y, primMax.z);
    point3 = float3(primMax.x, primMax.y, primMin.z);

    TransformVertex(triStream, point0, color, float3(0.f, 1.f, 0.f),texCoord);
    TransformVertex(triStream, point1, color, float3(0.f, 1.f, 0.f),texCoord);
    TransformVertex(triStream, point2, color, float3(0.f, 1.f, 0.f),texCoord);
    TransformVertex(triStream, point3, color, float3(0.f, 1.f, 0.f),texCoord);
    triStream.RestartStrip();
}

[maxvertexcount(36)]
void GS(triangle VS_DATA vertex[3], inout TriangleStream<GS_DATA> triStream)
{
    VS_DATA vert0 = vertex[0];
    VS_DATA vert1 = vertex[1];
    VS_DATA vert2 = vertex[2];

    if (gShowOriginalMesh)
    {
        TransformVertex(triStream, vert0.pos, vertex[0].color, vert0.normal,vert0.texCoord);
        TransformVertex(triStream, vert1.pos, vertex[1].color, vert1.normal,vert1.texCoord);
        TransformVertex(triStream, vert2.pos, vertex[2].color, vert2.normal,vert2.texCoord);
    }
    else
    {
        float4 color = (vert0.color + vert1.color + vert2.color) / .3f;
        float3 primMin, primMax;
        GetAABB(vert0.pos, vert1.pos, vert2.pos, primMin, primMax);
	
        if (gShowBoundingBoxes)
        {
            GenerateCube(triStream, primMin, primMax, color,vert0.texCoord);
        }
        else
        {
            ResizeBoundingBox(primMin, primMax);
            GenerateCube(triStream, primMin, primMax, color,vert0.texCoord);
        }

		// Calculating color
	
	if (gUseDiffuseTexture)
	{
		
		color = gTextureDiffuse.SampleLevel(gDiffuseSamplerState,vertex[0].texCoord,0);
		color += gTextureDiffuse.SampleLevel(gDiffuseSamplerState,vertex[1].texCoord,0);
		color += gTextureDiffuse.SampleLevel(gDiffuseSamplerState,vertex[2].texCoord,0);
		color/=3;
	}
		
		
    }
}


//MAIN PIXEL SHADER
//*****************
float4 PS(GS_DATA input) : SV_TARGET
{	
	// Normalise
	input.normal = normalize(input.normal);


	float3 diffuseColor = gColorDiffuse;
	if(gUseDiffuseTexture)
	{diffuseColor = gTextureDiffuse.Sample( gDiffuseSamplerState,input.texCoord );
	float3 color_rgb= diffuseColor.rgb;
	}
	
	//HalfLambert Diffuse 
	float diffuseStrength = dot(input.normal, -gLightDirection);
	diffuseStrength = diffuseStrength * 0.5 + 0.5;
	diffuseStrength = saturate(diffuseStrength);
	diffuseColor = diffuseColor * diffuseStrength;
	

	// Specularity - using the blinn model
	float3 viewDirection = normalize(input.pos.xyz - gMatViewInv[3].xyz);
	float3 specularColor = CalculateSpecularBlinn(viewDirection, input.normal);
    diffuseColor+=specularColor;

    return float4(diffuseColor,1);
	
	
		/*// Diffuse
	float3 diffuseColor = CalculateDiffuseIntensity(input.color, input.normal);
	
	if (gUseDiffuseTexture)
	{	
		diffuseColor = gTextureDiffuse.SampleLevel(gDiffuseSamplerState,input.texCoord,0);	
	}*/
	
	/*float3 color_rgb = diffuseColor;
	
   float diffuseStrength = saturate(dot(input.normal, -normalize(gLightDirection)) * .5f + .5f);
   color_rgb = color_rgb * diffuseStrength;
	
    
   return float4(diffuseStrength, diffuseStrength, diffuseStrength, 1.f);*/
	
	
	
	// NORMALIZE
	/*input.normal = normalize(input.normal);
	
	float3 viewDirection = normalize(input.pos.xyz - gMatViewInv[3].xyz);
		
	//SPECULAR
	float3 specColor = CalculateSpecular(viewDirection, input.normal, input.texCoord);
		
	//DIFFUSE
	float3 diffColor = CalculateDiffuse(input.normal, input.texCoord);
		
		
	//FINAL COLOR CALCULATION
	float3 finalColor = diffColor + specColor;
	
	
	
	return float4(finalColor,1.f);*/
		
   /*float3 color_rgb = input.color.rgb;
	
   float diffuseStrength = saturate(dot(input.normal, -normalize(gLightDirection)) * .5f + .5f);
   color_rgb = color_rgb * diffuseStrength;
	
    
   return float4(diffuseStrength, diffuseStrength, diffuseStrength, 1.f);*/
}

//TECHNIQUES
//**********
technique10 Default
{
    pass P0
    {
        SetRasterizerState(NoCulling);
        SetDepthStencilState(EnableDepth, 0);
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(CompileShader(gs_4_0, GS()));
        SetPixelShader(CompileShader(ps_4_0, PS()));
    }
}
