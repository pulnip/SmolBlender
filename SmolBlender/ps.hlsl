static const float3 fogColor = float3(0, 0, 0);

float3 ApplyFogSimple(float3 color, float z)
{
    float fogFactor = saturate(z);
    return lerp(color, fogColor, fogFactor);
}

static const float fogDensity = 1.5;

float3 ApplyFogExp2(float3 color, float z)
{
    float f = z * fogDensity;
    float fogFactor = saturate(1.0 - exp(-f * f));
    return lerp(color, fogColor, fogFactor);
}

struct VertexOut
{
    float4 position : SV_Position;
    float4 color : COLOR0;
};

float4 ps_main(VertexOut input) : SV_Target
{
    float3 baseColor = input.color.rgb;
    float z = input.position.z;
    float remappedZ = z * 0.5 + 0.5;
    
    return float4(ApplyFogExp2(baseColor, remappedZ), 1.0);
}