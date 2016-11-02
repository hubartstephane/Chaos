#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Metaprogramming.h>
#include <chaos/EmptyClass.h>

namespace chaos
{


#if 0





	namespace TextureAtlas
	{
		using TagType = intptr_t;

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

		class BitmapEntry : public NamedObject
		{
			/** the atlas in which it is stored in result */
			size_t bitmap_index;
			/** the top-left corner of the texture */
			int    x;
			/** the top-left corner of the texture */
			int    y;
			/** the size of the texture (beware, 2 x padding must be add for correct result) */
			int    width;
			/** the size of the texture (beware, 2 x padding must be add for correct result) */
			int    height;
		};

		class FontEntry : public BitmapEntry
		{
		public:
			FT_Vector advance;
			int       bitmap_left; // from 'CharacterMetrics' class
			int       bitmap_top;
		};

		class AtlasEntryBase : public NamedObject
		{
		public:

			virtual ~AtlasEntryBase(){};		
		};








		template<typename T>
		class Entry : public NamedObject
		{
		public:

			using type = T;



			
			std::vector<type> elements;
		};

		using BitmapSetEntry = Entry<BitmapEntry>;
		using FontEntry      = Entry<FontEntry>;




		class Atlas
		{

		public:

			virtual ~Atlas(){}


		public:
			
			std::vector<AtlasEntryBase*> entries;

		};


		using BitmapSetEntry = ElementList<BitmapEntry, AtlasEntryBase>;
		using FontEntry      = ElementList<BitmapEntry, AtlasEntryBase>;


		class Atlas : public ElementList<AtlasEntryBase *, EmptyClass>
		{

		}

		class BitmapSetInput
		{
		public:

			AddBitmap();		
		};

		class FontInput
		{
		public:

			AddFont();		
		};

		class AtlasInput
		{
		public:

			BitmapSetInput * NewBitmapSet(char const * name, TypeType * tag)
			{
				// test whether entry is already existing
				if (name != nullptr && bitmaps.Find(name) != nullptr) 
					return nullptr;
				if (tag != 0 && bitmaps.Find(name) != nullptr) 
					return nullptr;

				BitmapSetInput * result = new BitmapSetInput(name, tag);
				if (result != nullptr)
				{
					bitmaps.push_back(result);

				}
				return result;			
			}

			FontInput * NewFont()

		protected:

			ElementList<BitmapSetInput*> bitmaps;
			ElementList<FontInput*>      fonts;
		};













	};



#endif // shu




#if 0

  namespace TextureAtlas
  {




    class AtlasDataSetBase : public ClassBase<bool, false>
    {


    };

    template<typename T>
    class AtlasDataSet : public AtlasDataSetBase
    {


    };

    class BitmapDataSet : public AtlasDataSet<BitmapEntry>
    {


    };

    class FontDataSet : public AtlasDataSet<FontEntry>
    {


    };




    class Atlas
    {


      public:



        std::vector<FIBITMAP *> bitmaps;

        std::vector<

    };



  };



  using TagType = intptr_t;


  





  template<typename T>
  class AutoDestroyObjectArray
  {
  public:

    using type = T;
    
    using const_type = typename boost::add_const<type>::type;

    using ptr_type = typename meta::add_uniq_pointer<type>::type;

    using const_ptr_type = typename meta::add_uniq_pointer<const_type>::type;

    using is_pointer_type = typename boost::is_pointer<type>::type;

    /** destructor */
    ~AutoDestroyObjectArray()
    {
      Clear(true);
    }

    /** destroy the whole content */
    void Clear(bool release = true)    
    {
      if (release)
        DestroyElements(is_pointer_type());
      elements.clear();
    }



  protected:

    /** the full destruction method */
    void DestroyElements(boost::mpl::true_)
    {
      for (auto elem : elements)
        delete(elem);
    }

    /** empty implementation for destruction */
    void DestroyElements(boost::mpl::false_){}
  };


  template<typename T>
  class NamedObjectArray : public AutoDestroyObjectArray<T>
  {
  public:

    using type = typename AutoDestroyObjectArray<T>::type;

    using const_type = typename AutoDestroyObjectArray<T>::const_type;

    using ptr_type = typename AutoDestroyObjectArray<T>::ptr_type;

    using const_ptr_type = typename AutoDestroyObjectArray<T>::const_ptr_type;

    ptr_type GetElementByName(char const * name)
    {
      if (name != nullptr)
      {
        size_t count = elements.size();
        for(size_t i = 0 ; i < count ; ++i)
          if (MatchName(elements[i], name))
            return ElemToPointer(elements[i]);
      }
      return nullptr;
    }

    const_ptr_type GetElementByName(char const * name) const
    {
      if (name != nullptr)
      {
        size_t count = elements.size();
        for (size_t i = 0 ; i < count ; ++i)
          if (MatchName(elements[i], name))
            return ElemToPointer(elements[i]);
      }
      return nullptr;
    }

    ptr_type GetElementByTag(TagType tag)
    {
      size_t count = elements.size();
      for (size_t i = 0 ; i < count ; ++i)
        //if (MatchTag(elements[i], tag))
          return ElemToPointer(elements[i]);
      return nullptr;
    }

    const_ptr_type GetElementByTag(TagType tag) const
    {
      size_t count = elements.size();
      for (size_t i = 0 ; i < count ; ++i)
    //    if (MatchTag(elements[i], tag))
          return ElemToPointer(elements[i]);
      return nullptr;
    }

  protected:



    bool MatchName(const_type & elem, char const * name) const
    {
      return (elem.name == name);
    }

    bool MatchName(const_ptr_type elem, char const * name) const
    {
      return (elem->name == name);
    }

    bool MatchTag(const_type & elem, TagType tag) const
    {
      return (elem.tag == tag);
    }

    bool MatchTag(const_ptr_type elem, TagType tag) const
    {
      return (elem->tag == tag);
    }






    ptr_type ElemToPointer(ptr_type elem)
    {
      return elem;
    }

    ptr_type ElemToPointer(type & elem)
    {
      return &elem;
    }

    const_ptr_type ElemToPointer(const_ptr_type elem) const
    {
      return elem;
    }

    const_ptr_type ElemToPointer(const_type & elem) const
    {
      return &elem;
    }
  };



#endif










  /**
   * TextureAtlasEntry : class to represents an entry inside the atlas
   */

  class TextureAtlasEntry
  {
  public:

    /** the name of the texture */
    std::string name;   
    /** the atlas in which it is stored in result */
    size_t      atlas;
    /** the top-left corner of the texture */
    int         x;
    /** the top-left corner of the texture */
    int         y;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         width;
    /** the size of the texture (beware, 2 x padding must be add for correct result) */
    int         height;
  };

  void SaveIntoJSON(TextureAtlasEntry const & entry, nlohmann::json & json_entry);

  void LoadFromJSON(TextureAtlasEntry & entry, nlohmann::json const & json_entry);

  /**
   * TextureAtlasBase : base class for FontAtlas / TextureAtlas
   */

  class TextureAtlasBase
  {
  public:

    /** clear all the textures */
    void Clear();
    /** get the number of atlas */
    size_t GetAtlasCount() const { return bitmaps.size(); }
    /** get the size of bitmap composing the atlas */
    glm::ivec2 GetAtlasDimension() const;

    /** function to save the results */
    bool SaveAtlas(boost::filesystem::path const & filename) const;
    /** load an atlas from an index file */
    bool LoadAtlas(boost::filesystem::path const & filename);
    /** load an atlas from a json object */
    bool LoadAtlas(nlohmann::json const & j, boost::filesystem::path const & src_dir);

  protected:

    /** split a filename into DIRECTORY, INDEX_FILENAME and IMAGE prefix path */
    void TextureAtlasBase::SplitFilename(boost::filesystem::path const & filename, boost::filesystem::path & target_dir, boost::filesystem::path & index_filename, boost::filesystem::path & image_filename) const;
    /** get the name of an atlas image */
    boost::filesystem::path GetAtlasImageName(boost::filesystem::path image_filename, int index) const;

    /** function to save images */
    bool SaveAtlasImages(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const;
    /** function to save images */
    bool SaveAtlasIndex(boost::filesystem::path const & target_dir, boost::filesystem::path const & index_filename, boost::filesystem::path const & image_filename) const;
    /** save all entries inside a json object */
    virtual void SaveAtlasEntriesInIndex(nlohmann::json & json_entries) const {};

    /** load one entry from json object and insert it into entries array */
    virtual void LoadAtlasEntryFromIndex(nlohmann::json const & json_entry) {};

  public:

    /** all the image for the output */
    std::vector<FIBITMAP *> bitmaps;
  };

  /**
  * TextureAtlasTypedBase : base class for FontAtlas / TextureAtlas
  */

  template<typename ENTRY_TYPE>
  class TextureAtlasTypedBase : public TextureAtlasBase
  {
  public:

    using entry_type = ENTRY_TYPE;

    /** constructor */
    TextureAtlasTypedBase() {}
    /** destructor */
    ~TextureAtlasTypedBase() { Clear(); }

    /** clear all the textures */
    void Clear()
    {
      // super method
      TextureAtlasBase::Clear();
      // destroy the entries
      entries.empty();
    }

  protected:

    virtual void SaveAtlasEntriesInIndex(nlohmann::json & json_entries) const override
    {
      for (size_t i = 0; i < entries.size(); ++i)
      {
        auto json_entry = nlohmann::json();
        SaveIntoJSON(entries[i], json_entry);
        json_entries.push_back(json_entry);
      }
    }

    virtual void LoadAtlasEntryFromIndex(nlohmann::json const & json_entry) override
    {
      entry_type entry;
      LoadFromJSON(entry, json_entry);
      entries.push_back(entry);
    }

  public:

    /** all the textures for the input */
    std::vector<entry_type> entries;
  };

  /**
   * TextureAtlas : the result computation of TextureAtlasGenerator
   */

  class TextureAtlas : public TextureAtlasTypedBase<TextureAtlasEntry>
  {
    friend class TextureAtlasGenerator;
    friend class TextureAtlasHTMLGenerator;

  public:

    /** returns the surface for an atlas */
    float ComputeSurface(size_t atlas_index) const;

    /** display information about all textures */
    void OutputTextureInfo(std::ostream & stream) const;
    /** display information about one texture */
    void OutputTextureInfo(TextureAtlasEntry const & entry, std::ostream & stream) const;

    /** display information about all textures */
    std::string GetTextureInfoString() const;
    /** display information about one texture */
    std::string GetTextureInfoString(TextureAtlasEntry const & entry) const;

  protected:

    /** get a string with the general information */
    std::string GetGeneralInformationString() const;
    /** get a string with the surface occupation of all atlas */
    std::string GetAtlasSpaceOccupationString() const;
    /** get a string with the surface occupation of one atlas */
    std::string GetAtlasSpaceOccupationString(size_t atlas_index) const;
    /** display the surface occupation of all atlas */
    void OutputAtlasSpaceOccupation(std::ostream & stream = std::cout) const;
    /** display the surface occupation of all atlas */
    void OutputAtlasSpaceOccupation(size_t atlas_index, std::ostream & stream = std::cout) const;
    /** display the general information if the atlas */
    void OutputGeneralInformation(std::ostream & stream = std::cout) const;
  };
};
