#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class SceneRenderer;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

namespace chaos
{
	class SceneRenderer
	{
	public:

		/** display a whole scene from the root node */
		void DisplayScene(SceneNode* root_node, GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params);

	protected:

		/** display a single node */
		void DisplayNode(SceneNode* node, GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params);

	protected:

		/** the local to world matrix */
		glm::mat4 local_to_world = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION







