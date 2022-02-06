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
static TYPE const & ARGNAME = chaos::ApplicationArgumentManager::GetInstance()->RegisterArgument<TYPE>(#ARGNAME, __VA_ARGS__);\
};

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ApplicationArgumentBase: the base class for stored arguments
	 */

	class ApplicationArgumentBase : public NamedObject
	{
	public:

		/** gets the class id for this entry */
		uintptr_t GetClassID() const { return class_id; }
		/** register an option into a description */
		virtual void RegisterProgramOption(boost::program_options::options_description& desc) = 0;

	protected:

		/** an id for class checking */
		uintptr_t class_id = 0;
	};

	/**
	 * ApplicationArgumentBase: the base class for stored arguments
	 */

	template<typename T>
	class ApplicationArgument : public ApplicationArgumentBase
	{
		friend class ApplicationArgumentManager;

	public:

		/** constructor */
		ApplicationArgument(T in_value = {}) : value(in_value)
		{
			class_id = meta::GetClassUniqueID<T>();
		}

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

	protected:

		/** the value where the argument is being stored */
		T value = {};
	};

	/**
	 * ApplicationArgumentManager: a singleton to manage all arguments
	 */

	class ApplicationArgumentManager : public Singleton<ApplicationArgumentManager>
	{
	public:

		/** register an argument */
		template<typename T>
		T const& RegisterArgument(char const* name, T default_value = {})
		{
			if (ApplicationArgument<T>* result = FindArgument<T>(name))
			{
				return result->value;
			}

			ApplicationArgument<T>* result = new ApplicationArgument<T>(default_value);
			assert(result != nullptr);
			result->SetName(name);
			arguments.push_back(result);
			return result->value;
		}

		/** parse all arguments */
		void ParseArguments(int argc, char** argv);

		/** gets the option string */
		char const* GetOptionString() const;

	protected:

		/** find whether an argument with given name and type is already existing */
		template<typename T>
		ApplicationArgument<T>* FindArgument(char const* name)
		{
			if (ApplicationArgumentBase* result = ObjectRequest(name).FindObject(arguments))
				if (result->GetClassID() == meta::GetClassUniqueID<T>())
					return (ApplicationArgument<T>*)result;
			return nullptr;
		}

	protected:

		/** options descriptions */
		std::string options_string;
		/** list of all arguments */
		std::vector<ApplicationArgumentBase*> arguments;
	};

#endif

}; // namespace chaos