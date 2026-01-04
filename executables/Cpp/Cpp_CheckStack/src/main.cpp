#include <chaos/chaos.h>

bool IsOnStack(void* p)
{
	ULONG_PTR stack_low = 0;
	ULONG_PTR stack_high = 0;
	GetCurrentThreadStackLimits(&stack_low, &stack_high);

	return uint64_t(p) >= uint64_t(stack_low) && uint64_t(p) < uint64_t(stack_high);
}

int main(int argc, char** argv, char** env)
{
	assert(IsOnStack(&argc));
	assert(!IsOnStack(new int));
	return 0;
}