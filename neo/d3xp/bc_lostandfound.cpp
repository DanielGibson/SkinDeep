//#include "sys/platform.h"
//#include "Entity.h"

#include "framework/DeclEntityDef.h"
#include "Fx.h"

#include "Item.h"
#include "bc_lostandfound.h"
#include "Player.h"
#include "framework/Common.h"
#include "idlib/LangDict.h"

const int VENDING_DISPENSETIME = 1750;
const idVec3 COLOR_IDLE = idVec3(0, 1, 0);
const idVec3 COLOR_BUSY = idVec3(1, .7f, 0);
const idVec3 BUTTONOFFSET = idVec3(17, 3, 58.5f);

const int IDX_RETRIEVEBUTTON = 700;
const int UPDATE_CACHE_TIME = 1000;

CLASS_DECLARATION(idStaticEntity, idLostAndFound)
END_CLASS

idLostAndFound::idLostAndFound(void)
{
	proximityAnnouncer.sensor = this;
	proximityAnnouncer.checkHealth = false;
	proximityAnnouncer.coolDownPeriod = 10000;
}

idLostAndFound::~idLostAndFound(void)
{
	repairNode.Remove();
}

void idLostAndFound::Spawn(void)
{
	idDict args;		

	StartSound("snd_ambient", SND_CHANNEL_BODY, 0, false, NULL);	 //idle hum.
	fl.takedamage = true;
	GetPhysics()->SetContents(CONTENTS_SOLID);
	GetPhysics()->SetClipMask(MASK_SOLID);		
	
	vendState = VENDSTATE_IDLE;
	SetColor(COLOR_IDLE);

	needsRepair = false;
	repairrequestTimestamp = 0;
	repairNode.SetOwner(this);
	repairNode.AddToEnd(gameLocal.repairEntities);

	UpdateVisuals();

	//item the player wants to retrieve
	itemDef = nullptr;

	//the list that the items will populate
	itemList = uiManager->AllocListGUI();
	itemList->Config(renderEntity.gui[0], "itemList");
	itemList->Clear();
	itemList->SetSelection(0);

	proximityAnnouncer.Start();

	updateCacheTimer = 0;
	BecomeActive( TH_THINK );
}



void idLostAndFound::Save(idSaveGame *savefile) const
{
}

void idLostAndFound::Restore(idRestoreGame *savefile)
{
}

void idLostAndFound::Think(void)
{	
	if (vendState == VENDSTATE_DELAY)
	{
		if (gameLocal.time >= stateTimer)
		{
			idEntity *spawnedItem;			
			idVec3 candidateSpawnPos;
			bool successfulSpawn = false;
	
			if (itemDef == nullptr)
			{
				vendState = VENDSTATE_IDLE;
				return;
			}

			gameLocal.SpawnEntityDef(itemDef->dict, &spawnedItem, false);
			if (spawnedItem)
			{
				//Find a suitable place to spawn the item.
				candidateSpawnPos = GetDispenserPosition();
	
				trace_t boundTr;
				gameLocal.clip.TraceBounds(boundTr, candidateSpawnPos, candidateSpawnPos, spawnedItem->GetPhysics()->GetBounds(), MASK_SOLID, NULL);
	
				if (boundTr.fraction >= 1.0f)
				{
					//all clear.
					spawnedItem->SetOrigin(candidateSpawnPos);
					gameLocal.DoParticle("smoke_ring13.prt", candidateSpawnPos);
					successfulSpawn = true;

					idVec3 forward, right;
					this->GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, &right,NULL);
					spawnedItem->GetPhysics()->SetLinearVelocity((forward * 24) + (right * (gameLocal.random.CRandomFloat() * 48)));
	
					if (spawnedItem->IsType(idItem::Type))
					{
						static_cast<idItem *>(spawnedItem)->SetJustDropped(true);
					}
				}
				else
				{
					spawnedItem->PostEventMS(&EV_Remove, 0);
				}
			}
	
			if (!successfulSpawn)
			{
				StartSound("snd_fail", SND_CHANNEL_ANY, 0, false, NULL);
			}
	
			SetColor(COLOR_IDLE);
			vendState = VENDSTATE_IDLE;
		}
	}
	else
	{
		bool hasCriticalItem = false;

		for (int i = 0; i < cachedEntityDefNums.Num(); i++)
		{
			int defNum = cachedEntityDefNums[i];
			const idDeclEntityDef* def = static_cast<const idDeclEntityDef*>(declManager->DeclByIndex(DECL_ENTITYDEF, defNum));
			
			bool isCriticalItem = def->dict.GetBool("criticalitem", "0");
			if (isCriticalItem)
			{
				hasCriticalItem = true;
				break;
			}
		}

		if( hasCriticalItem )
		{
			proximityAnnouncer.Update();
		}
	}

	if ( gameLocal.time > updateCacheTimer )
	{
		UpdateLostEntityCache();
	}

	idStaticEntity::Think();
}

idVec3 idLostAndFound::GetDispenserPosition()
{
	idVec3 forward, right, up;
	this->GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, &right, &up);
	return this->GetPhysics()->GetOrigin() + (forward * 24) + (up * 43) + (right * 14);
}



void idLostAndFound::DoRepairTick(int amount)
{
    Event_GuiNamedEvent(1, "onRepaired");
	SetColor(COLOR_IDLE);

	//Repair is done!
	health = maxHealth;
	needsRepair = false;
	vendState = VENDSTATE_IDLE;
	StartSound("snd_ambient", SND_CHANNEL_BODY, 0, false, NULL);	 //idle hum.
	fl.takedamage = true;

	renderEntity.shaderParms[7] = 1; //make sign turn on.
	UpdateVisuals();
}

void idLostAndFound::Killed(idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location)
{
	idVec3 forward;
	idAngles particleAng;

	if (!fl.takedamage)
		return;

	fl.takedamage = false;
	SetColor(idVec3(0, 0, 0));
	StopSound(SND_CHANNEL_BODY, false);
	StartSound("snd_powerdown", SND_CHANNEL_BODY, 0, false, NULL);

	needsRepair = true;
	repairrequestTimestamp = gameLocal.time;
	
	renderEntity.shaderParms[7] = 0; //make sign turn off.
	UpdateVisuals();

	this->GetPhysics()->GetAxis().ToAngles().ToVectors(&forward, NULL, NULL);
	idEntityFx::StartFx("fx/explosion_tnt", this->GetPhysics()->GetOrigin() + (forward * 20) + idVec3(0, 0, 48), mat3_identity, this, false);

	particleAng = this->GetPhysics()->GetAxis().ToAngles();
	particleAng.pitch -= 90;
	gameLocal.DoParticle("vent_purgesmoke.prt", this->GetPhysics()->GetOrigin() + (forward * 18) + idVec3(0, 0, 64), particleAng.ToForward());

    Event_GuiNamedEvent(1, "onDamaged");
}

//When the retrieve button is pressed.
void idLostAndFound::DoGenericImpulse(int index)
{
	if (index == IDX_RETRIEVEBUTTON)
	{
		//Retrieve the item.
		
		//First get the item definition.
		int selNum = itemList->GetSelection( nullptr, 0 );
		if (selNum == -1)
		{
			StartSound("snd_fail", SND_CHANNEL_ANY, 0, false, NULL);
			return;
		}

		itemDef = static_cast< const idDeclEntityDef* >( declManager->DeclByIndex( DECL_ENTITYDEF, selNum ) );
		//itemDef = gameLocal.FindEntityDef(spawnArgs.GetString("def_itemspawn"), false);

		if (itemDef == nullptr)
		{
			StartSound("snd_fail", SND_CHANNEL_ANY, 0, false, NULL);
			return;
		}

		if (vendState != VENDSTATE_IDLE)
		{
			StartSound("snd_fail", SND_CHANNEL_ANY, 0, false, NULL);
			return;
		}

		StartSound("snd_button", SND_CHANNEL_VOICE, 0, false, NULL);

		if (health <= 0)
			return; //machine is busted, don't continue.

		StartSound("snd_dispense", SND_CHANNEL_VOICE2, 0, false, NULL);
		vendState = VENDSTATE_DELAY;
		stateTimer = gameLocal.time + VENDING_DISPENSETIME;

		gameLocal.GetLocalPlayer()->RemoveLostInSpace( selNum );
		UpdateLostEntityCache();

		StartSound("snd_jingle", SND_CHANNEL_ANY, 0, false, NULL);
	}
}

void idLostAndFound::UpdateLostEntityCache()
{
	updateCacheTimer = gameLocal.time + UPDATE_CACHE_TIME;
	idPlayer* player = gameLocal.GetLocalPlayer();
	if ( !player )
		return;

	// Check if it's changed
	bool changed = player->lostInSpaceEntityDefNums.Num() != cachedEntityDefNums.Num();
	if ( !changed )
	{
		// Same size, check to see if each is equal
		for ( int i = 0; i < cachedEntityDefNums.Num(); i++ )
		{
			if ( player->lostInSpaceEntityDefNums[i] != cachedEntityDefNums[i] )
			{
				changed = true;
				break;
			}
		}
	}

	// It's changed, so update the cache and the UI list
	if ( changed )
	{
		cachedEntityDefNums = player->lostInSpaceEntityDefNums;
		itemList->Clear();

		for ( int i = 0; i < cachedEntityDefNums.Num(); i++ )
		{
			int defNum = cachedEntityDefNums[i];
			const idDeclEntityDef* def = static_cast< const idDeclEntityDef* >( declManager->DeclByIndex( DECL_ENTITYDEF, defNum ) );
			const char* displayName = def->dict.GetString( "displayname", "UNKNOWN" );
			if ( displayName[0] == '#' )
			{
				displayName = common->GetLanguageDict()->GetString( displayName );
			}
			itemList->Add( defNum, displayName, true );
		}

		itemList->SetSelection( 0 );
	}
}
