struct VertexOut
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

float4 ps_main(VertexOut input) : SV_Target{
    return input.color;
}