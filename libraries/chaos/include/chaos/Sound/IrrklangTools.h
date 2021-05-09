#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

/** boost/chaos smart pointer adapter */
extern void intrusive_ptr_add_ref(irrklang::IVirtualRefCounted* obj);
/** boost/chaos smart pointer adapter */
extern void intrusive_ptr_release(irrklang::IVirtualRefCounted* obj);
/** boost/chaos smart pointer adapter */
extern void intrusive_ptr_add_ref(irrklang::IRefCounted* obj);
/** boost/chaos smart pointer adapter */
extern void intrusive_ptr_release(irrklang::IRefCounted* obj);

#endif