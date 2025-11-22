#include "sys/platform.h"
#include "gamesys/SysCvar.h"
//#include "physics/Physics_RigidBody.h"
#include "Entity.h"
#include "Light.h"
#include "Player.h"
#include "Fx.h"
//#include "Moveable.h"
//#include "trigger.h"

#include "bc_key.h"

#include "bc_cargohide.h"



CLASS_DECLARATION(idStaticEntity, idCargohide)

END_CLASS


idCargohide::idCargohide(void)
{
}

idCargohide::~idCargohide(void)
{
}

void idCargohide::Spawn(void)
{
	GetPhysics()->SetContents(CONTENTS_SOLID);
	GetPhysics()->SetClipMask(MASK_SOLID);

	isFrobbable = true;
}

void idCargohide::Save(idSaveGame *savefile) const
{
	savefile->WriteInt( maxhealth ); //  int maxhealth
}

void idCargohide::Restore(idRestoreGame *savefile)
{
	savefile->ReadInt( maxhealth ); //  int maxhealth
}

void idCargohide::Think(void)
{
	idStaticEntity::Think();
}

void idCargohide::Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType )
{
}

bool idCargohide::DoFrob(int index, idEntity * frobber)
{
	if (frobber == NULL || frobber != gameLocal.GetLocalPlayer()) //Only player can frob.
	{
		return false;
	}

	//Determine whether someone saw me entering this cargo hide.
	gameLocal.GetLocalPlayer()->wasCaughtEnteringCargoHide = gameLocal.GetLocalPlayer()->IsCurrentlySeenByAI();
	if (gameLocal.GetLocalPlayer()->wasCaughtEnteringCargoHide)
		gameLocal.AddEventLog("#str_def_gameplay_caughthiding", gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin());


	isFrobbable = false;
	StartSound("snd_enter", SND_CHANNEL_ANY, 0, false, NULL);
	gameLocal.GetLocalPlayer()->SetHideState(this, spawnArgs.GetInt("cargotype", "0")); //Tell physics system to move player into the hidey spot.


	//BC 10-02-2025: SD 699. Push out any objects inside the cargohide. This addresses the issue where
	//a key inside the cargohide (ie laundry machine) cannot be frobbed, due to the cargohide frob box
	//taking priority.
	PushObjects();


	return true;
}


void idCargohide::PushObjects()
{
	if (spawnArgs.GetInt("cargotype") != 2) //only for the laundry cargohide
		return;

	idEntity* entityList[MAX_GENTITIES];
	int			listedEntities, i;

	//detect if there's stuff waiting to be trashed...
	listedEntities = gameLocal.EntitiesWithinAbsBoundingbox(GetPhysics()->GetAbsBounds(), entityList, MAX_GENTITIES);

	if (listedEntities > 0)
	{
		for (i = 0; i < listedEntities; i++)
		{
			idEntity* ent = entityList[i];

			if (!ent)
			{
				continue;
			}

			if (ent == this || ent->IsHidden())
			{
				continue;
			}

			if (gameLocal.GetLocalPlayer()->GetCarryable() != NULL)
			{
				if (gameLocal.GetLocalPlayer()->GetCarryable() == ent)
				{
					continue;
				}
			}

			if (ent->IsType(idSecurityKey::Type))
			{
				//Move object to the origin point, so that it's guaranteed to exit the space
				ent->SetOrigin(GetPhysics()->GetOrigin());


				//Throw the object.
				#define THROWAMOUNT_FORWARD 256
				#define THROWAMOUNT_UP 32
				idVec3 forward;
				this->GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);

				idVec3 throwVelocity = (forward * THROWAMOUNT_FORWARD) + (idVec3(0,0, THROWAMOUNT_UP));
				ent->GetPhysics()->SetLinearVelocity(throwVelocity);
			}

			
		}
	}
}
