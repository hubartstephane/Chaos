#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class SoundListenerCameraComponent;

}; // namespace chaos

#else

namespace chaos
{
	// =============================================
	// SoundListenerCameraComponent
	// =============================================

	class SoundListenerCameraComponent : public CameraComponent
	{
		CHAOS_DECLARE_OBJECT_CLASS2(SoundListenerCameraComponent, CameraComponent);

	protected:

		/** override */
		virtual bool DoTick(float delta_time) override;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION