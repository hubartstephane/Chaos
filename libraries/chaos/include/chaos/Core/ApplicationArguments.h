namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ApplicationArgumentBase;
	class ApplicationArgumentManager;

	template<typename T>
	class ApplicationArgument;

#define CHAOS_APPLICATION_ARG(TYPE, ARGNAME, ...)\
namespace Arguments\
{\
static inline chaos::ApplicationArgument<TYPE> const & ARGNAME = *chaos::ApplicationArgumentManager::GetInstance()->RegisterArgument<TYPE>(#ARGNAME, __VA_ARGS__);\
};

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ApplicationArgumentBase: the base class for stored arguments
	 */

	class CHAOS_API ApplicationArgumentBase : public NamedObject
	{
	public:

		/** gets the type_info for this entry */
		std::type_info const * GetTypeInfo() const { return type_info; }
		/** register an option into a description */
		virtual void RegisterProgramOption(boost::program_options::options_description& desc) = 0;

	protected:

		/** an id for class checking */
		std::type_info const* type_info = nullptr;
	};

	/**
	 * ApplicationArgumentBase: the base class for stored arguments
	 */

	template<typename T>
	class /*CHAOS_API*/ ApplicationArgument : public ApplicationArgumentBase
	{
		friend class ApplicationArgumentManager;

	public:

		/** constructor */
		ApplicationArgument(T in_value = {}) : value(in_value)
		{
			type_info = &typeid(T);
		}

		/** destructor */
		virtual ~ApplicationArgument() = default;

		/** override */
		virtual void RegisterProgramOption(boost::program_options::options_description& desc) override
		{
			if constexpr (meta::is_vector_type_v<T>)
			{
				// zero_tokens
				//   accepts empty lists as
				//     --PARAM
				// composing
				//    --PARAM a b c --PARAM d e
				//   is equivalent to
				//    --PARAM a b c d e
				desc.add_options()
					(GetName(), boost::program_options::value<T>(&value)->multitoken()->zero_tokens()->composing(), "");
			}
			else if constexpr (std::is_same_v<T, bool>)
			{
				// implicit value:
				//     --PARAM
				//   is equivalent to
				//     --PARAM true
				desc.add_options()
					(GetName(), boost::program_options::value<T>(&value)->implicit_value(true), "");
			}
			else
			{
				desc.add_options()
					(GetName(), boost::program_options::value<T>(&value), "");
			}
		}

		/** const getter */
		T const& Get() const
		{
			return value;
		}
		/** getter */
		T& Get()
		{
			return value;
		}

	protected:

		/** the value where the argument is being stored */
		T value = {};
	};

	/**
	 * ApplicationArgumentManager: a singleton to manage all arguments
	 */

	class CHAOS_API ApplicationArgumentManager : public Singleton<ApplicationArgumentManager>
	{
	public:

		/** register an argument */
		template<typename T>
		ApplicationArgument<T> const * RegisterArgument(char const* name, T default_value = {})
		{
			// an argument with the same name already exists ?
			if (ApplicationArgumentBase* result = ObjectRequest(name).FindObject(arguments))
			{
				// the type matches ?
				// (you may use CHAOS_APPLICATION_ARG(XXX) several times with the same string in several places. They will all point the same data)
				if (*result->GetTypeInfo() == typeid(T))
				{
					return dynamic_cast<ApplicationArgument<T>*>(result);
				}
				else
				{
					assert(0);
					return nullptr;
				}
			}
			// create an new entry
			ApplicationArgument<T>* result = new ApplicationArgument<T>(default_value);
			assert(result != nullptr);
			result->SetName(name);
			arguments.push_back(result);
			return result;
		}

		/** parse all arguments */
		void ParseArguments(int argc, char** argv);

		/** gets the option string */
		char const* GetOptionString() const;

	protected:

		/** options descriptions */
		std::string options_string;
		/** list of all arguments */
		std::vector<ApplicationArgumentBase*> arguments;
	};

#endif

}; // namespace chaos