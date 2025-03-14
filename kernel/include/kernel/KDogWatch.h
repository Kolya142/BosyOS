#pragma once
#include <lib/Types.h>

U0 KDogWatchInit();
U0 KDogWatchLog(const String str, Bool panic);
U0 KDogWatchLogF(const String format, ...);
U0 KDogWatchTick();
U0 KDogWatchPStart(U8 id, const String name);
U0 KDogWatchPTick(U8 id);
U0 KDogWatchPEnd(U8 id);
U0 KDogWatchPPlay(U8 id);