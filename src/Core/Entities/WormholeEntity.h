/*

WormholeEntity.h

Entity subclass representing a wormhole between systems. (This is -- to use
technical terminology -- the blue blobby thing you see hanging in space. The
purple tunnel is RingEntity.)

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

#import "Entity.h"

#define WORMHOLE_EXPIRES_TIMEINTERVAL	900.0
#define WORMHOLE_SHRINK_RATE			4000.0
#define WORMHOLE_LEADER_SPEED_FACTOR 0.25

@class ShipEntity, Universe;

typedef NS_ENUM(unsigned int, WORMHOLE_SCANINFO)
{
	WH_SCANINFO_NONE = 0,
	WH_SCANINFO_SCANNED,
	WH_SCANINFO_COLLAPSE_TIME,
	WH_SCANINFO_ARRIVAL_TIME,
	WH_SCANINFO_DESTINATION,
	WH_SCANINFO_SHIP,
};

@interface WormholeEntity: Entity
{
@private
	double			expiry_time;	// Time when wormhole entrance closes
	double			travel_time;	// Time taken for a ship to traverse the wormhole
	double			arrival_time;	// Time when wormhole exit opens
	double			estimated_arrival_time;	// Time when wormhole should open (be different to arrival_time for misjump wormholes)
	double			scan_time;		// Time when wormhole was scanned
	
	OOSystemID		origin;
	OOSystemID		destination;

	NSPoint			originCoords;      // May not equal our origin system if the wormhole opens from Interstellar Space
	NSPoint			destinationCoords; // May not equal the destination system if the wormhole misjumps

	NSMutableArray	*shipsInTransit;
	
	double			witch_mass;
	double			shrink_factor;	// used during nova mission
// not used yet
	double      exit_speed; // exit speed of ships in this wormhole
	
	WORMHOLE_SCANINFO	scan_info;
	BOOL			hasExitPosition;
	BOOL			_misjump;
	GLfloat   _misjumpRange;
  BOOL      containsPlayer;
}

- (WormholeEntity*) initWithDict:(NSDictionary*)dict;
- (WormholeEntity*) initWormholeTo:(OOSystemID) s fromShip:(ShipEntity *) ship;

- (BOOL) suckInShip:(ShipEntity *) ship;
- (void) disgorgeShips;
- (void) setExitPosition:(HPVector)pos;

@property (readonly) OOSystemID origin;
@property (readonly) OOSystemID destination;
@property (readonly) NSPoint originCoordinates;
@property (readonly) NSPoint destinationCoordinates;

- (void) setMisjump;	// Flags up a wormhole as 'misjumpy'
- (void) setMisjumpWithRange:(GLfloat)range;	// Flags up a wormhole as 'misjumpy'
@property (readonly) BOOL withMisjump;
@property (readonly, nonatomic) GLfloat misjumpRange;

@property  double exitSpeed;	// exit speed from this wormhole	// set exit speed from this wormhole

@property (readonly) double expiryTime;	// Time at which the wormholes entrance closes
@property (readonly) double arrivalTime;	// Time at which the wormholes exit opens
@property (readonly) double estimatedArrivalTime;	// Time when wormhole should open (different from arrival_time for misjump wormholes)
@property (readonly) double travelTime;	// Time needed for a ship to traverse the wormhole
@property (readonly) double scanTime;	// Time when wormhole was scanned
- (void) setScannedAt:(double)time;
- (void) setContainsPlayer:(BOOL)val; // mark the wormhole as waiting for player exit

@property (getter=isScanned, readonly, atomic) BOOL scanned;		// True if the wormhole has been scanned by the player
@property  WORMHOLE_SCANINFO scanInfo; // Stage of scanning

@property (readonly, copy, atomic) NSArray *shipsInTransit;

- (NSString *) identFromShip:(ShipEntity*) ship;

@property (getter=getDict, readonly, copy, atomic) NSDictionary *dict;

@end
