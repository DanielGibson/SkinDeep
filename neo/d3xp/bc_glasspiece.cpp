#include "sys/platform.h"
#include "gamesys/SysCvar.h"
#include "physics/Physics_RigidBody.h"
#include "Entity.h"
#include "Light.h"
#include "Player.h"
#include "Fx.h"
#include "framework/DeclEntityDef.h"

#include "WorldSpawn.h"
#include "bc_meta.h"
#include "bc_glasspiece.h"


#define GLASSWOUND_COOLDOWNTIME 100 //if player has received a glass wound, have a cooldown period before another glasswound happens.
#define BREAK_THRESHOLD 400 //if collide & above this velocity, then shatter it.



#define SHINE_MINTIME 3000 //timing parameters for the shine particle effect.
#define SHINE_MAXTIME 8000

#define GLASSWOUND_DISTANCETHRESHOLD 16 //player's feet have to be XX close to glass in order to get a glasswound.

CLASS_DECLARATION(idMoveableItem, idGlassPiece)
EVENT(EV_Touch, idGlassPiece::Event_Touch)
END_CLASS

void idGlassPiece::Save(idSaveGame *savefile) const
{
}

void idGlassPiece::Restore(idRestoreGame *savefile)
{
}


void idGlassPiece::Spawn(void)
{
	GetPhysics()->SetContents(CONTENTS_RENDERMODEL | CONTENTS_TRIGGER);
	//GetPhysics()->SetClipMask(MASK_SOLID | CONTENTS_MOVEABLECLIP);
	physicsObj.SetClipMask(MASK_SOLID | CONTENTS_RENDERMODEL);
	nextTouchTime = 0;
	hasSettledDown = false;
	spawnTimer = gameLocal.time;
	shineTimer = gameLocal.time + 300;
	initialSpawnFallTimer = 0;
	spawnfallDone = false;

	BecomeActive(TH_THINK);
}

void idGlassPiece::Create(const idVec3 &start, const idMat3 &axis)
{
	Unbind();
	GetPhysics()->SetOrigin(start);
	GetPhysics()->SetAxis(axis);

	UpdateVisuals();
}

void idGlassPiece::Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType )
{
	const idDeclEntityDef *damageDef = gameLocal.FindEntityDef(damageDefName, false);

	if (!fl.takedamage)
		return;

	if (!damageDef)
	{
		common->Warning("%s unable to find damagedef %s\n", this->GetName(), damageDefName);
		return;
	}

	if (damageDef->dict.GetBool("ischem"))
	{
		//ignore chemical damage. This is so chem grenades can spawn glass shards.
		return;
	}

	idMoveableItem::Damage(inflictor, attacker, dir, damageDefName, damageScale, location);
}


//This gets called when the player walks onto a glasspiece.
void idGlassPiece::Event_Touch(idEntity *other, trace_t *trace)
{
	//Ouch!!!!!!!!! Player has stepped on a glass shard.
	if (nextTouchTime > gameLocal.time  )
		return;

	nextTouchTime = gameLocal.time + GLASSWOUND_COOLDOWNTIME;

	if (gameLocal.GetLocalPlayer()->GetCarryable() != NULL)
	{
		if (gameLocal.GetLocalPlayer()->GetCarryable() == this)
		{
			return;
		}
	}

	if (!other->IsType(idActor::Type))
	{
		if (other != this && hasSettledDown && !other->GetPhysics()->IsAtRest())
		{
			//When several glass pieces fall down in a pile, we want to ensure they don't just destroy each other immediately upon landing on the ground together
			if (other->IsType(idGlassPiece::Type))
			{
				if (static_cast<idGlassPiece *>(other)->IsDoingInitialSpawnFall() || other->GetPhysics()->IsAtRest() || other->GetPhysics()->GetLinearVelocity().Length() <= 8)
				{
					return;
				}
			}

			ShatterAndRemove();
		}

		return;
	}

	//Cap the rate at which we accrue glass wounds on player. Don't do multiple wounds on same frame.
	if (other->IsType(idPlayer::Type))
	{
		if (gameLocal.time < gameLocal.GetLocalPlayer()->GetLastGlasswoundTime() + 10)
			return;

		if (gameLocal.GetLocalPlayer()->airless) //if in zero-g , then ignore glass wound.
			return;

		float distanceToPlayer = (other->GetPhysics()->GetOrigin() - this->GetPhysics()->GetOrigin()).Length();
		if (distanceToPlayer > GLASSWOUND_DISTANCETHRESHOLD && !gameLocal.GetLocalPlayer()->IsCrouching())
			return;

		if (gameLocal.GetLocalPlayer()->IsJockeying())
			return;
	}


	//if player is looking downward, then player auto-avoids stepping on glass.
	//if (gameLocal.GetLocalPlayer()->viewAngles.pitch < 50)
	{
		//Player was NOT looking down.
		//if (other->IsType(idPlayer::Type) && !gameLocal.GetLocalPlayer()->IsCrouching() && (gameLocal.GetLocalPlayer()->usercmd.forwardmove != 0 || gameLocal.GetLocalPlayer()->usercmd.rightmove != 0))
		if (other->IsType(idPlayer::Type) && !gameLocal.GetLocalPlayer()->IsCrouching() && gameLocal.GetLocalPlayer()->GetPhysics()->GetLinearVelocity().Length() > 0)
		{
			//Puncture the foot.		
			trace_t floorPos;

			gameLocal.clip.TracePoint(floorPos, this->GetPhysics()->GetOrigin(), this->GetPhysics()->GetOrigin() + idVec3(0, 0, -8), MASK_SOLID, gameLocal.GetLocalPlayer());
			gameLocal.ProjectDecal(floorPos.endpos, -floorPos.c.normal, 8.0f, true, 20.0f + gameLocal.random.RandomFloat() * 20.0f, "textures/decals/bloodsplat00");
			gameLocal.GetLocalPlayer()->SetGlasswound(1);
		}
	}
	
	//Delete glass pieces that are within close proximity.
	RemoveNearbyGlassPieces();

	ShatterAndRemove();
}

bool idGlassPiece::IsDoingInitialSpawnFall()
{
	return !spawnfallDone;
}

void idGlassPiece::RemoveNearbyGlassPieces()
{
	idEntity		*entityList[MAX_GENTITIES];
	int entityCount = gameLocal.EntitiesWithinRadius(this->GetPhysics()->GetOrigin(), CLEANUP_RADIUS, entityList, MAX_GENTITIES);
	for (int i = 0; i < entityCount; i++)
	{
		idEntity *ent = entityList[i];

		if (!ent)
			continue;

		if (ent->IsHidden() || !ent->IsType(idGlassPiece::Type) || ent == this)
			continue;

		if (gameLocal.GetLocalPlayer()->GetCarryable() != NULL)
		{
			if (gameLocal.GetLocalPlayer()->GetCarryable() == ent)
			{
				continue;
			}
		}

		static_cast<idGlassPiece *>(ent)->ShatterAndRemove();
	}
}


void idGlassPiece::ShatterAndRemove()
{
	if (health <= 0)
		return;

	health = 0;

	//Do this before removing objects from world.
	this->Hide();
	physicsObj.SetContents(0);
	

	idAngles fxAngle;
	fxAngle.yaw = gameLocal.random.RandomInt(350);
	idEntityFx::StartFx("fx/glass_shard_break", GetPhysics()->GetOrigin(), fxAngle.ToMat3());

	//Remove the shard.
	PostEventMS(&EV_Remove, 0);


	//When a glass piece is destroyed, destroy nearby glass pieces.
	//We do this via idMeta, because we want to limit how often this gets called. If it gets recursively called forever it causes problems, so
	//idMeta adds a cooldown timer to how often it gets called.
	static_cast<idMeta *>(gameLocal.metaEnt.GetEntity())->DestroyNearbyGlassPieces(GetPhysics()->GetOrigin());
	//RemoveNearbyGlassPieces();
}

void idGlassPiece::Think(void)
{
	idMoveableItem::Think();

	if (!hasSettledDown)
	{
		float velocityLength = GetPhysics()->GetLinearVelocity().LengthFast();
		float angularLength = GetPhysics()->GetAngularVelocity().LengthFast();

		if (GetPhysics()->IsAtRest() ||  (gameLocal.time - spawnTimer > 1000 && velocityLength <= 50 && angularLength <= 50)) //if I am at rest OR if I've been alive for XX ms and my velocity is below XX...
		{
			hasSettledDown = true;
			//BecomeInactive(TH_THINK);
			initialSpawnFallTimer = gameLocal.time + 500;
		}
	}
	else if (hasSettledDown && !spawnfallDone)
	{
		if (gameLocal.time > initialSpawnFallTimer)
		{
			spawnfallDone = true;
		}
	}

	UpdateShine();
}



void idGlassPiece::UpdateShine()
{
	if (gameLocal.GetLocalPlayer()->GetCarryable() != NULL)
	{
		if (gameLocal.GetLocalPlayer()->GetCarryable() == this)
		{
			return;
		}
	}

	if (hasSettledDown && gameLocal.time > shineTimer && gameLocal.InPlayerPVS(this) && !IsHidden())
	{
		shineTimer = gameLocal.time + gameLocal.random.RandomInt(SHINE_MINTIME, SHINE_MAXTIME);

		//do the shine effect.
		gameLocal.DoParticle("sparkle_glass.prt", GetPhysics()->GetOrigin() + idVec3(0, 0, 1));
	}
}

//This gets called when the glasspiece touches the world.
bool idGlassPiece::Collide(const trace_t &collision, const idVec3 &velocity)
{
	if (gameLocal.GetLocalPlayer()->GetCarryable() != NULL)
	{
		if (gameLocal.GetLocalPlayer()->GetCarryable() == this)
		{
			return false;
		}
	}

	if (!hasSettledDown)
	{
		//GetPhysics()->SetLinearVelocity(velocity * 0.5f); //Slow down.
		//GetPhysics()->SetAngularVelocity(this->GetPhysics()->GetAngularVelocity() * .99f);

		return idMoveableItem::Collide(collision, velocity);
	}

	float velocityLength = velocity.LengthFast();
	if (velocityLength > BREAK_THRESHOLD && spawnfallDone)
	{
		//shatter it.
		ShatterAndRemove();
	}	

	return idMoveableItem::Collide(collision, velocity);
}

bool idGlassPiece::DoFrob(int index, idEntity * frobber)
{
	bool value = idMoveableItem::DoFrob(index, frobber);

	if (frobber != NULL && value)
	{
		if (frobber == gameLocal.GetLocalPlayer())
		{
			RemoveNearbyGlassPieces();
		}
	}

	return value;
}