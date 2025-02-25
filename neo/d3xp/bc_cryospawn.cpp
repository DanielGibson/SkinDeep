#include "sys/platform.h"
#include "gamesys/SysCvar.h"
#include "Entity.h"
#include "Light.h"
#include "Player.h"
#include "Fx.h"
#include "framework/DeclEntityDef.h"


#include "bc_cryospawn.h"

const int DOZERHATCH_SPAWNOFFSET  = 96;
const int DOZERRADIUS = 32;

const int OPENANIM_TIME = 2000;

const int COOLDOWNTIME = 4000; //time between bot spawns.

const int CLOSEDELAY_TIME = 500;

const int SMOKE_DELAYTIME = 3000; //close anim time + delay before smokepuff disappearance sequence
const int DESPAWNDISTANCE = 384;


CLASS_DECLARATION(idAnimated, idCryospawn)
END_CLASS

idCryospawn::idCryospawn(void)
{
}

idCryospawn::~idCryospawn(void)
{
}

void idCryospawn::Spawn(void)
{
	stateTimer = 0;
	state = OPEN;

	BecomeInactive(TH_THINK);
}



void idCryospawn::Save(idSaveGame *savefile) const
{
}

void idCryospawn::Restore(idRestoreGame *savefile)
{
}


void idCryospawn::Think(void)
{
	if (state == OPEN)
	{
		//We *ideally* want the player to see this thing close. But we can't force player to see it.

		//So, we wait for the player to turn toward the general direction of this ent, and then do the anim then.

		idAngles playerAng;
		idAngles triggerAng;
		float vdot;

		idVec3 triggerVec = this->GetPhysics()->GetOrigin() - gameLocal.GetLocalPlayer()->GetEyePosition();
		triggerVec.Normalize();
		triggerAng = triggerVec.ToAngles();
		
		playerAng = idAngles(0, gameLocal.GetLocalPlayer()->viewAngles.yaw, 0);

		vdot = DotProduct(playerAng.ToForward(), triggerAng.ToForward());

		if (vdot >= .6f)
		{
			stateTimer = gameLocal.time + CLOSEDELAY_TIME;
			state = CLOSEDELAY;
		}
	}
	else if (state == CLOSEDELAY)
	{
		if (gameLocal.time >= stateTimer)
		{
			Event_PlayAnim("close", 1, false);
			state = SMOKEDELAY;
			stateTimer = gameLocal.time + SMOKE_DELAYTIME;
		}
	}
	else if (state == SMOKEDELAY)
	{
		if (gameLocal.time >= stateTimer)
		{
			float distanceToPlayer = (gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin() - this->GetPhysics()->GetOrigin()).LengthFast();

			if (distanceToPlayer >= DESPAWNDISTANCE)
			{
				state = CLOSED;

				idVec3 forward;
				this->GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);
				gameLocal.DoParticle("smoke_ring15.prt", GetPhysics()->GetOrigin() + forward * 8);

				//StartSound("snd_despawn", SND_CHANNEL_ANY, 0, false, NULL); //TODO: Make this only play if player is looking at it, has LOS to it.
				this->PostEventMS(&EV_Remove, 500);
				this->Hide();
			}
		}
	}

	idAnimated::Think();
}

