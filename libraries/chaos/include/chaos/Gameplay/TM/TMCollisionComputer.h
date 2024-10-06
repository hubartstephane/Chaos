namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =====================================
	// TileCollisionComputer
	// =====================================

	class CHAOS_API TileCollisionComputer
	{
	public:

		/** constructor */
		TileCollisionComputer(TileCollisionComputer const& src) = default;
		/** constructor */
		TileCollisionComputer(TMLevelInstance* in_level_instance, box2 in_src_box, box2 in_dst_box, int in_collision_mask, ParticleAllocationBase* in_ignore_allocation, glm::vec2 const& in_box_extend, char const* in_wangset_name) :
			level_instance(in_level_instance),
			src_box(in_src_box),
			dst_box(in_dst_box),
			collision_mask(in_collision_mask),
			ignore_allocation(in_ignore_allocation),
			box_extend(in_box_extend),
			wangset_name(in_wangset_name),
			delta_position(dst_box.position - src_box.position) {}

	public:

		/** the entry point for the whole computation */
		box2 Run(LightweightFunction<void(TileCollisionInfo const& collision_info)> func);

		/** compute reaction for a a particle */
		void ComputeReaction(TileCollisionInfo const& collision_info, LightweightFunction<bool(TileCollisionInfo const&, Edge)> func);

	public:

		/** the level instance used */
		TMLevelInstance* level_instance = nullptr;
		/** the initial position of the moving object */
		box2 src_box;
		/** the final position of the moving object (if there were no collision) */
		box2 dst_box;
		/** the mask for all searched objects */
		int collision_mask = 0;
		/** allocation to be ignored */
		ParticleAllocationBase* ignore_allocation = nullptr;
		/** some extension for the collision surface*/
		glm::vec2 const& box_extend;
		/** the name of the collision wangset (use NEIGHBOUR FLAGS if unspecified) */
		char const* wangset_name = nullptr;

		/** the difference of position between src_box and dst_box before any reaction is computed */
		glm::vec2 delta_position = glm::vec2(0.0f, 0.0f);

	protected:

		/** cache for wangset */
		TiledMap::Wangset const* wangset = nullptr;
		/** cache for wangset */
		TiledMap::TileSet const* tileset = nullptr;
	};

#endif

}; // namespace chaos

