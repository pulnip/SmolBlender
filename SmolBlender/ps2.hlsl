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

Texture2D tex : register(t0);
SamplerState samp : register(s0);

float4 ps_textured(VertexOut input) : SV_Target
{
    float4 sampled = tex.Sample(samp, input.uv);
    
    return sampled;
}
