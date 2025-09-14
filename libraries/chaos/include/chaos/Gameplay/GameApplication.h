namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class GameApplication;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	class CHAOS_API GameApplication : public WindowApplication
	{
	public:

		/** constructor */
		GameApplication(SubClassOf<Game> in_game_class);

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
		virtual bool TraverseInputEventReceiverHierarchy(TraverseInputEventReceiverHierarchyFunction event_func) override;

	protected:

		/** the class for the game */
		SubClassOf<Game> game_class;
		/** pointer on the game */
		shared_ptr<Game> game;
	};

	template<typename GAME_TYPE, typename GAME_APPLICATION_TYPE = GameApplication, typename ...PARAMS>
	decltype(auto) RunGame(int argc, char** argv, char** env, PARAMS && ...params)
	{
		return RunApplication<GAME_APPLICATION_TYPE>(
			argc, argv, env,
			GAME_TYPE::GetStaticClass(),
			std::forward<PARAMS>(params)...);
	}

#endif

}; // namespace chaos