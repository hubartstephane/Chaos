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

	protected:

		/** override */
		virtual int GetImGuiWindowFlags() const override;
		/** override */
		virtual void OnDrawImGuiContent() override;
	};

#endif

}; // namespace chaos