struct Vertex
{
    float2 position : POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

cbuffer uniforms : register(b0)
{
    float4x4 mat;
};

VertexOut vs_main(Vertex input)
{
    VertexOut output;
    output.position = mul(mat, float4(input.position, 0, 1));
    output.color = input.color;
    output.uv = input.uv;
    
    return output;
}
