namespace chaos
{
	namespace TiledMap
	{

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		// ==========================================
		// TileFlagProcessor
		// ==========================================

		class CHAOS_API TileFlagProcessor : public Object, public JSONSerializableInterface
		{
			CHAOS_DECLARE_OBJECT_CLASS(TileFlagProcessor, Object);

		public:

			/** the method to be apply on the layer */
			virtual void Process(TileLayer* in_layer) {}
		};

		// ==========================================
		// EightBitsModeTileFlagProcessor
		// ==========================================

		class CHAOS_API EightBitsModeTileFlagProcessor : public TileFlagProcessor
		{
			CHAOS_DECLARE_OBJECT_CLASS(EightBitsModeTileFlagProcessor, TileFlagProcessor)("EightBitsMode");

		public:

			/** the method to be apply on the layer */
			virtual void Process(TileLayer* in_layer);
		};

		// ==========================================
		// ComputeNeighbourFlagProcessor
		// ==========================================

		class CHAOS_API ComputeNeighbourFlagProcessor : public TileFlagProcessor
		{
			CHAOS_DECLARE_OBJECT_CLASS(ComputeNeighbourFlagProcessor, TileFlagProcessor)("ComputeNeighbour");

		public:

			/** override */
			virtual void Process(TileLayer* in_layer) override;
			/** override */
			virtual bool SerializeIntoJSON(nlohmann::json * json) const override;
			/** override */
			virtual bool SerializeFromJSON(nlohmann::json const * json) override;

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

		class CHAOS_API ComputeCustomFlagProcessorEntry
		{
		public:

			/** the type of the tile */
			std::string type;
			/** the flag applyed */
			int flag = 0;
		};

		/** JSON serialization */
		CHAOS_API bool DoSaveIntoJSON(nlohmann::json * json, ComputeCustomFlagProcessorEntry const& src);
		/** JSON serialization */
		CHAOS_API bool DoLoadFromJSON(JSONReadConfiguration config, ComputeCustomFlagProcessorEntry& dst);


		class CHAOS_API ComputeCustomFlagProcessor : public TileFlagProcessor
		{
			CHAOS_DECLARE_OBJECT_CLASS(ComputeCustomFlagProcessor, TileFlagProcessor);

		public:

			/** override */
			virtual void Process(TileLayer* in_layer) override;
			/** override */
			virtual bool SerializeIntoJSON(nlohmann::json * json) const override;
			/** override */
			virtual bool SerializeFromJSON(nlohmann::json const * json) override;

		protected:

			/** additionnal flags per type */
			std::vector<ComputeCustomFlagProcessorEntry> custom_flags;
		};

#endif

	}; // namespace TiledMap

}; // namespace chaos
