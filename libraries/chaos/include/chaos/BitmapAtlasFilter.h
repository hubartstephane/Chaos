#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ImageProcessor.h>
#include <chaos/JSONTools.h>
#include <chaos/Class.h>
#include <chaos/Object.h>

namespace chaos
{
	namespace BitmapAtlas
	{
		/**
		* BitmapAtlasFilterCondition : a condition to know whether the bitmap is to be processed
		*/

		class BitmapAtlasFilterCondition : public Object
		{
		public:

			/** whether the processing is to be done on the bitmap */
			virtual bool AcceptBitmap(class BitmapInfoInput const* input) const;
			/** whether the processing is to be done on the font */
			virtual bool AcceptFont(class FontInfoInput const* input) const;

			/** loading method from JSON */
			virtual bool LoadFromJSON(nlohmann::json const& entry);
			/** saving method from JSON */
			virtual bool SaveIntoJSON(nlohmann::json& entry) const;
		};

		CHAOS_REGISTER_CLASS1(BitmapAtlasFilterCondition);


		/**
		* BitmapAtlasFilter : check condition on an entry and start the processor
		*/

		class BitmapAtlasFilter
		{

		public:

			/** loading method from JSON */
			virtual bool LoadFromJSON(nlohmann::json const& entry);
			/** saving method from JSON */
			virtual bool SaveIntoJSON(nlohmann::json& entry) const;

		public:

			/** the condition for handling the image */
			shared_ptr<BitmapAtlasFilterCondition> condition;
			/** the condition for handling the image */
			shared_ptr<ImageProcessor> processor;
		};

		/**
		* BitmapAtlasFilterSet : a whole set of condition/image processor
		*/

		class BitmapAtlasFilterSet : public Object
		{

		public:

			/** insert a new filter inside the list */
			void AddFilter(BitmapAtlasFilterCondition* condition, ImageProcessor* processor);

			/** loading method from JSON */
			virtual bool LoadFromJSON(nlohmann::json const& entry);
			/** saving method from JSON */
			virtual bool SaveIntoJSON(nlohmann::json& entry) const;

		protected:

			/** the filters to apply */
			std::vector<BitmapAtlasFilter> filters;
		};

	}; // namespace BitmapAtlas

}; // namespace chaos
