/*

OOEquipmentType.h

Class representing a type of ship equipment. Exposed to JavaScript as
EquipmentInfo.


Copyright (C) 2008-2013 Jens Ayton and contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#import "OOCocoa.h"
#import "OOTypes.h"
#import "OOScript.h"
#import "Universe.h"


@interface OOEquipmentType: NSObject <NSCopying>
{
@private
	OOTechLevelID			_techLevel;
	OOCreditsQuantity		_price;
	NSString				*_name;
	NSString				*_identifier;
	NSString				*_description;
	unsigned				_isAvailableToAll: 1,
							_requiresEmptyPylon: 1,
							_requiresMountedPylon: 1,
							_requiresClean: 1,
							_requiresNotClean: 1,
							_portableBetweenShips: 1,
							_requiresFreePassengerBerth: 1,
							_requiresFullFuel: 1,
							_requiresNonFullFuel: 1,
							_isMissileOrMine: 1,
							_isVisible: 1,
							_isAvailableToPlayer: 1,
							_isAvailableToNPCs: 1,
							_fastAffinityA: 1,
							_fastAffinityB: 1,
							_canCarryMultiple: 1;
	NSUInteger				_installTime;
	NSUInteger				_repairTime;
	GLfloat     			_damageProbability;
	OOCargoQuantity			_requiredCargoSpace;
	NSSet					*_requiresEquipment;
	NSSet					*_requiresAnyEquipment;
	NSSet					*_incompatibleEquipment;
	NSArray					*_conditions;
	NSArray					*_provides;
	NSDictionary			*_scriptInfo;
	NSDictionary			*_weaponInfo;
	NSString				*_script;
	NSString				*_condition_script;
	
	struct JSObject			*_jsSelf;
}

+ (void) loadEquipment;			// Load equipment data; called on loading and when changing to/from strict mode.
+ (void) addEquipmentWithInfo:(NSArray *)itemInfo;	// Used to generate equipment from missile_role entries.

+ (NSString *) getMissileRegistryRoleForShip:(NSString *)shipKey;
+ (void) setMissileRegistryRole:(NSString *)roles forShip:(NSString *)shipKey;

+ (NSArray *) allEquipmentTypes;
+ (NSEnumerator *) equipmentEnumerator;
+ (NSEnumerator *) reverseEquipmentEnumerator;

+ (OOEquipmentType *) equipmentTypeWithIdentifier:(NSString *)identifier;

@property (readonly, copy) NSString *identifier;
@property (readonly, copy, atomic) NSString *damagedIdentifier;
@property (readonly, copy) NSString *name;			// localized
@property (readonly, copy) NSString *descriptiveText;	// localized
@property (readonly) OOTechLevelID techLevel;
@property (readonly) OOCreditsQuantity price;	// Tenths of credits

@property (atomic, getter=isAvailableToAll, readonly) BOOL availableToAll;
@property (atomic, readonly) BOOL requiresEmptyPylon;
@property (atomic, readonly) BOOL requiresMountedPylon;
@property (atomic, readonly) BOOL requiresCleanLegalRecord;
@property (atomic, readonly) BOOL requiresNonCleanLegalRecord;
@property (atomic, readonly) BOOL requiresFreePassengerBerth;
@property (atomic, readonly) BOOL requiresFullFuel;
@property (atomic, readonly) BOOL requiresNonFullFuel;
@property (atomic, getter=isPrimaryWeapon, readonly) BOOL primaryWeapon;
@property (atomic, getter=isMissileOrMine, readonly) BOOL missileOrMine;
@property (atomic, getter=isPortableBetweenShips, readonly) BOOL portableBetweenShips;

@property (atomic, readonly) BOOL canCarryMultiple;
@property (atomic, readonly) GLfloat damageProbability;
@property (atomic, readonly) BOOL canBeDamaged;
@property (atomic, getter=isVisible, readonly) BOOL visible;				// Visible in UI?

@property (atomic, getter=isAvailableToPlayer, readonly) BOOL availableToPlayer;
@property (atomic, getter=isAvailableToNPCs, readonly) BOOL availableToNPCs;

@property (readonly) OOCargoQuantity requiredCargoSpace;
@property (readonly, copy) NSSet *requiresEquipment;		// Set of equipment identifiers; all items required
@property (readonly, copy) NSSet *requiresAnyEquipment;	// Set of equipment identifiers; any item required
@property (readonly, copy) NSSet *incompatibleEquipment;	// Set of equipment identifiers; all items prohibited

// FIXME: should have general mechanism to handle scripts or legacy conditions.
@property (readonly, copy) NSArray *conditions;

@property (readonly, copy) NSString *conditionScript;

@property (readonly, copy) NSDictionary *scriptInfo;
@property (readonly, copy) NSString *scriptName;

@property (atomic, readonly) BOOL fastAffinityDefensive;
@property (atomic, readonly) BOOL fastAffinityOffensive;

@property (readonly) NSUInteger installTime;
@property (readonly, atomic) NSUInteger repairTime;

@property (readonly, copy) NSArray *providesForScripting;
- (BOOL) provides:(NSString *)key;

// weapon properties
@property (atomic, getter=isTurretLaser, readonly) BOOL turretLaser;
@property (atomic, getter=isMiningLaser, readonly) BOOL miningLaser;
@property (atomic, readonly) GLfloat weaponRange;
@property (atomic, readonly) GLfloat weaponEnergyUse;
@property (atomic, readonly) GLfloat weaponDamage;
@property (atomic, readonly) GLfloat weaponRechargeRate;
@property (atomic, readonly) GLfloat weaponShotTemperature;
@property (atomic, readonly) GLfloat weaponThreatAssessment;
@property (atomic, readonly, copy) OOColor *weaponColor;

@end


@interface OOEquipmentType (Conveniences)

@property (readonly) OOTechLevelID effectiveTechLevel;

@end
