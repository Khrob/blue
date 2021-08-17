#include <simd/simd.h>

using namespace metal;

struct Vertex
{
    float4      position [[ position ]];
    float4      colour;
    //float2      uv;
    //uint16_t    texture;
};

vertex Vertex vertex_func (
    constant Vertex *vertices [[ buffer(0) ]],
    uint vid [[ vertex_id ]])
{
    Vertex v = vertices[vid];
    v.position.xy = v.position.xy * 2.0 - 1.0;
    return v;
}

fragment float4 fragment_func (Vertex in [[stage_in]])
{
    return in.colour;
}


//
// These sporadically crop up:
//
// error: expected unqualified-id
// error: unknown type name '�'
//
// (Don't seem to be an issue in here - just need to wait for the pipelineState to get built properly)
//

