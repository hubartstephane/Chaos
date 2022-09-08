namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Level;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// =====================================
	// Level : a simple game level
	// =====================================

	class CHAOS_API Level : public Object, public FileResource, public NamedObject
	{
		CHAOS_GAMEPLAY_ALLFRIENDS;

		CHAOS_DECLARE_OBJECT_CLASS(Level, Object);

	public:

		/** create a level instance for that level */
		LevelInstance* CreateLevelInstance(Game* in_game);

		/** returns the level timeout */
		float GetLevelTimeout() const { return level_timeout; }
		/** returns the title for that level */
		std::string const& GetLevelTitle() const { return level_title; }
		/** the index of that level */
		int GetLevelIndex() const { return level_index; }

	protected:

		/** create a level instance for that level user specified function */
		virtual class LevelInstance* DoCreateLevelInstance();

	protected:

		/** get the level timeout */
		float level_timeout = -1.0f;
		/** the title of the level*/
		std::string level_title;
		/** the index of the level (maybe a key or an ordered index) */
		int level_index = 0;

		/** the level instance class */
		SubClassOf<LevelInstance> level_instance_class;
	};

#endif

}; // namespace chaos