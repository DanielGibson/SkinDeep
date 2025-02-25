#include "Misc.h"
#include "Entity.h"
//#include "Mover.h"
//#include "Item.h"
//#include "Light.h"


class idEmailflag : public idAnimatedEntity
{
public:
	CLASS_PROTOTYPE(idEmailflag);

							idEmailflag(void);
	virtual					~idEmailflag(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);

private:
	
	enum					{UNDEPLOYED, DEPLOYED};
	int						state;	


	int						checkTimer;
	int						flagType;

	int						idleTimer;



};
//#pragma once