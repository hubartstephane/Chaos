namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameApplication;
	class GameApplicationData;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GameApplicationData : public WindowApplicationData
	{
		CHAOS_DECLARE_OBJECT_CLASS(GameApplicationData, WindowApplicationData);

	public:

		Class<Game> const * game_class = nullptr;
	};


	class CHAOS_API GameApplication : public WindowApplication
	{
		CHAOS_DECLARE_OBJECT_CLASS(GameApplication, WindowApplication);

	public:

		/** gets the game */
		Game* GetGame() { return game.get(); }
		/** gets the game */
		Game const* GetGame() const { return game.get(); }

		/** whether the game should "ignored" */
		virtual bool IsGameSuspended() const;

	protected:

		/** override */
		virtual bool InitializeGPUResources(JSONReadConfiguration config) override;
		/** override */
		virtual void FinalizeGPUResources() override;
		/** override */
		virtual bool DoTick(float delta_time) override;
		/** override */
		virtual bool FillAtlasGeneratorInput(AtlasInput& input) override;

		/** override */
		virtual bool TraverseInputReceiver(InputReceiverTraverser & in_traverser, InputDeviceInterface const* in_input_device) override;
		/** override */
		virtual bool DoPollGamepad(PhysicalGamepad * physical_gamepad) override;

		/** override */
		virtual bool IsApplicationDataValid(ApplicationData const * in_application_data) const override;

		/** get the casted application data */
		GameApplicationData const* GetGameApplicationData() const;

	protected:

		/** pointer on the game */
		shared_ptr<Game> game;
	};

#endif

}; // namespace chaos