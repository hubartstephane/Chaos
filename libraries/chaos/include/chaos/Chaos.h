#pragma once

#include "chaos/ChaosPCH.h"

// ==========================================================================================
// Chaos inclusions
// ==========================================================================================

// first step, just forward declarations
#define CHAOS_FORWARD_DECLARATION
#include "chaos/ChaosInternals.h"
#undef CHAOS_FORWARD_DECLARATION

// second step, full declarations
#include "chaos/ChaosInternals.h"

// last step, template implementation
#define CHAOS_TEMPLATE_IMPLEMENTATION
#include "chaos/ChaosInternals.h"
#undef CHAOS_TEMPLATE_IMPLEMENTATION
