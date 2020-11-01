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

	class CameraComponent : public Tickable
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS2(CameraComponent, Tickable);

	public:

		CHAOS_DECLARE_GAMEPLAY_GETTERS();

		/** gets the camera the component belongs to */
		AutoCastable<Camera> GetCamera() { return camera; }
		/** gets the camera the component belongs to */
		AutoConstCastable<Camera> GetCamera() const { return camera; }

		/** modifier */
		virtual box2 ApplyModifier(box2 const & src) const;

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