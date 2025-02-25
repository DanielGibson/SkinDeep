
#include "Player.h"
#include "ai/AI.h"

class idBossMonster : public idAI
{
public:
	CLASS_PROTOTYPE(idBossMonster);

	
	void					Spawn(void);

	virtual void			Think(void);

	void					StartBossTeleportSequence();
	void					StartBossExitSequence();

private:

	
	enum				{ BOSSSTATE_DORMANT, BOSSSTATE_TELPORTCHARGE, BOSSSTATE_ACTIVE };
	int					bossState;
	int					bossStateTimer;
	idVec3				bossSpawnPos;

	



};
//#pragma once