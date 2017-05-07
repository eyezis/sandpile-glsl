#version 440

layout(r32ui, binding=0) uniform readonly uimage2D src;
layout(r32ui, binding=1) uniform readonly uimage2D dst;

layout(packed, binding=3) buffer Outputs{
        uint resizeNeeded;
};

layout(packed, binding=4) buffer SharedVars{
        uint xOffset;
        uint yOffset;
};

layout(local_size_x = 32, local_size_y = 1, local_size_z = 1) in;
void main() {
    if (gl_GlobalInvocationID.x == 0) {
        if (resizeNeeded > 0) {
            xOffset = 0;
            yOffset = 0;
        }
        resizeNeeded = 0;

        if (imageSize(src) != imageSize(dst)) {
            resizeNeeded = 1;
        }
    }

    memoryBarrierBuffer();
    barrier();

    if (resizeNeeded > 0) {
        return;
    }

    const uint size = imageSize(dst).x-1;
    const uint topple = 3;
    const uint i = gl_GlobalInvocationID.x;

    if (imageLoad(src, ivec2(i, 0)).r > topple || imageLoad(src, ivec2(i, size)).r > topple
        || imageLoad(src, ivec2(0, i)).r > topple || imageLoad(src, ivec2(size, i)).r > topple) {
        resizeNeeded = 1;
    }

    memoryBarrierBuffer();
    barrier();

    if (gl_LocalInvocationID.x == 0) {
        if (resizeNeeded > 0) {
            xOffset = (size+1)/2;
            yOffset = xOffset;
        }
    }
}
