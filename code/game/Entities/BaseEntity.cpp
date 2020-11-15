#include "Maths/Vector.hpp"

#include "Game/g_local.hpp"
#include "BaseEntity.hpp"
#include "BasePlayer.hpp"
#include "Entities/KeyValueElement.hpp"
#include "Game/GameWorld.hpp"
#include "../qcommon/IEngineExports.h"
#include "Game/IGameImports.h"
#include "Components/IComponent.hpp"
#include "Components/SharedComponent.hpp"

using namespace Entities;
using namespace Components;

KeyValueElement BaseQuakeEntity::keyValues[] =
{
	//KeyValueElement( "origin",	0,												KVHandlers::Vector ),
	//KeyValueElement( "angles",	0,												KVHandlers::Vector ),
	KeyValueElement( "spawnflags",	offsetof( BaseQuakeEntity, spawnFlags ),		KVHandlers::Int ),
	KeyValueElement( "spawnflags2", offsetof( BaseQuakeEntity, spawnFlagsExtra ),	KVHandlers::Int ),
	KeyValueElement()
};

void BaseQuakeEntity::Spawn()
{
	auto comp = CreateComponent<SharedComponent>();
	const char* model = spawnArgs->GetCString( "model", nullptr );

	comp->origin = spawnArgs->GetVector( "origin", Vector(0, 0, 64) );
	comp->angles = spawnArgs->GetVector( "angles", Vector::Zero );
	comp->entityIndex = GetEntityIndex();

	engine->Print( va( "Spawned a BaseQuakeEntity at %i\n", GetEntityIndex() ) );

	if ( !model )
	{
		engine->Print( "This entity has no model\n" );
	}
	else
	{
		engine->Print( "Setting model " );
		engine->Print( model );
		engine->Print( "\n" );

		if ( model[0] == '*' )
		{
			gameImports->SetBrushModel( this, model );
		}

		else
		{
			engine->Print( "This is not a brush model, not yet supported\n" );
		}
	}
}

void BaseQuakeEntity::PreKeyValue()
{

}

void BaseQuakeEntity::PostKeyValue()
{
	
}

void BaseQuakeEntity::KeyValue()
{
	auto& map = spawnArgs->GetMap();

	PreKeyValue();

	// For every keyvalue string pair
	for ( auto& keyValue : map )
	{
		// Check to see if we can dispatch any of our keyvalues on it
		for ( auto& element : keyValues )
		{
			// Skip this element if it's already handled
			if ( element.IsHandled() )
				continue;

			// Try dispatching the keyvalue
			int result = element.KeyValue( keyValue.first.c_str(), keyValue.second.c_str(), this );
		
			// If it's a success, then stop processing this specific keyvalue
			if ( result == KVHandlers::Success )
				break;
		}
	}

	PostKeyValue();
}

void BaseQuakeEntity::Think()
{
	if ( nullptr == thinkFunction )
	{
		return;
	}

	if ( nextThink > (level.time * 0.001f) )
	{
		return;
	}

	(this->*thinkFunction)();
}

const char* BaseQuakeEntity::GetName() const
{
	return targetName.c_str();
}

const char* BaseQuakeEntity::GetClassname() const
{
	return className.c_str();
}

const char* BaseQuakeEntity::GetTarget() const
{
	return target.c_str();
}

const Vector& BaseQuakeEntity::GetOrigin() const
{
	return Vector( shared.s.origin[0], shared.s.origin[1], shared.s.origin[2] );
}

void BaseQuakeEntity::SetOrigin( const Vector& newOrigin )
{
	shared.s.origin[0] = newOrigin[0];
	shared.s.origin[1] = newOrigin[1];
	shared.s.origin[2] = newOrigin[2];
}

const Vector& BaseQuakeEntity::GetAngles() const
{
	return Vector( shared.s.angles[0], shared.s.angles[1], shared.s.angles[2] );
}

void BaseQuakeEntity::SetAngles( const Vector& newAngles )
{
	shared.s.angles[0] = newAngles[0];
	shared.s.angles[1] = newAngles[1];
	shared.s.angles[2] = newAngles[2];
}

const Vector& BaseQuakeEntity::GetVelocity() const
{
	shared.s.pos.trDelta;
}

void BaseQuakeEntity::SetVelocity( const Vector& newVelocity )
{

}

const Vector& BaseQuakeEntity::GetMins() const
{
	// TODO: insert return statement here
}

const Vector& BaseQuakeEntity::GetMaxs() const
{
	// TODO: insert return statement here
}

const Vector& BaseQuakeEntity::GetAverageOrigin() const
{
	const Vector& mins = GetMins();
	const Vector& maxs = GetMaxs();
	return (mins + maxs) / 2.0f;
}

const int& BaseQuakeEntity::GetSpawnflags() const
{
	return spawnFlags;
}

void BaseQuakeEntity::SetSpawnflags( int flags )
{
	spawnFlags = flags;
}

void BaseQuakeEntity::UseTargets( IEntity* activator )
{
	UseTargets( activator, "target" );
}

void BaseQuakeEntity::UseTargets( IEntity* activator, const char* targetName )
{
	BaseQuakeEntity a;
}

void BaseQuakeEntity::KillBox( bool onlyPlayers = false )
{
	int	i, num;
	int	touch[MAX_GENTITIES];
	IEntity* hit;
	Vector mins, maxs;

	mins = GetOrigin();
	maxs = mins;

	mins += Vector( (float*)shared.r.mins );
	maxs += Vector( (float*)shared.r.maxs );

	gameImports->EntitiesInBox( mins, maxs, touch, GameWorld::MaxEntities );

	for ( i = 0; i < num; i++ ) 
	{
		hit = gEntities[touch[i]];
		
		if ( nullptr == hit )
			continue;

		if ( onlyPlayers )
		{
			if ( nullptr == dynamic_cast<BasePlayer*>(hit) )
			{
				continue;
			}
		}

		// nail it
		hit->TakeDamage( this, this, DAMAGE_NO_PROTECTION, 100000 );
	}
}

void BaseQuakeEntity::KillBox( const Vector& size, bool onlyPlayers = false )
{
	int	i, num;
	int	touch[MAX_GENTITIES];
	IEntity* hit;
	Vector mins, maxs;

	mins = GetOrigin();
	maxs = mins;

	mins += size / 2.0f;
	maxs += size / 2.0f;

	gameImports->EntitiesInBox( mins, maxs, touch, GameWorld::MaxEntities );

	for ( i = 0; i < num; i++ )
	{
		hit = gEntities[touch[i]];

		if ( nullptr == hit )
			continue;

		if ( onlyPlayers )
		{
			if ( nullptr == dynamic_cast<BasePlayer*>(hit) )
			{
				continue;
			}
		}

		// nail it
		hit->TakeDamage( this, this, DAMAGE_NO_PROTECTION, 100000 );
	}
}

void BaseEntity_Test::Spawn()
{
	SetThink( &BaseEntity_Test::MyThink );
}

void BaseEntity_Test::MyThink()
{
	static float cycle = 0.0f;
	static float originalZ = 0;
	static float iterator = 0.008f;

	cycle += iterator;
	if ( fabs( cycle ) > 1.000f )
		iterator *= -1.0f;
}
