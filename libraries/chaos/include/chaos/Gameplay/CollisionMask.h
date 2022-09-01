namespace chaos
{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	namespace CollisionMask
	{
		/** for object/layer ... that can collides with player */
		static constexpr int PLAYER = (1 << 0);
		/** for object/layer ... that can collides with camera */
		static constexpr int CAMERA = (1 << 1);
	};

#endif

}; // namespace chaos