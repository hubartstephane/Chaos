namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SoundListenerCameraComponent;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

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

#endif

}; // namespace chaos