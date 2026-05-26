struct Vertex
{
    float2 position : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

cbuffer uniforms: register(b0)
{
    float4x4 mat;
};

VertexOut vs_main(Vertex input)
{
    VertexOut output;
    output.position = mul(mat, float4(input.position, 0, 1));
    output.color = input.color;
    
    return output;
}
