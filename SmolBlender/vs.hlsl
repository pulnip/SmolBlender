struct VertexOut
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VertexOut vs_main(uint vertexID : SV_VertexID)
{
    static const float2 ndc[3] =
    {
        float2(0, 0.8), float2(-0.6, -0.8), float2(0.6, -0.8)
    };
    static const float4 colors[3] =
    {
        float4(1, 0, 0, 1), float4(0, 1, 0, 1), float4(0, 0, 1, 1)
    };

    VertexOut output;
    output.position = float4(ndc[vertexID], 0, 1);
    output.color = colors[vertexID];

    return output;
}