namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameWindow;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GameWindow : public Window
	{
		CHAOS_DECLARE_OBJECT_CLASS(GameWindow, Window);

	public:

		/** override */
		virtual bool OnInitialize(JSONReadConfiguration config) override;

	protected:

		/** override */
		virtual bool CreateRootWidget() override;

	protected:

		/** the game being played */
		shared_ptr<Game> game;
	};

#endif

}; // namespace chaos