#include "Misc.h"
#include "Entity.h"
#include "Mover.h"
//#include "Item.h"
#include "Light.h"

#define CAT_INHABITANTS_MAX 6

class idCatpodInterior : public idStaticEntity
{
public:
	CLASS_PROTOTYPE(idCatpodInterior);

							idCatpodInterior(void);
	virtual					~idCatpodInterior(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);	

	void					SetPlayerEnter();

	virtual void			Event_PostSpawn(void);

	bool					GetPlayerIsInPod();

private:

	enum					{ IDLE };
	int						state;
	
	idLight *				ceilingLight;	

	void					EquipSlots();

	idStr					GetEquipClassname(idStr defCategory);
	idEntity*				SpawnEquipSingle(idStr defName);

	//Frob cube for the exit.
	idEntity*				frobBar;

	void					DoExitPod();
	idVec3					lastPlayerPosition;
	idAngles				lastPlayerViewangle;
	void					LaunchCatPod();

	idEntity*				GetPirateShip();

	int						fanfareTimer;
	int						fanfareState;
	enum					{ FF_DORMANT, FF_WAITING, FF_DONE };

	idAnimated*				catprisonerModel[CAT_INHABITANTS_MAX];
	int						totalcatInhabitants;
	bool					playerIsInCatpod;

	int						trackerSequence;
	enum                    {TRK_NONE, TRK_WAITINGFORPLAYER, TRK_SPAWNDELAY, TRK_VOLINE_1, TRK_VOLINE_2, TRK_DONE};
	idAnimated*				trackerModel;
	int						trackerTimer;

	float					trackerProgressStartValue; //0.0-1.0
	float					trackerProgressEndValue; //0.0-1.0
	int						trackerProgressState;
	int						trackerProgressTimer;
	enum					{TPS_DORMANT, TPS_STARTPAUSE, TPS_LERPING, };

	idStr					GetCatViaModel(idStr modelname, int *voiceprint);
	


};
//#pragma once