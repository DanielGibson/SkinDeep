
#include "Entity.h"


class idSpectateTimeline : public idEntity
{
public:
	CLASS_PROTOTYPE(idSpectateTimeline);

	idSpectateTimeline();
	~idSpectateTimeline();

	void					Spawn(void);
	virtual void			Think(void);

	void					ToggleAllNodes(bool enable);


private:
	
	int						rolloverDebounceIdx;
	bool					hasLOS(idEntity* nodeEnt);
};