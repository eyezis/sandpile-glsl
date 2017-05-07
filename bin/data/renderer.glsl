#version 440

layout(r32ui, binding=1) uniform readonly uimage2D src;
layout(rgba8, binding=2) uniform writeonly image2D dst;

const vec4 PALETTE[32] = vec4[](
    vec4(0.00, 0.00, 0.00, 1),
    vec4(0.00, 0.00, 0.13, 1),
    vec4(0.00, 0.00, 0.25, 1),
    vec4(0.00, 0.00, 0.38, 1),
    vec4(0.00, 0.00, 0.51, 1),
    vec4(0.00, 0.06, 0.64, 1),
    vec4(0.00, 0.12, 0.77, 1),
    vec4(0.00, 0.32, 0.90, 1),
    vec4(0.00, 0.55, 1.00, 1),
    vec4(0.00, 0.74, 1.00, 1),
    vec4(0.00, 0.93, 1.00, 1),
    vec4(0.00, 1.00, 0.87, 1),
    vec4(0.00, 1.00, 0.67, 1),
    vec4(0.00, 1.00, 0.48, 1),
    vec4(0.00, 1.00, 0.29, 1),
    vec4(0.04, 1.00, 0.09, 1),
    vec4(0.09, 1.00, 0.04, 1),
    vec4(0.29, 1.00, 0.00, 1),
    vec4(0.48, 1.00, 0.00, 1),
    vec4(0.67, 1.00, 0.00, 1),
    vec4(0.87, 1.00, 0.00, 1),
    vec4(1.00, 0.93, 0.00, 1),
    vec4(1.00, 0.74, 0.00, 1),
    vec4(1.00, 0.55, 0.04, 1),
    vec4(1.00, 0.42, 0.09, 1),
    vec4(1.00, 0.35, 0.22, 1),
    vec4(1.00, 0.35, 0.35, 1),
    vec4(1.00, 0.48, 0.48, 1),
    vec4(1.00, 0.61, 0.61, 1),
    vec4(1.00, 0.74, 0.74, 1),
    vec4(1.00, 0.87, 0.87, 1),
    vec4(1.00, 1.00, 1.00, 1)
);

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    const uint val = imageLoad(src, ivec2(gl_GlobalInvocationID.xy)).r;
    imageStore(dst, ivec2(gl_GlobalInvocationID.xy), PALETTE[val > 31 ? 31 : val]);
}
