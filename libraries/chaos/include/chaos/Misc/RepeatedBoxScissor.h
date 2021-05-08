namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class RepeatedBoxScissor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =====================================
	// RepeatedBoxScissor : an utility object to compute instances in 2D of a box that collide a given scissor
	// =====================================

	class RepeatedBoxScissor
	{
	public:

		/** constructor */
		RepeatedBoxScissor() = default;
		/** constructor */
		RepeatedBoxScissor(box2 const& in_target_box, box2 const& in_scissor_box, bool in_wrap_x, bool in_wrap_y);

		/** offset of a given instance */
		glm::vec2 GetInstanceOffset(glm::ivec2 const& index) const;

	public:

		/** the 'index' of the first instance to render/collide ... (included) */
		glm::ivec2 start_instance = { 0, 0 };
		/** the 'index' of the last instance to render/collide ... (excluded) */
		glm::ivec2 last_instance = { 0, 0 };
		/** copy of the construction parameters */
		box2 target_box;
		/** copy of the construction parameters */
		box2 scissor_box;
		/** copy of the construction parameters */
		bool wrap_x = false;
		/** copy of the construction parameters */
		bool wrap_y = false;
	};

#endif

}; // namespace chaos