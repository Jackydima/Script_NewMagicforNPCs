#pragma once

#include "Script.h"
#include "util/Memory.h"
#include "util/Logging.h"
#include "util/Hook.h"
#include "Script.h"
#include "g3MagicSpells.h"
#include <iostream>
#include <time.h>  

enum MageType {
	MageType_Blackmage,
	MageType_Firemage,
	MageType_Watermage,
	MageType_None,
};

GEBool GE_STDCALL hasMember ( Entity* entityMem );
MageType getMageType ( Entity* p_entity );