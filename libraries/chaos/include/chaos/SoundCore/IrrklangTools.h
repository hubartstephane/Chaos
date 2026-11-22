namespace irrklang
{

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** boost/chaos smart pointer adapter */
	CHAOS_API void IntrusiveAddSharedReference(IVirtualRefCounted* obj);
	/** boost/chaos smart pointer adapter */
	CHAOS_API void IntrusiveSubSharedReference(IVirtualRefCounted* obj);
	/** boost/chaos smart pointer adapter */
	CHAOS_API void IntrusiveAddSharedReference(IRefCounted* obj);
	/** boost/chaos smart pointer adapter */
	CHAOS_API void IntrusiveSubSharedReference(IRefCounted* obj);

#endif

}; // namespace irrklang

