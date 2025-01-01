namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiHelpObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiHelpObject: a drawable that displays some help for keys
	*/

	class ImGuiHelpObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiHelpObject, ImGuiObject);

		/** constructor */
		ImGuiHelpObject();

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window * window) override;
	};

#endif

}; // namespace chaos