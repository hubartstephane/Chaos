namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiInputStateObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiInputStateObject: an ImGUIObject that display the state of inputs (mouse + keyboard)
	*/

	class ImGuiInputStateObject : public ImGuiInputStateObjectBase
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiInputStateObject, ImGuiInputStateObjectBase);

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window * window) override;
	};

#endif

}; // namespace chaos