/**
* =============================================================================
* Source Python
* Copyright (C) 2014 Source Python Development Team.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, the Source Python Team gives you permission
* to link the code of this program (as well as its derivative works) to
* "Half-Life 2," the "Source Engine," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, the Source.Python
* Development Team grants this exception to all derivative works.
*/

#ifndef _ENGINES_WRAP_PYTHON_BMS_H
#define _ENGINES_WRAP_PYTHON_BMS_H

//---------------------------------------------------------------------------------
// Includes.
//---------------------------------------------------------------------------------
#include "eiface.h"
#include "engine/IEngineSound.h"
#include "engine/IEngineTrace.h"


//---------------------------------------------------------------------------------
// External variables.
//---------------------------------------------------------------------------------
extern IEngineTrace* enginetrace;


//---------------------------------------------------------------------------------
// IVEngineServer visitor function.
//---------------------------------------------------------------------------------
template<class T>
T IVEngineServer_Visitor(T cls)
{
	cls
		.def("get_game_server_steamid",
			&IVEngineServer::GetGameServerSteamID,
			"Returns the SteamID of the game server.",
			reference_existing_object_policy()
		)

		.def("get_server_version",
			&IVEngineServer::GetServerVersion,
			"Returns the version of the server."
		)

		.def("get_time",
			&IVEngineServer::Time,
			"Returns a high precision timer for doing profiling work"
		)

		.def("multiplayer_end_game",
			&IVEngineServer::MultiplayerEndGame,
			"Matchmaking"
		)

		.def("create_fake_client_ex",
			&IVEngineServer::CreateFakeClientEx,
			reference_existing_object_policy()
		)
	;

	return cls;
}


//---------------------------------------------------------------------------------
// IEngineSound visitor function.
//---------------------------------------------------------------------------------
template<class T>
T IEngineSound_Visitor(T cls)
{
	return cls;
}

#endif // _ENGINES_WRAP_PYTHON_BMS_H