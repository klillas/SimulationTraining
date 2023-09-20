#pragma once

namespace GPUPhysics
{
	class GPUPhysicsConfiguration
	{
	public:
		// TODO: Automate synchronization between compute shader code and this invocation size parameter
		/// <summary>
		/// Compute shader invocation (thread) size
		/// Must be the same as the invocation size defined in the compute shader glsl code
		/// </summary>
		static inline const unsigned GPU_COMPUTE_SHADER_INVOCATION_SIZE = 32;

		/// <summary>
		/// TODO: Automate how manuy groups should be active depending on GPU
		/// Amount of concurrent groups that are run to execute compute shader scripts
		/// The total amount of parallel threads are GPU_COMPUTE_SHADER_INVOCATION_SIZE * GPU_COMPUTE_SHADER_COMPUTE_SHADER_GROUP_COUNT
		/// TODO: Note: I assume if we make this too large that not all thread will execute in parallel anymore. Need to learn more about this.
		/// </summary>
		static inline const unsigned GPU_COMPUTE_SHADER_COMPUTE_SHADER_GROUP_COUNT = 2;
	};
};