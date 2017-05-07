#version 440

layout(r32ui, binding=0) uniform readonly uimage2D src;
layout(r32ui, binding=1) uniform uimage2D dst;

layout(packed, binding=4) buffer SharedVars {
        uint xOffset;
        uint yOffset;
};

layout(local_size_x = 32, local_size_y = 32, local_size_z = 1) in;
void main() {
    const uint x = gl_GlobalInvocationID.x;
    const uint y = gl_GlobalInvocationID.y;

    const uint current = (imageLoad(src, ivec2(gl_GlobalInvocationID.xy)).r & 3)
            + (imageLoad(src, ivec2(x-1, y)).r >> 2)
            + (imageLoad(src, ivec2(x+1, y)).r >> 2)
            + (imageLoad(src, ivec2(x, y-1)).r >> 2)
            + (imageLoad(src, ivec2(x, y+1)).r >> 2);

    const ivec2 dstLoc = ivec2(x + xOffset, y + yOffset);

    imageAtomicExchange(dst, dstLoc, current);
}
