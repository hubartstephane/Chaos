namespace chaos
{	
	namespace TiledMap
	{

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// TileFlagProcessor
		// ==========================================

		class TileFlagProcessor : public Object, public JSONSerializable
		{
			CHAOS_DECLARE_OBJECT_CLASS2(TileFlagProcessor, Object);

		public:

			/** the method to be apply on the layer */
			virtual void Process(TileLayer* in_layer) {}
		};

		// ==========================================
		// ComputeNeighbourFlagProcessor
		// ==========================================

		class ComputeNeighbourFlagProcessor : public TileFlagProcessor
		{
			CHAOS_DECLARE_OBJECT_CLASS2(ComputeNeighbourFlagProcessor, TileFlagProcessor);

		public:

			/** override */
			virtual void Process(TileLayer* in_layer) override;
			/** override */
			virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const override;
			/** override */
			virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override;

		protected:

			/** internal method to detect whether the tile is of interrest for the processing */
			bool ShouldProcessedTiled(Map const* map, Tile tile) const;

		protected:

			/** the types that are searched for neighbours */
			std::vector<std::string> types;
		};

		// ==========================================
		// ComputeNeighbourFlagProcessor
		// ==========================================

		class ComputeCustomFlagProcessorEntry
		{
		public:

			/** the type of the tile */
			std::string type;
			/** the flag applyed */
			int flag = 0;
		};

		/** JSON serialization */
		bool SaveIntoJSON(nlohmann::json& json_entry, ComputeCustomFlagProcessorEntry const& src);
		/** JSON serialization */
		bool LoadFromJSON(nlohmann::json const& json_entry, ComputeCustomFlagProcessorEntry& dst);


		class ComputeCustomFlagProcessor : public TileFlagProcessor
		{
			CHAOS_DECLARE_OBJECT_CLASS2(ComputeCustomFlagProcessor, TileFlagProcessor);

		public:

			/** override */
			virtual void Process(TileLayer* in_layer) override;
			/** override */
			virtual bool SerializeIntoJSON(nlohmann::json& json_entry) const override;
			/** override */
			virtual bool SerializeFromJSON(nlohmann::json const& json_entry) override;

		protected:

			/** additionnal flags per type */
			std::vector<ComputeCustomFlagProcessorEntry> custom_flags;
		};

#endif

	}; // namespace TiledMap

}; // namespace chaos
