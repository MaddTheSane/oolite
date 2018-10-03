/*

OOPlanetEntity.h

Entity subclass representing a planet.

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

#import "OOStellarBody.h"
#if !NEW_PLANETS
#import "PlanetEntity.h"
#else

#import "Entity.h"
#import "OOColor.h"


@class OOPlanetDrawable, ShipEntity, OOMaterial;


@interface OOPlanetEntity: Entity <OOStellarBody>
{
@private
	OOPlanetDrawable		*_planetDrawable;
	OOPlanetDrawable		*_atmosphereDrawable;
	OOPlanetDrawable		*_atmosphereShaderDrawable;
	
	BOOL					_miniature;
	OOColor					*_airColor;
	double					_mesopause2;
	
	Vector					_rotationAxis;
	float					_rotationalVelocity;
	Quaternion				_atmosphereOrientation;
	float					_atmosphereRotationalVelocity;
	
	unsigned				_shuttlesOnGround;
	OOTimeDelta				_lastLaunchTime;
	OOTimeDelta				_shuttleLaunchInterval;
	
	NSDictionary			*_materialParameters;
	NSString				*_textureName;

	NSString				*_name;
}

- (instancetype) initAsMainPlanetForSystem:(OOSystemID)s;

- (instancetype) initFromDictionary:(NSDictionary *)dict withAtmosphere:(BOOL)atmosphere andSeed:(Random_Seed)seed forSystem:(OOSystemID)systemID;

- (instancetype) miniatureVersion;

@property (atomic) double rotationalVelocity;

@property (atomic, readonly) BOOL planetHasStation;
- (void) launchShuttle;
- (void) welcomeShuttle:(ShipEntity *)shuttle;

@property (atomic, readonly) BOOL hasAtmosphere;

// FIXME: need material model.
@property (atomic, copy) NSString *textureFileName;

- (BOOL) setUpPlanetFromTexture:(NSString *)fileName;

@property (atomic, readonly, strong) OOMaterial *material;
@property (atomic, readonly, strong) OOMaterial *atmosphereMaterial;
- (OOMaterial *) atmosphereShaderMaterial;

@property (atomic, getter=isFinishedLoading, readonly) BOOL finishedLoading;

@end

#endif	// NEW_PLANETS
