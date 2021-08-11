#include <simd/simd.h>

using namespace metal;

vertex float4 vertex_func (
    constant packed_float3 *vertices  [[ buffer(0) ]], 
    constant packed_float2 *positions [[ buffer(1) ]],
    uint vid [[ vertex_id ]]) 
{
    return float4(vertices[vid].xy*2.0-1.0, 0.0, 1.0);
}

fragment half4 fragment_func ()
{
    return half4(0.7, 1, 1, 1);
}

//
// These sporadically crop up:
//
// error: expected unqualified-id
// error: unknown type name '�'
//
// (Don't seem to be an issue in here - just need to wait for the pipelineState to get built properly)
//

