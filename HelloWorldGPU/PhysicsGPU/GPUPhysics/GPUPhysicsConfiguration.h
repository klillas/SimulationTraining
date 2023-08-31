#pragma once

namespace GPUPhysics
{
	class GPUPhysicsConfiguration
	{
		// TODO: Automate synchronization between compute shader code and this invocation size parameter
		/// <summary>
		/// Compute shader invocation (thread) size
		/// Must be the same as the invocation size defined in the compute shader glsl code
		/// </summary>
		static inline const unsigned GPU_COMPUTE_SHADER_INVOCATION_SIZE = 32;
	};
};