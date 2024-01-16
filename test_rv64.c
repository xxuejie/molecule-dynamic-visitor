/* A minimal file to test rv64 build */

#define MDP_DEBUG(...)
void exit(int);
#define CKB_C_STDLIB_PRINTF
#define MDP_VSNPRINTF vsnprintf_

#include "clib/molecule-dynamic-visitor.h"
