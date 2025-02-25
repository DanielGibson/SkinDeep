#include "Misc.h"
#include "Entity.h"

class idSabotagePoint : public idAnimatedEntity
{
public:
	CLASS_PROTOTYPE(idSabotagePoint);

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );
	void					Spawn( void );
	virtual void			Think( void );
	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

	bool					IsAvailable();

protected:

	enum					{ IDLE, ARMED, EXHAUSTED };
	int						state;
};

class idSabotagePoint_SparkHazard : public idSabotagePoint
{
	public:
		CLASS_PROTOTYPE(idSabotagePoint_SparkHazard);

		void			Spawn(void);
		virtual bool	DoFrob(int index = 0, idEntity* frobber = NULL);
		virtual void	Think(void);

	private:
		// Spawnargs
		int triggerRadius; // How close does a bad guy have to be to trigger this?
		const idDeclEntityDef* sparkJetDef; // Info about the electrical hazard that pops out when triggered
		// End of spawnargs
		
		const int RADIUS_CHECK_TIME = 250; // How frequently to check if an enemy is within our radius
		int lastRadiusCheck; // When did we last check for a nearby enemy?
		idEntityFx* armedFx;

};
