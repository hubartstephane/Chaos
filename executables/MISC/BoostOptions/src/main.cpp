#include <chaos/Chaos.h>

// options
// -------
// 
// HELP
// IGNORE-DEFAULT-CONFIGFILE
// CONFIGFILE
// IGNORE-ENV
//
//

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
        ("A,a", boost::program_options::value<int>()->default_value(20)->implicit_value(10), "set compression level")
        ("B,t", boost::program_options::value<TTT>(), "set compression level")
        ("C", boost::program_options::value<std::string>()->implicit_value("toto"), "set compression level")
        ("D", boost::program_options::value<std::vector<std::string>>()->multitoken(), "set compression level");

    boost::program_options::positional_options_description pos_options;
  //  pos_options.add("E", 2);  // take the 2 first positionals
  //  pos_options.add("F", -1); // redirect position argument to other entries


    boost::program_options::options_description all_options("all options"); // creer une ligne vide dans l'aide !!
    all_options.add(gen_options).add(gen_options2).add(other_options);




    boost::program_options::variables_map vm;
    

    try{

    boost::program_options::parsed_options parsed = 
        boost::program_options::command_line_parser(argc, argv).
       // extra_parser(reg_foo).
        options(all_options).
 //       positional(pos_options).
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

    if (vm.count("A"))
    {
        int value = vm["A"].as<int>();

        std::cout << "A = " << value << std::endl;
        
    }

    if (vm.count("C"))
    {
        std::string value = vm["C"].as<std::string>();

        std::cout << "C = " << value << std::endl;

    }

    if (vm.count("D"))
    {
        std::vector<std::string> value = vm["D"].as<std::vector<std::string>>();

        for (size_t i = 0 ; i < value.size() ; ++i)
            std::cout << "D[" << i << "] = " << value[i] << std::endl;
    }



    if (vm.count("B"))
    {
        TTT value = vm["B"].as<TTT>();

        std::cout << "B = " << value.value << std::endl;

    }











    }catch(std::exception & e)
    {
        std::cout << "exception " << e.what() << std::endl;
    }

    std::cin >> argc;

  return 0;
}

