namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiWindowInformationObject;


#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiWindowInformationObject: a drawable that displays some window informations
	*/

	class ImGuiWindowInformationObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiWindowInformationObject, ImGuiObject);

		/** constructor */
		ImGuiWindowInformationObject();

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window* window) override;
	};

#endif

}; // namespace chaos