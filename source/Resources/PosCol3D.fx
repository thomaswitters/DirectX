//-----------------------
// Input/Output Structs
//-----------------------

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    //float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float3 Color : COLOR;
    //float2 TexCoord : TEXCOORD;
};

//-----------------------
// Vertex Shader
//-----------------------

float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    output.Color = input.Color;
    //output.TexCoord = input.TexCoord;
    return output;
}

//-----------------------
// Pixel Shader
//-----------------------

SamplerState samPoint
{
    filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    // Sample the texture using the specified sampler state
    //float4 texColor = gDiffuseMap.Sample(samPoint, input.TexCoord);
    
    //return texColor * float4(input.Color, 1.0f);
    return float4(input.Color, 1.0f);

}

//-----------------------
// Technique
//-----------------------

technique11 DefaultTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}

