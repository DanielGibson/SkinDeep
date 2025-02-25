#include "sys/platform.h"
#include "gamesys/SysCvar.h"
#include "physics/Physics_RigidBody.h"
#include "Entity.h"
#include "Light.h"
#include "Player.h"
#include "Fx.h"
#include "framework/DeclEntityDef.h"

#include "WorldSpawn.h"
#include "bc_randpackage.h"



//Banana
//Turns into a banana peel when it collides with something.

#define COLLISION_VELOCITY_THRESHOLD 10

CLASS_DECLARATION(idMoveableItem, idRandPackage)
	EVENT(EV_Touch, idRandPackage::Event_Touch)
END_CLASS

void idRandPackage::Save(idSaveGame *savefile) const
{
}

void idRandPackage::Restore(idRestoreGame *savefile)
{
}

void idRandPackage::Spawn(void)
{
	hasBeenOpened = false;
	spawnTime = gameLocal.time;

	//Error checking.
	const idKeyValue* kv;
	kv = spawnArgs.MatchPrefix("def_item", NULL);
	while (kv)
	{
		//validate definition.
		idStr defname = kv->GetValue();
		const idDeclEntityDef* itemDef = gameLocal.FindEntityDef(defname.c_str(), false);
		if (!itemDef)
		{
			gameLocal.Error("idRandPackage '%s' has invalid item definition: '%s'", GetName(), defname.c_str());
			return;
		}
		
		kv = spawnArgs.MatchPrefix("def_item", kv);
	}
}

bool idRandPackage::Collide(const trace_t& collision, const idVec3& velocity)
{
	//Don't do checks if being held by player
	if (gameLocal.GetLocalPlayer()->GetCarryable() != NULL)
	{
		if (gameLocal.GetLocalPlayer()->GetCarryable() == this)
		{
			return false;
		}
	}

	idMoveableItem::Collide(collision, velocity);

	//package explodes open.
	float v;
	v = -(velocity * collision.c.normal);	
	if (v > COLLISION_VELOCITY_THRESHOLD && !hasBeenOpened && gameLocal.time > spawnTime + 2000) //don't open it right at game start.
	{
		//turn into banana peel.
		hasBeenOpened = true;

		idStr itemSpawnDef = GetRandomSpawnDef();
		if (itemSpawnDef.Length() > 0)
		{			
			const idDeclEntityDef* itemDef;
			itemDef = gameLocal.FindEntityDef(itemSpawnDef, false);
			if (itemDef)
			{
				//found definition for the item to spawn.
				//Spawn item.
				idEntity* itemEnt;
				idDict args;
				args.Set("classname", itemSpawnDef);
				args.SetVector("origin", GetPhysics()->GetOrigin());
				gameLocal.SpawnEntityDef(args, &itemEnt);

			}
		}

		//Destroy self.
		gameLocal.DoParticle(spawnArgs.GetString("model_open"), GetPhysics()->GetOrigin()); //Particle effect for package bursting open.
		Hide();
		physicsObj.SetContents(0);
		PostEventMS(&EV_Remove, 0);
		return false;
	}

	return true;
}

idStr idRandPackage::GetRandomSpawnDef()
{
	idStrList itemlist;

	const idKeyValue* kv;
	kv = spawnArgs.MatchPrefix("def_item", NULL);
	while (kv)
	{
		itemlist.Append(kv->GetValue());
		kv = spawnArgs.MatchPrefix("def_item", kv);
	}

	if (itemlist.Num() <= 0)
		return "";

	int randomIdx = gameLocal.random.RandomInt(itemlist.Num());	
	return itemlist[randomIdx];
}