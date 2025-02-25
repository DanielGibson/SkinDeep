//#include "script/Script_Thread.h"
#include "framework/DeclEntityDef.h"

#include "player.h"
#include "mover.h"
#include "bc_vacuumspline.h"

//TODO: particle trail path

const float MOVETIME = .8f;
const float MOVETIME_PLAYER = 1.0f;

const int FLINGSPEED = 128;
const int FLINGSPEED_PLAYER = 1280;

CLASS_DECLARATION(idEntity, idVacuumSpline)
END_CLASS

idVacuumSpline::idVacuumSpline()
{
}

idVacuumSpline::~idVacuumSpline()
{
	delete splineEnt;
	delete mover;
}


void idVacuumSpline::Spawn()
{
	const char *actorName = spawnArgs.GetString("actor");
	if (actorName[0] == '\0')
	{
		gameLocal.Error("idVacuumSpline: invalid actor.");
	}

	targetActor = gameLocal.FindEntity(actorName);
	if (!targetActor.IsValid())
	{
		gameLocal.Error("idVacuumSpline: can't find actor '%s')", actorName);
	}

	int t = 0; //time value for the spline.

	spline = new idCurve_CatmullRomSpline<idVec3>();

	spline->AddValue(t, targetActor.GetEntity()->GetPhysics()->GetOrigin());

	idVec3 initialBend;
	if ( spawnArgs.GetVector( "initialBend", "", initialBend ) )
	{
		t += 100;
		spline->AddValue( t, initialBend );
	}

	t += 100;
	spline->AddValue(t, spawnArgs.GetVector("midpoint"));

	t += 100;
	spline->AddValue(t, spawnArgs.GetVector("endpoint"));



	//Spawn spline ENTITY from the spline info.	
	idDict args;
	args.Set("classname", "func_splinemover");
	args.SetVector("origin", spline->GetValue(0));

	idStr splineString = idStr::Format( "%d ( ", spline->GetNumValues() );
	for ( int i = 0; i < spline->GetNumValues(); i++ )
	{
		splineString += idStr::Format( "%f %f %f ", spline->GetValue( i ).x, spline->GetValue( i ).y, spline->GetValue( i ).z );
	}
	splineString += ")";
	
	args.Set("curve_CatmullRomSpline", splineString);
	gameLocal.SpawnEntityDef(args, &splineEnt);
	if (!splineEnt)
	{
		gameLocal.Error("idVacuumSpline: failed to create splineEnt.");
	}	
	//splineEnt->spawnArgs.Set("model", splineEnt->GetName());	


	//Spawn mover.	
	is_player = spawnArgs.GetBool("is_player", "0");
	args.Clear();
	args.Set("classname", "func_mover");
	
	args.SetFloat("move_time", is_player ? MOVETIME_PLAYER : MOVETIME); //the player has a different spline move speed. we slow it down a little to make it more readable.	

	args.SetBool("solid", false);
	args.SetBool("cinematic", true);
	gameLocal.SpawnEntityDef(args, &mover);
	if (!mover)
	{
		common->Error("idCameraView %s failed to spawn mover.\n", GetName());
		return;
	}		
	mover->SetOrigin(splineEnt->GetPhysics()->GetOrigin());	

	//make player duck when on the vacuum spline.
	if (is_player)
	{
		gameLocal.GetLocalPlayer()->ForceDuck(100);
	}

	targetActor.GetEntity()->Bind(mover, false);
	((idMover *)mover)->Event_DisableSplineAngles();
	((idMover *)mover)->Event_StartSpline(splineEnt);

	//Get direction to fling when done.
	flingDirection = spawnArgs.GetVector("endpoint") - spawnArgs.GetVector("midpoint");
	flingDirection.Normalize();

	startTime = gameLocal.time;
	state = VS_VACUUMING;
}


void idVacuumSpline::Think(void)
{
	if (state == VS_VACUUMING)
	{
		if (!gameLocal.GetAirlessAtPoint(mover->GetPhysics()->GetOrigin()))
		{
			Detach();
		}

		if (gameLocal.time >= startTime + (int)(MOVETIME * 1000.0f) )
		{
			//End of the move.
			Detach();
			targetActor.GetEntity()->GetPhysics()->SetLinearVelocity(flingDirection * (is_player ? FLINGSPEED_PLAYER  : FLINGSPEED));
		}
	}
}

void idVacuumSpline::Detach()
{
	state = VS_DONE;
	targetActor.GetEntity()->Unbind();
	this->PostEventMS(&EV_Remove, 0);
}