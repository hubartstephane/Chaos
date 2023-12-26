namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ImGuiSystemInformationObject;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ImGuiSystemInformationObject: a drawable that displays some system informations
	*/

	class ImGuiSystemInformationObject : public ImGuiObject
	{
	public:

		CHAOS_DECLARE_OBJECT_CLASS(ImGuiSystemInformationObject, ImGuiObject);

	protected:

		/** override */
		virtual int GetImGuiWindowFlags() const override;
		/** override */
		virtual void OnDrawImGuiContent(ImGuiDrawFlags flags) override;
	};

#endif

}; // namespace chaos