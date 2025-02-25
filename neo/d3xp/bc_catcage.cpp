#include "sys/platform.h"
#include "physics/Physics_RigidBody.h"
#include "Entity.h"
#include "Player.h"
#include "Fx.h"
#include "Moveable.h"
#include "idlib/LangDict.h"

#include "WorldSpawn.h"
#include "bc_meta.h"
#include "bc_cat.h"
#include "bc_catcage.h"

const int OPENING_TIME = 800; //make it say 'cat friend rescued' after XX milliseconds. We add a delay so it doesnt stomp over the MEOW text overlay.

const int MEOW_OVERLAY_SHADERPARM = 7;

const int MEOW_OVERLAY_MAXTIME = 200;

const int HELPME_VO_INTERVALTIME = 5000; //how often to play the cat help VO.
const int HELPME_VO_RANDOMVARIATIONTIME = 1000; //random variation time for the help VO.
const int HELPME_ACTIVATIONRADIUS = 256; //how close player has to be to activate the help VO.

const int WORDBUBBLETIME = 4000;
const int WORDBUBBLE_TRANSITIONTIME = 150;
const float WORDBUBBLE_JIGGLEAMOUNT = .2f;

//const int CAGEOPENANIMATION_DURATION = 2000;


#define LIGHTRADIUS 12
#define LIGHTINTENSITY .4f

const idEventDef EV_SetCallForHelp("setCallForHelp", "d");

CLASS_DECLARATION(idEntity, idCatcage)
	EVENT(EV_SetCallForHelp, idCatcage::Event_SetCallForHelp)
END_CLASS


idCatcage::idCatcage(void)
{
	catcageNode.SetOwner(this);
	catcageNode.AddToEnd(gameLocal.catcageEntities);

	wordbubbleState = WBS_OFF;
	wordbubbleTimer = 0;
	wordbubbleBasePosition = vec3_zero;
	wordbubbleJiggleTimer = 0;

	

	memset(&headlight, 0, sizeof(headlight));
	headlightHandle = -1;
}

idCatcage::~idCatcage(void)
{
	catcageNode.Remove();

	soundwaves->PostEventMS(&EV_Remove, 0);

	if (headlightHandle != -1)
		gameRenderWorld->FreeLightDef(headlightHandle);
}

void idCatcage::Spawn(void)
{
	GetPhysics()->SetContents(CONTENTS_RENDERMODEL);
	GetPhysics()->SetClipMask(MASK_SOLID | CONTENTS_MOVEABLECLIP);
	state = CATCAGE_IDLE;

	//Spawn soundwave particles.
	idDict args;
	args.Clear();
	args.Set("model", "sound_waves.prt");
	args.Set("start_off", "1");
	args.SetVector("origin", GetPhysics()->GetOrigin());
	soundwaves = static_cast<idFuncEmitter *>(gameLocal.SpawnEntityType(idFuncEmitter::Type, &args));

	//this->GetRenderEntity()->gui[0] = uiManager->FindGui(spawnArgs.GetString("gui"), true, true); //Create a UNIQUE gui so that it doesn't auto sync with other guis.
	timer = 0;
	
	//Spawn cat.
	args.Clear();
	args.SetVector("origin", GetPhysics()->GetOrigin());
	args.SetMatrix("rotation", GetPhysics()->GetAxis());
	args.Set("classname", spawnArgs.GetString("def_inhabitant"));
	args.Set("model", spawnArgs.GetString("model_cat", "model_cat"));

	idStr skinName = spawnArgs.GetString("skin_cat");
	if (skinName.Length() > 0)
	{
		args.Set("skin", skinName.c_str());
	}

	args.SetInt("hide", 1);
	args.SetBool("caged", true);
	//idEntity *catEnt = (idAnimatedEntity *)gameLocal.SpawnEntityType(idAnimatedEntity::Type, &args);
	idEntity *catEnt;
	gameLocal.SpawnEntityDef(args, &catEnt);
	//catEnt->Event_PlayAnim("sitting", 0, true);
	if (catEnt)
	{
		catPtr = catEnt;
	}

	idVec3 forward, up;
	GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, &up);	
	args.Clear();	
	args.Set("model", spawnArgs.GetString("model_meowoverlay"));
	args.SetBool("noshadows", true);
	args.SetBool("hide", true);
	meowOverlay = (idStaticEntity *)gameLocal.SpawnEntityType(idStaticEntity::Type, &args);
	meowOverlay->SetDepthHack(true);
	


	//spawn wordbubble.
	wordbubbleBasePosition = GetPhysics()->GetOrigin() + (forward * 12.2f) + (up * 12);
	args.Clear();
	args.Set("model", spawnArgs.GetString("model_wordbubble"));
	args.SetBool("noshadows", true);
	args.SetBool("hide", true);
	args.SetVector("origin", wordbubbleBasePosition);
	args.SetMatrix("rotation", GetPhysics()->GetAxis());
	args.Set("gui", "guis/game/wordbubble.gui");
	wordbubble = (idStaticEntity*)gameLocal.SpawnEntityType(idStaticEntity::Type, &args);



	overlayTimer = 0;
	overlayState = MEOWSTATE_TRANSITIONON;
	
	helpme_VO_timer = 0;

	team = spawnArgs.GetInt("team");

	callForHelp = spawnArgs.GetBool("call_for_help", "1");

	awaitingPlayerResponse = false;




	idVec3 catprisonerPos = GetPhysics()->GetOrigin() + (forward * -1) + (up * -13);
	args.Clear();
	args.SetVector("origin", catprisonerPos);
	args.Set("model", spawnArgs.GetString("model_cat", "model_cat"));

	if (skinName.Length() > 0)
	{
		args.Set("skin", skinName.c_str());
	}

	args.Set("anim", "sitting");
	args.SetInt("cycle", -1);
	args.SetInt("notriggeronanim", 1);
	args.SetFloat("wait", .1f);
	args.SetMatrix("rotation", GetPhysics()->GetAxis());
	catprisonerModel = (idAnimated*)gameLocal.SpawnEntityType(idAnimated::Type, &args);

	
	args.Clear();
	args.SetVector("origin", GetPhysics()->GetOrigin());
	args.Set("model", spawnArgs.GetString( "model_opencage"));
	args.Set("anim", "default");
	args.SetBool("hide", true);
	args.SetInt("cycle", -1);
	args.SetFloat("wait", .1f);
	args.SetInt("notriggeronanim", 1);
	args.SetMatrix("rotation", GetPhysics()->GetAxis());
	args.Set("bind", GetName());
	catcage_animated = (idAnimated*)gameLocal.SpawnEntityType(idAnimated::Type, &args);


	
	//Light.
	
	idVec3 lightPos = GetPhysics()->GetOrigin() + (forward * 10) + (up * 8);
	headlight.shader = declManager->FindMaterial("lights/defaultPointLight", false);
	headlight.pointLight = true;
	headlight.lightRadius[0] = headlight.lightRadius[1] = headlight.lightRadius[2] = LIGHTRADIUS;
	headlight.shaderParms[0] = LIGHTINTENSITY;
	headlight.shaderParms[1] = LIGHTINTENSITY;
	headlight.shaderParms[2] = LIGHTINTENSITY;
	headlight.shaderParms[3] = 1.0f;
	headlight.noShadows = true;
	headlight.isAmbient = false;
	headlight.axis = mat3_identity;
	headlight.origin = lightPos;
	headlightHandle = gameRenderWorld->AddLightDef(&headlight);	
	gameRenderWorld->UpdateLightDef(headlightHandle, &headlight);

	
	displayName = idStr::Format("Release %s", spawnArgs.GetString("name_cat", "???")).c_str();


	BecomeActive(TH_THINK);
}

void idCatcage::Save(idSaveGame *savefile) const
{
}

void idCatcage::Restore(idRestoreGame *savefile)
{
}

void idCatcage::Think(void)
{
	idEntity::Think();

	if (state == CATCAGE_IDLE)
	{
		if (gameLocal.InPlayerConnectedArea(this))
		{
			CatLookAtPlayer();
		}

		if (callForHelp && gameLocal.time > helpme_VO_timer && gameLocal.InPlayerConnectedArea(this) && gameLocal.RequirementMet_Inventory(gameLocal.GetLocalPlayer(), spawnArgs.GetString("requires"), 0))
		{
			bool playedHelpVO = false;
			float distanceToPlayer = (gameLocal.GetLocalPlayer()->GetPhysics()->GetOrigin() - this->GetPhysics()->GetOrigin()).LengthFast();

			if (distanceToPlayer < HELPME_ACTIVATIONRADIUS)
			{
				//do a traceline to see if player has LOS to the catcage.
				trace_t eyeTr;
				gameLocal.clip.TracePoint(eyeTr,  gameLocal.GetLocalPlayer()->firstPersonViewOrigin, GetPhysics()->GetOrigin(), MASK_SOLID, this);
				if (eyeTr.fraction >= .98f)
				{
					StartSound("snd_helpme", SND_CHANNEL_ANY);
					helpme_VO_timer = gameLocal.time + HELPME_VO_INTERVALTIME + gameLocal.random.RandomInt(HELPME_VO_RANDOMVARIATIONTIME);
					playedHelpVO = true;
					awaitingPlayerResponse = true;

					idVec3 forward;
					GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);
					gameLocal.DoParticle("sound_burst.prt", GetPhysics()->GetOrigin() + (forward * 12.5f));
				}
			}

			if (!playedHelpVO)
			{
				//If failed to play, then wait for a very short time before doing the check again. This is so to prevent it from checking every
				//frame, but we also want to have the delay be very short so that it feels reponsive.
				helpme_VO_timer = gameLocal.time + 300;
			}
		}

		//have the player respond.
		if (awaitingPlayerResponse)
		{
			if (!this->refSound.referenceSound->CurrentlyPlaying())
			{
				awaitingPlayerResponse = false;
				gameLocal.GetLocalPlayer()->SayVO_WithIntervalDelay_msDelayed("snd_vo_cat_encourage", 200);
			}
		}
	}
	else if (state == CATCAGE_OPENING)
	{
		//update overlay transition fx.
		if (overlayState == MEOWSTATE_TRANSITIONON)
		{
			float lerp = (overlayTimer - gameLocal.hudTime) / (float)MEOW_OVERLAY_MAXTIME;
			lerp = idMath::ClampFloat(0, 1, lerp);
			meowOverlay->GetRenderEntity()->shaderParms[MEOW_OVERLAY_SHADERPARM] = lerp;
			meowOverlay->UpdateVisuals();

			if (gameLocal.hudTime >= overlayTimer)
			{
				overlayState = MEOWSTATE_IDLE;
				meowOverlay->SetSkin(declManager->FindSkin( "skins/meow_overlay/meow_blend"));				
			}
		}
		

		if (gameLocal.time >= timer)
		{
			state = CATCAGE_COMPLETED;


			
			overlayTimer = gameLocal.hudTime + MEOW_OVERLAY_MAXTIME;
			overlayState = MEOWSTATE_TRANSITIONOFF;
			meowOverlay->SetSkin(declManager->FindSkin("skins/meow_overlay/meow"));

			//make smoke puff at cat position.
			//idVec3 bodyPos;
			//idMat3 bodyAxis;
			//jointHandle_t bodyJoint = catEnt->GetAnimator()->GetJointHandle("body");
			//catEnt->GetJointWorldTransform(bodyJoint, gameLocal.time, bodyPos, bodyAxis);
			//idEntityFx::StartFx("fx/pickupitem", &bodyPos, &mat3_identity, NULL, false); //BC


			//gameLocal.GetLocalPlayer()->SetHotbarslotsUnlockDelta(1);

			SpawnGoodie();
		}
	}
	else if (state == CATCAGE_COMPLETED)
	{
		if (overlayState == MEOWSTATE_TRANSITIONOFF)
		{
			float lerp = (overlayTimer - gameLocal.hudTime) / (float)MEOW_OVERLAY_MAXTIME;
			lerp = idMath::ClampFloat(0, 1, lerp);
			lerp = 1 - lerp;
			meowOverlay->GetRenderEntity()->shaderParms[MEOW_OVERLAY_SHADERPARM] = lerp;
			meowOverlay->UpdateVisuals();

			if (gameLocal.hudTime >= overlayTimer)
			{
				meowOverlay->Hide();
				overlayState = MEOWSTATE_DONE;

				//Give update on catcage count.
				int totalcages = 0;
				int openedCages = 0;
				for (idEntity* cageEnt = gameLocal.catcageEntities.Next(); cageEnt != NULL; cageEnt = cageEnt->catcageNode.Next())
				{
					if (!cageEnt)
						continue;

					if (!cageEnt->IsType(idCatcage::Type))
						continue;

					totalcages++;

					if (static_cast<idCatcage *>(cageEnt)->IsOpened())
					{
						openedCages++;
					}
				}

				idStr cageUpdateStr = idStr::Format(common->GetLanguageDict()->GetString("#str_def_gameplay_catreleasedupdate"), openedCages, totalcages);
				gameLocal.AddEventLog(cageUpdateStr.c_str(), GetPhysics()->GetOrigin());

				//script call.
				gameLocal.RunMapScriptArgs(spawnArgs.GetString("call", ""), gameLocal.GetLocalPlayer(), this);				

				if (openedCages >= totalcages)
				{					
					//All cat cages are now open.
					if (gameLocal.world->spawnArgs.GetBool("objectives", "1"))
					{
						gameLocal.GetLocalPlayer()->SetObjectiveText("#str_obj_summonpod"); //"Use a Signal Lamp to summon a Cat Evac pod"
					}


				}
				else
				{
					//Big fanfare text (not the final cat).
					idStr releaseMessage = idStr::Format(common->GetLanguageDict()->GetString("#str_def_gameplay_catreleased"), spawnArgs.GetString("name_cat", "???"));
					gameLocal.GetLocalPlayer()->SetFanfareMessage(releaseMessage.c_str());
				}

				idVec3 forward;
				GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);
				idVec3 interestPosition = GetPhysics()->GetOrigin() + (forward * 13);
				gameLocal.SpawnInterestPoint(this, interestPosition, spawnArgs.GetString("interest_catcageopen"));
			}			
		}

		
	}

	if (wordbubbleState == WBS_TRANSITIONON)
	{
		float lerp = (wordbubbleTimer - gameLocal.time) / (float)WORDBUBBLE_TRANSITIONTIME;
		lerp = idMath::ClampFloat(0, 1, lerp);
		wordbubble->SetShaderParm(7, lerp);
		

		if (gameLocal.time >= wordbubbleTimer)
		{
			wordbubble->SetShaderParm(7, 0);
			wordbubbleState = WBS_ON;
			wordbubbleTimer = gameLocal.time + WORDBUBBLETIME;
		}
	}
	else if (wordbubbleState == WBS_ON)
	{
		if (gameLocal.time > wordbubbleJiggleTimer)
		{
			wordbubbleJiggleTimer = gameLocal.time + 200;

			idVec3 right, up;
			GetPhysics()->GetAxis().ToAngles().ToVectors(NULL, &right, &up);
			
			idVec3 newPosition = wordbubbleBasePosition + (right * (gameLocal.random.CRandomFloat() * WORDBUBBLE_JIGGLEAMOUNT)) + (up * (gameLocal.random.CRandomFloat() * WORDBUBBLE_JIGGLEAMOUNT));
			wordbubble->SetOrigin(newPosition);
		}

		if (gameLocal.time >= wordbubbleTimer)
		{
			wordbubbleState = WBS_TRANSITIONOFF;
			wordbubbleTimer = gameLocal.time + WORDBUBBLE_TRANSITIONTIME;
		}
	}
	else if (wordbubbleState == WBS_TRANSITIONOFF)
	{
		float lerp = (wordbubbleTimer - gameLocal.time) / (float)WORDBUBBLE_TRANSITIONTIME;
		lerp = idMath::ClampFloat(0, 1, lerp);
		lerp = 1.0f - lerp;
		wordbubble->SetShaderParm(7, lerp);

		if (gameLocal.time >= wordbubbleTimer)
		{
			wordbubble->SetShaderParm(7, 1);
			wordbubbleState = WBS_OFF;
			wordbubble->Hide();
		}
	}
}

void idCatcage::CatLookAtPlayer()
{
	idVec3 lookPoint = gameLocal.GetLocalPlayer()->firstPersonViewOrigin;

	jointHandle_t headJoint = catprisonerModel->GetAnimator()->GetJointHandle("head");

	idRotation lookRotation;
	idVec3 lookVec;	

	idVec3 jointPos;
	idMat3 jointAxis;
	idMat3 mat;
	catprisonerModel->GetAnimator()->GetJointTransform(headJoint, gameLocal.time, jointPos, jointAxis);
	jointPos = catprisonerModel->GetPhysics()->GetOrigin() + jointPos * catprisonerModel->GetPhysics()->GetAxis();
	lookVec = lookPoint - jointPos;
	lookVec.Normalize();

	mat = catprisonerModel->GetPhysics()->GetAxis().Transpose();
	catprisonerModel->GetAnimator()->SetJointAxis(headJoint, JOINTMOD_WORLD, lookVec.ToMat3() * mat);
}

void idCatcage::SetWordbubble(idStr text)
{
	wordbubble->Event_SetGuiParm("gui_parm0", text.c_str());
	
	if (wordbubbleState == WBS_TRANSITIONOFF || wordbubbleState == WBS_OFF)
	{
		//enter transitionOn state.
		wordbubble->SetShaderParm(7, 1);
		wordbubbleState = WBS_TRANSITIONON;
		wordbubbleTimer = gameLocal.time + WORDBUBBLE_TRANSITIONTIME;
	}
	else if (wordbubbleState == WBS_ON)
	{
		//already on. refresh the lifetime.
		wordbubbleTimer = gameLocal.time + WORDBUBBLETIME;
	}

	wordbubble->Show();
}

void idCatcage::SpawnGoodie()
{
	idStr goodieName = spawnArgs.GetString("def_itemdrop");
	if (goodieName.Length() <= 0)
		return;

	//Spawn a goodie.
	idVec3 forward;
	GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);
	idVec3 spawnPosition = GetPhysics()->GetOrigin() + forward * 16;

	idDict args;
	args.SetVector("origin", spawnPosition);
	args.Set("classname", spawnArgs.GetString("def_itemdrop"));
	idEntity *goodieEnt = NULL;
	gameLocal.SpawnEntityDef(args, &goodieEnt);

	if (goodieEnt)
	{
		//Nudge the item toward player's position.
		idVec3 dirToPlayer = gameLocal.GetLocalPlayer()->GetEyePosition() - spawnPosition;
		dirToPlayer.NormalizeFast();
		goodieEnt->GetPhysics()->SetLinearVelocity(dirToPlayer * gameLocal.random.RandomInt(32, 48));
		goodieEnt->GetPhysics()->SetAngularVelocity(idVec3(gameLocal.random.RandomInt(-64, 64), gameLocal.random.RandomInt(-64, 64), 0));

		StartSound("snd_sparkle", SND_CHANNEL_ANY, 0, false, NULL);
		gameLocal.DoParticle("sparkle_lifepod_hint.prt", spawnPosition);
	}
}

void idCatcage::Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType )
{	
	//take damage. meow!!!
}


bool idCatcage::DoFrobHold(int index, idEntity* frobber)
{
	//ask cat where catkey is. We prioritize the information on most useful to least useful.	

	StartSound("snd_ask", SND_CHANNEL_VOICE);

	idStr scriptName = spawnArgs.GetString("call_ask", "");
	if (scriptName.Length() > 0)
	{
		gameLocal.RunMapScriptArgs(scriptName.c_str(), frobber, this);
	}


	//See if player is holding a cat key.
	if (gameLocal.GetLocalPlayer()->HasItemViaClassname("item_cat_key"))
	{
		SetWordbubble("#str_def_gameplay_catcage_playerhold");
		return true;
	}

	idLocationEntity* cageLoc = gameLocal.LocationForEntity(this);
	idEntityPtr<idLocationEntity> closestCatKeyLoc;
	closestCatKeyLoc = NULL;
	float closestDistance = 999999;
	idVec3 fallbackPosition = vec3_zero;
	for (idEntity* ent = gameLocal.spawnedEntities.Next(); ent != NULL; ent = ent->spawnNode.Next())
	{
		if (!ent)
			continue;

		if (ent->IsHidden() || !ent->IsType(idMoveableItem::Type))
			continue;

		idStr invname = ent->spawnArgs.GetString("inv_name");
		if (idStr::Icmp(invname, "catkey") == 0)
		{
			//found a catkey.
			idLocationEntity* locEntCatkey = gameLocal.LocationForEntity(ent);
			if (locEntCatkey)
			{
				if (cageLoc != nullptr)
				{
					if (cageLoc->entityNumber == locEntCatkey->entityNumber)
					{
						//the cat cage and the cat key are in the SAME room.
						SetWordbubble("#str_def_gameplay_catcage_inroom");
						return true;
					}
				}

				//Find the closest key to me.
				float distanceToKey = (ent->GetPhysics()->GetOrigin() - GetPhysics()->GetOrigin()).Length();
				if (distanceToKey < closestDistance)
				{
					closestCatKeyLoc = locEntCatkey;
					closestDistance = distanceToKey;
				}
			}
			else
			{
				//If the key somehow ended up in a place that does not have a location...
				fallbackPosition = ent->GetPhysics()->GetOrigin();
			}
		}
	}

	//Print name of closest key.
	if (closestCatKeyLoc.IsValid())
	{
		SetWordbubble(idStr::Format(common->GetLanguageDict()->GetString("#str_def_gameplay_catcage_keyatroom"), closestCatKeyLoc.GetEntity()->GetLocation()));
		return true;
	}


	//Check lost and found.
	if (gameLocal.GetLocalPlayer()->IsEntityLostInSpace("item_cat_key"))
	{
		SetWordbubble("#str_def_gameplay_catcage_lostfound");
		return true;
	}

	//Do fallback of the cat XYZ position.
	if (fallbackPosition != vec3_zero)
	{
		SetWordbubble("#str_def_gameplay_catcage_arrow");
		gameRenderWorld->DebugArrow(colorWhite, this->GetPhysics()->GetOrigin(), fallbackPosition, 8, 90000);
		return true;
	}

		
	SetWordbubble("#str_def_gameplay_catcage_dontknow");
	return true;
}

bool idCatcage::DoFrob(int index, idEntity * frobber)
{
	if (frobber == NULL || state == CATCAGE_COMPLETED)
		return false;

	//player frobbed catcage.	

	//Check if player has key.
	if (!gameLocal.RequirementMet_Inventory(frobber, spawnArgs.GetString("requires"), 1))
	{
		if (frobber == gameLocal.GetLocalPlayer())
		{
			StartSound("snd_ask", SND_CHANNEL_VOICE);
			SetWordbubble("#str_def_gameplay_catcage_needkey");
		}

		return false;
	}

	//gameLocal.GetLocalPlayer()->SetCatkeyDelta(-1);

	ReleaseCat();

	return true;
}

void idCatcage::ReleaseCat()
{
	if (wordbubble != nullptr)
	{
		wordbubble->Hide();
	}

	catprisonerModel->Hide();
	catprisonerModel->PostEventMS(&EV_Remove, 0);


	Hide();
	catcage_animated->Event_PlayAnim(spawnArgs.GetString("anim_openanimation"), false);
	catcage_animated->Show();

	if (headlightHandle != -1)
	{
		gameRenderWorld->FreeLightDef(headlightHandle);
	}

	


	// Run script calls if necessary
	idStr scriptName = spawnArgs.GetString("callOnRelease", "");
	if (!scriptName.IsEmpty())
		gameLocal.RunMapScriptArgs(scriptName, gameLocal.GetLocalPlayer(), this);

	//Release the cat.
	gameLocal.GetLocalPlayer()->SetImpactSlowmo(true);
	StopSound(SND_CHANNEL_ANY);
	StartSound("snd_rescue", SND_CHANNEL_BODY); //play the cat meow power-reverb
	state = CATCAGE_OPENING;

	idVec3 forward;
	GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);
	gameLocal.DoParticle("explosion_small.prt", GetPhysics()->GetOrigin() + forward * 12);
	gameLocal.GetLocalPlayer()->playerView.DoDurabilityFlash();
	isFrobbable = false;
	timer = gameLocal.time + OPENING_TIME;

	//catEnt->Event_PlayAnim("jump_rescue", 2);
	if (catPtr.IsValid())
	{
		if (catPtr.GetEntity()->IsType(idCat::Type))
		{
			catPtr.GetEntity()->Show();
			static_cast<idCat *>(catPtr.GetEntity())->DoCageBurst(); //Play animation to burst out of cage.
		}
		catPtr.GetEntity()->SetDepthHack(true);
	}

	meowOverlay->SetShaderParm(MEOW_OVERLAY_SHADERPARM, 1);
	meowOverlay->UpdateVisuals();

	idAngles overlayAngle = idAngles(0, gameLocal.GetLocalPlayer()->viewAngles.yaw + 180, 0);
	meowOverlay->SetAxis(overlayAngle.ToMat3());

	idVec3 overlayPos = gameLocal.GetLocalPlayer()->firstPersonViewOrigin + overlayAngle.ToForward() * -112;
	meowOverlay->SetOrigin(overlayPos);
	meowOverlay->Show();
	overlayTimer = gameLocal.hudTime + MEOW_OVERLAY_MAXTIME;

	

	gameLocal.GetLocalPlayer()->SayVO_WithIntervalDelay_msDelayed("snd_vo_cat_release", 1200);

	static_cast<idMeta *>(gameLocal.metaEnt.GetEntity())->UpdateCagecageObjectiveText();


	//chain particle.

	idVec3 particlePos = GetPhysics()->GetOrigin() + forward * 12;
	idAngles particleAngle = GetPhysics()->GetAxis().ToAngles();
	particleAngle.pitch += 70;
	gameLocal.DoParticle(spawnArgs.GetString("model_chainparticle"), particlePos, particleAngle.ToForward());


	//make skin change.
	catcage_animated->SetSkin(declManager->FindSkin(spawnArgs.GetString("skin_off")));
}


bool idCatcage::IsOpened()
{
	return (state == CATCAGE_OPENING || state == CATCAGE_COMPLETED);
}

//cat cage fanfare completely done.
bool idCatcage::IsOpenedCompletely()
{
	return (state == CATCAGE_COMPLETED);
}

void idCatcage::DoHack()
{
	gameLocal.AddEventLog("#str_def_gameplay_catcagehack", GetPhysics()->GetOrigin());
	ReleaseCat();
}


void idCatcage::Event_SetCallForHelp(int toggle)
{
	//Toggle ability for cat to call for help or not.
	if (toggle != 0)
	{
		callForHelp = true;
	}
	else
	{
		callForHelp = false;
	}
}
