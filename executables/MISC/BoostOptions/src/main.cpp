#include <chaos/Chaos.h>

#if 0
		{ "position", VertexAttributeSemantic::POSITION },
		{ "color",      VertexAttributeSemantic::COLOR },
		{ "normal",     VertexAttributeSemantic::NORMAL },
		{ "binormal",   VertexAttributeSemantic::BINORMAL },
		{ "tangent",    VertexAttributeSemantic::TANGENT },
		{ "texcoord",   VertexAttributeSemantic::TEXCOORD },
		{ "boneindex",  VertexAttributeSemantic::BONEINDEX },
		{ "boneweight", VertexAttributeSemantic::BONEWEIGHT },
		{ "userdata",   VertexAttributeSemantic::USERDATA },


			//BOOST_TTI_HAS_MEMBER_FUNCTION(AMemberFunction)



			BOOST_TTI_HAS_MEMBER_DATA(positionX)

			BOOST_MPL_HAS_XXX_TRAIT_NAMED_DEF(has_position_tag, position, true)




			class A
		{
		public:
			typedef float position;
		};

		class A2 : public A
		{


		};

		class B
		{
		public:
			A2 positionX;
		};

		template<typename T, typename HAS_DATA = has_member_data_positionX<A2>::type>
		void FF();

		template<typename T>
		void FF<T, boost::mpl::false_>()
		{

		}

		template<typename T>
		void FF<T, boost::mpl::true_>()
		{

		}


		class P
		{
		public:

			int x;


		};


		bool b1 = has_position_tag<A>::value;
		bool b2 = has_position_tag<B>::value;

		bool b3 = has_member_data_positionX<A, float>::value;
		bool b4 = has_member_data_positionX<B, float>::value;
		bool b5 = has_member_data_positionX<B, A>::value;
		bool b6 = has_member_data_positionX<B, A2>::value;


		FF<A>();
		FF<B>();

#endif


// options
// -------
// 
// HELP
// IGNORE-DEFAULT-CONFIGFILE
// CONFIGFILE
// IGNORE-ENV
//
//
/*
void f()
{
    ProgramOptionBuilder boot_builder;    
    BootProgramOptionProvider().ProvideOptions(boot_builder);
    boot_builder.Parse(argc, argv);

    if (boot_builder.HasBooleanOption("help"))
        boot_builder.LogHelp();
    else
    {





    }

    
    
    





}
*/

class ProgramOptionCategory
{
    friend class ProgramOptionBuilder;

    ProgramOptionCategory(class ProgramOptionBuilder * in_builder, char const * category_name) : builder(in_builder), name(category_name){}

    ~ProgramOptionCategory(){}

public:

    bool AddBooleanOption(char const * option_name, char const * option_desc, bool default_value)
    {
        return true;
    }

    std::string const & GetName() const { return name;}

private:

    class ProgramOptionBuilder * builder;

    std::string name;

    
};


class ProgramOptionBuilder
{

public:

    ~ProgramOptionBuilder(){ Clear(); }

    void Clear()
    {
        for(auto cat:categories)
            delete(cat);
        categories.clear();
    }

    ProgramOptionCategory * FindCategory(char const * category_name)
    {
        for(auto cat:categories)
            if (cat->GetName() == category_name)
                return cat;
        return nullptr;
    }

    ProgramOptionCategory const * FindCategory(char const * category_name) const
    {
        for(auto cat:categories)
            if (cat->GetName() == category_name)
                return cat;
        return nullptr;
    }

    ProgramOptionCategory * AddCategory(char const * category_name)
    {
        ProgramOptionCategory * result = FindCategory(category_name);
        if (result == nullptr)
        {
            result = new ProgramOptionCategory(this, category_name);
            if (result != nullptr)
                categories.push_back(result);
        }
        return result;
    }


private:

    std::vector<ProgramOptionCategory *> categories;

};





class ProgramOptionProvider
{

};






class BootProgramOptionProvider : public ProgramOptionProvider
{
public:

    virtual bool ProvideOptions(ProgramOptionBuilder & builder)
    {
        ProgramOptionCategory * category = builder.AddCategory("generic options");
        if (category != nullptr)
            return category->AddBooleanOption("help", "Produce help message", true);
        return false;
    }


};






#if 0


class FileReader
{
public:

    virtual ~FileReader(){}
    
    virtual bool ReadFile(char const * url, size_t len, size_t offset) = 0;
};

class Win32FileReader : public FileReader
{
public:

    virtual bool ReadFile(char const * url, size_t len, size_t offset)
    {


        return true;
    }
};

class ResourceFinder
{
public:

    virtual FileReader * FindReader(char const * url);
};

class Logger
{

}
 

class Options
{
    OptionCategory * AddCategory(char const * category_name);






};




class OptionProvider
{
public:

    virtual void ProvideOptions(
};



int f()
{
    



}



#endif

// XXX le programme a du mal a comprendre les arguments negatifs

// -mavaleur -10 



//printf("%s(Oct) is %i(Dec)\n", num, strtol(num, NULL,  8));
//printf("%s(Dec) is %i(Dec)\n", num, strtol(num, NULL, 10));
//printf("%s(hex) is %i(Dec)\n", num, strtol(num, NULL, 16));

class TTT
{
public:

    TTT():value(0){}


    int value;
};

void validate(boost::any& v,
              const std::vector<std::string>& values,
              TTT*t, int k)  {
               //   const std::string& s = validators::get_single_string(values);
               //   v = any(my_type1(lexical_cast<int>(s)));

                  TTT x;
                  x.value = 777;

                  v = boost::any(x);

                  

                  k = k;
                  ++k;
}

std::pair<std::string, std::string> reg_foo(const std::string& s)
{
    if (s.find("-f") == 0) {
        if (s.substr(2, 3) == "no-")
            return std::make_pair(s.substr(5), std::string("false"));
        else
            return std::make_pair(s.substr(2), std::string("true"));
    } else {
        return std::make_pair(std::string(), std::string());
    }
}


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  boost::filesystem::path p1 = "c:/toto/titi.txt";
  boost::filesystem::path p2 = "c:/toto/../toto/titi.txt";

  boost::filesystem::path p3 = p2.branch_path();
  boost::filesystem::path p4 = p2.generic();
  
  boost::filesystem::path p5 = p2.make_preferred();

  boost::filesystem::path p6 = p2.remove_trailing_separator();

  boost::filesystem::path p7 = p2.stem();


  std::string s1 = p1.string();
  std::string s2 = p2.string();
  std::string s3 = p3.string();
  std::string s4 = p4.string();
  std::string s5 = p5.string();
  std::string s6 = p6.string();
  std::string s7 = p7.string();

  boost::filesystem::path a = "c:/titi.txt";
  boost::filesystem::path b = "titi.txt";
  boost::filesystem::path c = "../truc/titi.txt";

  bool a1 = a.is_absolute();
  bool a2 = a.is_complete();
  bool a3 = a.is_relative();

  bool b1 = b.is_absolute();
  bool b2 = b.is_complete();
  bool b3 = b.is_relative();

  bool c1 = c.is_absolute();
  bool c2 = c.is_complete();
  bool c3 = c.is_relative();


  
  if (p1 == p2)
    env = env;





  return 0;











    long l = strtol("0x17", NULL,  16);

    l =l;

#if 0
    if (env != nullptr)
    {
        int i = 0;
        while (env[i] != nullptr)
        {
            char * e = env[i];

            std::cout << "ENV[" << i << "] = [" << e << "]" << std::endl;
            ++i;
        }
    }
#endif

    std::cout << "==============================" << std::endl;

    // NOTE : si une option a une valeur, et qu elle n est pas specifiee => crash
    //          --mon_option_int    
    //        sauf si on utilise   implicit_value        

    //        si c est pas le bon type (int attendu, string en entree) => crash
    //
    //        pour les vector<string>, si on ne mets pas multitoken, un seul element est mangé


    boost::program_options::options_description gen_options("general options");
    gen_options.add_options()
        ("help", "produce help message");


    boost::program_options::options_description gen_options2("general options");
    gen_options2.add_options()
        ("help2", "produce help message2");



    // XXX : on peut utiliser des noms longs & court : "compression1,a"
    //       ATTENTION, le nom court contient 1 seul charactere

    // voir ->composing pour les vector

    boost::program_options::options_description other_options("other options");
    other_options.add_options()
        ("compression1,a", boost::program_options::value<int>()->default_value(20)->implicit_value(10), "set compression level")


        ("compression5,t", boost::program_options::value<TTT>(), "set compression level")

        ("compression2", boost::program_options::value<std::string>()->implicit_value("toto"), "set compression level")
        ("compression3", boost::program_options::value<std::vector<std::string>>()->multitoken(), "set compression level")
        ("compression4", boost::program_options::value<std::vector<std::string>>()->multitoken(), "set compression level");

    boost::program_options::positional_options_description pos_options;
    pos_options.add("compression3", 2);  // take the 2 first positionals
    pos_options.add("compression4", -1); // redirect position argument to other entries


    boost::program_options::options_description all_options("all options"); // creer une ligne vide dans l'aide !!
    all_options.add(gen_options).add(gen_options2).add(other_options);




    boost::program_options::variables_map vm;
    

    try{

    boost::program_options::parsed_options parsed = 
        boost::program_options::command_line_parser(argc, argv).
        extra_parser(reg_foo).
        options(all_options).
        positional(pos_options).
        allow_unregistered().
        run();   
    boost::program_options::store(parsed, vm);
    boost::program_options::notify(vm);        

    /* Lance exception en cas d' option inconnue !!!
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
    boost::program_options::notify(vm);    
    */

    // all options detected that means nothing
    std::vector<std::string> to_pass_further = boost::program_options::collect_unrecognized(parsed.options, boost::program_options::exclude_positional /*include_positional*/);
    for (size_t i = 0 ; i < to_pass_further.size() ; ++i)
        std::cout << "to_pass_further[" << i << "] = " << to_pass_further[i] << std::endl;











    if (vm.count("help"))
    {
        std::cout << all_options << std::endl;
    }

    if (vm.count("toto"))
    {
        std::cout << "toto present" << std::endl;

    }

    if (vm.count("compression1"))
    {
        int value = vm["compression1"].as<int>();

        std::cout << "compression1 = " << value << std::endl;
        
    }

    if (vm.count("compression2"))
    {
        std::string value = vm["compression2"].as<std::string>();

        std::cout << "compression2 = " << value << std::endl;

    }

    if (vm.count("compression3"))
    {
        std::vector<std::string> value = vm["compression3"].as<std::vector<std::string>>();

        for (size_t i = 0 ; i < value.size() ; ++i)
            std::cout << "compression3[" << i << "] = " << value[i] << std::endl;
    }

    if (vm.count("compression4"))
    {
        std::vector<std::string> value = vm["compression4"].as<std::vector<std::string>>();

        for (size_t i = 0 ; i < value.size() ; ++i)
            std::cout << "compression4[" << i << "] = " << value[i] << std::endl;

    }


    if (vm.count("compression5"))
    {
        TTT value = vm["compression5"].as<TTT>();

        std::cout << "compression5 = " << value.value << std::endl;

    }











    }catch(std::exception & e)
    {
        std::cout << "exception " << e.what() << std::endl;
    }

    std::cin >> argc;

  return 0;
}

