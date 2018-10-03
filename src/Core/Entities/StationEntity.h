/*

StationEntity.h

ShipEntity subclass representing a space station or dockable ship.

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

#import "ShipEntity.h"
#import "OOJSInterfaceDefinition.h"
#import "Universe.h"

@class OOWeakSet;


typedef NS_ENUM(unsigned int, OOStationAlertLevel)
{
	STATION_ALERT_LEVEL_GREEN	= ALERT_CONDITION_GREEN,
	STATION_ALERT_LEVEL_YELLOW	= ALERT_CONDITION_YELLOW,
	STATION_ALERT_LEVEL_RED		= ALERT_CONDITION_RED
};

#define STATION_MAX_POLICE				8

#define STATION_DELAY_BETWEEN_LAUNCHES  6.0

#define STATION_LAUNCH_RETRY_INTERVAL   2.0

#define MAX_DOCKING_STAGES				16

#define DOCKING_CLEARANCE_WINDOW		126.0


@interface StationEntity: ShipEntity
{
@private
	OOWeakSet				*_shipsOnHold;
	DockEntity				*player_reserved_dock;
	double					last_launch_time;
	double					approach_spacing;
	OOStationAlertLevel		alertLevel;
	
	unsigned				max_police;					// max no. of police ships allowed
	unsigned				max_defense_ships;			// max no. of defense ships allowed
	unsigned				defenders_launched;
	
	unsigned				max_scavengers;				// max no. of scavenger ships allowed
	unsigned				scavengers_launched;
	
	OOTechLevelID			equivalentTechLevel;
	float					equipmentPriceFactor;
	
	Vector  				port_dimensions;
	double					port_radius;
	
	unsigned				no_docking_while_launching: 1,
							hasNPCTraffic: 1;
	BOOL					hasPatrolShips;
	
	OOUniversalID			planet;

	NSString				*allegiance;
	
	OOCommodityMarket		*localMarket;
	OOCargoQuantity			marketCapacity;
	NSArray					*marketDefinition;
	NSString				*marketScriptName;
//	NSMutableArray			*localPassengers;
//	NSMutableArray			*localContracts;
	NSMutableArray			*localShipyard;
	
	NSMutableDictionary *localInterfaces;

	unsigned				docked_shuttles;
	double					last_shuttle_launch_time;
	double					shuttle_launch_interval;
	
	unsigned				docked_traders;
	double					last_trader_launch_time;
	double					trader_launch_interval;
	
	double					last_patrol_report_time;
	double					patrol_launch_interval;
	
	unsigned				suppress_arrival_reports: 1,
							requiresDockingClearance: 1,
							interstellarUndockingAllowed: 1,
							allowsFastDocking: 1,
							allowsSaving: 1,
							allowsAutoDocking: 1,
							hasBreakPattern: 1,
							marketMonitored: 1,
							marketBroadcast: 1;
}


@property (readonly) OOCargoQuantity marketCapacity;
@property (readonly, copy) NSArray *marketDefinition;
@property (readonly, copy) NSString *marketScriptName;
@property (readonly, atomic) BOOL marketMonitored;
@property (readonly, atomic) BOOL marketBroadcast;
- (OOCreditsQuantity) legalStatusOfManifest:(OOCommodityMarket *)manifest export:(BOOL)export;

- (OOCommodityMarket *) localMarket;
- (void) setLocalMarket:(NSArray *)market;
@property (readonly, copy, atomic) NSDictionary *localMarketForScripting;
- (void) setPrice:(OOCreditsQuantity) price forCommodity:(OOCommodityType) commodity;
- (void) setQuantity:(OOCargoQuantity) quantity forCommodity:(OOCommodityType) commodity;

/*- (NSMutableArray *) localPassengers;
- (void) setLocalPassengers:(NSArray *)market;
- (NSMutableArray *) localContracts;
- (void) setLocalContracts:(NSArray *)market; */
- (NSMutableArray *) localShipyard;
- (void) setLocalShipyard:(NSArray *)market;
@property (readonly, retain) NSMutableDictionary *localInterfaces;
- (void) setInterfaceDefinition:(OOJSInterfaceDefinition *)definition forKey:(NSString *)key;

@property (readonly, strong, atomic) OOCommodityMarket *initialiseLocalMarket;

@property  OOTechLevelID equivalentTechLevel;

@property (readonly, strong, atomic) NSEnumerator *dockSubEntityEnumerator;
@property (readonly) Vector virtualPortDimensions;
@property (readonly, strong) DockEntity *playerReservedDock;

@property (readonly, atomic) HPVector beaconPosition;

@property (readonly) float equipmentPriceFactor;


@property (strong, atomic) OOPlanetEntity *planet;

@property (copy) NSString *allegiance;

@property (readonly, atomic) unsigned int countOfDockedContractors;
@property (readonly, atomic) unsigned int countOfDockedPolice;
@property (readonly, atomic) unsigned int countOfDockedDefenders;

- (void) sanityCheckShipsOnApproach;

- (void) autoDockShipsOnApproach;

- (Vector) portUpVectorForShip:(ShipEntity *)ship;

- (NSDictionary *) dockingInstructionsForShip:(ShipEntity *)ship;

- (BOOL) shipIsInDockingCorridor:(ShipEntity *)ship;

@property (readonly, atomic) BOOL dockingCorridorIsEmpty;

- (void) clearDockingCorridor;

- (void) clear;


- (void) abortAllDockings;

- (void) abortDockingForShip:(ShipEntity *)ship;

@property (readonly, atomic) BOOL hasMultipleDocks;
@property (readonly, atomic) BOOL hasClearDock;
@property (readonly, atomic) BOOL hasLaunchDock;
- (DockEntity *) selectDockForDocking;
@property (readonly, atomic) unsigned int currentlyInLaunchingQueues;
@property (readonly, atomic) unsigned int currentlyInDockingQueues;


- (void) launchShip:(ShipEntity *)ship;

- (ShipEntity *) launchIndependentShip:(NSString *)role;

- (void) noteDockedShip:(ShipEntity *)ship;

@property (readonly, atomic) BOOL interstellarUndockingAllowed;
@property (atomic) BOOL hasNPCTraffic;

@property (readonly) OOStationAlertLevel alertLevel;
- (void) setAlertLevel:(OOStationAlertLevel)level signallingScript:(BOOL)signallingScript;

////////////////////////////////////////////////////////////// AI methods...

- (void) increaseAlertLevel;
- (void) decreaseAlertLevel;

@property (readonly, copy, atomic) NSArray *launchPolice;
@property (readonly, strong, atomic) ShipEntity *launchDefenseShip;
@property (readonly, strong, atomic) ShipEntity *launchScavenger;
@property (readonly, strong, atomic) ShipEntity *launchMiner;
/**Lazygun** added the following line*/
@property (readonly, strong, atomic) ShipEntity *launchPirateShip;
@property (readonly, strong, atomic) ShipEntity *launchShuttle;
@property (readonly, strong, atomic) ShipEntity *launchEscort;
@property (readonly, strong, atomic) ShipEntity *launchPatrol;

- (void) launchShipWithRole:(NSString *)role;

- (void) acceptPatrolReportFrom:(ShipEntity *)patrol_ship;

- (NSString *) acceptDockingClearanceRequestFrom:(ShipEntity *)other;
@property (atomic) BOOL requiresDockingClearance;

@property (atomic) BOOL allowsFastDocking;

@property (atomic) BOOL allowsAutoDocking;

@property (readonly, atomic) BOOL allowsSaving;
// no setting this after station creation

@property (readonly, copy, atomic) NSString *marketOverrideName;
@property (getter=isRotatingStation, readonly, atomic) BOOL rotatingStation;
@property (readonly, atomic) BOOL hasShipyard;

@property (atomic) BOOL suppressArrivalReports;

@property (atomic) BOOL hasBreakPattern;

- (BOOL) fitsInDock:(ShipEntity *)ship;
- (BOOL) fitsInDock:(ShipEntity *)ship andLogNoFit:(BOOL)logNoFit;

@end



NSDictionary *OOMakeDockingInstructions(StationEntity *station, HPVector coords, float speed, float range, NSString *ai_message, NSString *comms_message, BOOL match_rotation, int docking_stage);
