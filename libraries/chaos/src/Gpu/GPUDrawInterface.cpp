#include <chaos/chaos.h>

namespace chaos
{

template<typename VERTEX_TYPE>
GPUVertexDeclaration* GPUDrawInterface<VERTEX_TYPE>::GetVertexDeclaration()
{
	static shared_ptr<GPUVertexDeclaration> result;
	if (result == nullptr)
	{
		result = new GPUVertexDeclaration;
		::GetTypedVertexDeclaration(result.get(), boost::mpl::identity<VertexDefault>());
	}
	return result.get();
}

}; // namespace chaos

