#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/EmptyClass.h>

namespace chaos
{
	/** a class that describe an object that can be reference by tag and by name */
	class NamedObject
	{
	public:

		/** constructor */
		NamedObject() : tag(0){}

		/** the name of the object */
		std::string name;
		/** the tag of the object */
		TagType tag;
	};







	template<typename T, typename BASE = EmptyClass>
	class NamedObjectVector : public BASE
	{
	public:

#if 0
		using ref_type = boost::remove_pointer<ptr_type>::type;




		using ptr_type = typename meta::add_uniq_pointer<T>::type;

		using ref_type = boost::remove_pointer<ptr_type>::type;

		typename meta::add_uniq_pointer<T>::type;



		using const_ptr_type = typename meta::add_uniq_pointer<T>::type;

		using is_pointer_type = typename boost::is_pointer<type>::type;


		using type = T;

		using const_type = typename boost::add_const<type>::type;

		using ptr_type = typename meta::add_uniq_pointer<type>::type;


		/** destructor */
		~ElementVector(){ Clear(); }

		/** find an element by name */
		ptr_type Find(char const * name)
		{
			if (name != nullptr)
				for (auto & element : elements)
					if (MatchName(element, name))
						return ElementToPtr(element);			
			return nullptr;
		}
		/** find a const element by name */
		const_ptr_type Find(char const * name) const
		{
			if (name != nullptr)
				for (auto const & element : elements)
					if (MatchName(element, name))
						return ElementToPtr(element);
			return nullptr;
		}
		/** find an element by tag */
		ptr_type Find(TagType tag)
		{
			for (auto & element : elements)
				if (MatchTag(element, tag))
					return ElementToPtr(element);			
			return nullptr;
		}
		/** find a const element by tag */
		const_ptr_type Find(TagType tag) const
		{
			for (auto const & element : elements)
				if (MatchTag(element, tag))
					return ElementToPtr(element);		
			return nullptr;
		}
		/** find an element by tag and name */
		ptr_type Find(char const * name, TagType tag)
		{
			if (name != nullptr)
				return Find(name);
			if (tag != 0)
				return Find(tag);
			return nullptr;
		}
		/** find a const element by tag */
		const_ptr_type Find(TagType tag) const
		{
			if (name != nullptr)
				return Find(name);
			if (tag != 0)
				return Find(tag);
			return nullptr;
		}

		/** clear the list of elements */
		void Clear()
		{
			for (auto & element : elements)
				DeleteElement(element);
			elements.clear();
		}

	protected:

		/** convert an elemnt to a pointer */
		static ptr_type ElementToPtr(ptr_type element)
		{
			return element;
		}
		/** convert an elemnt to a pointer */
		static const_ptr_type ElementToPtr(const_ptr_type element)
		{
			return element;
		}
		/** convert an elemnt to a pointer */
		static ptr_type ElementToPtr(ref_type element)
		{
			return &element;
		}
		/** convert an elemnt to a pointer */
		static const_ptr_type ElementToPtr(const_ref_type element)
		{
			return &element;
		}
		/** search if element match a name */
		static bool MatchName(const_ref_type element, char const * name)
		{
			return (element.name == name);
		}
		/** search if element match a name */
		static bool MatchName(const_ptr_type element, char const * name)
		{
			return (element->name == name);
		}
		/** search if element match a tag */
		static bool MatchTag(const_ref_type element, TagType tag)
		{
			return (element.tag == tag);
		}
		/** search if element match a tag */
		static bool MatchTag(const_ptr_type element, TagType tag)
		{
			return (element->tag == tag);
		}
		/** fake destruction of elements */
		void DeleteElement(ref_type element)
		{
		}
		/** destruction of elements */
		void DeleteElement(ptr_type element) 
		{
			delete(element);
		}

	public:

		/** the elements of the arrays */
		std::vector<type> elements;

#endif

	};



}; // namespace chaos
