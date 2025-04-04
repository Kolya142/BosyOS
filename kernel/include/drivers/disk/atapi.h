#pragma once
#include <arch/x86/cpu/io.h>
#include <lib/Types.h>

Bool ATAPIRead(Bool slave, Ptr buf, U32 start);