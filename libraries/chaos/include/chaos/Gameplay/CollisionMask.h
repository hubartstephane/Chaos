#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	namespace CollisionMask
	{

	}; // namespace CollisionMask

}; // namespace chaos

#else

namespace chaos
{
	namespace CollisionMask
	{
		/** for object/layer ... that can collides with player */
		static constexpr int PLAYER = (1 << 0);
		/** for object/layer ... that can collides with camera */
		static constexpr int CAMERA = (1 << 1);
	};
	
}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION