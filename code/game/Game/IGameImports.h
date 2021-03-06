#pragma once

namespace Entities
{
	class IEntity;
}

// All the functions that the game DLL will call from the engine
class IGameImports
{
public:
	IGameImports() = default;
	virtual ~IGameImports() = default;

	virtual void			AddConsoleCommand( const char* commandName ) = 0;
	virtual void			RemoveConsoleCommand( const char* commandName ) = 0;
	virtual void			SendClientCommand( const char* s ) = 0;
	virtual void			ConsoleVariable_Register( vmCvar_t* cvar, const char* name, const char* value, int flags ) = 0;
	virtual void			ConsoleVariable_Update( vmCvar_t* cvar ) = 0;
	virtual void			ConsoleVariable_Set( const char* name, const char* value ) = 0;
	virtual int				ConsoleVariable_GetInteger( const char* name ) = 0;
	virtual void			ConsoleVariable_GetString( const char* name, char* buffer, int bufferSize ) = 0;

	virtual void			LocateGameData( sharedEntity_t* gEnts, int numGEntities, int sizeofGEntity, Entities::IEntity** iEnts, int numEntities, int sizeOfEntity, playerState_t* client, int sizeOfClient ) = 0;

	virtual void			DropClient( int clientNum, const char* reason ) = 0;
	virtual void			SendServerCommand( int clientNum, const char* commandText ) = 0;

	virtual void			SetConfigString( int num, const char* string ) = 0;
	virtual void			GetConfigString( int num, char* buffer, int bufferSize ) = 0;

	virtual void			GetUserInfo( int num, char* buffer, int bufferSize ) = 0;
	virtual void			SetUserInfo( int num, const char* buffer ) = 0;

	virtual void			GetServerInfo( char* buffer, int bufferSize ) = 0;

	virtual void			SetBrushModel( Entities::IEntity* ent, const char* name ) = 0;
	virtual void			SetBrushModel( sharedEntity_t* ent, const char* name ) = 0;

	virtual void			Trace( trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) = 0;
	virtual void			TraceCapsule( trace_t* results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentmask ) = 0;

	virtual int				PointContents( const vec3_t point, int passEntityNum ) = 0;

	virtual bool			IsInPVS( const vec3_t p1, const vec3_t p2 ) = 0;
	virtual bool			InPVSIgnorePortals( const vec3_t p1, const vec3_t p2 ) = 0;

	virtual void			AdjustAreaPortalState( Entities::IEntity* ent, bool open ) = 0;
	virtual void			AdjustAreaPortalState( sharedEntity_t* ent, bool open ) = 0;
	virtual bool			AreasConnected( int area1, int area2 ) = 0;

	virtual void			LinkEntity( Entities::IEntity* ent ) = 0;
	virtual void			LinkEntity( sharedEntity_t* ent ) = 0;

	virtual void			UnlinkEntity( Entities::IEntity* ent ) = 0;
	virtual void			UnlinkEntity( sharedEntity_t* ent ) = 0;

	virtual int				EntitiesInBox( const vec3_t mins, const vec3_t maxs, int* list, int maxcount ) = 0;

	virtual bool			EntityContact( vec3_t mins, vec3_t maxs, const Entities::IEntity* ent, bool capsule = false ) = 0;
	virtual bool			EntityContact( vec3_t mins, vec3_t maxs, const sharedEntity_t* ent ) = 0;
	virtual bool			EntityContactCapsule( vec3_t mins, vec3_t maxs, const sharedEntity_t* ent ) = 0;

	virtual int				BotAllocateClient( void ) = 0;
	virtual void			BotFreeClient( int clientNum ) = 0;
	virtual void			GetUsercmd( int clientNum, usercmd_t* cmd ) = 0;

	virtual int				DebugPolygonCreate( int color, int numPoints, vec3_t* points ) = 0;
	virtual void			DebugPolygonDelete( int id ) = 0;
};
// BotLib is not here

extern IGameImports* gameImports;
