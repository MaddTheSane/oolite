/*

Entity.h

Base class for entities, i.e. drawable world objects.

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


#import "OOCocoa.h"
#import "OOMaths.h"
#import "OOCacheManager.h"
#import "OOTypes.h"
#import "OOWeakReference.h"

@class Universe, CollisionRegion, ShipEntity, OOVisualEffectEntity;


#ifndef NDEBUG

extern uint32_t gLiveEntityCount;
extern size_t gTotalEntityMemory;

#endif


#define NO_DRAW_DISTANCE_FACTOR		1024.0
#define ABSOLUTE_NO_DRAW_DISTANCE2	(2500.0 * 2500.0 * NO_DRAW_DISTANCE_FACTOR * NO_DRAW_DISTANCE_FACTOR)
// ie. the furthest away thing we can draw is at 1280km (a 2.5km wide object would disappear at that range)


#define SCANNER_MAX_RANGE			25600.0
#define SCANNER_MAX_RANGE2			655360000.0

#define CLOSE_COLLISION_CHECK_MAX_RANGE2 1000000000.0

#define ENTRY(label, value) label = value,

#ifdef __APPLE__
typedef NS_ENUM(int, OOEntityStatus)
{
#include "OOEntityStatus.tbl"
};


#ifndef OO_SCANCLASS_TYPE
#define OO_SCANCLASS_TYPE
//typedef enum OOScanClass OOScanClass;
#endif

typedef NS_ENUM(int, OOScanClass)
{
#include "OOScanClass.tbl"
};

#else
typedef enum OOEntityStatus
{
	#include "OOEntityStatus.tbl"
} OOEntityStatus;


#ifndef OO_SCANCLASS_TYPE
#define OO_SCANCLASS_TYPE
typedef enum OOScanClass OOScanClass;
#endif

enum OOScanClass
{
	#include "OOScanClass.tbl"
};

#endif
#undef ENTRY


@interface Entity: OOWeakRefObject
{
	// the base object for ships/stations/anything actually
	//////////////////////////////////////////////////////
	//
	// @public variables:
	//
	// we forego encapsulation for some variables in order to
	// lose the overheads of Obj-C accessor methods...
	//
@public
	OOUniversalID			universalID;			// used to reference the entity
	
	unsigned				isShip: 1,
							isStation: 1,
							isPlayer: 1,
							isWormhole: 1,
							isSubEntity: 1,
							hasMoved: 1,
							hasRotated: 1,
							hasCollided: 1,
							isSunlit: 1,
							collisionTestFilter: 2,
							throw_sparks: 1,
							isImmuneToBreakPatternHide: 1,
							isExplicitlyNotMainStation: 1,
							isVisualEffect: 1;
	
	OOScanClass				scanClass;
	
	GLfloat					zero_distance;
	GLfloat					cam_zero_distance;
	GLfloat					no_draw_distance;		// 10 km initially
	GLfloat					collision_radius;
	HPVector					position; // use high-precision vectors for global position
	Vector						cameraRelativePosition;
	Quaternion				orientation;
	
	int						zero_index;
	
	// Linked lists of entites, sorted by position on each (world) axis
	Entity					*x_previous, *x_next;
	Entity					*y_previous, *y_next;
	Entity					*z_previous, *z_next;
	
	Entity					*collision_chain;
	
	OOUniversalID			shadingEntityID;
	
	Entity					*collider;
	
	CollisionRegion			*collisionRegion;		// initially nil - then maintained
	
@protected
	HPVector					lastPosition;
	Quaternion				lastOrientation;
	
	GLfloat					distanceTravelled;		// set to zero initially
	
	OOMatrix				rotMatrix;
	
	Vector					velocity;
	
	GLfloat					energy;
	GLfloat					maxEnergy;
	
	BoundingBox				boundingBox;
	GLfloat					mass;
	
	NSMutableArray			*collidingEntities;
	
	OOTimeAbsolute			spawnTime;
	
	struct JSObject			*_jsSelf;
	
@private
	NSUInteger				_sessionID;
	
	OOWeakReference			*_owner;
	OOEntityStatus			_status;
}

// The session in which the entity was created.
@property (readonly) NSUInteger sessionID;

@property (atomic, readonly, getter=isShip) BOOL ship;
@property (atomic, readonly, getter=isDock) BOOL dock;
@property (atomic, readonly, getter=isStation) BOOL station;
@property (atomic, readonly, getter=isSubEntity) BOOL subEntity;
@property (atomic, readonly, getter=isPlayer) BOOL player;
@property (atomic, readonly, getter=isPlanet) BOOL isPlanet;
@property (atomic, readonly, getter=isSun) BOOL sun;
@property (atomic, readonly, getter=isStellarObject) BOOL stellarObject;
@property (atomic, readonly, getter=isSky) BOOL sky;
@property (atomic, readonly, getter=isWormhole) BOOL isWormhole;
@property (atomic, readonly, getter=isEffect) BOOL effect;
@property (atomic, readonly, getter=isVisualEffect) BOOL visualEffect;
@property (atomic, readonly, getter=isWaypoint) BOOL waypoint;

@property (atomic, readonly) BOOL validForAddToUniverse;
- (void) addToLinkedLists;
- (void) removeFromLinkedLists;

- (void) updateLinkedLists;

- (void) wasAddedToUniverse;
- (void) wasRemovedFromUniverse;

- (void) warnAboutHostiles;

@property (retain) CollisionRegion *collisionRegion;

@property (assign) OOUniversalID universalID;

@property (atomic, getter=throwingSparks, setter=setThrowSparks:) BOOL throwsSparks;
- (void) throwSparks;

//@property (atomic, retain) Entity *owner;
- (void) setOwner:(Entity *)ent;
- (id) owner;
/// owner if self is subentity of owner, otherwise nil.
@property (atomic, readonly, assign) ShipEntity *parentEntity;
/// like parentEntity, but recursive.
@property (atomic, readonly, assign) ShipEntity *rootShipEntity;

- (void) setPositionX:(OOHPScalar)x y:(OOHPScalar)y z:(OOHPScalar)z;
@property (nonatomic) HPVector position;
@property (nonatomic, readonly) Vector cameraRelativePosition;
@property (atomic, readonly) GLfloat cameraRangeFront;
@property (atomic, readonly) GLfloat cameraRangeBack;

- (void) updateCameraRelativePosition;
// gets a low-position relative vector
- (Vector) vectorTo:(Entity *)entity;

@property (atomic, readonly) HPVector absolutePositionForSubentity;
- (HPVector) absolutePositionForSubentityOffset:(HPVector) offset;

@property (atomic, readonly) double zeroDistance;
@property (atomic, readonly) double camZeroDistance;
- (NSComparisonResult) compareZeroDistance:(Entity *)otherEntity;

@property (readonly) BoundingBox boundingBox;

@property (readonly) GLfloat mass;

@property (nonatomic) Quaternion orientation;
/// Historical wart: orientation.w is reversed for player; -normalOrientation corrects this.
@property (nonatomic) Quaternion normalOrientation;
- (void) orientationChanged;

@property Vector velocity;
@property (atomic, readonly) double speed;
@property GLfloat distanceTravelled;

@property OOEntityStatus status;
@property OOScanClass scanClass;

@property GLfloat energy;
@property GLfloat maxEnergy;

- (void) applyRoll:(GLfloat)roll andClimb:(GLfloat)climb;
- (void) applyRoll:(GLfloat)roll climb:(GLfloat) climb andYaw:(GLfloat)yaw;
- (void) moveForward:(double)amount;

@property (atomic, readonly) OOMatrix rotationMatrix;
@property (atomic, readonly) OOMatrix drawRotationMatrix;
@property (atomic, readonly) OOMatrix transformationMatrix;
@property (atomic, readonly) OOMatrix drawTransformationMatrix;

@property (readonly, atomic) BOOL canCollide;
@property GLfloat collisionRadius;
@property (readonly, retain) NSMutableArray *collisionArray;

- (void) update:(OOTimeDelta)delta_t;

- (void) applyVelocity:(OOTimeDelta)delta_t;
- (BOOL) checkCloseCollisionWith:(Entity *)other;

- (void) takeEnergyDamage:(double)amount from:(Entity *)ent becauseOf:(Entity *)other;

- (void) dumpState;		// General "describe situtation verbosely in log" command.
- (void) dumpSelfState;	// Subclasses should override this, not -dumpState, and call throught to super first.

// Subclass repsonsibilities
@property (readonly, atomic) double findCollisionRadius;
- (void) drawImmediate:(bool)immediate translucent:(bool)translucent;
@property (atomic, readonly, getter=isVisible) BOOL visible;
@property (atomic, readonly, getter=isInSpace) BOOL inSpace;
@property (atomic, readonly, getter=isImmuneToBreakPatternHide) BOOL immuneToBreakPatternHide;

// For shader bindings.
@property (nonatomic, readonly) GLfloat universalTime;
@property (nonatomic, readonly) GLfloat spawnTime;
@property (nonatomic, readonly) GLfloat timeElapsedSinceSpawn;

#ifndef NDEBUG
@property (readonly, copy, atomic) NSString *descriptionForObjDumpBasic;
@property (readonly, copy, atomic) NSString *descriptionForObjDump;

@property (readonly, copy, atomic) NSSet *allTextures;
#endif

@end

@protocol OOHUDBeaconIcon;

/// Methods that must be supported by entities with beacons, regardless of type.
@protocol OOBeaconEntity

- (NSComparisonResult) compareBeaconCodeWith:(Entity <OOBeaconEntity>*) other;
- (NSString *) beaconCode;
- (void) setBeaconCode:(NSString *)bcode;
- (NSString *) beaconLabel;
- (void) setBeaconLabel:(NSString *)blabel;
- (BOOL) isBeacon;
- (id <OOHUDBeaconIcon>) beaconDrawable;
- (Entity <OOBeaconEntity> *) prevBeacon;
- (Entity <OOBeaconEntity> *) nextBeacon;
- (void) setPrevBeacon:(Entity <OOBeaconEntity> *)beaconShip;
- (void) setNextBeacon:(Entity <OOBeaconEntity> *)beaconShip;
- (BOOL) isJammingScanning;

@end


enum
{
	// Values used for unknown strings.
	kOOEntityStatusDefault		= STATUS_INACTIVE,
	kOOScanClassDefault			= CLASS_NOT_SET
};

NSString *OOStringFromEntityStatus(OOEntityStatus status) CONST_FUNC;
OOEntityStatus OOEntityStatusFromString(NSString *string) PURE_FUNC;

NSString *OOStringFromScanClass(OOScanClass scanClass) CONST_FUNC;
OOScanClass OOScanClassFromString(NSString *string) PURE_FUNC;
