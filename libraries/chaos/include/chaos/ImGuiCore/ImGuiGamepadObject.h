namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiGamepadObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiGamepadObject: an ImGUIObject that display the state of gamepads
	*/

	class ImGuiGamepadObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiGamepadObject, ImGuiObject);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window * window) override;
	};

#endif

}; // namespace chaos