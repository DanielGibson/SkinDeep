#include "sys/platform.h"
#include "gamesys/SysCvar.h"
#include "physics/Physics_RigidBody.h"
#include "Entity.h"
#include "Light.h"
#include "Player.h"
#include "Fx.h"

#include "ai/AI.h"
#include "bc_gunner.h"
#include "bc_interestpoint.h"
#include "bc_meta.h"

CLASS_DECLARATION(idEntity, idInterestPoint)
END_CLASS



void idInterestPoint::Spawn(void)
{
	const char *typeName;
	
	priority = spawnArgs.GetInt("priority");
	noticeRadius = spawnArgs.GetInt("noticeRadius", "0");
	duplicateRadius = spawnArgs.GetInt("duplicateRadius", "0");
	sensoryTimer = gameLocal.time + SENSORY_UPDATETIME;
	isClaimed = false;
	claimant = NULL;
	forceCombat = spawnArgs.GetBool("forcecombat", "0");
	breaksConfinedStealth = spawnArgs.GetBool("breaks_confinedstealth", "0");
	cleanupWhenUnobserved = false;
	arrivalDistance = spawnArgs.GetInt("arrivaldistance", "80");
	creationTime = gameLocal.time;

	typeName = spawnArgs.GetString("type");
	if (!idStr::Icmp(typeName, "noise"))
	{
		int lifetime;		
		lifetime = spawnArgs.GetInt("lifetime");
		expirationTime = (lifetime > 0) ? gameLocal.time + lifetime : 0;

		interesttype = IPTYPE_NOISE;
	}
	else if (!idStr::Icmp(typeName, "visual"))
	{
		int lifetime;		
		lifetime = spawnArgs.GetInt("lifetime", "0");
		expirationTime = (lifetime > 0) ? gameLocal.time + lifetime : 0;

		interesttype = IPTYPE_VISUAL;		
	}
	else
	{
		gameLocal.Error("InterestPoint '%s' with invalid type: '%s'", GetName(), typeName);
		return;
	}

	onlyLocalPVS = spawnArgs.GetBool("onlyLocalPVS", "0");

	GetPhysics()->SetContents(0);
	GetPhysics()->SetClipMask(0);

	ownerDisplayName = "";	

	BecomeActive(TH_THINK);
}

idInterestPoint::idInterestPoint(void)
{
	interestNode.SetOwner(this);
	interestNode.AddToEnd(gameLocal.interestEntities);
}

idInterestPoint::~idInterestPoint(void)
{
	interestNode.Remove();
}

void idInterestPoint::Save(idSaveGame *savefile) const
{
}

void idInterestPoint::Restore(idRestoreGame *savefile)
{
}

void idInterestPoint::Think()
{
	if (isClaimed)
	{
		Present();
	}

	if (ai_showInterestPoints.GetInteger() == 1)
	{
		//debug.
		idStr intName;
		int arrowLength = 48;
		idVec4 debugcolor = (isClaimed ? colorGreen : colorRed);

		intName = this->GetName();
		if (intName.Find("idinterestpoint", false) == 0)
		{
			intName = intName.Right(intName.Length() - 16);
		}

		gameRenderWorld->DebugArrow(debugcolor, this->GetPhysics()->GetOrigin() + idVec3(0, 0, arrowLength), this->GetPhysics()->GetOrigin(), 4, 100);
		gameRenderWorld->DrawTextA(idStr::Format("%s", intName.c_str()), this->GetPhysics()->GetOrigin() + idVec3(0, 0, arrowLength + 16), .1f, debugcolor, gameLocal.GetLocalPlayer()->viewAngles.ToMat3(), 1, 100);
		gameRenderWorld->DrawTextA(idStr::Format("priority: %d", this->priority), this->GetPhysics()->GetOrigin() + idVec3(0, 0, arrowLength + 12), .1f, debugcolor, gameLocal.GetLocalPlayer()->viewAngles.ToMat3(), 1, 100);

		idStr ownername;
		if (this->GetPhysics()->GetClipModel()->GetOwner() == NULL)
			ownername = "none";
		else
			ownername = this->GetPhysics()->GetClipModel()->GetOwner()->GetName();
		gameRenderWorld->DrawTextA(idStr::Format("owner: %s", ownername.c_str()), this->GetPhysics()->GetOrigin() + idVec3(0, 0, arrowLength + 8), .1f, debugcolor, gameLocal.GetLocalPlayer()->viewAngles.ToMat3(), 1, 100);


		if (this->claimant.IsValid())
			gameRenderWorld->DebugLine(colorWhite, this->GetPhysics()->GetOrigin(), this->claimant.GetEntity()->GetPhysics()->GetOrigin(), 4);
		for (int i = 0; i < observers.Num(); i++)
		{
			if (observers[i] != NULL)
			{
				gameRenderWorld->DebugLine(debugcolor * 0.25, this->GetPhysics()->GetOrigin(), observers[i]->GetPhysics()->GetOrigin(), 4);
			}
		}
	}
}

void idInterestPoint::SetClaimed(bool value, idAI* _claimant)
{
	this->isClaimed = value;
	if (value)
	{
		this->claimant = _claimant;
	}
	else
	{
		this->claimant = NULL;
	}
}

int idInterestPoint::GetExpirationTime(void)
{
	return expirationTime;
}

void idInterestPoint::AddObserver(idAI* observer)
{
	observers.Append(observer);
}

void idInterestPoint::RemoveObserver(idAI* observer)
{
	observers.Remove(observer);
}

bool idInterestPoint::HasObserver(idAI* observer)
{
	return observers.Find(observer) != NULL;
}

void idInterestPoint::ClearObservers(void)
{
	observers.Clear();
}

void idInterestPoint::SetOwnerDisplayName(idStr _name)
{
	ownerDisplayName = _name;
}

idStr idInterestPoint::GetOwnerDisplayName()
{
	return ownerDisplayName;
}

idStr idInterestPoint::GetHUDName()
{
	if ( displayName.Length() <= 0 )
	{
		//If interestpoint def has no displayname, then fall back to see if the interestpoint's owner has a displayname.
		return GetOwnerDisplayName();
	}

	return displayName;
}

int idInterestPoint::GetCreationTime()
{
	return creationTime;
}