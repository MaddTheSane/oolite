/*
 
 ShipEntity.h
 
 Entity subclass representing a ship, or various other flying things like cargo
 pods and stations (a subclass).
 
 Oolite
 Copyright (C) 2004-2013 Giles C Williams and contributors
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA 02110-1301, USA.
 
 */

#import "OOEntityWithDrawable.h"
#import "OOPlanetEntity.h"
#import "OOJSPropID.h"

@class	OOColor, StationEntity, WormholeEntity, AI, Octree, OOMesh, OOScript,
	OOJSScript, OORoleSet, OOShipGroup, OOEquipmentType, OOWeakSet,
	OOExhaustPlumeEntity, OOFlasherEntity;

#define MAX_TARGETS						24
#define RAIDER_MAX_CARGO				5
#define MERCHANTMAN_MAX_CARGO			125

#define PIRATES_PREFER_PLAYER			YES

#define TURRET_MINIMUM_COS				0.20f

#define SHIP_THRUST_FACTOR				5.0f
#define AFTERBURNER_BURNRATE			0.25f

#define CLOAKING_DEVICE_ENERGY_RATE		12.8f
#define CLOAKING_DEVICE_MIN_ENERGY		128
#define CLOAKING_DEVICE_START_ENERGY	0.75f

#define MILITARY_JAMMER_ENERGY_RATE		3
#define MILITARY_JAMMER_MIN_ENERGY		128

#define COMBAT_IN_RANGE_FACTOR			0.035f
#define COMBAT_BROADSIDE_IN_RANGE_FACTOR			0.020f
#define COMBAT_OUT_RANGE_FACTOR			0.500f
#define COMBAT_BROADSIDE_RANGE_FACTOR			0.900f
#define COMBAT_WEAPON_RANGE_FACTOR		1.200f
#define COMBAT_JINK_OFFSET				500.0f

#define SHIP_COOLING_FACTOR				0.1f
// heat taken from energy damage depends on mass
// but limit maximum rate since masses vary so much
// Cobra III ~=215000
#define SHIP_ENERGY_DAMAGE_TO_HEAT_FACTOR  (mass > 400000 ? 200000 / mass : 0.5)
#define SHIP_INSULATION_FACTOR			0.00175f
#define SHIP_MAX_CABIN_TEMP				256.0f
#define SHIP_MIN_CABIN_TEMP				60.0f
#define EJECTA_TEMP_FACTOR				0.85f
#define DEFAULT_HYPERSPACE_SPIN_TIME	15.0f

#define SUN_TEMPERATURE					1250.0f

#define MAX_ESCORTS						16
#define ESCORT_SPACING_FACTOR			3.0

#define SHIPENTITY_MAX_MISSILES			32

#define TURRET_TYPICAL_ENERGY			25.0f
#define TURRET_SHOT_SPEED				2000.0f
#define TURRET_SHOT_DURATION			3.0
#define TURRET_SHOT_RANGE				(TURRET_SHOT_SPEED * TURRET_SHOT_DURATION)
#define TURRET_SHOT_FREQUENCY			(TURRET_SHOT_DURATION * TURRET_SHOT_DURATION * TURRET_SHOT_DURATION / 100.0)

#define NPC_PLASMA_SPEED				1500.0f
#define MAIN_PLASMA_DURATION			5.0
#define NPC_PLASMA_RANGE				(MAIN_PLASMA_DURATION * NPC_PLASMA_SPEED)

#define PLAYER_PLASMA_SPEED				1000.0f
#define PLAYER_PLASMA_RANGE				(MAIN_PLASMA_DURATION * PLAYER_PLASMA_SPEED)

#define TRACTOR_FORCE					2500.0f

#define AIMS_AGGRESSOR_SWITCHED_TARGET	@"AGGRESSOR_SWITCHED_TARGET"

// number of vessels considered when scanning around
#define MAX_SCAN_NUMBER					32

#define BASELINE_SHIELD_LEVEL			128.0f			// Max shield level with no boosters.
#define INITIAL_SHOT_TIME				100.0

#define	MIN_FUEL						0				// minimum fuel required for afterburner use
#ifdef OO_DUMP_PLANETINFO
// debugging planetinfo needs rapid jumping
#define MAX_JUMP_RANGE					150.0
#else
#define MAX_JUMP_RANGE					7.0				// the 7 ly limit
#endif

#define ENTITY_PERSONALITY_MAX			0x7FFFU
#define ENTITY_PERSONALITY_INVALID		0xFFFFU


#define WEAPON_COOLING_FACTOR			6.0f
#define NPC_MAX_WEAPON_TEMP				256.0f
#define WEAPON_COOLING_CUTOUT			0.85f

#define COMBAT_AI_WEAPON_TEMP_READY		0.25f * NPC_MAX_WEAPON_TEMP
#define COMBAT_AI_WEAPON_TEMP_USABLE	WEAPON_COOLING_CUTOUT * NPC_MAX_WEAPON_TEMP
// factor determining how close to target AI has to be to be confident in aim
// higher factor makes confident at longer ranges
#define COMBAT_AI_CONFIDENCE_FACTOR		1250000.0f
#define COMBAT_AI_ISNT_AWFUL			0.0f
// removes BEHAVIOUR_ATTACK_FLY_TO_TARGET_SIX/TWELVE (unless thargoid)
#define COMBAT_AI_IS_SMART				5.0f
// adds BEHAVIOUR_(FLEE_)EVASIVE_ACTION
#define COMBAT_AI_FLEES_BETTER			6.0f
// adds BEHAVIOUR_ATTACK_BREAK_OFF_TARGET
#define COMBAT_AI_DOGFIGHTER			6.5f
// adds BEHAVIOUR_ATTACK_SLOW_DOGFIGHT
#define COMBAT_AI_TRACKS_CLOSER			7.5f
#define COMBAT_AI_USES_SNIPING			8.5f
// adds BEHAVIOUR_ATTACK_SNIPER
#define COMBAT_AI_FLEES_BETTER_2		9.0f
// AI reacts to changes in target path in about 1.5 seconds.
#define COMBAT_AI_STANDARD_REACTION_TIME	1.5f



#define MAX_LANDING_SPEED				50.0
#define MAX_LANDING_SPEED2				(MAX_LANDING_SPEED * MAX_LANDING_SPEED)

#define MAX_COS							0.995	// cos(5 degrees) is close enough in most cases for navigation
#define MAX_COS2						(MAX_COS * MAX_COS)


#define ENTRY(label, value) label = value,

typedef enum OOBehaviour
{
#include "OOBehaviour.tbl"
} OOBehaviour;

#undef ENTRY


/*typedef enum
{
	WEAPON_NONE						= 0U,
	WEAPON_PLASMA_CANNON			= 1,
	WEAPON_PULSE_LASER				= 2,
	WEAPON_BEAM_LASER				= 3,
	WEAPON_MINING_LASER				= 4,
	WEAPON_MILITARY_LASER			= 5,
	WEAPON_THARGOID_LASER			= 10,
	WEAPON_UNDEFINED
	} OOWeaponType; */
typedef OOEquipmentType* OOWeaponType;


typedef NS_ENUM(unsigned int, OOAlertCondition)
{
	// Alert conditions are used by player and station entities.
	// NOTE: numerical values are available to scripts and shaders.
	ALERT_CONDITION_DOCKED	= 0,
	ALERT_CONDITION_GREEN	= 1,
	ALERT_CONDITION_YELLOW	= 2,
	ALERT_CONDITION_RED		= 3
};


typedef NS_ENUM(unsigned int, OOShipDamageType)
{
#define DIFF_STRING_ENTRY(label, string) label,
#include "OOShipDamageType.tbl"
#undef DIFF_STRING_ENTRY
	
	kOOShipDamageTypeDefault = kOODamageTypeEnergy
};


@interface ShipEntity: OOEntityWithDrawable <OOSubEntity, OOBeaconEntity>
{
@public
	// derived variables
	OOTimeDelta				shot_time;					// time elapsed since last shot was fired
	
	// navigation
	Vector					v_forward, v_up, v_right;	// unit vectors derived from the direction faced
	
	// variables which are controlled by AI
	HPVector				_destination;				// for flying to/from a set point

	GLfloat					desired_range;				// range to which to journey/scan
	GLfloat					desired_speed;				// speed at which to travel
	// next three used to set desired attitude, flightRoll etc. gradually catch up to target
	GLfloat					stick_roll;					// stick roll
	GLfloat					stick_pitch;				// stick pitch
	GLfloat					stick_yaw;					// stick yaw
	OOBehaviour				behaviour;					// ship's behavioural state
	
	BoundingBox				totalBoundingBox;			// records ship configuration
	
@protected
	//set-up
	NSDictionary			*shipinfoDictionary;
	
	Quaternion				subentityRotationalVelocity;
	
	//scripting
	OOJSScript				*script;
	OOJSScript				*aiScript;
	OOTimeAbsolute    aiScriptWakeTime;
	
	//docking instructions
	NSDictionary			*dockingInstructions;
	
	OOColor					*laser_color;
	OOColor					*default_laser_color;
	OOColor					*exhaust_emissive_color;
	OOColor					*scanner_display_color1;
	OOColor					*scanner_display_color2;
	OOColor					*scanner_display_color_hostile1;
	OOColor					*scanner_display_color_hostile2;
	
	// per ship-type variables
	//
	GLfloat					maxFlightSpeed;				// top speed			(160.0 for player)  (200.0 for fast raider)
	GLfloat					max_flight_roll;			// maximum roll rate	(2.0 for player)	(3.0 for fast raider)
	GLfloat					max_flight_pitch;			// maximum pitch rate   (1.0 for player)	(1.5 for fast raider) also radians/sec for (* turrets *)
	GLfloat					max_flight_yaw;
	GLfloat					cruiseSpeed;				// 80% of top speed
	
	GLfloat					max_thrust;					// acceleration
	GLfloat					thrust;						// acceleration
	float					hyperspaceMotorSpinTime;	// duration of hyperspace countdown
	
	unsigned				military_jammer_active: 1,	// military_jammer
	
							docking_match_rotation: 1,
	
							pitching_over: 1,			// set to YES if executing a sharp loop
							rolling_over: 1,			// set to YES if executing a sharp roll
							reportAIMessages: 1,		// normally NO, suppressing AI message reporting
	
							being_mined: 1,				// normally NO, set to Yes when fired on by mining laser
	
							being_fined: 1,
	
							isHulk: 1,					// This is used to distinguish abandoned ships from cargo
							trackCloseContacts: 1,
							isNearPlanetSurface: 1,		// check for landing on planet
							isFrangible: 1,				// frangible => subEntities can be damaged individually
							cloaking_device_active: 1,	// cloaking_device
							cloakPassive: 1,			// cloak deactivates when main weapons or missiles are fired
							cloakAutomatic: 1,			// cloak activates itself automatic during attack
							canFragment: 1,				// Can it break into wreckage?
							isWreckage: 1,              // Is it wreckage?
							_showDamage: 1,             // Show damage?
							suppressExplosion: 1,		// Avoid exploding on death (script hook)
							suppressAegisMessages: 1,	// No script/AI messages sent by -checkForAegis,
							isMissile: 1,				// Whether this was launched by fireMissile (used to track submunitions).
							_explicitlyUnpiloted: 1,	// Is meant to not have crew
							hasScoopMessage: 1,			// suppress scoop messages when false.
							
							// scripting
							scripted_misjump: 1,
							haveExecutedSpawnAction: 1,
							haveStartedJSAI: 1,
							noRocks: 1,
							_lightsActive: 1;

	GLfloat    _scriptedMisjumpRange; 
	
	GLfloat		sunGlareFilter;							// Range 0.0 - 1.0, where 0 means no sun glare filter, 1 means glare fully filtered
	
	OOFuelQuantity			fuel;						// witch-space fuel
	GLfloat					fuel_accumulator;
	
	GLfloat					afterburner_rate;
	GLfloat					afterburner_speed_factor;

	OOCargoQuantity			likely_cargo;				// likely amount of cargo (for pirates, this is what is spilled as loot)
	OOCargoQuantity			max_cargo;					// capacity of cargo hold
	OOCargoQuantity			extra_cargo;				// capacity of cargo hold extension (if any)
	OOCargoQuantity			equipment_weight;			// amount of equipment using cargo space (excluding passenger_berth & extra_cargo_bay)
	OOCargoType				cargo_type;					// if this is scooped, this is indicates contents
	OOCargoFlag				cargo_flag;					// indicates contents for merchantmen
	OOCreditsQuantity		bounty;						// bounty (if any)
	
	GLfloat					energy_recharge_rate;		// recharge rate for energy banks
	
	OOWeaponFacingSet		weapon_facings;				// weapon mounts available (bitmask)
	OOWeaponType			forward_weapon_type;		// type of forward weapon (allows lasers, plasma cannon, others)
	OOWeaponType			aft_weapon_type;			// type of aft weapon (allows lasers, plasma cannon, others)
	OOWeaponType			port_weapon_type;			// type of port weapon
	OOWeaponType			starboard_weapon_type;			// type of starboard weapon
	GLfloat					weapon_damage;				// energy damage dealt by weapon
	GLfloat					weapon_damage_override;		// custom energy damage dealt by front laser, if applicable
	GLfloat					weaponRange;				// range of the weapon (in meters)
	OOWeaponFacing			currentWeaponFacing;		// not necessarily the same as view for the player
	
	GLfloat					weapon_energy_use, weapon_temp, weapon_shot_temperature; // active weapon temp, delta-temp
	GLfloat					forward_weapon_temp, aft_weapon_temp, port_weapon_temp, starboard_weapon_temp; // current weapon temperatures

	GLfloat					scannerRange;				// typically 25600
	
	unsigned				missiles;					// number of on-board missiles
	NSUInteger				max_missiles;				// number of missile pylons
	NSString				*_missileRole;
	OOTimeDelta				missile_load_time;			// minimum time interval between missile launches
	OOTimeAbsolute			missile_launch_time;		// time of last missile launch
	
	AI						*shipAI;					// ship's AI system
	
	NSString				*name;						// descriptive name
	NSString				*shipUniqueName;			// uniqish name e.g. "Terror of Lave"
	NSString				*shipClassName;				// e.g. "Cobra III"
	NSString				*displayName;				// name shown on screen
	NSString				*scan_description;			// scan class name
	OORoleSet				*roleSet;					// Roles a ship can take, eg. trader, hunter, police, pirate, scavenger &c.
	NSString				*primaryRole;				// "Main" role of the ship.

	NSArray 				*explosionType;				// explosion.plist entries
	
	// AI stuff
	Vector					jink;						// x and y set factors for offsetting a pursuing ship's position
	HPVector					coordinates;				// for flying to/from a set point
	Vector					reference;					// a direction vector of magnitude 1 (* turrets *)
	
	NSUInteger				_subIdx;					// serialisation index - used only if this ship is a subentity
	NSUInteger				_maxShipSubIdx;				// serialisation index - the number of ship subentities inside the shipdata
	double					launch_time;				// time at which launched
	double					launch_delay;				// delay for thinking after launch
	OOUniversalID			planetForLanding;			// for landing
	
	GLfloat					frustration,				// degree of dissatisfaction with the current behavioural state, factor used to test this
	success_factor;
	
	int						patrol_counter;				// keeps track of where the ship is along a patrol route
	
	NSMutableDictionary		*previousCondition;			// restored after collision avoidance
	
	// derived variables
	float					weapon_recharge_rate;		// time between shots
	int						shot_counter;				// number of shots fired
	OOTimeAbsolute			cargo_dump_time;			// time cargo was last dumped
	OOTimeAbsolute			last_shot_time;				// time shot was last fired
	
	NSMutableArray			*cargo;						// cargo containers go in here
	
	OOCommodityType			commodity_type;				// type of commodity in a container
	OOCargoQuantity			commodity_amount;			// 1 if unit is TONNES (0), possibly more if precious metals KILOGRAMS (1)
	// or gem stones GRAMS (2)
	
	// navigation
	GLfloat					flightSpeed;				// current speed
	GLfloat					flightRoll;					// current roll rate
	GLfloat					flightPitch;				// current pitch rate
	GLfloat					flightYaw;					// current yaw rate
	
	GLfloat					accuracy;
	GLfloat					pitch_tolerance;
	GLfloat					aim_tolerance;
	int					_missed_shots;
	
	OOAegisStatus			aegis_status;				// set to YES when within the station's protective zone
	OOSystemID				home_system; 
	OOSystemID				destination_system; 
	
	double					messageTime;				// counts down the seconds a radio message is active for
	
	double					next_spark_time;			// time of next spark when throwing sparks
	
	Vector					collision_vector;			// direction of colliding thing.
	
	GLfloat					_scaleFactor;  // scale factor for size variation

	//position of gun ports
	Vector					forwardWeaponOffset,
							aftWeaponOffset,
							portWeaponOffset,
							starboardWeaponOffset;
	
	// crew (typically one OOCharacter - the pilot)
	NSArray					*crew;
	
	// close contact / collision tracking
	NSMutableDictionary		*closeContactsInfo;
	
	NSString				*lastRadioMessage;
	
	// scooping...
	Vector					tractor_position;
	
	// from player entity moved here now we're doing more complex heat stuff
	float					ship_temperature;
	
	// for advanced scanning etc.
	ShipEntity				*scanned_ships[MAX_SCAN_NUMBER + 1];
	GLfloat					distance2_scanned_ships[MAX_SCAN_NUMBER + 1];
	unsigned				n_scanned_ships;
	
	// advanced navigation
	HPVector					navpoints[32];
	unsigned				next_navpoint_index;
	unsigned				number_of_navpoints;
	
	// Collision detection
	Octree					*octree;
	
#ifndef NDEBUG
	// DEBUGGING
	OOBehaviour				debugLastBehaviour;
#endif
	
	uint16_t				entity_personality;			// Per-entity random number. Exposed to shaders and scripts.
	NSDictionary			*scriptInfo;				// script_info dictionary from shipdata.plist, exposed to scripts.
	
	NSMutableArray			*subEntities;
	OOEquipmentType			*missile_list[SHIPENTITY_MAX_MISSILES];

	// various types of target
	OOWeakReference			*_primaryTarget;			// for combat or rendezvous
	OOWeakReference			*_primaryAggressor;			// recorded after attack
	OOWeakReference			*_targetStation;			// for docking
	OOWeakReference			*_foundTarget;				// from scans
	OOWeakReference			*_lastEscortTarget;			// last target an escort was deployed after
	OOWeakReference			*_thankedShip;				// last ship thanked
	OOWeakReference			*_rememberedShip;			// ship being remembered
	OOWeakReference			*_proximityAlert;			// a ShipEntity within 2x collision_radius
	
	// Stuff for the target tracking curve.  The ship records the position of the target every reactionTime/2 seconds, then fits a curve to the
	// last three recorded positions.  Instead of tracking the primary target's actual position it uses the curve to calculate the target's position.
	// This introduces a small amount of lag to the target tracking making the NPC more human.
	float				reactionTime;
	HPVector			trackingCurvePositions[4];
	OOTimeAbsolute			trackingCurveTimes[4];
	HPVector			trackingCurveCoeffs[3];
	

	
@private
	OOWeakReference			*_subEntityTakingDamage;	//	frangible => subEntities can be damaged individually

	NSString				*_shipKey;
	
	NSMutableArray			*_equipment;
	float					_heatInsulation;
	
	OOWeakReference			*_lastAegisLock;			// remember last aegis planet/sun
	
	OOShipGroup				*_group;
	OOShipGroup				*_escortGroup;
	uint8_t					_maxEscortCount;
	uint8_t					_pendingEscortCount;
	// Cache of ship-relative positions, managed by -coordinatesForEscortPosition:.
	Vector					_escortPositions[MAX_ESCORTS];
	BOOL					_escortPositionsValid;
	
	OOWeakSet				*_defenseTargets;			 // defense targets
	
	// ships in this set can't be collided with
	OOWeakSet				*_collisionExceptions;

	GLfloat					_profileRadius;
	
	OOWeakReference			*_shipHitByLaser;			// entity hit by the last laser shot
	
	// beacons
	NSString				*_beaconCode;
	NSString				*_beaconLabel;
	OOWeakReference			*_prevBeacon;
	OOWeakReference			*_nextBeacon;
	id <OOHUDBeaconIcon>	_beaconDrawable;

	double			_nextAegisCheck;
	
}

// ship brains
- (void) setStateMachine:(NSString *)ai_desc;
@property (getter=getAI, strong) AI *AI;
@property (readonly) BOOL hasAutoAI;
@property (readonly) BOOL hasNewAI;
- (void) setShipScript:(NSString *)script_name;
- (void) removeScript;
- (OOScript *) shipScript;
@property (readonly, strong) OOScript *shipAIScript;
@property (readonly) OOTimeAbsolute shipAIScriptWakeTime;
- (void) setAIScriptWakeTime:(OOTimeAbsolute) t;
@property (readonly) double frustration;
- (void) setLaunchDelay:(double)delay;

- (void) interpretAIMessage:(NSString *)message;

@property  GLfloat accuracy;

@property (copy) OOMesh *mesh;

@property (readonly) BoundingBox totalBoundingBox;

@property (readonly) Vector forwardVector;
@property (readonly) Vector upVector;
@property (readonly) Vector rightVector;

@property (readonly, copy) NSArray *subEntities;
@property (readonly) NSUInteger subEntityCount;
- (BOOL) hasSubEntity:(Entity<OOSubEntity> *)sub;

@property (readonly, strong) NSEnumerator *subEntityEnumerator;
@property (readonly, strong) NSEnumerator *shipSubEntityEnumerator;
@property (readonly, strong) NSEnumerator *flasherEnumerator;
@property (readonly, strong) NSEnumerator *exhaustEnumerator;

@property (strong) ShipEntity *subEntityTakingDamage;

- (void) clearSubEntities;	// Releases and clears subentity array, after making sure subentities don't think ship is owner.

@property  Quaternion subEntityRotationalVelocity;

// subentities management
@property (readonly, copy) NSString *serializeShipSubEntities;
- (void) deserializeShipSubEntitiesFrom:(NSString *)string;
@property (readonly) NSUInteger maxShipSubEntities;
@property  NSUInteger subIdx;

@property (readonly, strong) Octree *octree;
@property (readonly) float volume;

// octree collision hunting
- (GLfloat)doesHitLine:(HPVector)v0 :(HPVector)v1;
- (GLfloat)doesHitLine:(HPVector)v0 :(HPVector)v1 :(ShipEntity**)hitEntity;
- (GLfloat)doesHitLine:(HPVector)v0 :(HPVector)v1 withPosition:(HPVector)o andIJK:(Vector)i :(Vector)j :(Vector)k;	// for subentities

- (BoundingBox) findBoundingBoxRelativeToPosition:(HPVector)opv InVectors:(Vector)i :(Vector)j :(Vector)k;

@property (readonly) HPVector absoluteTractorPosition;

// beacons // definitions now in <OOBeaconEntity> protocol

@property  BOOL isBoulder;
@property (getter=isMinable, readonly) BOOL minable;

@property (readonly) BOOL countsAsKill;

- (void) setUpEscorts;
- (void) updateEscortFormation;

- (instancetype)initWithKey:(NSString *)key definition:(NSDictionary *)dict NS_DESIGNATED_INITIALIZER;
- (BOOL)setUpFromDictionary:(NSDictionary *) shipDict;
- (BOOL)setUpShipFromDictionary:(NSDictionary *) shipDict;
@property (readonly) BOOL setUpSubEntities;
- (BOOL) setUpOneStandardSubentity:(NSDictionary *) subentDict asTurret:(BOOL)asTurret;
@property (readonly) GLfloat frustumRadius;

@property (copy) NSString *shipDataKey;
@property (readonly, copy) NSString *shipDataKeyAutoRole;

@property (readonly, copy) NSDictionary *shipInfoDictionary;

- (void) setDefaultWeaponOffsets;
@property (readonly) Vector aftWeaponOffset;
@property (readonly) Vector forwardWeaponOffset;
@property (readonly) Vector portWeaponOffset;
@property (readonly) Vector starboardWeaponOffset;
@property (readonly) BOOL hasAutoWeapons;

@property (getter=isFrangible, readonly) BOOL frangible;
@property (readonly) BOOL suppressFlightNotifications;

- (void) respondToAttackFrom:(Entity *)from becauseOf:(Entity *)other;

// Equipment
@property (readonly) OOWeaponFacingSet weaponFacings;
- (BOOL) hasEquipmentItem:(id)equipmentKeys includeWeapons:(BOOL)includeWeapons whileLoading:(BOOL)loading;	// This can take a string or an set or array of strings. If a collection, returns YES if ship has _any_ of the specified equipment. If includeWeapons is NO, missiles and primary weapons are not checked.
- (BOOL) hasEquipmentItem:(id)equipmentKeys;			// Short for hasEquipmentItem:foo includeWeapons:NO whileLoading:NO
- (NSUInteger) countEquipmentItem:(NSString *)eqkey;
- (NSString *) equipmentItemProviding:(NSString *)equipmentType;
- (BOOL) hasEquipmentItemProviding:(NSString *)equipmentType;
- (BOOL) hasAllEquipment:(id)equipmentKeys includeWeapons:(BOOL)includeWeapons whileLoading:(BOOL)loading;		// Like hasEquipmentItem:includeWeapons:, but requires _all_ elements in collection.
- (BOOL) hasAllEquipment:(id)equipmentKeys;				// Short for hasAllEquipment:foo includeWeapons:NO
- (BOOL) setWeaponMount:(OOWeaponFacing)facing toWeapon:(NSString *)eqKey;
- (BOOL) canAddEquipment:(NSString *)equipmentKey inContext:(NSString *)context;		// Test ability to add equipment, taking equipment-specific constriants into account. 
- (BOOL) equipmentValidToAdd:(NSString *)equipmentKey inContext:(NSString *)context;	// Actual test if equipment satisfies validation criteria.
- (BOOL) equipmentValidToAdd:(NSString *)equipmentKey whileLoading:(BOOL)loading inContext:(NSString *)context;
- (BOOL) addEquipmentItem:(NSString *)equipmentKey inContext:(NSString *)context;
- (BOOL) addEquipmentItem:(NSString *)equipmentKey withValidation:(BOOL)validateAddition inContext:(NSString *)context;
@property (readonly) BOOL hasHyperspaceMotor;
@property  float hyperspaceSpinTime;


@property (readonly, strong) NSEnumerator *equipmentEnumerator;
@property (readonly) NSUInteger equipmentCount;
- (void) removeEquipmentItem:(NSString *)equipmentKey;
- (void) removeAllEquipment;
@property (readonly, copy) OOEquipmentType *selectMissile;
@property (readonly) OOCreditsQuantity removeMissiles;

// Internal, subject to change. Use the methods above instead.
- (BOOL) hasOneEquipmentItem:(NSString *)itemKey includeWeapons:(BOOL)includeMissiles whileLoading:(BOOL)loading;
- (BOOL) hasOneEquipmentItem:(NSString *)itemKey includeMissiles:(BOOL)includeMissiles whileLoading:(BOOL)loading;
- (BOOL) hasPrimaryWeapon:(OOWeaponType)weaponType;
- (BOOL) removeExternalStore:(OOEquipmentType *)eqType;

// Passengers and parcels - not supported for NPCs, but interface is here for genericity.
@property (readonly) NSUInteger parcelCount;
@property (readonly) NSUInteger passengerCount;
@property (readonly) NSUInteger passengerCapacity;

@property (readonly) NSUInteger missileCount;
@property (readonly) NSUInteger missileCapacity;

@property (readonly) NSUInteger extraCargo;

// Tests for the various special-cased equipment items
// (Nowadays, more convenience methods)
@property (readonly) BOOL hasScoop;
@property (readonly) BOOL hasFuelScoop;
@property (readonly) BOOL hasCargoScoop;
@property (readonly) BOOL hasECM;
@property (readonly) BOOL hasCloakingDevice;
@property (readonly) BOOL hasMilitaryScannerFilter;
@property (readonly) BOOL hasMilitaryJammer;
@property (readonly) BOOL hasExpandedCargoBay;
@property (readonly) BOOL hasShieldBooster;
@property (readonly) BOOL hasMilitaryShieldEnhancer;
@property (readonly) BOOL hasHeatShield;
@property (readonly) BOOL hasFuelInjection;
@property (readonly) BOOL hasCascadeMine;
@property (readonly) BOOL hasEscapePod;
@property (readonly) BOOL hasDockingComputer;
@property (readonly) BOOL hasGalacticHyperdrive;

// Shield information derived from equipment. NPCs can't have shields, but that should change at some point.
@property (readonly) float shieldBoostFactor;
@property (readonly) float maxForwardShieldLevel;
@property (readonly) float maxAftShieldLevel;
@property (readonly) float shieldRechargeRate;

@property (readonly) float maxHyperspaceDistance;
@property  float afterburnerFactor;
@property  float afterburnerRate;
@property  float maxThrust;
- (float) thrust;



- (void) processBehaviour:(OOTimeDelta)delta_t;
// Behaviours
- (void) behaviour_stop_still:(double) delta_t;
- (void) behaviour_idle:(double) delta_t;
- (void) behaviour_tumble:(double) delta_t;
- (void) behaviour_tractored:(double) delta_t;
- (void) behaviour_track_target:(double) delta_t;
- (void) behaviour_intercept_target:(double) delta_t;
- (void) behaviour_attack_target:(double) delta_t;
- (void) behaviour_attack_slow_dogfight:(double) delta_t;
- (void) behaviour_evasive_action:(double) delta_t;
- (void) behaviour_attack_break_off_target:(double) delta_t;
- (void) behaviour_fly_to_target_six:(double) delta_t;
- (void) behaviour_attack_mining_target:(double) delta_t;
- (void) behaviour_attack_fly_to_target:(double) delta_t;
- (void) behaviour_attack_fly_from_target:(double) delta_t;
- (void) behaviour_running_defense:(double) delta_t;
- (void) behaviour_flee_target:(double) delta_t;
- (void) behaviour_attack_broadside:(double) delta_t;
- (void) behaviour_attack_broadside_left:(double) delta_t;
- (void) behaviour_attack_broadside_right:(double) delta_t;
- (void) behaviour_close_to_broadside_range:(double) delta_t;
- (void) behaviour_close_with_target:(double) delta_t;
- (void) behaviour_attack_broadside_target:(double) delta_t leftside:(BOOL)leftside;
- (void) behaviour_attack_sniper:(double) delta_t;
- (void) behaviour_fly_range_from_destination:(double) delta_t;
- (void) behaviour_face_destination:(double) delta_t;
- (void) behaviour_land_on_planet:(double) delta_t;
- (void) behaviour_formation_form_up:(double) delta_t;
- (void) behaviour_fly_to_destination:(double) delta_t;
- (void) behaviour_fly_from_destination:(double) delta_t;
- (void) behaviour_avoid_collision:(double) delta_t;
- (void) behaviour_track_as_turret:(double) delta_t;
- (void) behaviour_fly_thru_navpoints:(double) delta_t;
- (void) behaviour_scripted_ai:(double) delta_t;

@property  float reactionTime;
@property (readonly) HPVector calculateTargetPosition;
- (void) startTrackingCurve;
- (void) updateTrackingCurve;
- (void) calculateTrackingCurve;

- (GLfloat *) scannerDisplayColorForShip:(ShipEntity*)otherShip :(BOOL)isHostile :(BOOL)flash :(OOColor *)scannerDisplayColor1 :(OOColor *)scannerDisplayColor2 :(OOColor *)scannerDisplayColorH1 :(OOColor *)scannerDisplayColorH2 NS_RETURNS_INNER_POINTER;
@property (copy) OOColor *scannerDisplayColor1;
@property (copy) OOColor *scannerDisplayColor2;
@property (copy) OOColor *scannerDisplayColorHostile1;
@property (copy) OOColor *scannerDisplayColorHostile2;

@property (getter=isCloaked) BOOL cloaked;
@property (readonly) BOOL hasAutoCloak;
- (void)setAutoCloak:(BOOL)automatic;

- (void) applyThrust:(double) delta_t;
- (void) applyAttitudeChanges:(double) delta_t;

- (void) avoidCollision;
- (void) resumePostProximityAlert;

@property  double messageTime;

@property (strong) OOShipGroup *group;

@property (strong) OOShipGroup *escortGroup;	// Only for use in unconventional set-up situations.

@property (readonly, strong) OOShipGroup *stationGroup; // should probably be defined in stationEntity.m

@property (readonly) BOOL hasEscorts;
@property (readonly, strong) NSEnumerator *escortEnumerator;
@property (readonly, copy) NSArray *escortArray;

@property (readonly) uint8_t escortCount;

// Pending escort count: number of escorts to set up "later".
@property  uint8_t pendingEscortCount;

// allow adjustment of escort numbers from shipdata.plist levels
@property  uint8_t maxEscortCount;

@property (copy) NSString *name;
@property (copy) NSString *shipUniqueName;
@property (copy) NSString *shipClassName;
@property (copy) NSString *displayName;
@property (copy) NSString *scanDescription;
@property (readonly, copy) NSString *scanDescriptionForScripting;
- (NSString *) identFromShip:(ShipEntity*) otherShip; // name displayed to other ships

- (BOOL) hasRole:(NSString *)role;
@property (readonly, strong) OORoleSet *roleSet;

- (void) addRole:(NSString *)role;
- (void) addRole:(NSString *)role withProbability:(float)probability;
- (void) removeRole:(NSString *)role;

@property (copy) NSString *primaryRole;
- (BOOL)hasPrimaryRole:(NSString *)role;

@property (getter=isPolice, readonly) BOOL police;		// Scan class is CLASS_POLICE
@property (getter=isThargoid, readonly) BOOL thargoid;		// Scan class is CLASS_THARGOID
@property (getter=isTrader, readonly) BOOL trader;		// Primary role is "trader" || isPlayer
@property (getter=isPirate, readonly) BOOL pirate;		// Primary role is "pirate"
@property (getter=isMissile, readonly) BOOL missile;		// Primary role has suffix "MISSILE"
@property (getter=isMine, readonly) BOOL mine;			// Primary role has suffix "MINE"
@property (getter=isWeapon, readonly) BOOL weapon;		// isMissile || isWeapon
@property (getter=isEscort, readonly) BOOL escort;		// Primary role is "escort" or "wingman"
@property (getter=isShuttle, readonly) BOOL shuttle;		// Primary role is "shuttle"
@property (getter=isTurret, readonly) BOOL turret;		// Behaviour is BEHAVIOUR_TRACK_AS_TURRET
@property (getter=isPirateVictim, readonly) BOOL pirateVictim;	// Primary role is listed in pirate-victim-roles.plist
@property (getter=isExplicitlyUnpiloted, readonly) BOOL explicitlyUnpiloted; // Has unpiloted = yes in its shipdata.plist entry
@property (getter=isUnpiloted, readonly) BOOL unpiloted;	// Explicitly unpiloted, hulk, rock, cargo, debris etc; an open-ended criterion that may grow.

@property (readonly) OOAlertCondition alertCondition; // quick calc for shaders
@property (readonly) OOAlertCondition realAlertCondition; // full calculation for scripting
@property (readonly) BOOL hasHostileTarget;
- (BOOL) isHostileTo:(Entity *)entity;

// defense target handling
@property (readonly) NSUInteger defenseTargetCount;
@property (readonly, copy) NSArray *allDefenseTargets;
@property (readonly, strong) NSEnumerator *defenseTargetEnumerator;
- (void) validateDefenseTargets;
- (BOOL) addDefenseTarget:(Entity *)target;
- (BOOL) isDefenseTarget:(Entity *)target;
- (void) removeDefenseTarget:(Entity *)target;
- (void) removeAllDefenseTargets;

// collision exceptions
@property (readonly, copy) NSArray *collisionExceptions;
- (void) addCollisionException:(ShipEntity *)ship;
- (void) removeCollisionException:(ShipEntity *)ship;
- (BOOL) collisionExceptedFor:(ShipEntity *)ship;



@property  GLfloat weaponRange;
- (void) setWeaponDataFromType:(OOWeaponType)weapon_type;
@property  float energyRechargeRate; // final rate after energy units
@property  float weaponRechargeRate;
- (void) setWeaponEnergy:(float)value;
@property (readonly) OOWeaponFacing currentWeaponFacing;

@property  GLfloat scannerRange;

@property  Vector reference;

@property  BOOL reportAIMessages;

- (void) transitionToAegisNone;
@property (readonly, strong) OOPlanetEntity *findNearestPlanet;
@property (readonly, strong) Entity<OOStellarBody> *findNearestStellarBody;		// NOTE: includes sun.
@property (readonly, strong) OOPlanetEntity *findNearestPlanetExcludingMoons;
@property (readonly) OOAegisStatus checkForAegis;
- (void) forceAegisCheck;
@property (readonly) BOOL withinStationAegis;
- (void) setLastAegisLock:(Entity<OOStellarBody> *)lastAegisLock;

@property  OOSystemID homeSystem;
@property  OOSystemID destinationSystem;


@property (copy) NSArray *crew;
@property (readonly, copy) NSArray *crewForScripting;
/**
	Convenience to set the crew to a single character of the given role,
	originating in the ship's home system. Does nothing if unpiloted.
 */
- (void) setSingleCrewWithRole:(NSString *)crewRole;

// Fuel and capacity in tenths of light-years.
@property  OOFuelQuantity fuel;
@property (readonly) OOFuelQuantity fuelCapacity;

@property (readonly) GLfloat fuelChargeRate;

- (void) setRoll:(double)amount;
- (void) setRawRoll:(double)amount; // does not multiply by PI/2
- (void) setPitch:(double)amount;
- (void) setYaw:(double)amount;
- (void) setThrust:(double)amount;
- (void) applySticks:(double)delta_t;


- (void)setThrustForDemo:(float)factor;

/*
 Sets the bounty on this ship to amount.  
 Does not check to see if the ship is allowed to have a bounty, for example if it is police.
 */
- (void) setBounty:(OOCreditsQuantity)amount withReason:(OOLegalStatusReason)reason;
- (void) setBounty:(OOCreditsQuantity)amount withReasonAsString:(NSString *)reason;
@property  OOCreditsQuantity bounty;

@property (readonly) int legalStatus;

@property (getter=isTemplateCargoPod, readonly) BOOL templateCargoPod;
- (void) setUpCargoType:(NSString *)cargoString;
- (void) setCommodity:(OOCommodityType)co_type andAmount:(OOCargoQuantity)co_amount;
- (void) setCommodityForPod:(OOCommodityType)co_type andAmount:(OOCargoQuantity)co_amount;
@property (readonly, copy) OOCommodityType commodityType;
@property (readonly) OOCargoQuantity commodityAmount;

@property  OOCargoQuantity maxAvailableCargoSpace;
@property (readonly) OOCargoQuantity availableCargoSpace;
@property (readonly) OOCargoQuantity cargoQuantityOnBoard;
@property (readonly) OOCargoType cargoType;
@property (readonly, copy) NSArray *cargoListForScripting;
- (NSMutableArray *) cargo;
- (void) setCargo:(NSArray *)some_cargo;
- (BOOL) addCargo:(NSArray *) some_cargo;
- (BOOL) removeCargo:(OOCommodityType)commodity amount:(OOCargoQuantity) amount;
@property (readonly) BOOL showScoopMessage;

@property (readonly, copy) NSArray *passengerListForScripting;
@property (readonly, copy) NSArray *parcelListForScripting;
@property (readonly, copy) NSArray *contractListForScripting;
@property (readonly, copy) NSArray *equipmentListForScripting;
- (OOWeaponType) weaponTypeIDForFacing:(OOWeaponFacing)facing strict:(BOOL)strict;
- (OOEquipmentType *) weaponTypeForFacing:(OOWeaponFacing)facing strict:(BOOL)strict;
@property (readonly, copy) NSArray *missilesList;

@property  OOCargoFlag cargoFlag;

- (void) setSpeed:(double)amount;
@property  double desiredSpeed;
@property  double desiredRange;

@property (readonly) double cruiseSpeed;

@property (readonly) Vector thrustVector;
- (void) setTotalVelocity:(Vector)vel;	// Set velocity to vel - thrustVector, effectively setting the instanteneous velocity to vel.

- (void) increase_flight_speed:(double)delta;
- (void) decrease_flight_speed:(double)delta;
- (void) increase_flight_roll:(double)delta;
- (void) decrease_flight_roll:(double)delta;
- (void) increase_flight_pitch:(double)delta;
- (void) decrease_flight_pitch:(double)delta;
- (void) increase_flight_yaw:(double)delta;
- (void) decrease_flight_yaw:(double)delta;

@property (readonly) GLfloat flightRoll;
@property (readonly) GLfloat flightPitch;
@property (readonly) GLfloat flightYaw;
@property (readonly) GLfloat flightSpeed;
@property  GLfloat maxFlightPitch;
@property  GLfloat maxFlightSpeed;
@property  GLfloat maxFlightRoll;
@property  GLfloat maxFlightYaw;
@property (readonly) GLfloat speedFactor;

@property  GLfloat temperature;
@property  GLfloat heatInsulation;

@property (readonly) float randomEjectaTemperature;
- (float) randomEjectaTemperatureWithMaxFactor:(float)factor;

// the percentage of damage taken (100 is destroyed, 0 is fine)
@property (readonly) int damage;

- (void) dealEnergyDamage:(GLfloat) baseDamage atRange:(GLfloat) range withBias:(GLfloat) velocityBias;
- (void) dealEnergyDamageWithinDesiredRange;
- (void) dealMomentumWithinDesiredRange:(double)amount;

// Dispatch shipTakingDamage() event.
- (void) noteTakingDamage:(double)amount from:(Entity *)entity type:(OOShipDamageType)type;
// Dispatch shipDied() and possibly shipKilledOther() events. This is only for use by getDestroyedBy:damageType:, but needs to be visible to PlayerEntity's version.
- (void) noteKilledBy:(Entity *)whom damageType:(OOShipDamageType)type;

- (void) getDestroyedBy:(Entity *)whom damageType:(OOShipDamageType)type;
- (void) becomeExplosion;
- (void) becomeLargeExplosion:(double) factor;
- (void) becomeEnergyBlast;
- (void) broadcastEnergyBlastImminent;
- (void) setIsWreckage:(BOOL)isw;
@property (readonly) BOOL showDamage;

- (Vector) positionOffsetForAlignment:(NSString*) align;
Vector positionOffsetForShipInRotationToAlignment(ShipEntity* ship, Quaternion q, NSString* align);

- (void) collectBountyFor:(ShipEntity *)other;

@property (readonly) BoundingBox findSubentityBoundingBox;

@property (readonly) Triangle absoluteIJKForSubentity;

@property (readonly) GLfloat weaponRecoveryTime;
@property (readonly) GLfloat laserHeatLevel;
@property (readonly) GLfloat laserHeatLevelAft;
@property (readonly) GLfloat laserHeatLevelForward;
@property (readonly) GLfloat laserHeatLevelPort;
@property (readonly) GLfloat laserHeatLevelStarboard;
@property (readonly) GLfloat hullHeatLevel;
@property (readonly) GLfloat entityPersonality;
- (GLint)entityPersonalityInt;
- (void) setEntityPersonalityInt:(uint16_t)value;

- (void)setSuppressExplosion:(BOOL)suppress;

- (void) resetExhaustPlumes;

- (void) removeExhaust:(OOExhaustPlumeEntity *)exhaust;
- (void) removeFlasher:(OOFlasherEntity *)flasher;


/*-----------------------------------------
 
 AI piloting methods
 
 -----------------------------------------*/

- (void) checkScanner;
- (void) checkScannerIgnoringUnpowered;
@property (readonly) ShipEntity **scannedShips;
@property (readonly) int numberOfScannedShips;

@property (strong) Entity *foundTarget;
@property (strong) Entity *primaryAggressor;
@property (strong) Entity *lastEscortTarget;
@property (strong) Entity *thankedShip;
@property (strong) Entity *rememberedShip;
- (Entity *)proximityAlert;
- (void) setProximityAlert:(ShipEntity *) targetEntity;
- (void) setTargetStation:(Entity *) targetEntity;
- (BOOL) isValidTarget:(Entity *) target;
- (void) addTarget:(Entity *) targetEntity;
- (void) removeTarget:(Entity *) targetEntity;
@property (readonly) BOOL canStillTrackPrimaryTarget;
@property (readonly, strong) id primaryTarget;
@property (readonly, strong) id primaryTargetWithoutValidityCheck;
- (StationEntity *) targetStation;

- (BOOL) isFriendlyTo:(ShipEntity *)otherShip;

@property (readonly, strong) ShipEntity *shipHitByLaser;

- (void) noteLostTarget;
- (void) noteLostTargetAndGoIdle;
- (void) noteTargetDestroyed:(ShipEntity *)target;

@property  OOBehaviour behaviour;

- (void) trackOntoTarget:(double) delta_t withDForward: (GLfloat) dp;

- (double) ballTrackLeadingTarget:(double) delta_t atTarget:(Entity *)target;

- (GLfloat) rollToMatchUp:(Vector) up_vec rotating:(GLfloat) match_roll;

@property (readonly) GLfloat rangeToDestination;
- (double) trackDestination:(double) delta_t :(BOOL) retreat;

- (void) setCoordinate:(HPVector)coord;
@property (readonly) HPVector coordinates;
@property  HPVector destination;
- (HPVector) distance_six: (GLfloat) dist;
- (HPVector) distance_twelve: (GLfloat) dist withOffset:(GLfloat)offset;

- (void) setEvasiveJink:(GLfloat) z;
- (void) evasiveAction:(double) delta_t;
- (double) trackPrimaryTarget:(double) delta_t :(BOOL) retreat;
- (double) trackSideTarget:(double) delta_t :(BOOL) leftside;
- (double) missileTrackPrimaryTarget:(double) delta_t;

//return 0.0 if there is no primary target
@property (readonly) double rangeToPrimaryTarget;
@property (readonly) double approachAspectToPrimaryTarget;
- (double) rangeToSecondaryTarget:(Entity *)target;
- (BOOL) hasProximityAlertIgnoringTarget:(BOOL)ignore_target;
@property (readonly) GLfloat currentAimTolerance;
/* This method returns a value between 0.0f and 1.0f, depending on how directly our view point
   faces the sun and is used for generating the "staring at the sun" glare effect. 0.0f means that
   we are not facing the sun, 1.0f means that we are looking directly at it. The cosine of the 
   threshold angle between view point and sun, below which we consider the ship as looking
   at the sun, is passed as parameter to the method.
*/
- (GLfloat) lookingAtSunWithThresholdAngleCos:(GLfloat) thresholdAngleCos;

- (BOOL) onTarget:(OOWeaponFacing)direction withWeapon:(OOWeaponType)weapon;

@property (readonly) OOTimeDelta shotTime;
- (void) resetShotTime;

- (BOOL) fireMainWeapon:(double)range;
- (BOOL) fireAftWeapon:(double)range;
- (BOOL) firePortWeapon:(double)range;
- (BOOL) fireStarboardWeapon:(double)range;
- (BOOL) fireTurretCannon:(double)range;
@property (copy) OOColor *laserColor;
@property (copy) OOColor *exhaustEmissiveColor;
- (BOOL) fireSubentityLaserShot:(double)range;
- (BOOL) fireDirectLaserShot:(double)range;
@property (readonly) BOOL fireDirectLaserDefensiveShot;
- (BOOL) fireDirectLaserShotAt:(Entity *)my_target;
- (Vector) laserPortOffset:(OOWeaponFacing)direction;
- (BOOL) fireLaserShotInDirection:(OOWeaponFacing)direction;
- (void) adjustMissedShots:(int)delta;
@property (readonly) int missedShots;
- (BOOL) firePlasmaShotAtOffset:(double)offset speed:(double)speed color:(OOColor *)color;
- (void) considerFiringMissile:(double)delta_t;
@property (readonly) Vector missileLaunchPosition;
@property (readonly, strong) ShipEntity *fireMissile;
- (ShipEntity *) fireMissileWithIdentifier:(NSString *) identifier andTarget:(Entity *) target;
@property (getter=isMissileFlagSet, readonly) BOOL missileFlagSet;
- (void) setIsMissileFlag:(BOOL)newValue;
@property  OOTimeDelta missileLoadTime;
- (void) noticeECM;
@property (readonly) BOOL fireECM;
- (BOOL) cascadeIfAppropriateWithDamageAmount:(double)amount cascadeOwner:(Entity *)owner;
@property (readonly) BOOL activateCloakingDevice;
- (void) deactivateCloakingDevice;
@property (readonly) BOOL launchCascadeMine;
@property (readonly, strong) ShipEntity *launchEscapeCapsule;
@property (readonly, copy) OOCommodityType dumpCargo;
- (ShipEntity *) dumpCargoItem:(OOCommodityType)preferred;
- (OOCargoType) dumpItem: (ShipEntity*) jetto;

- (void) manageCollisions;
- (BOOL) collideWithShip:(ShipEntity *)other;
- (void) adjustVelocity:(Vector) xVel;
- (void) addImpactMoment:(Vector) moment fraction:(GLfloat) howmuch;
- (BOOL) canScoop:(ShipEntity *)other;
- (void) getTractoredBy:(ShipEntity *)other;
- (void) scoopIn:(ShipEntity *)other;
- (void) scoopUp:(ShipEntity *)other;

@property (readonly) BOOL abandonShip;

- (void) takeScrapeDamage:(double) amount from:(Entity *) ent;
- (void) takeHeatDamage:(double) amount;

- (void) enterDock:(StationEntity *)station;
- (void) leaveDock:(StationEntity *)station;

- (void) enterWormhole:(WormholeEntity *) w_hole;
- (void) enterWormhole:(WormholeEntity *) w_hole replacing:(BOOL)replacing;
- (void) enterWitchspace;
- (void) leaveWitchspace;
@property (readonly) BOOL witchspaceLeavingEffects;

/* 
 Mark this ship as an offender, this is different to setBounty as some ships such as police 
 are not markable.  The final bounty may not be equal to existing bounty plus offence_value.
 */
- (void) markAsOffender:(int)offence_value;
- (void) markAsOffender:(int)offence_value withReason:(OOLegalStatusReason)reason;

- (void) switchLightsOn;
- (void) switchLightsOff;
@property (readonly) BOOL lightsActive;

- (void) setEscortDestination:(HPVector) dest;

- (BOOL) canAcceptEscort:(ShipEntity *)potentialEscort;
- (BOOL) acceptAsEscort:(ShipEntity *) other_ship;
- (void) deployEscorts;
- (void) dockEscorts;

- (void) setTargetToNearestFriendlyStation;
- (void) setTargetToNearestStation;
- (void) setTargetToSystemStation;

- (void) landOnPlanet:(OOPlanetEntity *)planet;

- (void) abortDocking;
@property (readonly, copy) NSDictionary *dockingInstructions;

- (void) broadcastThargoidDestroyed;

- (void) broadcastHitByLaserFrom:(ShipEntity*) aggressor_ship;

// Sun glare filter - 0 for no filter, 1 for full filter
@property  GLfloat sunGlareFilter;

// Unpiloted ships cannot broadcast messages, unless the unpilotedOverride is set to YES.
- (void) sendExpandedMessage:(NSString *) message_text toShip:(ShipEntity*) other_ship;
- (void) sendMessage:(NSString *) message_text toShip:(ShipEntity*) other_ship withUnpilotedOverride:(BOOL)unpilotedOverride;
- (void) broadcastAIMessage:(NSString *) ai_message;
- (void) broadcastMessage:(NSString *) message_text withUnpilotedOverride:(BOOL) unpilotedOverride;
- (void) setCommsMessageColor;
- (void) receiveCommsMessage:(NSString *) message_text from:(ShipEntity *) other;
- (void) commsMessage:(NSString *)valueString withUnpilotedOverride:(BOOL)unpilotedOverride;

@property (readonly) BOOL markedForFines;
@property (readonly) BOOL markForFines;

@property (getter=isMining, readonly) BOOL mining;

- (void) spawn:(NSString *)roles_number;

@property (readonly) int checkShipsInVicinityForWitchJumpExit;

@property  BOOL trackCloseContacts;

/*
 * Changes a ship to a hulk, for example when the pilot ejects.
 * Aso unsets hulkiness for example when a new pilot gets in.
 */
@property (getter=isHulk) BOOL hulk;
#if OO_SALVAGE_SUPPORT
- (void) claimAsSalvage;
- (void) sendCoordinatesToPilot;
- (void) pilotArrived;
#endif

@property (readonly, strong) OOJSScript *script;
@property (readonly, copy) NSDictionary *scriptInfo;
- (void) overrideScriptInfo:(NSDictionary *)override;	// Add items from override (if not nil) to scriptInfo, replacing in case of duplicates. Used for subentities.

@property  BOOL scriptedMisjump;
@property  GLfloat scriptedMisjumpRange;


@property (readonly, strong) Entity *entityForShaderProperties;

/*	*** Script events.
	For NPC ships, these call doEvent: on the ship script.
	For the player, they do that and also call doWorldScriptEvent:.
*/
- (void) doScriptEvent:(jsid)message;
- (void) doScriptEvent:(jsid)message withArgument:(id)argument;
- (void) doScriptEvent:(jsid)message withArgument:(id)argument1 andArgument:(id)argument2;
- (void) doScriptEvent:(jsid)message withArguments:(NSArray *)arguments;
- (void) doScriptEvent:(jsid)message withArguments:(jsval *)argv count:(uintN)argc;
- (void) doScriptEvent:(jsid)message inContext:(JSContext *)context withArguments:(jsval *)argv count:(uintN)argc;

/*	Convenience to send an event with raw JS values, for example:
	ShipScriptEventNoCx(ship, "doSomething", INT_TO_JSVAL(42));
*/
#define ShipScriptEvent(context, ship, event, ...) do { \
jsval argv[] = { __VA_ARGS__ }; \
uintN argc = sizeof argv / sizeof *argv; \
[ship doScriptEvent:OOJSID(event) inContext:context withArguments:argv count:argc]; \
} while (0)

#define ShipScriptEventNoCx(ship, event, ...) do { \
jsval argv[] = { __VA_ARGS__ }; \
uintN argc = sizeof argv / sizeof *argv; \
[ship doScriptEvent:OOJSID(event) withArguments:argv count:argc]; \
} while (0)

- (void) reactToAIMessage:(NSString *)message context:(NSString *)debugContext;	// Immediate message
- (void) sendAIMessage:(NSString *)message;		// Queued message
- (void) doScriptEvent:(jsid)scriptEvent andReactToAIMessage:(NSString *)aiMessage;
- (void) doScriptEvent:(jsid)scriptEvent withArgument:(id)argument andReactToAIMessage:(NSString *)aiMessage;

@end


#ifndef NDEBUG
@interface ShipEntity (Debug)

@property (readonly, strong) OOShipGroup *rawEscortGroup;

@end
#endif


@interface Entity (SubEntityRelationship)

/*	For the common case of testing whether foo is a ship, bar is a ship, bar
	is a subentity of foo and this relationship is represented sanely.
*/
- (BOOL) isShipWithSubEntityShip:(Entity *)other;

@end


NSDictionary *OODefaultShipShaderMacros(void);

GLfloat getWeaponRangeFromType(OOWeaponType weapon_type);

// Defined in OOConstToString.m
NSString *OOStringFromBehaviour(OOBehaviour behaviour) CONST_FUNC;

// Weapon strings prefixed with EQ_, used in shipyard.plist.
NSString *OOEquipmentIdentifierFromWeaponType(OOWeaponType weapon) CONST_FUNC;
OOWeaponType OOWeaponTypeFromEquipmentIdentifierSloppy(NSString *string) PURE_FUNC;	// Uses suffix match for backwards compatibility.
OOWeaponType OOWeaponTypeFromEquipmentIdentifierStrict(NSString *string) PURE_FUNC;
OOWeaponType OOWeaponTypeFromEquipmentIdentifierLegacy(NSString *string);


NSString *OOStringFromWeaponType(OOWeaponType weapon) CONST_FUNC;
OOWeaponType OOWeaponTypeFromString(NSString *string) PURE_FUNC;

BOOL isWeaponNone(OOWeaponType weapon);

NSString *OODisplayStringFromAlertCondition(OOAlertCondition alertCondition);

NSString *OOStringFromShipDamageType(OOShipDamageType type) CONST_FUNC;

