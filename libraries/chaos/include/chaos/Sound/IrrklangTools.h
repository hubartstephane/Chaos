#ifdef CHAOS_FORWARD_DECLARATION


#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else

/** boost/chaos smart pointer adapter */
extern void intrusive_ptr_add_ref(irrklang::IVirtualRefCounted* obj);
/** boost/chaos smart pointer adapter */
extern void intrusive_ptr_release(irrklang::IVirtualRefCounted* obj);
/** boost/chaos smart pointer adapter */
extern void intrusive_ptr_add_ref(irrklang::IRefCounted* obj);
/** boost/chaos smart pointer adapter */
extern void intrusive_ptr_release(irrklang::IRefCounted* obj);

namespace chaos
{
	/** convert a glm vector into an irrklang vector */
	irrklang::vec3df ToIrrklangVector(glm::vec3 const& src);

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION