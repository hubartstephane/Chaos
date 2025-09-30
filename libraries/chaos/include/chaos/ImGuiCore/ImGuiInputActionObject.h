namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiInputActionObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiInputActionObject: a drawable that displays some help for keys
	*/

	class ImGuiInputActionObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiInputActionObject, ImGuiObject);

		/** constructor */
		ImGuiInputActionObject();

	protected:

		/** override */
		virtual void OnDrawImGuiContent(Window * window) override;
	};

#endif

}; // namespace chaos