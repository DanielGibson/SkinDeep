#include "sys/platform.h"
#include "gamesys/SysCvar.h"
#include "physics/Physics_RigidBody.h"
#include "Entity.h"
#include "Light.h"
#include "Player.h"
#include "Fx.h"
#include "Moveable.h"
#include "Trigger.h"

#include "bc_meta.h"
#include "bc_radiowall.h"

const int LIGHT_FORWARDPOS = 8;
const int LIGHT_UPPOS = -6;
const int LIGHT_RADIUS = 16;
const int ACTIVATE_STATIC_TIME = 2000; //when turned on, how long before the interestpoint is spawned
const int INTEREST_INTERVALTIME = 1000;

const int MONSTER_FROBINDEX = 7; //make sure this matches the frobindex in interestpoint .def interest_radio

const idVec3 IDLECOLOR = idVec3(0, 1, 0);
const idVec3 FROBCOLOR = idVec3(1, .9f, 0);

CLASS_DECLARATION(idStaticEntity, idRadioWall)
END_CLASS

idRadioWall::idRadioWall(void)
{
	memset(&headlight, 0, sizeof(headlight));
	headlightHandle = -1;

	idleSmoke = NULL;

	needsRepair = false;
	repairrequestTimestamp = 0;
	repairNode.SetOwner(this);
	repairNode.AddToEnd(gameLocal.repairEntities);
}

idRadioWall::~idRadioWall(void)
{
	if (headlightHandle != -1)
		gameRenderWorld->FreeLightDef(headlightHandle);

	repairNode.Remove();
}


void idRadioWall::Spawn(void)
{
	isOn = spawnArgs.GetBool("start_on", "0");

	idVec3 speakerPos = GetSpeakerPos();

	//spawn soundwave particle.
	idAngles particleAng = GetPhysics()->GetAxis().ToAngles();
	particleAng.pitch += 90;
	
	idDict args;	
	args.Clear();
	args.Set("model", "music_loop.prt");
	args.SetBool("start_off", isOn);
	musicNotes = static_cast<idFuncEmitter *>(gameLocal.SpawnEntityType(idFuncEmitter::Type, &args));
	musicNotes->SetOrigin(speakerPos);
	musicNotes->SetAngles(particleAng);
	musicNotes->Bind(this, false);

	args.Clear();
	args.Set("model", "sound_waves_small.prt");
	args.SetBool("start_off", isOn);
	soundwaves = static_cast<idFuncEmitter *>(gameLocal.SpawnEntityType(idFuncEmitter::Type, &args));
	soundwaves->SetOrigin(speakerPos);
	soundwaves->SetAngles(particleAng);
	soundwaves->Bind(this, true);

	SetActivate(isOn);
	activateTimer = 0;

	if (!isOn)
	{
		musicNotes->Hide();
		soundwaves->Hide();
	}

	interestTimer = 0;
	isFrobbable = true;
	fl.takedamage = true;
	GetPhysics()->SetContents(CONTENTS_RENDERMODEL);
	GetPhysics()->SetClipMask(MASK_SOLID | CONTENTS_MOVEABLECLIP);

	SetActivate(isOn);
}


void idRadioWall::Save(idSaveGame *savefile) const
{
}

void idRadioWall::Restore(idRestoreGame *savefile)
{
}

idVec3 idRadioWall::GetSpeakerPos()
{
	idVec3 forwardDir,  upDir;
	GetPhysics()->GetAxis().ToAngles().ToVectors(&forwardDir, NULL, &upDir);
	return GetPhysics()->GetOrigin() + (forwardDir * 2) + (upDir * 3);
}

bool idRadioWall::DoFrob(int index, idEntity * frobber)
{
	if (index == MONSTER_FROBINDEX)
	{
		//Monster has is investigating the radio and wants to turn it off.
		if (isOn)
		{
			SetActivate(false); //turn radio off.
			gameLocal.DoParticle("frob_lines.prt", GetPhysics()->GetOrigin()); //do particle fx.
			
			GetPhysics()->ApplyImpulse(0, GetPhysics()->GetAbsBounds().GetCenter() + idVec3(2,2,2), idVec3(0, 0, 64)); //give it a little upward physics jostle.
		}

		return true;
	}
	else if (index == JOCKEYFROB_INDEX)
	{
		//during jockey struggle, struggle never turns off radio.
		if (!isOn)
		{
			SetActivate(true);
		}
	}
	else
	{
		if (health <= 0)
		{
			StartSound("snd_turnon", SND_CHANNEL_BODY3);
			return true;
		}

		SetActivate(!isOn);
		return true;
	}

	return true;
	
	//return idStaticEntity::DoFrob(index, frobber);
}


void idRadioWall::SetActivate(bool value)
{
	if (value)
	{
		//turn ON.
		if (!isOn)
		{
			StartSound("snd_turnon", SND_CHANNEL_BODY3);
			StartSound("snd_tune", SND_CHANNEL_BODY2);
			activateTimer = gameLocal.time + ACTIVATE_STATIC_TIME;
		}
		
		SetColor(FROBCOLOR);
		SetShaderParm(3, 1);
		BecomeActive(TH_THINK);
	}
	else if (!value)
	{
		//turn OFF.

		if (isOn)
		{
			StartSound("snd_turnoff", SND_CHANNEL_BODY3);
		}

		if (headlightHandle >= 0)
		{
			gameRenderWorld->FreeLightDef(headlightHandle); //TODO: this causes a 'idRenderWorld::FreeLightDef: handle %i is NULL' warning.
			headlightHandle = -1;
		}

		if (soundwaves->GetParticleActive())
			soundwaves->SetActive(false);

		if (musicNotes->GetParticleActive())
			musicNotes->SetActive(false);

		SetColor(IDLECOLOR);
		SetShaderParm(3, 0);

		StopSound(SND_CHANNEL_MUSIC);
		BecomeInactive(TH_THINK);
	}

	isOn = value;
}

void idRadioWall::Think(void)
{
	if (isOn)
	{
		if (headlightHandle < 0)
		{
			//light doesn't exist. create it.
			idVec3 forward, up;
			GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, &up);
			headlight.origin = GetPhysics()->GetOrigin() + (forward * LIGHT_FORWARDPOS) + (up * LIGHT_UPPOS);

			headlight.shader = declManager->FindMaterial("lights/pulse04", false);
			headlight.pointLight = true;
			headlight.lightRadius[0] = headlight.lightRadius[1] = headlight.lightRadius[2] = LIGHT_RADIUS;
			headlight.shaderParms[0] = .3f;
			headlight.shaderParms[1] = .3f;
			headlight.shaderParms[2] = .2f;
			headlight.shaderParms[3] = 1.0f;
			headlight.noShadows = true;
			headlight.isAmbient = false;
			headlight.axis = mat3_identity;
			headlightHandle = gameRenderWorld->AddLightDef(&headlight);
		}

		if (!IsPlayingSound(SND_CHANNEL_MUSIC) && gameLocal.time >= activateTimer)
		{
			//The radio track has ended. Play the next track.
			StartSound("snd_radio", SND_CHANNEL_MUSIC); //play the station.
		}

		if (gameLocal.time > interestTimer && gameLocal.time >= activateTimer)
		{
			interestTimer = gameLocal.time + INTEREST_INTERVALTIME;
			if ( !interestPoint.IsValid() )
			{
				idVec3 forward, right, up;
				GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, &right, &up); //spawn interestpoint in center of radio.
				idVec3 interestPos = GetSpeakerPos();
				interestPoint = gameLocal.SpawnInterestPoint( this, interestPos, spawnArgs.GetString( "interest_radiosound" ) );
			}
		}

		if (gameLocal.time >= activateTimer)
		{
			if (musicNotes->IsHidden())
			{
				musicNotes->Show();				
			}

			if (soundwaves->IsHidden())
			{
				soundwaves->Show();
				
			}			

			if (!musicNotes->GetParticleActive())
				musicNotes->SetActive(true);

			if (!soundwaves->GetParticleActive())
				soundwaves->SetActive(true);
		}
	}
	else if ( interestPoint.IsValid() )
	{
		interestPoint.GetEntity()->PostEventMS( &EV_Remove, 0 );
		interestPoint = nullptr;
	}

	idStaticEntity::Think();
}

void idRadioWall::Killed(idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location)
{
	if (!fl.takedamage)
		return;

	SetActivate(false);
	SetColor(0, 0, 0);
	fl.takedamage = false;
	StopSound(SND_CHANNEL_ANY, false);
	//isFrobbable = false;
	needsRepair = true;
	repairrequestTimestamp = gameLocal.time;

	idVec3 forward;
	this->GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);
	idEntityFx::StartFx(spawnArgs.GetString("fx_destroy"), GetPhysics()->GetOrigin() + (forward * 4), mat3_identity);	

	if (idleSmoke == NULL)
	{
		idDict args;
		args.Clear();
		args.Set("model", spawnArgs.GetString("model_damagesmoke"));
		args.Set("start_off", "0");
		idleSmoke = static_cast<idFuncEmitter *>(gameLocal.SpawnEntityType(idFuncEmitter::Type, &args));
		idleSmoke->SetOrigin(GetSpeakerPos());
		idAngles angles = GetPhysics()->GetAxis().ToAngles();
		angles.pitch += 90;
		idleSmoke->SetAxis(angles.ToMat3());
	}
	else
	{
		idleSmoke->SetActive(true);
	}
}

void idRadioWall::DoRepairTick(int amount)
{
	//Repair is done!
	health = maxHealth;
	needsRepair = false;
	fl.takedamage = true;
	//UpdateVisuals();

	if (idleSmoke != NULL)
	{
		idleSmoke->SetActive(false);
	}
}
