/*

OOSystemDescriptionManager.h

Class responsible for planet description data.

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
#import "OOTypes.h"
#import "legacy_random.h"

typedef NS_ENUM(int, OOSystemLayer)
{
	OO_LAYER_CORE = 0,
	OO_LAYER_OXP_STATIC = 1,
	OO_LAYER_OXP_DYNAMIC = 2,
	OO_LAYER_OXP_PRIORITY = 3
};

#define OO_SYSTEM_LAYERS        4
#define OO_SYSTEMS_PER_GALAXY	(kOOMaximumSystemID+1)
#define OO_GALAXIES_AVAILABLE	(kOOMaximumGalaxyID+1)
#define OO_SYSTEMS_AVAILABLE    OO_SYSTEMS_PER_GALAXY * OO_GALAXIES_AVAILABLE
// don't bother caching interstellar properties
#define OO_SYSTEM_CACHE_LENGTH  OO_SYSTEMS_AVAILABLE

@interface OOSystemDescriptionEntry : NSObject
{
@private
	NSMutableDictionary			*layers[OO_SYSTEM_LAYERS];
}

- (void) setProperty:(NSString *)property forLayer:(OOSystemLayer)layer toValue:(id)value;
- (id) getProperty:(NSString *)property forLayer:(OOSystemLayer)layer;

@end

/**
 * Note: forSystem: inGalaxy: returns from the (fast) propertyCache
 *
 * forSystemKey calculates the values - but is necessary for
 * interstellar space
 */
@interface OOSystemDescriptionManager : NSObject
{
@private
	NSMutableDictionary			*universalProperties;
	OOSystemDescriptionEntry	*interstellarSpace;
	NSMutableDictionary			*systemDescriptions;
	NSMutableDictionary			*propertyCache[OO_SYSTEM_CACHE_LENGTH];
	NSMutableSet				*propertiesInUse;
	NSPoint						coordinatesCache[OO_SYSTEM_CACHE_LENGTH];
	NSMutableArray				*neighbourCache[OO_SYSTEM_CACHE_LENGTH];
	NSMutableDictionary			*scriptedChanges;
}

// this needs to be re-called every time system coordinates change
// changing system coordinates after plist loading is probably
// too much of a can of worms *anyway*, so currently it's only
// called just after the manager data is loaded.
- (void) buildRouteCache;

- (void) setUniversalProperties:(NSDictionary *)properties;
- (void) setInterstellarProperties:(NSDictionary *)properties;

// this is used by planetinfo.plist and has default layer 1
- (void) setProperties:(NSDictionary *)properties forSystemKey:(NSString *)key;

// this is used by Javascript property setting
- (void) setProperty:(NSString *)property forSystemKey:(NSString *)key andLayer:(OOSystemLayer)layer toValue:(id)value fromManifest:(NSString *)manifest;

- (void) importScriptedChanges:(NSDictionary *)scripted;
- (void) importLegacyScriptedChanges:(NSDictionary *)scripted;
@property (readonly, copy) NSDictionary *exportScriptedChanges;

- (NSDictionary *) getPropertiesForSystemKey:(NSString *)key;
@property (getter=getPropertiesForCurrentSystem, readonly, copy) NSDictionary *propertiesForCurrentSystem;
- (id) getProperty:(NSString *)property forSystemKey:(NSString *)key;
- (id) getProperty:(NSString *)property forSystem:(OOSystemID)s inGalaxy:(OOGalaxyID)g;

- (NSPoint) getCoordinatesForSystem:(OOSystemID)s inGalaxy:(OOGalaxyID)g;
- (NSArray *) getNeighbourIDsForSystem:(OOSystemID)s inGalaxy:(OOGalaxyID)g;

@property (getter=getRandomSeedForCurrentSystem, readonly) Random_Seed randomSeedForCurrentSystem;
- (Random_Seed) getRandomSeedForSystem:(OOSystemID)s inGalaxy:(OOGalaxyID)g;

@end


