struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 ps_main(VertexOut input) : SV_Target
{
    return float4(input.uv, 0, 1);
}
