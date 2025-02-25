#include "Trigger.h"
#include "framework/DeclEntityDef.h"
#include "Player.h"

#include "bc_meta.h"
#include "bc_trigger_deodorant.h"

//Flammable gas cloud. Emits from deodorant and gas pipe.

CLASS_DECLARATION(idTrigger_Multi, idTrigger_deodorant)
EVENT(EV_Touch, idTrigger_deodorant::Event_Touch)
END_CLASS

idTrigger_deodorant::idTrigger_deodorant()
{	
}

idTrigger_deodorant::~idTrigger_deodorant(void)
{
}

void idTrigger_deodorant::Spawn()
{
	displayName = "Flammable Gas";

	fl.takedamage = true;
	idTrigger_Multi::Spawn();

	active = true;

	maxlifetime = gameLocal.time + spawnArgs.GetInt("spewLifetime");

	//Particle fx.
	particleEmitter = NULL;
	idDict splashArgs;
	splashArgs.Set("classname", "func_emitter");
	splashArgs.Set("model", spawnArgs.GetString("spewParticle", "deodorantburst01.prt"));
	splashArgs.Set("start_off", "1");
	splashArgs.SetVector("origin", this->GetPhysics()->GetOrigin());
	particleEmitter = static_cast<idFuncEmitter *>(gameLocal.SpawnEntityType(idFuncEmitter::Type, &splashArgs));
	particleEmitter->SetActive(true);
}



void idTrigger_deodorant::Event_Touch(idEntity* other, trace_t* trace)
{
	if (other->IsType(idMoveableItem::Type))
	{
		if (static_cast<idMoveableItem *>(other)->IsOnFire())
		{
			//touched by an entity that is on fire, or is sparking.
			BurstIntoFlames(other);
		}
	}
	
	if (other->IsType(idPlayer::Type))
	{
		if (((idPlayer *)other)->GetSmelly())
		{
			((idPlayer *)other)->SetSmelly(0);
		}
	}
}

void idTrigger_deodorant::BurstIntoFlames(idEntity *inflictor)
{
	if (!active)
		return;

	active = false;

	if (inflictor != nullptr)
	{
		idStr inflictorname = (inflictor->displayName.Length() > 0) ? inflictor->displayName : idStr(inflictor->GetName());
		idStr ignitionText = idStr::Format("#str_def_gameplay_cloud_ignited", inflictorname.c_str());
		gameLocal.AddEventLog(ignitionText.c_str(), GetPhysics()->GetOrigin());

		static_cast<idMeta*>(gameLocal.metaEnt.GetEntity())->DoHighlighter(inflictor, this);
	}

	//Make a fireball.
	idEntity *fireballEnt;
	idDict args;
	args.Set("classname", "env_fireball");
	args.SetVector("origin", this->GetPhysics()->GetOrigin());
	args.Set("displayname", "Fireball");
	gameLocal.SpawnEntityDef(args, &fireballEnt);

	if (particleEmitter)
	{
		particleEmitter->PostEventMS(&EV_Remove, 500); //let the particle linger a bit so that it disappears during the middle of the fireball.
	}

	//Remove self.
	this->PostEventMS(&EV_Remove, 0);
}

void idTrigger_deodorant::Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType)
{
	const idDeclEntityDef *damageDef = gameLocal.FindEntityDef(damageDefName, false);

	if (!damageDef)
		return;

	if (damageDef->dict.GetBool("isfire"))
	{
		BurstIntoFlames(attacker); //Received damage that has the isfire bool.
	}
}

void idTrigger_deodorant::Think()
{
	if (!active)
		return;

	if (gameLocal.time > maxlifetime)
	{
		particleEmitter->SetActive(false);
		particleEmitter->PostEventMS(&EV_Remove, 1000);

		this->PostEventMS(&EV_Remove, 0);
	}
}
