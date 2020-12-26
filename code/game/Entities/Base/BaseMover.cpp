#include "Maths/Vector.hpp"

#include "Game/g_local.hpp"
#include "Entities/BaseEntity.hpp"
#include "Entities/KeyValueElement.hpp"
#include "Game/GameWorld.hpp"
#include "../qcommon/IEngineExports.h"
#include "Game/IGameImports.h"
#include "Entities/BasePlayer.hpp"

#include "BaseMover.hpp"

using namespace Entities;

DefineAbstractEntityClass( BaseMover, BaseQuakeEntity );

pushed_t pushed[MAX_GENTITIES];
pushed_t* pushed_p;

void BaseMover::Think()
{
	if ( CheckAndClearEvents() )
		return;

	// Temp ents don't think
	if ( freeAfterEvent )
		return;

	if ( !shared.r.linked && neverFree )
		return;

	MoverThink();

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

void BaseMover::MoverThink()
{
	// If stationary at one of the positions, MOVE
	if ( GetState()->pos.trType == TR_STATIONARY && GetState()->apos.trType == TR_STATIONARY )
		return;

	Vector		move, amove;
	BaseQuakeEntity* part, * obstacle;
	Vector		origin, angles;

	obstacle = nullptr;

	// make sure all team slaves can move before committing
	// any moves or calling any think functions
	// if the move is blocked, all moved objects will be backed out
	pushed_p = pushed;
	for ( part = this; part; part = part->chain )
	{
		// get current position
		BG_EvaluateTrajectory( &part->GetState()->pos, level.time, origin );
		BG_EvaluateTrajectory( &part->GetState()->apos, level.time, angles );
		VectorSubtract( origin, part->GetShared()->currentOrigin, move );
		VectorSubtract( angles, part->GetShared()->currentAngles, amove );
		if ( !MoverPush( part, move, amove, &obstacle ) )
		{
			break;	// move was blocked
		}
	}

	if ( part )
	{
		// go back to the previous position
		for ( part = this; part; part = part->chain )
		{
			part->GetState()->pos.trTime += level.time - level.previousTime;
			part->GetState()->apos.trTime += level.time - level.previousTime;
			BG_EvaluateTrajectory( &part->GetState()->pos, level.time, part->GetShared()->currentOrigin );
			BG_EvaluateTrajectory( &part->GetState()->apos, level.time, part->GetShared()->currentAngles );
			gameImports->LinkEntity( part );
		}

		// if the pusher has a "blocked" function, call it
		Blocked( obstacle );
		return;
	}

	// the move succeeded
	for ( part = this; part; part = part->chain )
	{
		// call the reached function if time is at or past end point
		if ( part->GetState()->pos.trType == TR_LINEAR_STOP )
		{
			if ( level.time >= part->GetState()->pos.trTime + part->GetState()->pos.trDuration )
			{
				//if ( part->reached ) 
				//{
				//	part->reached( part );
				//}
			}
		}
	}
}

/*
============
MoverPush

Objects need to be moved back on a failed push,
otherwise riders would continue to slide.
If false is returned, *obstacle will be the blocking entity
============
*/
bool BaseMover::MoverPush( IEntity* pusher, Vector move, Vector amove, BaseQuakeEntity** obstacle )
{
	int i, e;
	BaseQuakeEntity* check;
	vec3_t mins, maxs;
	pushed_t* p;
	int entityList[MAX_GENTITIES];
	int listedEntities;
	vec3_t totalMins, totalMaxs;

	*obstacle = nullptr;

	// mins/maxs are the bounds at the destination
	// totalMins / totalMaxs are the bounds for the entire move
	if ( pusher->GetShared()->currentAngles[0] || pusher->GetShared()->currentAngles[1] || pusher->GetShared()->currentAngles[2]
		 || amove[0] || amove[1] || amove[2] )
	{
		float radius;

		radius = RadiusFromBounds( pusher->GetShared()->mins, pusher->GetShared()->maxs );
		for ( i = 0; i < 3; i++ )
		{
			mins[i] = pusher->GetShared()->currentOrigin[i] + move[i] - radius;
			maxs[i] = pusher->GetShared()->currentOrigin[i] + move[i] + radius;
			totalMins[i] = mins[i] - move[i];
			totalMaxs[i] = maxs[i] - move[i];
		}
	}
	else
	{
		for ( i = 0; i < 3; i++ )
		{
			mins[i] = pusher->GetShared()->absmin[i] + move[i];
			maxs[i] = pusher->GetShared()->absmax[i] + move[i];
		}

		VectorCopy( pusher->GetShared()->absmin, totalMins );
		VectorCopy( pusher->GetShared()->absmax, totalMaxs );
		for ( i = 0; i < 3; i++ )
		{
			if ( move[i] > 0 )
			{
				totalMaxs[i] += move[i];
			}
			else
			{
				totalMins[i] += move[i];
			}
		}
	}

	// unlink the pusher so we don't get it in the entityList
	gameImports->UnlinkEntity( pusher );

	listedEntities = trap_EntitiesInBox( totalMins, totalMaxs, entityList, MAX_GENTITIES );

	// move the pusher to its final position
	VectorAdd( pusher->GetShared()->currentOrigin, move, pusher->GetShared()->currentOrigin );
	VectorAdd( pusher->GetShared()->currentAngles, amove, pusher->GetShared()->currentAngles );
	gameImports->LinkEntity( pusher );

	// see if any solid entities are inside the final position
	for ( e = 0; e < listedEntities; e++ )
	{
		check = static_cast<BaseQuakeEntity*>(gEntities[entityList[e]]);

		// only push items and players
		if ( check->GetState()->eType != ET_ITEM && check->GetState()->eType != ET_PLAYER && !check->isPhysicsObject )
		{
			continue;
		}

		// if the entity is standing on the pusher, it will definitely be moved
		if ( check->GetState()->groundEntityNum != pusher->GetState()->number ) 
		{
			// see if the ent needs to be tested
			if ( check->GetShared()->absmin[0] >= maxs[0]
				 || check->GetShared()->absmin[1] >= maxs[1]
				 || check->GetShared()->absmin[2] >= maxs[2]
				 || check->GetShared()->absmax[0] <= mins[0]
				 || check->GetShared()->absmax[1] <= mins[1]
				 || check->GetShared()->absmax[2] <= mins[2] ) 
			{
				continue;
			}
			// see if the ent's bbox is inside the pusher's final position
			// this does allow a fast moving object to pass through a thin entity...
			if ( !check->TestEntityPosition() )
			{
				continue;
			}
		}

		// the entity needs to be pushed
		if ( static_cast<BaseQuakeEntity*>(pusher)->TryPushingEntity( check, move, amove ) )
		{
			continue;
		}

		// the move was blocked an entity

		// bobbing entities are instant-kill and never get blocked
		if ( pusher->GetState()->pos.trType == TR_SINE || pusher->GetState()->apos.trType == TR_SINE )
		{
			check->TakeDamage( pusher, pusher, 0, 99999 );
			continue;
		}


		// save off the obstacle so we can call the block function (crush, etc)
		*obstacle = check;

		// move back any entities we already moved
		// go backwards, so if the same entity was pushed
		// twice, it goes back to the original position
		for ( p = pushed_p - 1; p >= pushed; p-- )
		{
			VectorCopy( p->origin, p->ent->GetState()->pos.trBase );
			VectorCopy( p->angles, p->ent->GetState()->apos.trBase );

			if ( p->ent->IsClass( BasePlayer::ClassInfo ) )
			{
				static_cast<BasePlayer*>(p->ent)->GetClient()->ps.delta_angles[YAW] = p->deltayaw;
				VectorCopy( p->origin, static_cast<BasePlayer*>(p->ent)->GetClient()->ps.origin );
			}
			gameImports->LinkEntity( p->ent );
		}
		
		return false;
	}

	return true;
}