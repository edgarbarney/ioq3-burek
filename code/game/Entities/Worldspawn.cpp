#include "Game/g_local.hpp"
#include "BaseEntity.hpp"
#include "Entities/KeyValueElement.hpp"
#include "Game/GameWorld.hpp"
#include "../qcommon/IEngineExports.h"
#include "Game/IGameImports.h"
#include "Maths/Vector.hpp"
#include "Components/IComponent.hpp"

#include "Worldspawn.hpp"

using namespace Entities;

void Worldspawn::Spawn()
{
	float gravity = spawnArgs->GetFloat( "gravity", 800 );
	gameImports->ConsoleVariable_Set( "g_gravity", va( "%f", gravity ) );

	bool enableDust = spawnArgs->GetBool( "enableDust", false );
	gameImports->ConsoleVariable_Set( "g_enableDust", va( "%d", (int)enableDust ) );

	bool enableBreath = spawnArgs->GetBool( "enableBreath", false );
	gameImports->ConsoleVariable_Set( "g_enableBreath", va( "%d", (int)enableBreath ) );
}