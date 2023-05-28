#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	BufferBase::~BufferBase()
	{
		if (policy != nullptr)
			policy->DestroyBuffer(this);
	}

	BufferPolicyBase* BufferBase::GetPolicy()
	{
		return policy;
	}

	BufferPolicyBase const* BufferBase::GetPolicy() const
	{
		return policy;
	}

	void BufferBase::SetPolicy(BufferPolicyBase* in_policy)
	{
		policy = in_policy;
	}

	void BufferBase::CopyFromBuffer(BufferBase const* src)
	{
		src->policy->CopyBuffer(this, src);
	}

}; // namespace chaos
