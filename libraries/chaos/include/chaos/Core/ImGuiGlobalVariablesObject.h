namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiGlobalVariablesObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiGlobalVariablesObject: an ImGUIObject that display the global variables
	*/

	class ImGuiGlobalVariablesObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiGlobalVariablesObject, ImGuiObject);

	protected:

		/** override */
		virtual void OnDrawImGuiContent() override;
	};

#endif

}; // namespace chaos