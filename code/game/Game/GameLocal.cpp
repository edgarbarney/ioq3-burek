#include "Game/g_local.hpp"

#include "Game/IGame.h"
#include "Game/IGameImports.h"
#include "Game/GameExportImport.h"
#include "Game/GameLocal.h"

extern void G_InitGame( int levelTime, int randomSeed, int restart );
extern void G_ShutdownGame( int restart );
extern void ClientUserinfoChanged( int clientNum );
extern void G_RunFrame( int levelTime );

// This is pretty damn ugly, but it'll work for now
// The engine has just begun to be C++-ified, so
// there's a lot of work to do
// 
// For example, instead of having to call global functions
// like G_InitGame, all related variables should be stored
// as members of the GameLocal class, and all the functionality
// should be moved there as well

void GameLocal::Init( int levelTime, int randomSeed, int restart )
{
	::G_InitGame( levelTime, randomSeed, restart );
}

void GameLocal::Shutdown( bool restart )
{
	::G_ShutdownGame( restart );
}

const char* GameLocal::ClientConnect( int clientNum, bool firstTime, bool isBot )
{
	return ::ClientConnect( clientNum, firstTime, isBot );
}

void GameLocal::ClientBegin( int clientNum )
{
	::ClientBegin( clientNum );
}

void GameLocal::ClientUserInfoChanged( int clientNum )
{
	::ClientUserinfoChanged( clientNum );
}

void GameLocal::ClientDisconnect( int clientNum )
{
	::ClientDisconnect( clientNum );
}

void GameLocal::ClientCommand( int clientNum )
{
	::ClientCommand( clientNum );
}

void GameLocal::ClientThink( int clientNum )
{
	::ClientThink( clientNum );
}

void GameLocal::RunFrame( int levelTime )
{
	::G_RunFrame( levelTime );
}

bool GameLocal::ConsoleCommand( void )
{
	return ::ConsoleCommand() != 0;
}

int GameLocal::BotAI_StartFrame( int time )
{
	return BotAIStartFrame( time );
}