/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#pragma once


#ifndef __AI_H__
#define __AI_H__

#include "physics/Physics_Monster.h"
#include "Misc.h"
#include "Entity.h"
#include "Actor.h"
#include "Projectile.h"

#include "bc_actoricon.h"

const int AI_SUSPICIONCOUNTER = 10;
const int AI_SUSPICIONEXPIRETIMER = 2000;

const int AI_LIMBCOUNT = 4;

const float AI_CHECKSEARCH_DOT = 0.2f; //ranges 0.0 - 1.0. 0.0 = 180 arc, 1.0 = micro arc.

const int AI_TAKEDOWN_MAXTIME = 14; //how long to do the melee takedown. this is NOT a time interval!!!

class idFuncEmitter;

/*
===============================================================================

	idAI

===============================================================================
*/

const float	SQUARE_ROOT_OF_2			= 1.414213562f;
const float	AI_TURN_PREDICTION			= 0.2f;
const float	AI_TURN_SCALE				= 60.0f;
const float	AI_SEEK_PREDICTION			= 0.3f;
const float	AI_FLY_DAMPENING			= 0.15f;
const float	AI_HEARING_RANGE			= 2048;
const int	DEFAULT_FLY_OFFSET			= 68;



#define ATTACK_IGNORE			0
#define ATTACK_ON_DAMAGE		1
#define ATTACK_ON_ACTIVATE		2
#define ATTACK_ON_SIGHT			4

// defined in script/ai_base.script.  please keep them up to date.
typedef enum {
	MOVETYPE_DEAD,
	MOVETYPE_ANIM,
	MOVETYPE_SLIDE,
	MOVETYPE_FLY,
	MOVETYPE_STATIC,
	MOVETYPE_PUPPET,
	NUM_MOVETYPES
} moveType_t;

typedef enum {
	MOVE_NONE,
	MOVE_FACE_ENEMY,
	MOVE_FACE_ENTITY,

	// commands < NUM_NONMOVING_COMMANDS don't cause a change in position
	NUM_NONMOVING_COMMANDS,

	MOVE_TO_ENEMY = NUM_NONMOVING_COMMANDS,
	MOVE_TO_ENEMYHEIGHT,
	MOVE_TO_ENTITY,
	MOVE_OUT_OF_RANGE,
	MOVE_TO_ATTACK_POSITION,
	MOVE_TO_COVER,
	MOVE_TO_POSITION,
	MOVE_TO_POSITION_DIRECT,
	MOVE_SLIDE_TO_POSITION,
	MOVE_WANDER,
	NUM_MOVE_COMMANDS
} moveCommand_t;

typedef enum {
	TALK_NEVER,
	TALK_DEAD,
	TALK_OK,
	TALK_BUSY,
	NUM_TALK_STATES
} talkState_t;

//
// status results from move commands
// make sure to change script/doom_defs.script if you add any, or change their order
//
typedef enum {
	MOVE_STATUS_DONE,
	MOVE_STATUS_MOVING,
	MOVE_STATUS_WAITING,
	MOVE_STATUS_DEST_NOT_FOUND,
	MOVE_STATUS_DEST_UNREACHABLE,
	MOVE_STATUS_BLOCKED_BY_WALL,
	MOVE_STATUS_BLOCKED_BY_OBJECT,
	MOVE_STATUS_BLOCKED_BY_ENEMY,
	MOVE_STATUS_BLOCKED_BY_MONSTER
} moveStatus_t;

#define	DI_NODIR	-1

// obstacle avoidance
typedef struct obstaclePath_s {
	idVec3				seekPos;					// seek position avoiding obstacles
	idEntity *			firstObstacle;				// if != NULL the first obstacle along the path
	idVec3				startPosOutsideObstacles;	// start position outside obstacles
	idEntity *			startPosObstacle;			// if != NULL the obstacle containing the start position
	idVec3				seekPosOutsideObstacles;	// seek position outside obstacles
	idEntity *			seekPosObstacle;			// if != NULL the obstacle containing the seek position
} obstaclePath_t;

// path prediction
typedef enum {
	SE_BLOCKED			= BIT(0),
	SE_ENTER_LEDGE_AREA	= BIT(1),
	SE_ENTER_OBSTACLE	= BIT(2),
	SE_FALL				= BIT(3),
	SE_LAND				= BIT(4)
} stopEvent_t;

typedef struct predictedPath_s {
	idVec3				endPos;						// final position
	idVec3				endVelocity;				// velocity at end position
	idVec3				endNormal;					// normal of blocking surface
	int					endTime;					// time predicted
	int					endEvent;					// event that stopped the prediction
	const idEntity *	blockingEntity;				// entity that blocks the movement
} predictedPath_t;

//
// events
//
extern const idEventDef AI_BeginAttack;
extern const idEventDef AI_EndAttack;
extern const idEventDef AI_MuzzleFlash;
extern const idEventDef AI_CreateMissile;
extern const idEventDef AI_AttackMissile;
extern const idEventDef AI_FireMissileAtTarget;
#ifdef _D3XP
extern const idEventDef AI_LaunchProjectile;
extern const idEventDef AI_TriggerFX;
extern const idEventDef AI_StartEmitter;
extern const idEventDef AI_StopEmitter;
#endif
extern const idEventDef AI_AttackMelee;
extern const idEventDef AI_DirectDamage;
extern const idEventDef AI_JumpFrame;
extern const idEventDef AI_EnableClip;
extern const idEventDef AI_DisableClip;
extern const idEventDef AI_EnableGravity;
extern const idEventDef AI_DisableGravity;
extern const idEventDef AI_TriggerParticles;
extern const idEventDef AI_RandomPath;

//BC events
extern const idEventDef AI_LaunchMissileAtLaser;
extern const idEventDef AI_DoDamage;
extern const idEventDef AI_EjectBrass;

class idPathCorner;

typedef struct particleEmitter_s {
	particleEmitter_s() {
		particle = NULL;
		time = 0;
		joint = INVALID_JOINT;
	};
	const idDeclParticle *particle;
	int					time;
	jointHandle_t		joint;
} particleEmitter_t;

#ifdef _D3XP
typedef struct funcEmitter_s {
	char				name[64];
	idFuncEmitter*		particle;
	jointHandle_t		joint;
} funcEmitter_t;
#endif

class idMoveState {
public:
							idMoveState();

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	moveType_t				moveType;
	moveCommand_t			moveCommand;
	moveStatus_t			moveStatus;
	idVec3					moveDest;
	idVec3					moveDir;			// used for wandering and slide moves
	idEntityPtr<idEntity>	goalEntity;
	idVec3					goalEntityOrigin;	// move to entity uses this to avoid checking the floor position every frame
	int						toAreaNum;
	int						startTime;
	int						duration;
	float					speed;				// only used by flying creatures
	float					range;
	float					wanderYaw;
	int						nextWanderTime;
	int						blockTime;
	idEntityPtr<idEntity>	obstacle;
	idVec3					lastMoveOrigin;
	int						lastMoveTime;
	int						anim;
};

class idAASFindCover : public idAASCallback {
public:
						idAASFindCover( const idVec3 &hideFromPos );
						~idAASFindCover();

	virtual bool		TestArea( const idAAS *aas, int areaNum );

private:
	pvsHandle_t			hidePVS;
	int					PVSAreas[ idEntity::MAX_PVS_AREAS ];
};

class idAASFindAreaOutOfRange : public idAASCallback {
public:
						idAASFindAreaOutOfRange( const idVec3 &targetPos, float maxDist );

	virtual bool		TestArea( const idAAS *aas, int areaNum );

private:
	idVec3				targetPos;
	float				maxDistSqr;
};

class idAI;

class idAASFindAttackPosition : public idAASCallback {
public:
						idAASFindAttackPosition( const idAI *self, const idMat3 &gravityAxis, idEntity *target, const idVec3 &targetPos, const idVec3 &fireOffset );
						~idAASFindAttackPosition();

	virtual bool		TestArea( const idAAS *aas, int areaNum );

private:
	const idAI			*self;
	idEntity			*target;
	idBounds			excludeBounds;
	idVec3				targetPos;
	idVec3				fireOffset;
	idMat3				gravityAxis;
	pvsHandle_t			targetPVS;
	int					PVSAreas[ idEntity::MAX_PVS_AREAS ];
};



//Darkmod: Find Observation Position

class idAASFindObservationPosition : public idAASCallback {
public:
	idAASFindObservationPosition(const idAI *self, const idMat3 &gravityAxis, const idVec3 &targetPos, const idVec3 &eyeOffset, float maxDistanceFromWhichToObserve);
	~idAASFindObservationPosition();

	virtual bool		TestArea(const idAAS *aas, int areaNum);

	// Gets the best goal result, even if it didn't meet the maximum distance
	bool getBestGoalResult
	(
		float& out_bestGoalDistance,
		aasGoal_t& out_bestGoal
	);

private:
	const idAI			*self;
	idBounds			excludeBounds;
	idVec3				targetPos;
	idVec3				eyeOffset;
	idMat3				gravityAxis;
	pvsHandle_t			targetPVS;
	int					PVSAreas[idEntity::MAX_PVS_AREAS];
	float				maxObservationDistance;

	// The best goal found, even if it was greater than the maxObservationDistance
	float bestGoalDistance;
	bool b_haveBestGoal;
	aasGoal_t		bestGoal;
};







class idAI : public idActor {
public:
	CLASS_PROTOTYPE( idAI );

							idAI();
							~idAI();

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	void					Spawn( void );
	//void					HeardSound( idEntity *ent, const char *action );
	idActor					*GetEnemy( void ) const;
	void					TalkTo( idActor *actor );
	talkState_t				GetTalkState( void ) const;

	bool					GetAimDir( const idVec3 &firePos, idEntity *aimAtEnt, const idEntity *ignore, idVec3 &aimDir ) const;

	void					TouchedByFlashlight( idActor *flashlight_owner );

							// Outputs a list of all monsters to the console.
	static void				List_f( const idCmdArgs &args );

							// Finds a path around dynamic obstacles.
	static bool				FindPathAroundObstacles(const idAI* ai, const idPhysics *physics, const idAAS *aas, const idEntity *ignore, const idVec3 &startPos, const idVec3 &seekPos, obstaclePath_t &path );
							// Frees any nodes used for the dynamic obstacle avoidance.
	static void				FreeObstacleAvoidanceNodes( void );
							// Predicts movement, returns true if a stop event was triggered.
	static bool				PredictPath( const idEntity *ent, const idAAS *aas, const idVec3 &start, const idVec3 &velocity, int totalTime, int frameTime, int stopEvent, predictedPath_t &path );
							// Return true if the trajectory of the clip model is collision free.
	static bool				TestTrajectory( const idVec3 &start, const idVec3 &end, float zVel, float gravity, float time, float max_height, const idClipModel *clip, int clipmask, const idEntity *ignore, const idEntity *targetEntity, int drawtime, bool hasSolidTarget = true );
							// Finds the best collision free trajectory for a clip model.
	static bool				PredictTrajectory( const idVec3 &firePos, const idVec3 &target, float projectileSpeed, const idVec3 &projGravity, const idClipModel *clip, int clipmask, float max_height, const idEntity *ignore, const idEntity *targetEntity, int drawtime, idVec3 &aimDir, bool hasSolidTarget = true );

#ifdef _D3XP
	virtual void			Gib( const idVec3 &dir, const char *damageDefName );
#endif


	//BC PUBLIC
	
	

	void					EnterVacuum(void);
	void					HeardSuspiciousNoise();
	moveType_t				GetAIMovetype();

	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);
	void					SetDragFrobbable(bool value);
	bool					lastFovCheck;

	virtual void			SetAlertedState(bool investigateLKP);


	bool					GetBleedingOut();
	
	virtual void			Resurrect();
	int						combatState;



	virtual bool			InterestPointCheck(idEntity *interestpoint);
	virtual void			InterestPointReact(idEntity *interestpoint, int roletype);
	virtual void			ClearInterestPoint();
	virtual idEntityPtr<idEntity> GetLastInterest();
	virtual void			GotoState(int _state);

	int						aiState;

	bool					MoveToPosition(const idVec3 &pos); //moved to public, was in protected.
	float					TravelDistance(const idVec3& start, const idVec3& end) const; // was protected
	virtual idVec3			FindValidPosition(idVec3 targetPos); // SW: Was a private gunner function, lifted up so we can use it in idMeta

	idEntityPtr<idEntity>	lastEnemySeen;


	virtual void			SetJockeyState(bool value);
	virtual void			DoJockeySlamDamage();
	virtual void			DoJockeyBrutalSlam();

	virtual bool			CanAcceptStimulus();

	virtual void			DoWorldDamage();


	bool					TurnToward(float yaw);

	bool					IsInCombatstate();

	bool					doesRepairVerify;

	//BC PUBLIC END

	//SW
	int						GetYieldPriority() const;
	
	// blendo eric: calculate hearing through doors and around corners, returns [0.0f(silent),1.0f(max vol)]
	float					GetSoundIntensityObstructed(idVec3 soundPos, float soundDistanceMax);

protected:
	// navigation
	idAAS *					aas;
	int						travelFlags;

	idMoveState				move;
	idMoveState				savedMove;

	float					kickForce;
	bool					ignore_obstacles;
	float					blockedRadius;
	int						blockedMoveTime;
	int						blockedAttackTime;
	int						yieldPriority; // SW: unique value that tells AIs who gets right-of-way during obstacle avoidance. This should only be set upon construction.
	static int				nextYieldPriority; // SW: static class member counter to ensure each AI gets assigned a unique yield priority

	// turning
	float					ideal_yaw;
	float					current_yaw;
	float					turnRate;
	float					turnVel;
	float					anim_turn_yaw;
	float					anim_turn_amount;
	float					anim_turn_angles;

	// physics
	idPhysics_Monster		physicsObj;

	// flying
	jointHandle_t			flyTiltJoint;
	float					fly_speed;
	float					fly_bob_strength;
	float					fly_bob_vert;
	float					fly_bob_horz;
	int						fly_offset;					// prefered offset from player's view
	float					fly_seek_scale;
	float					fly_roll_scale;
	float					fly_roll_max;
	float					fly_roll;
	float					fly_pitch_scale;
	float					fly_pitch_max;
	float					fly_pitch;

	bool					allowMove;					// disables any animation movement
	bool					allowHiddenMovement;		// allows character to still move around while hidden
	bool					disableGravity;				// disables gravity and allows vertical movement by the animation
	bool					af_push_moveables;			// allow the articulated figure to push moveable objects

	// weapon/attack vars
	bool					lastHitCheckResult;
	int						lastHitCheckTime;
	int						lastAttackTime;
	float					melee_range;
	float					projectile_height_to_distance_ratio;	// calculates the maximum height a projectile can be thrown
	idList<idVec3>			missileLaunchOffset;

	const idDict *			projectileDef;
	mutable idClipModel		*projectileClipModel;
	float					projectileRadius;
	float					projectileSpeed;
	idVec3					projectileVelocity;
	idVec3					projectileGravity;
	idEntityPtr<idProjectile> projectile;
	idStr					attack;

	// chatter/talking
	const idSoundShader		*chat_snd;
	int						chat_min;
	int						chat_max;
	int						chat_time;
	talkState_t				talk_state;
	idEntityPtr<idActor>	talkTarget;

	// cinematics
	int						num_cinematics;
	int						current_cinematic;

	bool					allowJointMod;
	idEntityPtr<idEntity>	focusEntity;
	idVec3					currentFocusPos;
	int						focusTime;
	int						alignHeadTime;
	int						forceAlignHeadTime;
	idAngles				eyeAng;
	idAngles				lookAng;
	idAngles				destLookAng;
	idAngles				lookMin;
	idAngles				lookMax;
	idList<jointHandle_t>	lookJoints;
	idList<idAngles>		lookJointAngles;
	float					eyeVerticalOffset;
	float					eyeHorizontalOffset;
	float					eyeFocusRate;
	float					headFocusRate;
	int						focusAlignTime;

	// special fx
	float					shrivel_rate;
	int						shrivel_start;

	bool					restartParticles;			// should smoke emissions restart
	bool					useBoneAxis;				// use the bone vs the model axis
	idList<particleEmitter_t> particles;				// particle data

	renderLight_t			worldMuzzleFlash;			// positioned on world weapon bone
	int						worldMuzzleFlashHandle;
	jointHandle_t			flashJointWorld;
	int						muzzleFlashEnd;
	int						flashTime;

	// joint controllers
	idAngles				eyeMin;
	idAngles				eyeMax;
	jointHandle_t			focusJoint;
	jointHandle_t			orientationJoint;

	// enemy variables
	idEntityPtr<idActor>	enemy;
	idVec3					lastVisibleEnemyPos;
	idVec3					lastVisibleEnemyEyeOffset;
	idVec3					lastVisibleReachableEnemyPos;
	idVec3					lastReachableEnemyPos;
	bool					wakeOnFlashlight;

#ifdef _D3XP
	bool					spawnClearMoveables;

	idHashTable<funcEmitter_t> funcEmitters;

	idEntityPtr<idHarvestable>	harvestEnt;
#endif

	// script variables
	idScriptBool			AI_TALK;
	idScriptBool			AI_DAMAGE;
	idScriptBool			AI_PAIN;
	idScriptFloat			AI_SPECIAL_DAMAGE;
	idScriptBool			AI_DEAD;
	idScriptBool			AI_ENEMY_VISIBLE;
	idScriptBool			AI_ENEMY_IN_FOV;
	idScriptBool			AI_ENEMY_DEAD;
	idScriptBool			AI_MOVE_DONE;
	idScriptBool			AI_ONGROUND;
	idScriptBool			AI_ACTIVATED;
	idScriptBool			AI_FORWARD;
	idScriptBool			AI_JUMP;
	idScriptBool			AI_ENEMY_REACHABLE;
	idScriptBool			AI_BLOCKED;
	idScriptBool			AI_OBSTACLE_IN_PATH;
	idScriptBool			AI_DEST_UNREACHABLE;
	idScriptBool			AI_HIT_ENEMY;
	idScriptBool			AI_PUSHED;

	//BC custom script variables
	idScriptBool			AI_BACKWARD;
	idScriptBool			AI_LEFT;
	idScriptBool			AI_RIGHT;
	idScriptBool			AI_SHIELDHIT;
	idScriptBool			AI_CUSTOMIDLEANIM;
	idScriptBool			AI_NODEANIM;
	idScriptBool			AI_DODGELEFT;
	idScriptBool			AI_DODGERIGHT;

	//
	// ai/ai.cpp
	//
	void					SetAAS( void );
	virtual	void			DormantBegin( void );	// called when entity becomes dormant
	virtual	void			DormantEnd( void );		// called when entity wakes from being dormant
	void					Think( void );
	void					Activate( idEntity *activator );
	int						ReactionTo( idEntity *ent );
	bool					CheckForEnemy( void );
	void					EnemyDead( void );
	virtual bool			CanPlayChatterSounds( void ) const;
	void					SetChatSound( void );
	void					PlayChatter( void );
	virtual void			Hide( void );
	virtual void			Show( void );
	idVec3					FirstVisiblePointOnPath( const idVec3 origin, const idVec3 &target, int travelFlags ) const;
	void					CalculateAttackOffsets( void );
	void					PlayCinematic( void );

	// movement
	virtual void			ApplyImpulse( idEntity *ent, int id, const idVec3 &point, const idVec3 &impulse );
	void					GetMoveDelta( const idMat3 &oldaxis, const idMat3 &axis, idVec3 &delta );
	void					CheckObstacleAvoidance( const idVec3 &goalPos, idVec3 &newPos );
	void					DeadMove( void );
	void					AnimMove( void );
	void					SlideMove( void );
	void					AdjustFlyingAngles( void );
	void					AddFlyBob( idVec3 &vel );
	void					AdjustFlyHeight( idVec3 &vel, const idVec3 &goalPos );
	void					FlySeekGoal( idVec3 &vel, idVec3 &goalPos );
	void					AdjustFlySpeed( idVec3 &vel );
	void					FlyTurn( void );
	void					FlyMove( void );
	void					StaticMove( void );

	// damage
	virtual bool			Pain( idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location, bool playPainSound = 1, const char *damageDef = NULL);
	virtual void			Killed( idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location );

	// navigation
	void					KickObstacles( const idVec3 &dir, float force, idEntity *alwaysKick );
	bool					ReachedPos( const idVec3 &pos, const moveCommand_t moveCommand ) const;
	int						PointReachableAreaNum( const idVec3 &pos, const float boundsScale = 2.0f ) const;
	bool					PathToGoal( aasPath_t &path, int areaNum, const idVec3 &origin, int goalAreaNum, const idVec3 &goalOrigin ) const;
	void					DrawRoute( void ) const;
	bool					GetMovePos( idVec3 &seekPos );
	bool					MoveDone( void ) const;
	bool					EntityCanSeePos( idActor *actor, const idVec3 &actorOrigin, const idVec3 &pos );
	void					BlockedFailSafe( void );

	// movement control
	void					StopMove( moveStatus_t status );
	bool					FaceEnemy( void );
	bool					FaceEntity( idEntity *ent );
	bool					DirectMoveToPosition( const idVec3 &pos );
	bool					MoveToEnemyHeight( void );
	bool					MoveOutOfRange( idEntity *entity, float range );
	bool					MoveToAttackPosition( idEntity *ent, int attack_anim );
	bool					MoveToEnemy( void );
	bool					MoveToEntity( idEntity *ent );	
	bool					MoveToCover( idEntity *entity, const idVec3 &pos );
	bool					SlideToPosition( const idVec3 &pos, float time );
	bool					WanderAround( void );
	bool					StepDirection( float dir );
	bool					NewWanderDir( const idVec3 &dest );

	// effects
	const idDeclParticle	*SpawnParticlesOnJoint( particleEmitter_t &pe, const char *particleName, const char *jointName );
	void					SpawnParticles( const char *keyName );
	bool					ParticlesActive( void );

	// turning
	bool					FacingIdeal( void );
	void					Turn( void );	
	bool					TurnToward( const idVec3 &pos );

	// enemy management
	void					ClearEnemy( void );
	bool					EnemyPositionValid( void ) const;
	void					SetEnemyPosition( void );
	void					UpdateEnemyPosition( void );
	void					SetEnemy( idActor *newEnemy );

	// attacks
	void					CreateProjectileClipModel( void ) const;
	idProjectile			*CreateProjectile( const idVec3 &pos, const idVec3 &dir );
	void					RemoveProjectile( void );
	idProjectile			*LaunchProjectile( const char *jointname, idEntity *target, bool clampToAttackCone, idVec3 fallbackTarget );
	virtual void			DamageFeedback( idEntity *victim, idEntity *inflictor, int &damage );
	void					DirectDamage( const char *meleeDefName, idEntity *ent );
	bool					TestMelee( void ) const;
	bool					AttackMelee( const char *meleeDefName );
	void					BeginAttack( const char *name );
	void					EndAttack( void );
	void					PushWithAF( void );

	// special effects
	void					GetMuzzle( const char *jointname, idVec3 &muzzle, idMat3 &axis );
	void					InitMuzzleFlash( void );
	void					TriggerWeaponEffects( const idVec3 &muzzle );
	void					UpdateMuzzleFlash( void );
	virtual bool			UpdateAnimationControllers( void );
	void					UpdateParticles( void );
	void					TriggerParticles( const char *jointName );

#ifdef _D3XP
	void					TriggerFX( const char* joint, const char* fx );
	idEntity*				StartEmitter( const char* name, const char* joint, const char* particle );
	idEntity*				GetEmitter( const char* name );
	void					StopEmitter( const char* name );
#endif

	// AI script state management
	void					LinkScriptVariables( void );
	void					UpdateAIScript( void );

	//
	// ai/ai_events.cpp
	//
	virtual void			Event_PostSpawn();
	void					Event_Activate( idEntity *activator );
	void					Event_Touch( idEntity *other, trace_t *trace );
	void					Event_FindEnemy( int useFOV );
	idEntity				*Event_FindEnemyAI( int useFOV );
	void					Event_FindEnemyInCombatNodes( void );
	void					Event_ClosestReachableEnemyOfEntity( idEntity *team_mate );
	//void					Event_HeardSound( int ignore_team );
	void					Event_SetEnemy( idEntity *ent );
	void					Event_ClearEnemy( void );
	void					Event_MuzzleFlash( const char *jointname );
	void					Event_CreateMissile( const char *jointname );
	void					Event_AttackMissile( const char *jointname );
	void					Event_FireMissileAtTarget( const char *jointname, const char *targetname );
	void					Event_LaunchMissile( const idVec3 &muzzle, const idAngles &ang );
#ifdef _D3XP
	void					Event_LaunchProjectile( const char *entityDefName );
#endif
	void					Event_AttackMelee( const char *meleeDefName );
	void					Event_DirectDamage( idEntity *damageTarget, const char *damageDefName );
	void					Event_RadiusDamageFromJoint( const char *jointname, const char *damageDefName );
	void					Event_BeginAttack( const char *name );
	void					Event_EndAttack( void );
	void					Event_MeleeAttackToJoint( const char *jointname, const char *meleeDefName );
	void					Event_RandomPath( void );
	void					Event_CanBecomeSolid( void );
	void					Event_BecomeSolid( void );
	void					Event_BecomeNonSolid( void );
	void					Event_BecomeRagdoll( void );
	void					Event_StopRagdoll( void );
	void					Event_SetHealth( float newHealth );
	void					Event_GetHealth( void );
	void					Event_AllowDamage( void );
	void					Event_IgnoreDamage( void );
	void					Event_GetCurrentYaw( void );
	void					Event_TurnTo( float angle );
	void					Event_TurnToPos( const idVec3 &pos );
	void					Event_TurnToEntity( idEntity *ent );
	void					Event_MoveStatus( void );
	void					Event_StopMove( void );
	void					Event_MoveToCover( void );
	void					Event_MoveToEnemy( void );
	void					Event_MoveToEnemyHeight( void );
	void					Event_MoveOutOfRange( idEntity *entity, float range );
	void					Event_MoveToAttackPosition( idEntity *entity, const char *attack_anim );
	void					Event_MoveToEntity( idEntity *ent );
	void					Event_MoveToPosition( const idVec3 &pos );
	void					Event_SlideTo( const idVec3 &pos, float time );
	void					Event_Wander( void );
	void					Event_FacingIdeal( void );
	void					Event_FaceEnemy( void );
	void					Event_FaceEntity( idEntity *ent );
	void					Event_WaitAction( const char *waitForState );
	void					Event_GetCombatNode( void );
	void					Event_EnemyInCombatCone( idEntity *ent, int use_current_enemy_location );
	void					Event_WaitMove( void );
	void					Event_GetJumpVelocity( const idVec3 &pos, float speed, float max_height );
	void					Event_EntityInAttackCone( idEntity *ent );
	void					Event_CanSeeEntity( idEntity *ent, bool idleMode );
	void					Event_SetTalkTarget( idEntity *target );
	void					Event_GetTalkTarget( void );
	void					Event_SetTalkState( int state );
	void					Event_EnemyRange( void );
	void					Event_EnemyRange2D( void );
	void					Event_GetEnemy( void );
	void					Event_GetEnemyPos( void );
	void					Event_GetEnemyEyePos( void );
	void					Event_PredictEnemyPos( float time );
	void					Event_CanHitEnemy( void );
	void					Event_CanHitEnemyFromAnim( const char *animname );
	void					Event_CanHitEnemyFromJoint( const char *jointname );
	void					Event_EnemyPositionValid( void );
	void					Event_ChargeAttack( const char *damageDef );
	void					Event_TestChargeAttack( void );
	void					Event_TestAnimMoveTowardEnemy( const char *animname );
	void					Event_TestAnimMove( const char *animname );
	void					Event_TestMoveToPosition( const idVec3 &position );
	void					Event_TestMeleeAttack( void );
	void					Event_TestAnimAttack( const char *animname );
	void					Event_Shrivel( float shirvel_time );
	void					Event_Burn( void );
	void					Event_PreBurn( void );
	void					Event_ClearBurn( void );
	void					Event_SetSmokeVisibility( int num, int on );
	void					Event_NumSmokeEmitters( void );
	void					Event_StopThinking( void );
	void					Event_GetTurnDelta( void );
	void					Event_GetMoveType( void );
	void					Event_SetMoveType( int moveType );
	void					Event_SaveMove( void );
	void					Event_RestoreMove( void );
	void					Event_AllowMovement( float flag );
	void					Event_JumpFrame( void );
	void					Event_EnableClip( void );
	void					Event_DisableClip( void );
	void					Event_EnableGravity( void );
	void					Event_DisableGravity( void );
	void					Event_EnableAFPush( void );
	void					Event_DisableAFPush( void );
	void					Event_SetFlySpeed( float speed );
	void					Event_SetFlyOffset( int offset );
	void					Event_ClearFlyOffset( void );
	void					Event_GetClosestHiddenTarget( const char *type );
	void					Event_GetRandomTarget( const char *type );
	void					Event_TravelDistanceToPoint( const idVec3 &pos );
	void					Event_TravelDistanceToEntity( idEntity *ent );
	void					Event_TravelDistanceBetweenPoints( const idVec3 &source, const idVec3 &dest );
	void					Event_TravelDistanceBetweenEntities( idEntity *source, idEntity *dest );
	void					Event_LookAtEntity( idEntity *ent, float duration );
	void					Event_LookAtEnemy( float duration );
	void					Event_SetJointMod( int allowJointMod );
	void					Event_ThrowMoveable( void );
	void					Event_ThrowAF( void );
	void					Event_SetAngles( idAngles const &ang );
	void					Event_GetAngles( void );
	void					Event_RealKill( void );
	void					Event_Kill( void );
	void					Event_WakeOnFlashlight( int enable );
	void					Event_LocateEnemy( void );
	void					Event_KickObstacles( idEntity *kickEnt, float force );
	void					Event_GetObstacle( void );
	void					Event_PushPointIntoAAS( const idVec3 &pos );
	void					Event_GetTurnRate( void );
	void					Event_SetTurnRate( float rate );
	void					Event_AnimTurn( float angles );
	void					Event_AllowHiddenMovement( int enable );
	void					Event_TriggerParticles( const char *jointName );
	void					Event_FindActorsInBounds( const idVec3 &mins, const idVec3 &maxs );
	void					Event_CanReachPosition( const idVec3 &pos );
	void					Event_CanReachEntity( idEntity *ent );
	void					Event_CanReachEnemy( void );
	void					Event_GetReachableEntityPosition( idEntity *ent );
#ifdef _D3XP
	void					Event_MoveToPositionDirect( const idVec3 &pos );
	void					Event_AvoidObstacles( int ignore);
	void					Event_TriggerFX( const char* joint, const char* fx );

	void					Event_StartEmitter( const char* name, const char* joint, const char* particle );
	void					Event_GetEmitter( const char* name );
	void					Event_StopEmitter( const char* name );
#endif

	//BC PROTECTED. Put all private vars/funcs here.
	void					Event_LookAtPoint(idVec3 _point, float duration);
	void					LookAtPointMS(idVec3 _point, int durationMS);
	void					Event_GetCoverNode(void);
	void					Event_GetLastEnemyPosition(void);
	void					Event_HeardSuspiciousNoise();
	void					Event_HeardSuspiciousPriority();
	void					Event_GetThrowableObject(const idVec3 &mins, const idVec3 &maxs, float speed, float minDist, float offset);
	void					Event_ThrowObjectAtEnemy(idEntity *ent, float speed);
	bool					Event_ThrowObjectAtPosition(idEntity *ent, idVec3 targetPos);
	void					Event_GetSearchNode();
	idEntity *				GetSearchNode();
	void					Event_SetLaserLock(idVec3 laserPos);
	void					Event_SetLaserSkin(const char* laserSkinName);
	void					Event_GetEnemyCenter();
	void					Event_SetFlyBobStrength(int value);
	void					Event_GetFlyBobStrength();
	void					Event_FindEnemyAIvisible();
	idEntity *				FindEnemyAIVisible();
	void					Event_GetLaserLock();
	void					Event_GetLaserHitPos();
	void					Event_LaunchMissileAtLaser(const char *jointname);
	void					Event_GetFlySpeed();
	void					Event_DoDamage(const char *damageDefName);
	void					Event_ResetLookPoint();
	void					LaunchProjectileAtPos(const char *jointname, idVec3 fireTarget);
	void					Event_CanHitFromAnim(const char *animname, idVec3 targetPos);
	bool					CanHitFromAnim(const char *animname, idVec3 targetPos);
	void					Event_SetLaserActive(int active);
	void					Event_CheckSearchLook(idVec3 lookPoint, int useFacing);
	bool					CheckSearchLook(idVec3 lookPoint, int useFacing, bool doProximityCheck);
	void					Event_SetLastVisiblePos(idVec3 pos);
	void					Event_CheckForwardDot(idVec3 lookPoint);
	void					Event_SetLaserEndLock(idVec3 pos);
	void					Event_StartStunState(const char* damageDefName);
	void					Event_IsBleedingOut(void);
	void					Event_GetSkullsaver(void);
	virtual void			Event_SetPathEntity(idEntity *pathEnt);


	idBeam*					lasersightbeam;
	idBeam*					lasersightbeamTarget;
	idEntity*				laserdot;
	idVec3					laserLockPosition;
	idVec3					laserEndLockPosition;

	idVec3					laserRadarEndPos;
	idVec3					laserRadarStartPos;
	int						laserRadarTimer;
	int						laserRadarState;
	idVec3					laserRadarDir;

	int						lastVisibleEnemyTime;
	bool					canSeeEnemy;

	idActorIcon				actorIcon;
	void					UpdateIcon();

	void					EjectBrass();
	jointHandle_t			brassJoint;
	idDict					brassDict;

	void					SpawnEmitters(const char *keyName);

	int						lastPlayerSightTimer;
	int						playersightCounter;

	idEntity*				dragButtons[AI_LIMBCOUNT]; //This is how player drags bodies around.
	void					SpawnDragButton(int index, const char * jointName, const char * displayString);


	const idDeclSkin		*currentskin;
	const idDeclSkin*		damageFlashSkin;
	int						damageflashTimer;
	bool					damageFlashActive;

	int						lastBrassTime;

	//bleed out ui
	idEntity*				bleedoutTube;	
	int						bleedoutTime;
	int						bleedoutState;
	int						bleedoutDamageTimer;
	enum					{ BLEEDOUT_NONE, BLEEDOUT_ACTIVE, BLEEDOUT_SKULLBURN, BLEEDOUT_DONE};

	


	//Darkmod.
	void					Event_GetObservationPosition(const idVec3& pointToObserve);
	idVec3					GetObservationPosition(const idVec3& pointToObserve, const float visualAcuityZeroToOne, const unsigned short maxCost); // grayman #4347





	virtual void			OnShieldHit();
	void					Event_GetObservationViaNodes(idVec3 pointToObserve);
	idVec3					GetObservationViaNodes(idVec3 pointToObserve);
	void					SetCombatState(int value, bool restartCombatTimer = false);

	void					SpawnSkullsaver();
	idEntity*				skullEnt; // refers to skullsaver if it exists, otherwise NULL

	idEntityPtr<idLocationEntity> skullSpawnOrigLoc; // blendo eric: where the ai originally spawns

	void					ForceStopDrag();

	
	void					Event_GetAIState();

	bool					CanReachPosition(const idVec3 &pos);

	int						GetSightExposure(idActor *enemyEnt);

	

	bool					TestAnimMove(const char *animname);

	bool					ActorIsNearOpenVentdoor(const idActor *actor);

    int                     triggerTimer;

	bool					playedBleedoutbeep1;
	bool					playedBleedoutbeep2;
	bool					playedBleedoutbeep3;

	idEntity *				GetSearchNodeSpreadOut();
	idEntity *				GetSearchNodeInLocEntNum(int locationEntnum);

    void                    PopoffHelmet(bool doSlowmo);

	void					RemoveInterestpointsBoundToMe();

	enum					{AILS_DEFEATED, AILS_ALIVE, AILS_BLEEDINGOUT, AILS_INSKULLSAVER};
	void					Event_GetLifeState();

	//BC PROTECTED end.


};

class idCombatNode : public idEntity {
public:
	CLASS_PROTOTYPE( idCombatNode );

						idCombatNode();

	void				Save( idSaveGame *savefile ) const;
	void				Restore( idRestoreGame *savefile );

	void				Spawn( void );
	bool				IsDisabled( void ) const;
	bool				EntityInView( idActor *actor, const idVec3 &pos );
	static void			DrawDebugInfo( void );

private:
	float				min_dist;
	float				max_dist;
	float				cone_dist;
	float				min_height;
	float				max_height;
	idVec3				cone_left;
	idVec3				cone_right;
	idVec3				offset;
	bool				disabled;

	void				Event_Activate( idEntity *activator );
	void				Event_MarkUsed( void );
};

#endif /* !__AI_H__ */
