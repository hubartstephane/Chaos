#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class CameraComponent;

}; // namespace chaos

#else

namespace chaos
{
	// =============================================
	// CameraComponent
	// =============================================

	class CameraComponent : public chaos::Tickable
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(CameraComponent, chaos::Tickable);

	public:

		CHAOS_DECLARE_GAMEPLAY_GETTERS();

		/** gets the camera the component belongs to */
		chaos::AutoCastable<Camera> GetCamera() { return camera; }
		/** gets the camera the component belongs to */
		chaos::AutoConstCastable<Camera> GetCamera() const { return camera; }

		/** modifier */
		virtual chaos::box2 ApplyModifier(chaos::box2 const & src) const;

	protected:

		/** called whenever the component is inserted into the camera */
		virtual void OnInsertedInto(Camera * in_camera);
		/** called whenever the component is removed from the camera */
		virtual void OnRemovedFrom(Camera * in_camera);

	protected:

		/** the owning camera */
		Camera * camera = nullptr;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION