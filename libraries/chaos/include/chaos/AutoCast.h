#pragma once

namespace chaos
{
	namespace details
	{
		template<typename T>
		class AutoCasted
		{
		public:

			/** the constructor */
			AutoCasted(T * in_ptr) : ptr(in_ptr) {}
			/** the conversion operator */
			template<typename U>
			operator U * () const 
			{ 
				if constexpr (std::is_base_of_v<U, T>)
					return ptr;
				return dynamic_cast<U *>(ptr); 
			}

		protected:

			/** the pointer */
			T * ptr = nullptr;
		};

		template<typename T>
		class AutoConstCasted
		{
		public:

			/** the constructor */
			AutoConstCasted(const T * in_ptr) : ptr(in_ptr) {}
			/** the conversion operator */
			template<typename U> 
			operator U const * () const 
			{ 
				if constexpr (std::is_base_of_v<U, T>)
					return ptr;
				return dynamic_cast<const U *>(ptr); 
			}

		protected:

			/** the pointer */
			T const * ptr = nullptr;
		};

	}; // details

	/** create a delayed dynamic_cast<> */
	template<typename T>
	details::AutoCasted<T> auto_cast(T * ptr) { return details::AutoCasted<T>(ptr); }
	/** create a delayed dynamic_cast<> */
	template<typename T>
	details::AutoConstCasted<T> auto_cast(T const * ptr) { return details::AutoConstCasted<T>(ptr); }

}; // chaos
