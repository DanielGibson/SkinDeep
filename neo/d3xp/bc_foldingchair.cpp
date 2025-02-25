#include "sys/platform.h"

#include "gamesys/SysCvar.h"
#include "Entity.h"
#include "Player.h"

#include "bc_frobcube.h"
#include "bc_foldingchair.h"

const int FROB_OFFTIME = 600; //UNfrobbable for XX seconds after frobbing.


CLASS_DECLARATION(idAnimatedEntity, idFoldingchair)
END_CLASS

idFoldingchair::idFoldingchair(void)
{
}

idFoldingchair::~idFoldingchair(void)
{
}

void idFoldingchair::Spawn(void)
{
	//Remove collision, but still allow frob/damage
	GetPhysics()->SetContents(CONTENTS_RENDERMODEL);
	GetPhysics()->SetClipMask(MASK_SOLID | CONTENTS_MOVEABLECLIP);

	fl.takedamage = true;
	state = FCH_CLOSED;

	//Spawn frobcube.
	idVec3 jointPos;
	idMat3 jointAxis;
	jointHandle_t jointHandle = GetAnimator()->GetJointHandle("handle");
	GetJointWorldTransform(jointHandle, gameLocal.time, jointPos, jointAxis);	
	idDict args;
	idVec3 upDir;
	this->GetPhysics()->GetAxis().ToAngles().ToVectors(NULL, NULL, &upDir);
	args.Clear();
	args.Set("model", "models/objects/frobcube/cube4x4.ase");
	args.Set("displayname", "Jumpseat");
	args.SetVector("origin", jointPos + (upDir * -2));
	frobCube = gameLocal.SpawnEntityType(idFrobcube::Type, &args);
	frobCube->GetPhysics()->GetClipModel()->SetOwner(this);
	//static_cast<idFrobcube*>(frobCube)->SetIndex(1);
	frobCube->BindToJoint(this, jointHandle, false);


	//The collision box that slides out when the seat opens.
	idEntity *ent;
	args.Clear();
	args.Set("model", spawnArgs.GetString("model_seat"));
	args.SetVector("origin", GetPhysics()->GetOrigin());
	args.SetMatrix("rotation", GetPhysics()->GetAxis());
	//args.SetFloat("time", .5f);
	args.Set("classname", "func_mover");
	gameLocal.SpawnEntityDef(args, &ent);
	if (ent)
	{
		seatMover = static_cast<idMover *>(ent);
		seatMover->Event_SetMoveTime(.3f);
		seatMover->Hide();
	}

	idVec3 forward;
	GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);
	seatPos_open = GetPhysics()->GetOrigin() + (forward * 26);
	
}


void idFoldingchair::Save(idSaveGame *savefile) const
{
}

void idFoldingchair::Restore(idRestoreGame *savefile)
{
}

void idFoldingchair::Think(void)
{
	if (gameLocal.time > stateTimer && !frobCube->isFrobbable)
	{
		frobCube->isFrobbable = true;
		BecomeInactive(TH_THINK);
	}
	
	idAnimatedEntity::Think();
}


bool idFoldingchair::DoFrob(int index, idEntity * frobber)
{
	if (state == FCH_CLOSED)
	{
		//Chair is closed. Open it.
		state = FCH_OPEN;
		Event_PlayAnim("open", 1);

		seatMover->SetOrigin(GetPhysics()->GetOrigin());
		seatMover->Show();
		seatMover->Event_MoveToPos(seatPos_open);
	}
	else
	{
		//Chair is open. Close it.
		state = FCH_CLOSED;
		Event_PlayAnim("close", 1);

		seatMover->SetOrigin(GetPhysics()->GetOrigin());
		seatMover->Hide();
	}

	// SW: scripting things being hidden in the seat, etc	
	gameLocal.RunMapScriptArgs(spawnArgs.GetString("call"), frobber, this);	
	//idStr scriptName = spawnArgs.GetString("call", "");
	//if (scriptName.Length() > 0)
	//{
	//	const function_t* scriptFunction;
	//	scriptFunction = gameLocal.program.FindFunction(scriptName);
	//	if (scriptFunction)
	//	{
	//		assert(scriptFunction->parmSize.Num() <= 2);
	//		idThread* thread = new idThread();
	//
	//		// 1 or 0 args always pushes activator (frobber)
	//		thread->PushArg(frobber);
	//		// 2 args pushes self as well
	//		if (scriptFunction->parmSize.Num() == 2)
	//		{
	//			thread->PushArg(this);
	//		}
	//
	//		thread->CallFunction(scriptFunction, false);
	//		thread->DelayedStart(0);
	//	}
	//}

	frobCube->isFrobbable = false;
	stateTimer = gameLocal.time + FROB_OFFTIME;
	BecomeActive(TH_THINK);

	return true;
}
