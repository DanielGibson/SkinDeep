#include "sys/platform.h"
#include "gamesys/SysCvar.h"
#include "physics/Physics_RigidBody.h"
#include "Entity.h"
#include "Light.h"
#include "Player.h"
#include "Fx.h"
#include "Moveable.h"
#include "trigger.h"

#include "WorldSpawn.h"
#include "mover.h"
#include "bc_meta.h"
#include "bc_cryointerior.h"
#include "bc_doorbarricade.h"
#include "bc_stresstester.h"

#define GLOBALMAPRESTARTTIME_MINUTES 10
#define GLOBALUNLOCKFUSEBOX_MINUTES 5

#define STUCK_COUNTERTHRESHOLD 6

CLASS_DECLARATION(idEntity, idStressTester)
END_CLASS

//StressTester
//Make the player randomly wander around the map.
//Auto restarts the map every xx minutes.

//Some bugs only happen when multiple gameplay systems interact with each other. The intention of StressTester is
//to allow the game to be run overnight to catch any of these hard-to-repro gameplay bugs.

void idStressTester::Spawn(void)
{
	GetPhysics()->SetContents(0);
	fl.takedamage = false;
	timer = gameLocal.time + 1000;
	state = CRYOPOD_DELAY;
	pushledgeCount = 0;
	doorTimer = 0;
	jockeyTimer = 0;
	jockeySlamTimer = 0;
	
	gameLocal.GetLocalPlayer()->godmode = true;

	BecomeActive(TH_THINK);
	common->Printf("\n\n*********** STRESSTEST SPAWNED ***********\n\n");

	globalRestartTimer = gameLocal.time + (GLOBALMAPRESTARTTIME_MINUTES * (1000 * 60));
	globalUnlockFuseboxTimer = gameLocal.time + (GLOBALUNLOCKFUSEBOX_MINUTES * (1000 * 60));
	hasUnlockedGlobalFusebox = false;

	stuckTimer = 0;
	GetUpTimer = 0;
	stuckLastPosition = vec3_zero;
	stuckCounter = 10000;
}

void idStressTester::Save(idSaveGame *savefile) const
{
}

void idStressTester::Restore(idRestoreGame *savefile)
{
}

void idStressTester::Think(void)
{
	if (state == CRYOPOD_DELAY)
	{
		if (gameLocal.time > timer)
		{
			//Frob the cryo pod exit.
			state = POSTCRYO_DELAY;
			ExitCryopod();
			timer = gameLocal.time + 1000;
		}
	}
	else if (state == POSTCRYO_DELAY)
	{
		if (gameLocal.time > timer)
		{
			//The player might be on a vent or something.
			//Try to push them off. Push player around in all directions.
			state = PUSHLEDGE;
			pushledgeCount = 0;
			timer = 0;
			common->Printf("Stresstest: jumping.\n");
		}
	}
	else if (state == PUSHLEDGE)
	{
		//This nudges the player around. This is used at game start because the player sometimes
		//starts on a ledge/catwalk.

		if (gameLocal.time > timer)
		{
			idVec3 pushDir;
			if (pushledgeCount == 0)
				pushDir = idVec3(1, 0, .5f);
			else if (pushledgeCount == 1)
				pushDir = idVec3(-1, 0, .5f);
			else if (pushledgeCount == 2)
				pushDir = idVec3(0, 1, .5f);
			else if (pushledgeCount == 3)
				pushDir = idVec3(0, -1, .5f);

			#define PUSHFORCE 384
			gameLocal.GetLocalPlayer()->GetPhysics()->SetLinearVelocity(pushDir * PUSHFORCE);
			pushledgeCount++;
			timer = gameLocal.time + 900;
			if (pushledgeCount >= 4)
			{
				state = WANDER;
				cvarSystem->SetCVarBool("aas_randomPullPlayer", true);

				//unlock all barricades.
				for (idEntity* ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next())
				{
					if (!ent)
						continue;

					if (ent->IsType(idDoorBarricade::Type))
					{
						static_cast<idDoorBarricade*>(ent)->DoHack();
					}
				}
			}
		}
	}
	else if (state == WANDER)
	{
		//Try to frob any door in front of me.
		if (gameLocal.time > doorTimer)
		{
			doorTimer = gameLocal.time + 2000;
			FrobDoorAttempt();				
		}
		
		//Try to jockey enemies that are nearby.
		if (gameLocal.time > jockeyTimer)
		{
			jockeyTimer = gameLocal.time + 30000;
			if (!gameLocal.GetLocalPlayer()->IsJockeying())
			{
				AttemptJockey();
			}
		}

		//Try to jockey-slam if I am jockeying.
		if (gameLocal.GetLocalPlayer()->IsJockeying())
		{
			if (gameLocal.time > jockeySlamTimer)
			{
				jockeySlamTimer = gameLocal.time + 3000;
				AttemptJockeySlam();
			}
		}

		//See if we need to get up.
		if (gameLocal.time > GetUpTimer)
		{
			GetUpTimer = gameLocal.time + 5000;
			AttemptGetUp();
		}

		UpdateStuckLogic();
	}

	//Restart map 
	if (gameLocal.time > globalRestartTimer)
	{
		globalRestartTimer = gameLocal.time + 900000; //this is just so that we don't try to restart the map multiple times.
		//restart map.
		idStr restartCommand = "reloadScript";
		cmdSystem->BufferCommandText(CMD_EXEC_NOW, restartCommand.c_str());
	}

	//Unlock all fuseboxes after a set time.
	if (!hasUnlockedGlobalFusebox && gameLocal.time > globalUnlockFuseboxTimer)
	{
		hasUnlockedGlobalFusebox = true;
		UnlockAllFusebox();
	}
}

void idStressTester::UpdateStuckLogic()
{
	if (gameLocal.time < stuckTimer)
		return;

	stuckTimer = gameLocal.time + 300;

	float distanceTravelled = (stuckLastPosition - gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin()).Length();


	if (distanceTravelled < .1f)
	{
		//hasn't moved.
		stuckCounter++;

		if (stuckCounter >= STUCK_COUNTERTHRESHOLD)
		{
			cvarSystem->SetCVarBool("aas_randomPullPlayer", false);
			cvarSystem->SetCVarInteger("aas_pullPlayer", 0);
			
			stuckCounter = 0;
			state = POSTCRYO_DELAY;
			timer = 0;
		}
	}
	else
	{
		//reset the stuck counter.
		stuckCounter = 0;
	}
	

	stuckLastPosition = gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin();

}

void idStressTester::AttemptGetUp()
{
	if (!gameLocal.GetLocalPlayer()->GetFallenState())
		return;

	//Get up from fallen state.
	gameLocal.GetLocalPlayer()->Event_SetFallState(false, false, false);
}

void idStressTester::UnlockAllFusebox()
{
	static_cast<idMeta*>(gameLocal.metaEnt.GetEntity())->SetEnableTrashchutes(true);
	static_cast<idMeta*>(gameLocal.metaEnt.GetEntity())->SetEnableAirlocks(true);
	static_cast<idMeta*>(gameLocal.metaEnt.GetEntity())->SetEnableWindows(true);
	static_cast<idMeta*>(gameLocal.metaEnt.GetEntity())->SetEnableVents(true);
}

void idStressTester::AttemptJockeySlam()
{
	if (!meleeTarget.IsValid())
		return;

	if (!meleeTarget.GetEntity()->IsType(idAI::Type))
		return;

	static_cast<idAI*>(meleeTarget.GetEntity())->DoJockeyBrutalSlam();
	//gameLocal.GetLocalPlayer()->dojo
	//
	//if (aiEnt->jockeyAttackCurrentlyAvailable == JOCKATKTYPE_KILLENTITY)
	//	if (aiEnt->jockeyKillEntity.IsValid())
}

void idStressTester::AttemptJockey()
{
	//See if there's any enemies nearby.

	idEntity* enemy = FindVisibleEnemy();
	if (enemy == NULL)
		return;

	if (!enemy->IsType(idAI::Type))
		return;

	meleeTarget = enemy;
	gameLocal.GetLocalPlayer()->meleeTarget = static_cast<idAI*>(enemy);
	gameLocal.GetLocalPlayer()->SetJockeyMode(true);
}

idEntity* idStressTester::FindVisibleEnemy()
{
	for (idEntity* entity = gameLocal.aimAssistEntities.Next(); entity != NULL; entity = entity->aimAssistNode.Next())
	{
		if (!entity || !entity->IsActive() || entity->health <= 0 || entity->IsHidden())
			continue;

		trace_t tr;
		gameLocal.clip.TracePoint(tr, gameLocal.GetLocalPlayer()->firstPersonViewOrigin, entity->GetPhysics()->GetOrigin() + idVec3(0, 0, 32), MASK_SOLID, NULL);
		if (tr.fraction >= 1)
			return entity;
	}

	return NULL;
}

void idStressTester::FrobDoorAttempt()
{
	trace_t tr;
	gameLocal.clip.TracePoint(tr, gameLocal.GetLocalPlayer()->firstPersonViewOrigin, gameLocal.GetLocalPlayer()->firstPersonViewOrigin + gameLocal.GetLocalPlayer()->viewAngles.ToForward() * 64, MASK_SOLID, NULL);

	if (tr.fraction >= 1.0f)
		return;

	if (gameLocal.entities[tr.c.entityNum]->IsType(idWorldspawn::Type))
		return;

	if (gameLocal.entities[tr.c.entityNum]->IsType(idDoor::Type))
	{
		gameLocal.entities[tr.c.entityNum]->DoFrob(0, gameLocal.GetLocalPlayer());
	}
}

void idStressTester::ExitCryopod()
{
	//Player in cryo pod. Exit cryo pod.
	if (static_cast<idMeta*>(gameLocal.metaEnt.GetEntity())->GetPlayerExitedCryopod())
		return;

	//Find the cryo pod.
	for (idEntity* ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next())
	{
		if (!ent)
			continue;

		if (!ent->IsType(idCryointerior::Type))
			continue;

		common->Printf("Stresstest: frobbing cryointerior.\n");
		static_cast<idCryointerior*>(ent)->DoFrob(PEEKFROB_INDEX, gameLocal.GetLocalPlayer());
		return;
	}	
}