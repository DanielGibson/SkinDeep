
#include "Entity.h"


class idSpectateNode : public idEntity
{
public:
	CLASS_PROTOTYPE(idSpectateNode);

	idSpectateNode();
	~idSpectateNode();

	void					Spawn(void);
	virtual void			Think(void);

	void					Draw();


private:

	idStr					text;
	idStr					timestamp;
	
};