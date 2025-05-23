/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "sys/platform.h"
#include "idlib/LangDict.h"
#include "renderer/ModelManager.h"

#include "gamesys/SysCvar.h"
#include "script/Script_Thread.h"
#include "Light.h"
#include "Player.h"
#include "Mover.h"
#include "Misc.h"
#include "WorldSpawn.h"
#include "Sound.h"

#include "Target.h"

/*
===============================================================================

idTarget

Invisible entities that affect other entities or the world when activated.

===============================================================================
*/

CLASS_DECLARATION( idEntity, idTarget )
END_CLASS


/*
===============================================================================

idTarget_Remove

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_Remove )
	EVENT( EV_Activate, idTarget_Remove::Event_Activate )
END_CLASS

/*
================
idTarget_Remove::Event_Activate
================
*/
void idTarget_Remove::Event_Activate( idEntity *activator ) {
	int			i;
	idEntity	*ent;

	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( ent ) {
			ent->PostEventMS( &EV_Remove, 0 );
		}
	}

	// delete our self when done
	PostEventMS( &EV_Remove, 0 );
}


/*
===============================================================================

idTarget_Show

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_Show )
	EVENT( EV_Activate, idTarget_Show::Event_Activate )
END_CLASS

/*
================
idTarget_Show::Event_Activate
================
*/
void idTarget_Show::Event_Activate( idEntity *activator ) {
	int			i;
	idEntity	*ent;

	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( ent ) {
			ent->Show();
		}
	}

	// delete our self when done
	PostEventMS( &EV_Remove, 0 );
}


/*
===============================================================================

idTarget_Damage

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_Damage )
	EVENT( EV_Activate, idTarget_Damage::Event_Activate )
END_CLASS

/*
================
idTarget_Damage::Event_Activate
================
*/
void idTarget_Damage::Event_Activate( idEntity *activator ) {
	int			i;
	const char *damage;
	idEntity *	ent;

	damage = spawnArgs.GetString( "def_damage", "damage_generic" );
	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( ent ) {
			ent->Damage( this, this, vec3_origin, damage, 1.0f, INVALID_JOINT );
		}
	}
}


/*
===============================================================================

idTarget_SessionCommand

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_SessionCommand )
	EVENT( EV_Activate, idTarget_SessionCommand::Event_Activate )
END_CLASS

/*
================
idTarget_SessionCommand::Event_Activate
================
*/
void idTarget_SessionCommand::Event_Activate( idEntity *activator ) {
	gameLocal.sessionCommand = spawnArgs.GetString( "command" );
}


/*
===============================================================================

idTarget_EndLevel

Just a modified form of idTarget_SessionCommand
===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_EndLevel )
	EVENT( EV_Activate,		idTarget_EndLevel::Event_Activate )
END_CLASS

/*
================
idTarget_EndLevel::Event_Activate
================
*/
void idTarget_EndLevel::Event_Activate( idEntity *activator ) {
	idStr nextMap;

	if ( spawnArgs.GetBool( "endOfGame" ) ) {
		cvarSystem->SetCVarBool( "g_nightmare", true );
		gameLocal.sessionCommand = "disconnect";
		return;
	}

	if ( !spawnArgs.GetString( "nextMap", "", nextMap ) ) {
		gameLocal.Printf( "idTarget_SessionCommand::Event_Activate: no nextMap key\n" );
		return;
	}

	if ( spawnArgs.GetInt( "devmap", "0" ) ) {
		gameLocal.sessionCommand = "devmap ";	// only for special demos
	} else {
		gameLocal.sessionCommand = "map ";
	}

	gameLocal.sessionCommand += nextMap;
}


/*
===============================================================================

idTarget_WaitForButton

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_WaitForButton )
	EVENT( EV_Activate, idTarget_WaitForButton::Event_Activate )
END_CLASS

/*
================
idTarget_WaitForButton::Event_Activate
================
*/
void idTarget_WaitForButton::Event_Activate( idEntity *activator ) {
	if ( thinkFlags & TH_THINK ) {
		BecomeInactive( TH_THINK );
	} else {
		// always allow during cinematics
		cinematic = true;
		BecomeActive( TH_THINK );
	}
}

/*
================
idTarget_WaitForButton::Think
================
*/
void idTarget_WaitForButton::Think( void ) {
	idPlayer *player;

	if ( thinkFlags & TH_THINK ) {
		player = gameLocal.GetLocalPlayer();
		if ( player && !( player->oldButtons & BUTTON_ATTACK ) && ( player->usercmd.buttons & BUTTON_ATTACK ) ) {
			player->usercmd.buttons &= ~BUTTON_ATTACK;
			BecomeInactive( TH_THINK );
			ActivateTargets( player );
		}
	} else {
		BecomeInactive( TH_ALL );
	}
}


/*
===============================================================================

idTarget_SetGlobalShaderParm

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_SetGlobalShaderTime )
EVENT( EV_Activate,	idTarget_SetGlobalShaderTime::Event_Activate )
END_CLASS

/*
================
idTarget_SetGlobalShaderTime::Event_Activate
================
*/
void idTarget_SetGlobalShaderTime::Event_Activate( idEntity *activator ) {
	int parm = spawnArgs.GetInt( "globalParm" );
	float time = -MS2SEC( gameLocal.time );
	if ( parm >= 0 && parm < MAX_GLOBAL_SHADER_PARMS ) {
		gameLocal.globalShaderParms[parm] = time;
	}
}

/*
===============================================================================

idTarget_SetShaderParm

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_SetShaderParm )
	EVENT( EV_Activate,	idTarget_SetShaderParm::Event_Activate )
END_CLASS

/*
================
idTarget_SetShaderParm::Event_Activate
================
*/
void idTarget_SetShaderParm::Event_Activate( idEntity *activator ) {
	int			i;
	idEntity *	ent;
	float		value;
	idVec3		color;
	int			parmnum;

	// set the color on the targets
	if ( spawnArgs.GetVector( "_color", "1 1 1", color ) ) {
		for( i = 0; i < targets.Num(); i++ ) {
			ent = targets[ i ].GetEntity();
			if ( ent ) {
				ent->SetColor( color[ 0 ], color[ 1 ], color[ 2 ] );
			}
		}
	}

	// set any shader parms on the targets
	for( parmnum = 0; parmnum < MAX_ENTITY_SHADER_PARMS; parmnum++ ) {
		if ( spawnArgs.GetFloat( va( "shaderParm%d", parmnum ), "0", value ) ) {
			for( i = 0; i < targets.Num(); i++ ) {
				ent = targets[ i ].GetEntity();
				if ( ent ) {
					ent->SetShaderParm( parmnum, value );
				}
			}
			if (spawnArgs.GetBool("toggle") && (value == 0 || value == 1)) {
				int val = value;
				val ^= 1;
				value = val;
				spawnArgs.SetFloat(va("shaderParm%d", parmnum), value);
			}
		}
	}
}


/*
===============================================================================

idTarget_SetShaderTime

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_SetShaderTime )
	EVENT( EV_Activate,	idTarget_SetShaderTime::Event_Activate )
END_CLASS

/*
================
idTarget_SetShaderTime::Event_Activate
================
*/
void idTarget_SetShaderTime::Event_Activate( idEntity *activator ) {
	int			i;
	idEntity *	ent;
	float		time;

	time = -MS2SEC( gameLocal.time );
	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( ent ) {
			ent->SetShaderParm( SHADERPARM_TIMEOFFSET, time );
			if ( ent->IsType( idLight::Type ) ) {
				static_cast<idLight *>(ent)->SetLightParm( SHADERPARM_TIMEOFFSET, time );
			}
		}
	}
}

/*
===============================================================================

idTarget_FadeEntity

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_FadeEntity )
	EVENT( EV_Activate,				idTarget_FadeEntity::Event_Activate )
END_CLASS

/*
================
idTarget_FadeEntity::idTarget_FadeEntity
================
*/
idTarget_FadeEntity::idTarget_FadeEntity( void ) {
	fadeFrom.Zero();
	fadeStart = 0;
	fadeEnd = 0;
}

/*
================
idTarget_FadeEntity::Save
================
*/
void idTarget_FadeEntity::Save( idSaveGame *savefile ) const {
	savefile->WriteVec4( fadeFrom ); // idVec4 fadeFrom
	savefile->WriteInt( fadeStart ); // int fadeStart
	savefile->WriteInt( fadeEnd ); // int fadeEnd
}

/*
================
idTarget_FadeEntity::Restore
================
*/
void idTarget_FadeEntity::Restore( idRestoreGame *savefile ) {
	savefile->ReadVec4( fadeFrom ); // idVec4 fadeFrom
	savefile->ReadInt( fadeStart ); // int fadeStart
	savefile->ReadInt( fadeEnd ); // int fadeEnd
}

/*
================
idTarget_FadeEntity::Event_Activate
================
*/
void idTarget_FadeEntity::Event_Activate( idEntity *activator ) {
	idEntity *ent;
	int i;

	if ( !targets.Num() ) {
		return;
	}

	// always allow during cinematics
	cinematic = true;
	BecomeActive( TH_THINK );

	ent = this;
	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( ent ) {
			ent->GetColor( fadeFrom );
			break;
		}
	}

	fadeStart = gameLocal.time;
	fadeEnd = gameLocal.time + SEC2MS( spawnArgs.GetFloat( "fadetime" ) );
}

/*
================
idTarget_FadeEntity::Think
================
*/
void idTarget_FadeEntity::Think( void ) {
	int			i;
	idEntity	*ent;
	idVec4		color;
	idVec4		fadeTo;
	float		frac;

	if ( thinkFlags & TH_THINK ) {
		GetColor( fadeTo );
		if ( gameLocal.time >= fadeEnd ) {
			color = fadeTo;
			BecomeInactive( TH_THINK );
		} else {
			frac = ( float )( gameLocal.time - fadeStart ) / ( float )( fadeEnd - fadeStart );
			color.Lerp( fadeFrom, fadeTo, frac );
		}

		// set the color on the targets
		for( i = 0; i < targets.Num(); i++ ) {
			ent = targets[ i ].GetEntity();
			if ( ent ) {
				ent->SetColor( color );
			}
		}
	} else {
		BecomeInactive( TH_ALL );
	}
}

/*
===============================================================================

idTarget_LightFadeIn

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_LightFadeIn )
	EVENT( EV_Activate,				idTarget_LightFadeIn::Event_Activate )
END_CLASS

/*
================
idTarget_LightFadeIn::Event_Activate
================
*/
void idTarget_LightFadeIn::Event_Activate( idEntity *activator ) {
	idEntity *ent;
	idLight *light;
	int i;
	float time;

	if ( !targets.Num() ) {
		return;
	}

	time = spawnArgs.GetFloat( "fadetime" );
	ent = this;
	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( !ent ) {
			continue;
		}
		if ( ent->IsType( idLight::Type ) ) {
			light = static_cast<idLight *>( ent );
			light->FadeIn( time );
		} else {
			gameLocal.Printf( "'%s' targets non-light '%s'", name.c_str(), ent->GetName() );
		}
	}
}

/*
===============================================================================

idTarget_LightFadeOut

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_LightFadeOut )
	EVENT( EV_Activate,				idTarget_LightFadeOut::Event_Activate )
END_CLASS

/*
================
idTarget_LightFadeOut::Event_Activate
================
*/
void idTarget_LightFadeOut::Event_Activate( idEntity *activator ) {
	idEntity *ent;
	idLight *light;
	int i;
	float time;

	if ( !targets.Num() ) {
		return;
	}

	time = spawnArgs.GetFloat( "fadetime" );
	ent = this;
	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( !ent ) {
			continue;
		}
		if ( ent->IsType( idLight::Type ) ) {
			light = static_cast<idLight *>( ent );
			light->FadeOut( time );
		} else {
			gameLocal.Printf( "'%s' targets non-light '%s'", name.c_str(), ent->GetName() );
		}
	}
}

/*
===============================================================================

idTarget_Give

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_Give )
	EVENT( EV_Activate,				idTarget_Give::Event_Activate )
END_CLASS

/*
================
idTarget_Give::Spawn
================
*/
void idTarget_Give::Spawn( void ) {
	if ( spawnArgs.GetBool( "onSpawn" ) ) {
		PostEventMS( &EV_Activate, 50 );
	}
}

/*
================
idTarget_Give::Event_Activate
================
*/
void idTarget_Give::Event_Activate( idEntity *activator ) {

	if ( spawnArgs.GetBool( "development" ) && developer.GetInteger() == 0 ) {
		return;
	}

	static int giveNum = 0;
	idPlayer *player = gameLocal.GetLocalPlayer();
	if ( player ) {
		const idKeyValue *kv = spawnArgs.MatchPrefix( "item", NULL );
		while ( kv ) {
			const idDict *dict = gameLocal.FindEntityDefDict( kv->GetValue(), false );
			if ( dict ) {
				idDict d2;
				d2.Copy( *dict );
				d2.Set( "name", va( "givenitem_%i", giveNum++ ) );
				idEntity *ent = NULL;
				if ( gameLocal.SpawnEntityDef( d2, &ent ) && ent && ent->IsType( idItem::Type ) ) {
					idItem *item = static_cast<idItem*>(ent);
					item->GiveToPlayer( gameLocal.GetLocalPlayer() );
				}
			}
			kv = spawnArgs.MatchPrefix( "item", kv );
		}
	}
}

/*
===============================================================================

idTarget_GiveEmail

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_GiveEmail )
EVENT( EV_Activate,				idTarget_GiveEmail::Event_Activate )
END_CLASS

/*
================
idTarget_GiveEmail::Spawn
================
*/
void idTarget_GiveEmail::Spawn( void ) {
}

/*
================
idTarget_GiveEmail::Event_Activate
================
*/
void idTarget_GiveEmail::Event_Activate( idEntity *activator ) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	const idDeclPDA *pda = player->GetPDA();
	if ( pda ) {
		player->GiveEmail( spawnArgs.GetString( "email" ) );
	} else {
		player->ShowTip( spawnArgs.GetString( "text_infoTitle" ), spawnArgs.GetString( "text_PDANeeded" ), true );
	}
}


/*
===============================================================================

idTarget_SetModel

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_SetModel )
	EVENT( EV_Activate,	idTarget_SetModel::Event_Activate )
END_CLASS

/*
================
idTarget_SetModel::Spawn
================
*/
void idTarget_SetModel::Spawn( void ) {
	const char *model;

	model = spawnArgs.GetString( "newmodel" );
	if ( declManager->FindType( DECL_MODELDEF, model, false ) == NULL ) {
		// precache the render model
		renderModelManager->FindModel( model );
		// precache .cm files only
		collisionModelManager->LoadModel( model, true );
	}
}

/*
================
idTarget_SetModel::Event_Activate
================
*/
void idTarget_SetModel::Event_Activate( idEntity *activator ) {
	for( int i = 0; i < targets.Num(); i++ ) {
		idEntity *ent = targets[ i ].GetEntity();
		if ( ent ) {
			ent->SetModel( spawnArgs.GetString( "newmodel" ) );
		}
	}
}


/*
===============================================================================

idTarget_SetInfluence

===============================================================================
*/

const idEventDef EV_RestoreInfluence( "<RestoreInfluece>" );
const idEventDef EV_GatherEntities( "<GatherEntities>" );
const idEventDef EV_Flash( "<Flash>", "fd" );
const idEventDef EV_ClearFlash( "<ClearFlash>", "f" );

CLASS_DECLARATION( idTarget, idTarget_SetInfluence )
	EVENT( EV_Activate,	idTarget_SetInfluence::Event_Activate )
	EVENT( EV_RestoreInfluence,	idTarget_SetInfluence::Event_RestoreInfluence )
	EVENT( EV_GatherEntities, idTarget_SetInfluence::Event_GatherEntities )
	EVENT( EV_Flash, idTarget_SetInfluence::Event_Flash )
	EVENT( EV_ClearFlash, idTarget_SetInfluence::Event_ClearFlash )
END_CLASS

/*
================
idTarget_SetInfluence::idTarget_SetInfluence
================
*/
idTarget_SetInfluence::idTarget_SetInfluence( void ) {
	flashIn = 0.0f;
	flashOut = 0.0f;
	delay = 0.0f;
	switchToCamera = NULL;
	soundFaded = false;
	restoreOnTrigger = false;
}

/*
================
idTarget_SetInfluence::Save
================
*/
void idTarget_SetInfluence::Save( idSaveGame *savefile ) const {
	bool gather = lightList.Num() > 0 || guiList.Num() > 0 || soundList.Num() > 0 || genericList.Num() > 0;
	savefile->WriteBool( gather );

	//SaveFileWriteArray( lightList, lightList.Num(), WriteInt); // idList<int> lightList
	//SaveFileWriteArray( guiList, guiList.Num(), WriteInt); // idList<int> guiList
	//SaveFileWriteArray( soundList, soundList.Num(), WriteInt); // idList<int> soundList
	//SaveFileWriteArray( genericList, genericList.Num(), WriteInt); // idList<int> genericList

	savefile->WriteFloat( flashIn ); // float flashIn
	savefile->WriteFloat( flashOut ); // float flashOut
	savefile->WriteFloat( delay ); // float delay
	savefile->WriteString( flashInSound ); // idString flashInSound
	savefile->WriteString( flashOutSound ); // idString flashOutSound
	savefile->WriteObject( switchToCamera ); // idEntity * switchToCamera

	savefile->WriteFloat( fovSetting.GetStartTime() );// idInterpolate<float> fovSetting
	savefile->WriteFloat( fovSetting.GetDuration() );
	savefile->WriteFloat( fovSetting.GetStartValue() );
	savefile->WriteFloat( fovSetting.GetEndValue() );

	savefile->WriteBool( soundFaded ); // bool soundFaded
	savefile->WriteBool( restoreOnTrigger ); // bool restoreOnTrigger

	//savefile->WriteInt( savedGuiList.Num() );// idList<SavedGui_t> savedGuiList
	//for( int i = 0; i < savedGuiList.Num(); i++ ) {
	//	for(int j = 0; j < MAX_RENDERENTITY_GUI; j++) {
	//		// blendo eric: guis saved on renderentity, so just use ptr write
	//		savefile->WriteSGPtr( savedGuiList[i].gui[j] );
	//	}
	//}
}

/*
================
idTarget_SetInfluence::Restore
================
*/
void idTarget_SetInfluence::Restore( idRestoreGame *savefile ) {
	bool gather;
	savefile->ReadBool( gather );
	Event_GatherEntities();
	//SaveFileReadList( lightList, ReadInt); // idList<int> lightList
	//SaveFileReadList( guiList, ReadInt); // idList<int> guiList
	//SaveFileReadList( soundList, ReadInt); // idList<int> soundList
	//SaveFileReadList( genericList, ReadInt); // idList<int> genericList

	savefile->ReadFloat( flashIn ); // float flashIn
	savefile->ReadFloat( flashOut ); // float flashOut
	savefile->ReadFloat( delay ); // float delay
	savefile->ReadString( flashInSound ); // idString flashInSound
	savefile->ReadString( flashOutSound ); // idString flashOutSound
	savefile->ReadObject( switchToCamera ); // idEntity * switchToCamera

	float fVal;
	savefile->ReadFloat( fVal );// idInterpolate<float> fovSetting
	fovSetting.SetStartTime( fVal );
	savefile->ReadFloat( fVal );
	fovSetting.SetDuration( fVal );
	savefile->ReadFloat( fVal );
	fovSetting.SetStartValue( fVal );
	savefile->ReadFloat( fVal );
	fovSetting.SetEndValue( fVal );

	savefile->ReadBool( soundFaded ); // bool soundFaded
	savefile->ReadBool( restoreOnTrigger ); // bool restoreOnTrigger

	//int num;
	//savefile->ReadInt( num ); // idList<SavedGui_t> savedGuiList
	//for( int i = 0; i < num; i++ ) {
	//	SavedGui_t temp;
	//	for(int j = 0; j < MAX_RENDERENTITY_GUI; j++) {
	//		savefile->ReadUserInterface(temp.gui[j]);
	//	}
	//	savedGuiList.Append( temp );
	//}
}

/*
================
idTarget_SetInfluence::Spawn
================
*/
void idTarget_SetInfluence::Spawn() {
	PostEventMS( &EV_GatherEntities, 0 );
	flashIn = spawnArgs.GetFloat( "flashIn", "0" );
	flashOut = spawnArgs.GetFloat( "flashOut", "0" );
	flashInSound = spawnArgs.GetString( "snd_flashin" );
	flashOutSound = spawnArgs.GetString( "snd_flashout" );
	delay = spawnArgs.GetFloat( "delay" );
	soundFaded = false;
	restoreOnTrigger = false;

	// always allow during cinematics
	cinematic = true;
}

/*
================
idTarget_SetInfluence::Event_Flash
================
*/
void idTarget_SetInfluence::Event_Flash( float flash, int out ) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	player->playerView.Fade( idVec4( 1, 1, 1, 1 ), flash );
	const idSoundShader *shader = NULL;
	if ( !out && flashInSound.Length() ){
		shader = declManager->FindSound( flashInSound );
		player->StartSoundShader( shader, SND_CHANNEL_VOICE, 0, false, NULL );
	} else if ( out && ( flashOutSound.Length() || flashInSound.Length() ) ) {
		shader = declManager->FindSound( flashOutSound.Length() ? flashOutSound : flashInSound );
		player->StartSoundShader( shader, SND_CHANNEL_VOICE, 0, false, NULL );
	}
	PostEventSec( &EV_ClearFlash, flash, flash );
}


/*
================
idTarget_SetInfluence::Event_ClearFlash
================
*/
void idTarget_SetInfluence::Event_ClearFlash( float flash ) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	player->playerView.Fade( vec4_zero , flash );
}
/*
================
idTarget_SetInfluence::Event_GatherEntities
================
*/
void idTarget_SetInfluence::Event_GatherEntities() {
	int i, listedEntities;
	static idEntity *entityList[ MAX_GENTITIES ];
	memset( entityList, 0, sizeof(idEntity *)*MAX_GENTITIES );

	bool lights = spawnArgs.GetBool( "effect_lights" );
	bool sounds = spawnArgs.GetBool( "effect_sounds" );
	bool guis = spawnArgs.GetBool( "effect_guis" );
	bool models = spawnArgs.GetBool( "effect_models" );
	bool vision = spawnArgs.GetBool( "effect_vision" );
	bool targetsOnly = spawnArgs.GetBool( "targetsOnly" );

	lightList.Clear();
	guiList.Clear();
	soundList.Clear();
#ifdef _D3XP
	savedGuiList.Clear();
#endif

	if ( spawnArgs.GetBool( "effect_all" ) ) {
		lights = sounds = guis = models = vision = true;
	}

	if ( targetsOnly ) {
		listedEntities = targets.Num();
		for ( i = 0; i < listedEntities; i++ ) {
			entityList[i] = targets[i].GetEntity();
		}
	} else {
		float radius = spawnArgs.GetFloat( "radius" );
		listedEntities = gameLocal.EntitiesWithinRadius( GetPhysics()->GetOrigin(), radius, entityList, MAX_GENTITIES );
	}

	for( i = 0; i < listedEntities; i++ ) {
		idEntity *ent = entityList[ i ];
		if ( ent ) {
			if ( lights && ent->IsType( idLight::Type ) && ent->spawnArgs.FindKey( "color_demonic" ) ) {
				lightList.Append( ent->entityNumber );
				continue;
			}
			if ( sounds && ent->IsType( idSound::Type ) && ent->spawnArgs.FindKey( "snd_demonic" ) ) {
				soundList.Append( ent->entityNumber );
				continue;
			}
			if ( guis && ent->GetRenderEntity() && ent->GetRenderEntity()->gui[ 0 ] && ent->spawnArgs.FindKey( "gui_demonic" ) ) {
				guiList.Append( ent->entityNumber );
#ifdef _D3XP
				SavedGui_t temp;
				savedGuiList.Append(temp);
#endif
				continue;
			}
			if ( ent->IsType( idStaticEntity::Type ) && ent->spawnArgs.FindKey( "color_demonic" ) ) {
				genericList.Append( ent->entityNumber );
				continue;
			}
		}
	}
	idStr temp;
	temp = spawnArgs.GetString( "switchToView" );
	switchToCamera = ( temp.Length() ) ? gameLocal.FindEntity( temp ) : NULL;

}

/*
================
idTarget_SetInfluence::Event_Activate
================
*/
void idTarget_SetInfluence::Event_Activate( idEntity *activator ) {
	int i, j;
	idEntity *ent;
	idLight *light;
	idSound *sound;
	idStaticEntity *generic;
	const char *parm;
	const char *skin;
	bool update;
	idVec3 color;
	idVec4 colorTo;
	idPlayer *player;

	player = gameLocal.GetLocalPlayer();

	if ( spawnArgs.GetBool( "triggerActivate" ) ) {
		if ( restoreOnTrigger ) {
			ProcessEvent( &EV_RestoreInfluence );
			restoreOnTrigger = false;
			return;
		}
		restoreOnTrigger = true;
	}

	float fadeTime = spawnArgs.GetFloat( "fadeWorldSounds" );

	if ( delay > 0.0f ) {
		PostEventSec( &EV_Activate, delay, activator );
		delay = 0.0f;
		// start any sound fading now
		if ( fadeTime ) {
			gameSoundWorld->FadeSoundClasses( 0, -40.0f, fadeTime );
			soundFaded = true;
		}
		return;
	} else if ( fadeTime && !soundFaded ) {
		gameSoundWorld->FadeSoundClasses( 0, -40.0f, fadeTime );
		soundFaded = true;
	}

	if ( spawnArgs.GetBool( "triggerTargets" ) ) {
		ActivateTargets( activator );
	}

	if ( flashIn ) {
		PostEventSec( &EV_Flash, 0.0f, flashIn, 0 );
	}

	parm = spawnArgs.GetString( "snd_influence" );
	if ( parm && *parm ) {
		PostEventSec( &EV_StartSoundShader, flashIn, parm, SND_CHANNEL_ANY );
	}

	if ( switchToCamera ) {
		switchToCamera->PostEventSec( &EV_Activate, flashIn + 0.05f, this );
	}

	int fov = spawnArgs.GetInt( "fov" );
	if ( fov ) {
		fovSetting.Init( gameLocal.time, SEC2MS( spawnArgs.GetFloat( "fovTime" ) ), player->DefaultFov(), fov );
		BecomeActive( TH_THINK );
	}

	for ( i = 0; i < genericList.Num(); i++ ) {
		ent = gameLocal.entities[genericList[i]];
		if ( ent == NULL ) {
			continue;
		}
		generic = static_cast<idStaticEntity*>( ent );
		color = generic->spawnArgs.GetVector( "color_demonic" );
		colorTo.Set( color.x, color.y, color.z, 1.0f );
		generic->Fade( colorTo, spawnArgs.GetFloat( "fade_time", "0.25" ) );
	}

	for ( i = 0; i < lightList.Num(); i++ ) {
		ent = gameLocal.entities[lightList[i]];
		if ( ent == NULL || !ent->IsType( idLight::Type ) ) {
			continue;
		}
		light = static_cast<idLight *>(ent);
		parm = light->spawnArgs.GetString( "mat_demonic" );
		if ( parm && *parm ) {
			light->SetShader( parm );
		}

		color = light->spawnArgs.GetVector( "_color" );
		color = light->spawnArgs.GetVector( "color_demonic", color.ToString() );
		colorTo.Set( color.x, color.y, color.z, 1.0f );
		light->Fade( colorTo, spawnArgs.GetFloat( "fade_time", "0.25" ) );
	}

	for ( i = 0; i < soundList.Num(); i++ ) {
		ent = gameLocal.entities[soundList[i]];
		if ( ent == NULL || !ent->IsType( idSound::Type ) ) {
			continue;
		}
		sound = static_cast<idSound *>(ent);
		parm = sound->spawnArgs.GetString( "snd_demonic" );
		if ( parm && *parm ) {
			if ( sound->spawnArgs.GetBool( "overlayDemonic" ) ) {
				sound->StartSound( "snd_demonic", SND_CHANNEL_DEMONIC, 0, false, NULL );
			} else {
				sound->StopSound( SND_CHANNEL_ANY, false );
				sound->SetSound( parm );
			}
		}
	}

	for ( i = 0; i < guiList.Num(); i++ ) {
		ent = gameLocal.entities[guiList[i]];
		if ( ent == NULL || ent->GetRenderEntity() == NULL ) {
			continue;
		}
		update = false;

		for ( j = 0; j < MAX_RENDERENTITY_GUI; j++ ) {
			if ( ent->GetRenderEntity()->gui[ j ] && ent->spawnArgs.FindKey( j == 0 ? "gui_demonic" : va( "gui_demonic%d", j+1 ) ) ) {
#ifdef _D3XP
				//Backup the old one
				savedGuiList[i].gui[j] = ent->GetRenderEntity()->gui[ j ];
#endif
				ent->GetRenderEntity()->gui[ j ] = uiManager->FindGui( ent->spawnArgs.GetString( j == 0 ? "gui_demonic" : va( "gui_demonic%d", j+1 ) ), true );
				update = true;
			}
		}

		if ( update ) {
			ent->UpdateVisuals();
			ent->Present();
		}
	}

	player->SetInfluenceLevel( spawnArgs.GetInt( "influenceLevel" ) );

	int snapAngle = spawnArgs.GetInt( "snapAngle" );
	if ( snapAngle ) {
		idAngles ang( 0, snapAngle, 0 );
		player->SetViewAngles( ang );
		player->SetAngles( ang );
	}

	if ( spawnArgs.GetBool( "effect_vision" ) ) {
		parm = spawnArgs.GetString( "mtrVision" );
		skin = spawnArgs.GetString( "skinVision" );
		player->SetInfluenceView( parm, skin, spawnArgs.GetInt( "visionRadius" ), this );
	}

	parm = spawnArgs.GetString( "mtrWorld" );
	if ( parm && *parm ) {
		gameLocal.SetGlobalMaterial( declManager->FindMaterial( parm ) );
	}

	if ( !restoreOnTrigger ) {
		PostEventMS( &EV_RestoreInfluence, SEC2MS( spawnArgs.GetFloat( "time" ) ) );
	}
}

/*
================
idTarget_SetInfluence::Think
================
*/
void idTarget_SetInfluence::Think( void ) {
	if ( thinkFlags & TH_THINK ) {
		idPlayer *player = gameLocal.GetLocalPlayer();
		player->SetInfluenceFov( fovSetting.GetCurrentValue( gameLocal.time ) );
		if ( fovSetting.IsDone( gameLocal.time ) ) {
			if ( !spawnArgs.GetBool( "leaveFOV" ) ) {
				player->SetInfluenceFov( 0 );
			}
			BecomeInactive( TH_THINK );
		}
	} else {
		BecomeInactive( TH_ALL );
	}
}


/*
================
idTarget_SetInfluence::Event_RestoreInfluence
================
*/
void idTarget_SetInfluence::Event_RestoreInfluence() {
	int i, j;
	idEntity *ent;
	idLight *light;
	idSound *sound;
	idStaticEntity *generic;
	bool update;
	idVec3 color;
	idVec4 colorTo;

	if ( flashOut ) {
		PostEventSec( &EV_Flash, 0.0f, flashOut, 1 );
	}

	if ( switchToCamera ) {
		switchToCamera->PostEventMS( &EV_Activate, 0.0f, this );
	}

	for ( i = 0; i < genericList.Num(); i++ ) {
		ent = gameLocal.entities[genericList[i]];
		if ( ent == NULL ) {
			continue;
		}
		generic = static_cast<idStaticEntity*>( ent );
		colorTo.Set( 1.0f, 1.0f, 1.0f, 1.0f );
		generic->Fade( colorTo, spawnArgs.GetFloat( "fade_time", "0.25" ) );
	}

	for ( i = 0; i < lightList.Num(); i++ ) {
		ent = gameLocal.entities[lightList[i]];
		if ( ent == NULL || !ent->IsType( idLight::Type ) ) {
			continue;
		}
		light = static_cast<idLight *>(ent);
		if ( !light->spawnArgs.GetBool( "leave_demonic_mat" ) ) {
			const char *texture = light->spawnArgs.GetString( "texture", "lights/squarelight1" );
			light->SetShader( texture );
		}
		color = light->spawnArgs.GetVector( "_color" );
		colorTo.Set( color.x, color.y, color.z, 1.0f );
		light->Fade( colorTo, spawnArgs.GetFloat( "fade_time", "0.25" ) );
	}

	for ( i = 0; i < soundList.Num(); i++ ) {
		ent = gameLocal.entities[soundList[i]];
		if ( ent == NULL || !ent->IsType( idSound::Type ) ) {
			continue;
		}
		sound = static_cast<idSound *>(ent);
		sound->StopSound( SND_CHANNEL_ANY, false );
		sound->SetSound( sound->spawnArgs.GetString( "s_shader" ) );
	}

	for ( i = 0; i < guiList.Num(); i++ ) {
		ent = gameLocal.entities[guiList[i]];
		if ( ent == NULL || GetRenderEntity() == NULL ) {
			continue;
		}
		update = false;
		for( j = 0; j < MAX_RENDERENTITY_GUI; j++ ) {
			if ( ent->GetRenderEntity()->gui[ j ] ) {
#ifdef _D3XP
				ent->GetRenderEntity()->gui[ j ] = savedGuiList[i].gui[j];
#else
				ent->GetRenderEntity()->gui[ j ] = uiManager->FindGui( ent->spawnArgs.GetString( j == 0 ? "gui" : va( "gui%d", j+1 ) ) );
#endif
				update = true;
			}
		}
		if ( update ) {
			ent->UpdateVisuals();
			ent->Present();
		}
	}

	idPlayer *player = gameLocal.GetLocalPlayer();
	player->SetInfluenceLevel( 0 );
	player->SetInfluenceView( NULL, NULL, 0.0f, NULL );
	player->SetInfluenceFov( 0 );
	gameLocal.SetGlobalMaterial( NULL );
	float fadeTime = spawnArgs.GetFloat( "fadeWorldSounds" );
	if ( fadeTime ) {
		gameSoundWorld->FadeSoundClasses( 0, 0.0f, fadeTime / 2.0f );
	}

}

/*
===============================================================================

idTarget_SetKeyVal

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_SetKeyVal )
	EVENT( EV_Activate,	idTarget_SetKeyVal::Event_Activate )
END_CLASS

/*
================
idTarget_SetKeyVal::Event_Activate
================
*/
void idTarget_SetKeyVal::Event_Activate( idEntity *activator ) {
	int i;
	idStr key, val;
	idEntity *ent;
	const idKeyValue *kv;
	int n;

	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( ent ) {
			kv = spawnArgs.MatchPrefix("keyval");
			while ( kv ) {
				n = kv->GetValue().Find( ";" );
				if ( n > 0 ) {
					key = kv->GetValue().Left( n );
					val = kv->GetValue().Right( kv->GetValue().Length() - n - 1 );
					ent->spawnArgs.Set( key, val );
					for ( int j = 0; j < MAX_RENDERENTITY_GUI; j++ ) {
						if ( ent->GetRenderEntity()->gui[ j ] ) {
							if ( idStr::Icmpn( key, "gui_", 4 ) == 0 ) {
								ent->GetRenderEntity()->gui[ j ]->SetStateString( key, val );
								ent->GetRenderEntity()->gui[ j ]->StateChanged( gameLocal.time );
							}
						}
					}
				}
				kv = spawnArgs.MatchPrefix( "keyval", kv );
			}
			ent->UpdateChangeableSpawnArgs( NULL );
			ent->UpdateVisuals();
			ent->Present();
		}
	}
}

/*
===============================================================================

idTarget_SetFov

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_SetFov )
	EVENT( EV_Activate,	idTarget_SetFov::Event_Activate )
END_CLASS


/*
================
idTarget_SetFov::Save
================
*/
void idTarget_SetFov::Save( idSaveGame *savefile ) const {

	savefile->WriteFloat( fovSetting.GetStartTime() ); // idInterpolate<int> fovSetting
	savefile->WriteFloat( fovSetting.GetDuration() );
	savefile->WriteFloat( fovSetting.GetStartValue() );
	savefile->WriteFloat( fovSetting.GetEndValue() );
}

/*
================
idTarget_SetFov::Restore
================
*/
void idTarget_SetFov::Restore( idRestoreGame *savefile ) {
	float setting;

	savefile->ReadFloat( setting ); // idInterpolate<int> fovSetting
	fovSetting.SetStartTime( setting );
	savefile->ReadFloat( setting );
	fovSetting.SetDuration( setting );
	savefile->ReadFloat( setting );
	fovSetting.SetStartValue( setting );
	savefile->ReadFloat( setting );
	fovSetting.SetEndValue( setting );

	fovSetting.GetCurrentValue( gameLocal.time );
}

/*
================
idTarget_SetFov::Event_Activate
================
*/
void idTarget_SetFov::Event_Activate( idEntity *activator ) {
	// always allow during cinematics
	cinematic = true;

	idPlayer *player = gameLocal.GetLocalPlayer();
	fovSetting.Init( gameLocal.time, SEC2MS( spawnArgs.GetFloat( "time" ) ), player ? player->DefaultFov() : g_fov.GetFloat(), spawnArgs.GetFloat( "fov" ) );
	BecomeActive( TH_THINK );
}

/*
================
idTarget_SetFov::Think
================
*/
void idTarget_SetFov::Think( void ) {
	if ( thinkFlags & TH_THINK ) {
		idPlayer *player = gameLocal.GetLocalPlayer();
		player->SetInfluenceFov( fovSetting.GetCurrentValue( gameLocal.time ) );
		if ( fovSetting.IsDone( gameLocal.time ) ) {
			player->SetInfluenceFov( 0.0f );
			BecomeInactive( TH_THINK );
		}
	} else {
		BecomeInactive( TH_ALL );
	}
}


/*
===============================================================================

idTarget_SetPrimaryObjective

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_SetPrimaryObjective )
	EVENT( EV_Activate,	idTarget_SetPrimaryObjective::Event_Activate )
END_CLASS

/*
================
idTarget_SetPrimaryObjective::Event_Activate
================
*/
void idTarget_SetPrimaryObjective::Event_Activate( idEntity *activator ) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	if ( player && player->objectiveSystem ) {
		player->objectiveSystem->SetStateString( "missionobjective", spawnArgs.GetString( "text", common->GetLanguageDict()->GetString( "#str_04253" ) ) );
	}
}

/*
===============================================================================

idTarget_LockDoor

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_LockDoor )
	EVENT( EV_Activate,	idTarget_LockDoor::Event_Activate )
END_CLASS

/*
================
idTarget_LockDoor::Event_Activate
================
*/
void idTarget_LockDoor::Event_Activate( idEntity *activator ) {
	int i;
	idEntity *ent;
	int lock;

	lock = spawnArgs.GetInt( "locked", "1" );
	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( ent && ent->IsType( idDoor::Type ) ) {
			if ( static_cast<idDoor *>( ent )->IsLocked() ) {
				static_cast<idDoor *>( ent )->Lock( 0 );
			} else {
				static_cast<idDoor *>( ent )->Lock( lock );
			}
		}
	}
}

/*
===============================================================================

idTarget_CallObjectFunction

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_CallObjectFunction )
	EVENT( EV_Activate,	idTarget_CallObjectFunction::Event_Activate )
END_CLASS

/*
================
idTarget_CallObjectFunction::Event_Activate
================
*/
void idTarget_CallObjectFunction::Event_Activate( idEntity *activator ) {
	int					i;
	idEntity			*ent;
	const function_t	*func;
	const char			*funcName;
	idThread			*thread;

	funcName = spawnArgs.GetString( "call" );
	for( i = 0; i < targets.Num(); i++ ) {
		ent = targets[ i ].GetEntity();
		if ( ent && ent->scriptObject.HasObject() ) {
			func = ent->scriptObject.GetFunction( funcName );
			if ( !func ) {
				gameLocal.Error( "Function '%s' not found on entity '%s' for function call from '%s'", funcName, ent->name.c_str(), name.c_str() );
			}
			if ( func->type->NumParameters() != 1 ) {
				gameLocal.Error( "Function '%s' on entity '%s' has the wrong number of parameters for function call from '%s'", funcName, ent->name.c_str(), name.c_str() );
			}
			if ( !ent->scriptObject.GetTypeDef()->Inherits( func->type->GetParmType( 0 ) ) ) {
				gameLocal.Error( "Function '%s' on entity '%s' is the wrong type for function call from '%s'", funcName, ent->name.c_str(), name.c_str() );
			}
			// create a thread and call the function
			thread = new idThread();
			thread->CallFunction( ent, func, true );
			thread->Start();
		}
	}
}


/*
===============================================================================

idTarget_EnableLevelWeapons

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_EnableLevelWeapons )
	EVENT( EV_Activate,	idTarget_EnableLevelWeapons::Event_Activate )
END_CLASS

/*
================
idTarget_EnableLevelWeapons::Event_Activate
================
*/
void idTarget_EnableLevelWeapons::Event_Activate( idEntity *activator ) {
	int i;
	const char *weap;

	gameLocal.world->spawnArgs.SetBool( "no_Weapons", spawnArgs.GetBool( "disable" ) );

	if ( spawnArgs.GetBool( "disable" ) ) {
		for( i = 0; i < gameLocal.numClients; i++ ) {
			if ( gameLocal.entities[ i ] ) {
				gameLocal.entities[ i ]->ProcessEvent( &EV_Player_DisableWeapon );
			}
		}
	} else {
		weap = spawnArgs.GetString( "weapon" );
		for( i = 0; i < gameLocal.numClients; i++ ) {
			if ( gameLocal.entities[ i ] ) {
				gameLocal.entities[ i ]->ProcessEvent( &EV_Player_EnableWeapon );
				if ( weap && weap[ 0 ] ) {
					gameLocal.entities[ i ]->PostEventSec( &EV_Player_SelectWeapon, 0.5f, weap );
				}
			}
		}
	}
}

/*
===============================================================================

idTarget_Tip

===============================================================================
*/

const idEventDef EV_TipOff( "<TipOff>" );
extern const idEventDef EV_GetPlayerPos( "<getplayerpos>" );

CLASS_DECLARATION( idTarget, idTarget_Tip )
	EVENT( EV_Activate,		idTarget_Tip::Event_Activate )
	EVENT( EV_TipOff,		idTarget_Tip::Event_TipOff )
	EVENT( EV_GetPlayerPos,	idTarget_Tip::Event_GetPlayerPos )
END_CLASS


/*
================
idTarget_Tip::idTarget_Tip
================
*/
idTarget_Tip::idTarget_Tip( void ) {
	playerPos.Zero();
}

/*
================
idTarget_Tip::Spawn
================
*/
void idTarget_Tip::Spawn( void ) {
}

/*
================
idTarget_Tip::Save
================
*/
void idTarget_Tip::Save( idSaveGame *savefile ) const {
	savefile->WriteVec3( playerPos ); // idVec3 playerPos
}

/*
================
idTarget_Tip::Restore
================
*/
void idTarget_Tip::Restore( idRestoreGame *savefile ) {
	savefile->ReadVec3( playerPos ); // idVec3 playerPos
}

/*
================
idTarget_Tip::Event_Activate
================
*/
void idTarget_Tip::Event_GetPlayerPos( void ) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	if ( player ) {
		playerPos = player->GetPhysics()->GetOrigin();
		PostEventMS( &EV_TipOff, 100 );
	}
}

/*
================
idTarget_Tip::Event_Activate
================
*/
void idTarget_Tip::Event_Activate( idEntity *activator ) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	if ( player ) {
		if ( player->IsTipVisible() ) {
			PostEventSec( &EV_Activate, 5.1f, activator );
			return;
		}
		player->ShowTip( spawnArgs.GetString( "text_title" ), spawnArgs.GetString( "text_tip" ), false );
		PostEventMS( &EV_GetPlayerPos, 2000 );
	}
}

/*
================
idTarget_Tip::Event_TipOff
================
*/
void idTarget_Tip::Event_TipOff( void ) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	if ( player ) {
		idVec3 v = player->GetPhysics()->GetOrigin() - playerPos;
		if ( v.Length() > 96.0f ) {
			player->HideTip();
		} else {
			PostEventMS( &EV_TipOff, 100 );
		}
	}
}


/*
===============================================================================

idTarget_GiveSecurity

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_GiveSecurity )
EVENT( EV_Activate,	idTarget_GiveSecurity::Event_Activate )
END_CLASS

/*
================
idTarget_GiveEmail::Event_Activate
================
*/
void idTarget_GiveSecurity::Event_Activate( idEntity *activator ) {
	idPlayer *player = gameLocal.GetLocalPlayer();
	if ( player ) {
		player->GiveSecurity( spawnArgs.GetString( "text_security" ) );
	}
}


/*
===============================================================================

idTarget_RemoveWeapons

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_RemoveWeapons )
EVENT( EV_Activate,	idTarget_RemoveWeapons::Event_Activate )
END_CLASS

/*
================
idTarget_RemoveWeapons::Event_Activate
================
*/
void idTarget_RemoveWeapons::Event_Activate( idEntity *activator ) {
	for( int i = 0; i < gameLocal.numClients; i++ ) {
		if ( gameLocal.entities[ i ] ) {
			idPlayer *player = static_cast< idPlayer* >( gameLocal.entities[i] );
			const idKeyValue *kv = spawnArgs.MatchPrefix( "weapon", NULL );
			while ( kv ) {
				player->RemoveWeapon( kv->GetValue() );
				kv = spawnArgs.MatchPrefix( "weapon", kv );
			}
			player->SelectWeapon( 0, player->weapon_fists, true );
		}
	}
}


/*
===============================================================================

idTarget_LevelTrigger

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_LevelTrigger )
EVENT( EV_Activate,	idTarget_LevelTrigger::Event_Activate )
END_CLASS

/*
================
idTarget_LevelTrigger::Event_Activate
================
*/
void idTarget_LevelTrigger::Event_Activate( idEntity *activator ) {
	for( int i = 0; i < gameLocal.numClients; i++ ) {
		if ( gameLocal.entities[ i ] ) {
			idPlayer *player = static_cast< idPlayer* >( gameLocal.entities[i] );
			player->SetLevelTrigger( spawnArgs.GetString( "levelName" ), spawnArgs.GetString( "triggerName" ) );
		}
	}
}


/*
===============================================================================

idTarget_EnableStamina

===============================================================================
*/

CLASS_DECLARATION( idTarget, idTarget_EnableStamina )
EVENT( EV_Activate,	idTarget_EnableStamina::Event_Activate )
END_CLASS

/*
================
idTarget_EnableStamina::Event_Activate
================
*/
void idTarget_EnableStamina::Event_Activate( idEntity *activator ) {
	for( int i = 0; i < gameLocal.numClients; i++ ) {
		if ( gameLocal.entities[ i ] ) {
			idPlayer *player = static_cast< idPlayer* >( gameLocal.entities[i] );
			if ( spawnArgs.GetBool( "enable" ) ) {
				pm_stamina.SetFloat( player->spawnArgs.GetFloat( "pm_stamina" ) );
			} else {
				pm_stamina.SetFloat( 0.0f );
			}
		}
	}
}

/*
===============================================================================

idTarget_FadeSoundClass

===============================================================================
*/

const idEventDef EV_RestoreVolume( "<RestoreVolume>" );
CLASS_DECLARATION( idTarget, idTarget_FadeSoundClass )
EVENT( EV_Activate,	idTarget_FadeSoundClass::Event_Activate )
EVENT( EV_RestoreVolume, idTarget_FadeSoundClass::Event_RestoreVolume )
END_CLASS

/*
================
idTarget_FadeSoundClass::Event_Activate
================
*/
void idTarget_FadeSoundClass::Event_Activate( idEntity *activator ) {
	float fadeTime = spawnArgs.GetFloat( "fadeTime" );
	float fadeDB = spawnArgs.GetFloat( "fadeDB" );
	float fadeDuration = spawnArgs.GetFloat( "fadeDuration" );
	int fadeClass = spawnArgs.GetInt( "fadeClass" );
	// start any sound fading now
	if ( fadeTime ) {
		gameSoundWorld->FadeSoundClasses( fadeClass, spawnArgs.GetBool( "fadeIn" ) ? fadeDB : 0.0f - fadeDB, fadeTime );
		if ( fadeDuration ) {
			PostEventSec( &EV_RestoreVolume, fadeDuration );
		}
	}
}

/*
================
idTarget_FadeSoundClass::Event_RestoreVolume
================
*/
void idTarget_FadeSoundClass::Event_RestoreVolume() {
	float fadeTime = spawnArgs.GetFloat( "fadeTime" );
	float fadeDB = spawnArgs.GetFloat( "fadeDB" );
	// restore volume
	gameSoundWorld->FadeSoundClasses( 0, fadeDB, fadeTime );
}
