#include "Misc.h"
#include "Entity.h"



class idFireAttachment : public idEntity
{
public:
	CLASS_PROTOTYPE(idFireAttachment);

                            idFireAttachment(void);
    virtual					~idFireAttachment(void);

	void					Spawn(void);

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	virtual void			Think( void );

	void					AttachTo(idEntity *ent);
    

private:

    int                     lifetimer;
    int                     lifetimeMax;

    //int                     damageTimer;
    int                     damageTimerMax;

    const char *            damageDefname;

	idEntityPtr<idEntity>	attachOwner;
	idFuncEmitter			*particleEmitter;

	idBounds				damageBounds;

	void					Extinguish();

	

};
//#pragma once