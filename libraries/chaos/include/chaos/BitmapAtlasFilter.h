#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageProcessor.h>
#include <chaos/JSONTools.h>
#include <chaos/Class.h>
#include <chaos/Object.h>
#include <chaos/JSONSerializable.h>

namespace chaos
{
	namespace BitmapAtlas
	{
		/**
		* BitmapAtlasFilterCondition : a condition to know whether the bitmap is to be processed
		*/

		class BitmapAtlasFilterCondition : public Object, public JSONSerializable
		{
			CHAOS_OBJECT_DECLARE_CLASS2(BitmapAtlasFilterCondition, Object)

		public:

			/** whether the processing is to be done on the bitmap */
			virtual bool AcceptBitmap(class BitmapInfoInput const* input) const;
			/** whether the processing is to be done on the font */
			virtual bool AcceptFont(class FontInfoInput const* input) const;

			/** loading method from JSON */
			virtual bool LoadFromJSON(nlohmann::json const& entry) override;
			/** saving method from JSON */
			virtual bool SaveIntoJSON(nlohmann::json& entry) const override;
		};

		/**
		* BitmapAtlasFilter : check condition on an entry and start the processor
		*/

		class BitmapAtlasFilter
		{

		public:

			/** the condition for handling the image */
			shared_ptr<BitmapAtlasFilterCondition> condition;
			/** the condition for handling the image */
			shared_ptr<ImageProcessor> processor;
		};

		/** loading method from JSON */
		bool LoadFromJSON(nlohmann::json const& entry, BitmapAtlasFilter& dst);
		/** saving method from JSON */
		bool SaveIntoJSON(nlohmann::json& entry, BitmapAtlasFilter const& src);

		/**
		* BitmapAtlasFilterSet : a whole set of condition/image processor
		*/

		class BitmapAtlasFilterSet
		{
		public:

			/** the filters to apply */
			std::vector<BitmapAtlasFilter> filters;
		};

		/** loading method from JSON */
		bool LoadFromJSON(nlohmann::json const& entry, BitmapAtlasFilterSet& dst);
		/** saving method from JSON */
		bool SaveIntoJSON(nlohmann::json& entry, BitmapAtlasFilterSet const& src);

	}; // namespace BitmapAtlas

}; // namespace chaos