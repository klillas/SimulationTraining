#version 430

#define INVOCATION_SIZE_X 32
layout(local_size_x = INVOCATION_SIZE_X, local_size_y = 1) in;

layout (push_constant) uniform ComputeShaderPushConstants {
    uint InBufferSize_items;
} push_constants;

layout(std430, binding = 0) buffer lay0 { uint inbuf[]; };
layout(std430, binding = 1) buffer lay1 { uint outbuf[]; };

void main() {
	const uint globalInvocationID = gl_GlobalInvocationID.x;
	const uint totalGlobalInvocations = gl_NumWorkGroups.x * INVOCATION_SIZE_X;
	const uint itemsInBuf = push_constants.InBufferSize_items;
	// Assume exact division to avoid if case later. CPU side needs to configure amount of items correctly
	const uint itemsPerThread = itemsInBuf / totalGlobalInvocations;
	
	for (uint i = (globalInvocationID * itemsPerThread); i < ((globalInvocationID * itemsPerThread) + itemsPerThread); i++)
	{
		outbuf[i] = i;
	}
}