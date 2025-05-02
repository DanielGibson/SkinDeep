#include "Misc.h"
#include "Entity.h"

#include "Item.h"

#define YARNLOCBOXCOUNT 16

class idYarnBoard : public idStaticEntity
{
public:
	CLASS_PROTOTYPE(idYarnBoard);

							idYarnBoard(void);
	virtual					~idYarnBoard(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);
	
	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);
	
	virtual void			Think(void);

private:

	bool					hasFrobbed;

	idFuncEmitter			*sparkleParticles;

	void					SetYarnInfo(const char* startModel, const char* endModel, idVec3 lookOffset, const char* voString );

	idStr					model_endName;
	idVec3					lookOffset;

	int						stateTimer;
	int						state;
	enum                    {YB_IDLE, YB_LOOKLERP, YB_SHORTPAUSE, YB_SHORTPAUSE2, YB_DONE};

	idStr					voName;


	idEntity*				locboxes[YARNLOCBOXCOUNT];

	enum					{
								LB_ZENAMASKED,
								LB_IDO,
								LB_ZENACLONE,
								LB_NINA,
								LB_LEADSPIRATES,
								LB_PIRATESKILL,
								LB_SKULLSAVER,
								LB_MADABANDON,
								LB_WHEREISSHE,
								LB_BEASTBUG,
								LB_FINALMISSION,
								LB_NEWSPAPER,
								LB_LITTLELION,
								LB_PARTNERCRIME,
								LB_RICHRUDE,
								LB_LIKESZENAMORE
							};

	idStr					GetLocboxDefinitionViaIndex(int index);

	void					SetLocboxVisibility(idStr modelname);
};
//#pragma once