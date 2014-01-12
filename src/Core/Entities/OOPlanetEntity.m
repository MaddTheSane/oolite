/*

OOPlanetEntity.m

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

#import "OOPlanetEntity.h"

#if NEW_PLANETS

#define NEW_ATMOSPHERE 1

#import "OOPlanetDrawable.h"

#import "AI.h"
#import "Universe.h"
#import "ShipEntity.h"
#import "PlayerEntity.h"
#import "ShipEntityAI.h"
#import "OOCharacter.h"

#import "OOMaths.h"
#import "ResourceManager.h"
#import "OOStringParsing.h"
#import "OOCollectionExtractors.h"

#import "OOPlanetTextureGenerator.h"
#import "OOSingleTextureMaterial.h"
#import "OOShaderMaterial.h"
#import "OOEntityFilterPredicate.h"
#import "OOGraphicsResetManager.h"

@interface OOPlanetEntity (Private) <OOGraphicsResetClient>

- (void) setUpLandParametersWithSourceInfo:(NSDictionary *)sourceInfo targetInfo:(NSMutableDictionary *)targetInfo;
- (void) setUpAtmosphereParametersWithSourceInfo:(NSDictionary *)sourceInfo targetInfo:(NSMutableDictionary *)targetInfo;
- (void) setUpColorParametersWithSourceInfo:(NSDictionary *)sourceInfo targetInfo:(NSMutableDictionary *)targetInfo isAtmosphere:(BOOL)isAtmosphere;

@end


@implementation OOPlanetEntity

// this is exclusively called to initialise the main planet.
- (id) initAsMainPlanetForSystemSeed:(Random_Seed)seed
{
	NSMutableDictionary *planetInfo = [[UNIVERSE generateSystemData:seed] mutableCopy];
	[planetInfo autorelease];
	
	[planetInfo oo_setBool:YES forKey:@"mainForLocalSystem"];
	return [self initFromDictionary:planetInfo withAtmosphere:YES andSeed:seed];
}


const double kMesosphere = 10.0 * ATMOSPHERE_DEPTH;	// atmosphere effect starts at 10x the height of the clouds


- (id) initFromDictionary:(NSDictionary *)dict withAtmosphere:(BOOL)atmosphere andSeed:(Random_Seed)seed
{
	if (dict == nil)  dict = @{};
	RANROTSeed savedRanrotSeed = RANROTGetFullSeed();
	
	self = [self init];
	if (self == nil)  return nil;
	
	scanClass = CLASS_NO_DRAW;
	
	// Load random seed override.
	NSString *seedStr = [dict oo_stringForKey:@"seed"];
	if (seedStr != nil)
	{
		Random_Seed overrideSeed = RandomSeedFromString(seedStr);
		if (!is_nil_seed(overrideSeed))  seed = overrideSeed;
		else  OOLogERR(@"planet.fromDict", @"could not interpret \"%@\" as planet seed, using default.", seedStr);
	}
	
	// Generate various planet info.
	seed_for_planet_description(seed);
	NSMutableDictionary *planetInfo = [[UNIVERSE generateSystemData:seed] mutableCopy];
	[planetInfo autorelease];
	
	int radius_km = [dict oo_intForKey:KEY_RADIUS defaultValue:[planetInfo oo_intForKey:KEY_RADIUS]];
	collision_radius = radius_km * 10.0;	// Scale down by a factor of 100
	OOTechLevelID techLevel = [dict oo_intForKey:KEY_TECHLEVEL defaultValue:[planetInfo oo_intForKey:KEY_TECHLEVEL]];
	
	if (techLevel > 14)  techLevel = 14;
	_shuttlesOnGround = 1 + techLevel / 2;
	_shuttleLaunchInterval = 3600.0 / (double)_shuttlesOnGround;	// All are launched in one hour.
	_lastLaunchTime = [UNIVERSE getTime] + 30.0 - _shuttleLaunchInterval;	// launch 30s after player enters universe.
																			// make delay > 0 to allow scripts adding a station nearby.
	
	int percent_land = [planetInfo oo_intForKey:@"percent_land" defaultValue:24 + (gen_rnd_number() % 48)];
	planetInfo[@"land_fraction"] = @(0.01 * percent_land);
	
	RNG_Seed savedRndSeed = currentRandomSeed();
	
	_planetDrawable = [[OOPlanetDrawable alloc] init];
	
	// Load material parameters, including atmosphere.
	RANROTSeed planetNoiseSeed = RANROTGetFullSeed();
	planetInfo[@"noise_map_seed"] = [NSValue valueWithBytes:&planetNoiseSeed objCType:@encode(RANROTSeed)];
	[self setUpLandParametersWithSourceInfo:dict targetInfo:planetInfo];
	
	_airColor = nil;	// default to no air
	
#if NEW_ATMOSPHERE
	if (atmosphere)
	{
		_atmosphereDrawable = [[OOPlanetDrawable atmosphereWithRadius:collision_radius + ATMOSPHERE_DEPTH] retain];
		
		// convert the atmosphere settings to generic 'material parameters'
		percent_land = 100 - [dict oo_intForKey:@"percent_cloud" defaultValue:100 - (3 + (gen_rnd_number() & 31)+(gen_rnd_number() & 31))];
		planetInfo[@"cloud_fraction"] = @(0.01 * percent_land);
		[self setUpAtmosphereParametersWithSourceInfo:dict targetInfo:planetInfo];
		// planetInfo now contains a valid air_color
		_airColor = planetInfo[@"air_color"];
		// OOLog (@"planet.debug",@" translated air colour:%@ cloud colour:%@ polar cloud color:%@", [_airColor rgbaDescription],[(OOColor *)[planetInfo objectForKey:@"cloud_color"] rgbaDescription],[(OOColor *)[planetInfo objectForKey:@"polar_cloud_color"] rgbaDescription]);

		_materialParameters = [planetInfo dictionaryWithValuesForKeys:@[@"cloud_fraction", @"air_color",  @"cloud_color", @"polar_cloud_color", @"cloud_alpha",
															@"land_fraction", @"land_color", @"sea_color", @"polar_land_color", @"polar_sea_color", @"noise_map_seed", @"economy"]];
	}
	else
#else
	// NEW_ATMOSPHERE is 0? still differentiate between normal planets and moons.
	if (atmosphere)
	{
		_atmosphereDrawable = [[OOPlanetDrawable atmosphereWithRadius:collision_radius + ATMOSPHERE_DEPTH] retain];
		_airColor = [[OOColor colorWithRed:0.8f green:0.8f blue:0.9f alpha:1.0f] retain];
	}
	if (YES) // create _materialParameters when NEW_ATMOSPHERE is set to 0
#endif
	{
		_materialParameters = [planetInfo dictionaryWithValuesForKeys:@[@"land_fraction", @"land_color", @"sea_color", @"polar_land_color", @"polar_sea_color", @"noise_map_seed", @"economy"]];
	}
	[_materialParameters retain];
	
	_mesopause2 = (atmosphere) ? (kMesosphere + collision_radius) * (kMesosphere + collision_radius) : 0.0;
	
	_textureName = [[dict oo_stringForKey:@"texture"] retain];
	[self setUpPlanetFromTexture:_textureName];
	[_planetDrawable setRadius:collision_radius];	
		
	// Orientation should be handled by the code that calls this planetEntity. Starting with a default value anyway.
	orientation = (Quaternion){ M_SQRT1_2, M_SQRT1_2, 0, 0 };
	_rotationAxis = vector_up_from_quaternion(orientation);
	
	// set speed of rotation.
	if (dict[@"rotational_velocity"])
	{
		_rotationalVelocity = [dict oo_floatForKey:@"rotational_velocity" defaultValue:0.01f * randf()];	// 0.0 .. 0.01 avr 0.005
	}
	else
	{
		_rotationalVelocity = [planetInfo oo_floatForKey:@"rotation_speed" defaultValue:0.005 * randf()]; // 0.0 .. 0.005 avr 0.0025
		_rotationalVelocity *= [planetInfo oo_floatForKey:@"rotation_speed_factor" defaultValue:1.0f];
	}
	
	// set energy
	energy = collision_radius * 1000.0;
	
	setRandomSeed(savedRndSeed);
	RANROTSetFullSeed(savedRanrotSeed);
	
	// rotate planet based on current time, needs to be done here - backported from PlanetEntity.
	int		deltaT = floor(fmod([PLAYER clockTimeAdjusted], 86400));
	quaternion_rotate_about_axis(&orientation, _rotationAxis, _rotationalVelocity * deltaT);
	
	[self setStatus:STATUS_ACTIVE];
	
	[[OOGraphicsResetManager sharedManager] registerClient:self];

	return self;
}


static Vector RandomHSBColor(void)
{
	return (Vector)
	{
		gen_rnd_number() / 256.0,
		gen_rnd_number() / 256.0,
		0.5 + gen_rnd_number() / 512.0
	};
}


static Vector LighterHSBColor(Vector c)
{
	return (Vector)
	{
		c.x,
		c.y * 0.25f,
		1.0f - (c.z * 0.1f)
	};
}


static Vector HSBColorWithColor(OOColor *color)
{
	OOHSBAComponents c = [color hsbaComponents];
	return (Vector){ c.h, c.s, c.b };
}


static OOColor *ColorWithHSBColor(Vector c)
{
	return [OOColor colorWithHue:c.x saturation:c.y brightness:c.z alpha:1.0];
}


- (void) setUpLandParametersWithSourceInfo:(NSDictionary *)sourceInfo targetInfo:(NSMutableDictionary *)targetInfo
{
	[self setUpColorParametersWithSourceInfo:sourceInfo targetInfo:targetInfo isAtmosphere:NO];
}


- (void) setUpAtmosphereParametersWithSourceInfo:(NSDictionary *)sourceInfo targetInfo:(NSMutableDictionary *)targetInfo
{
	[self setUpColorParametersWithSourceInfo:sourceInfo targetInfo:targetInfo isAtmosphere:YES];
}


- (void) setUpColorParametersWithSourceInfo:(NSDictionary *)sourceInfo targetInfo:(NSMutableDictionary *)targetInfo isAtmosphere:(BOOL)isAtmosphere
{
	// Stir the PRNG fourteen times for backwards compatibility.
	unsigned i;
	for (i = 0; i < 14; i++)
	{
		gen_rnd_number();
	}
	
	Vector	landHSB, seaHSB, landPolarHSB, seaPolarHSB;
	OOColor	*color;
	
	landHSB = RandomHSBColor();
	
	if (!isAtmosphere)
	{
		do
		{
			seaHSB = RandomHSBColor();
		}
		while (dot_product(landHSB, seaHSB) > .80); // make sure land and sea colors differ significantly
		
		// saturation bias - avoids really grey oceans
		if (seaHSB.y < 0.22f) seaHSB.y = seaHSB.y * 0.3f + 0.2f;
		// brightness bias - avoids really bright landmasses
		if (landHSB.z > 0.66f) landHSB.z = 0.66f;
		
		// planetinfo.plist overrides
		color = [OOColor colorWithDescription:sourceInfo[@"land_color"]];
		if (color != nil) landHSB = HSBColorWithColor(color);
		else ScanVectorFromString([sourceInfo oo_stringForKey:@"land_hsb_color"], &landHSB);
		
		color = [OOColor colorWithDescription:sourceInfo[@"sea_color"]];
		if (color != nil) seaHSB = HSBColorWithColor(color);
		else ScanVectorFromString([sourceInfo oo_stringForKey:@"sea_hsb_color"], &seaHSB);
		
		// polar areas are brighter but have less colour (closer to white)
		landPolarHSB = LighterHSBColor(landHSB);
		seaPolarHSB = LighterHSBColor(seaHSB);
		
		targetInfo[@"land_color"] = ColorWithHSBColor(landHSB);
		targetInfo[@"sea_color"] = ColorWithHSBColor(seaHSB);
		targetInfo[@"polar_land_color"] = ColorWithHSBColor(landPolarHSB);
		targetInfo[@"polar_sea_color"] = ColorWithHSBColor(seaPolarHSB);
	}
	else
	{
		landHSB = RandomHSBColor();	// NB: randomcolor is called twice to make the cloud colour similar to the old one.

		// add a cloud_color tinge to sky blue({0.66, 0.3, 1}).
		seaHSB = vector_add(landHSB,((Vector){1.333, 0.6, 2}));	// 1 part cloud, 2 parts sky blue
		scale_vector(&seaHSB, 0.333);
				
		float cloudAlpha = OOClamp_0_1_f([sourceInfo oo_floatForKey:@"cloud_alpha" defaultValue:1.0f]);
		targetInfo[@"cloud_alpha"] = @(cloudAlpha);
		
		// planetinfo overrides
		color = [OOColor colorWithDescription:sourceInfo[@"atmosphere_color"]];
		if (color != nil) seaHSB = HSBColorWithColor(color);
		color = [OOColor colorWithDescription:sourceInfo[@"cloud_color"]];
	if (color != nil) landHSB = HSBColorWithColor(color);
		
		// polar areas: brighter, less saturation
		landPolarHSB = vector_add(landHSB,LighterHSBColor(landHSB));
		scale_vector(&landPolarHSB, 0.5);
		
		color = [OOColor colorWithDescription:sourceInfo[@"polar_cloud_color"]];
		if (color != nil) landPolarHSB = HSBColorWithColor(color);
		
		targetInfo[@"air_color"] = ColorWithHSBColor(seaHSB);
		targetInfo[@"cloud_color"] = ColorWithHSBColor(landHSB);
		targetInfo[@"polar_cloud_color"] = ColorWithHSBColor(landPolarHSB);
	}
}


- (id) initAsMiniatureVersionOfPlanet:(OOPlanetEntity *)planet
{
	// Nasty, nasty. I'd really prefer to have a separate entity class for this.
	if (planet == nil)
	{
		[self release];
		return nil;
	}
	
	self = [self init];
	if (self == nil)  return nil;
	
	scanClass = CLASS_NO_DRAW;
	[self setStatus:STATUS_COCKPIT_DISPLAY];
	
	collision_radius = planet->collision_radius * PLANET_MINIATURE_FACTOR;
	orientation = planet->orientation;
	_rotationAxis = planet->_rotationAxis;
	_rotationalVelocity = 0.04;
	
	_miniature = YES;
	
	_planetDrawable = [planet->_planetDrawable copy];
	[_planetDrawable setRadius:collision_radius];
	
	// FIXME: in old planet code, atmosphere (if textured) is set to 0.6 alpha.
	_atmosphereDrawable = [planet->_atmosphereDrawable copy];
	[_atmosphereDrawable setRadius:collision_radius + ATMOSPHERE_DEPTH * PLANET_MINIATURE_FACTOR * 2.0]; //not to scale: invisible otherwise
	
	[_planetDrawable setLevelOfDetail:0.8f];
	[_atmosphereDrawable setLevelOfDetail:0.8f];
	
	return self;
}


- (void) dealloc
{
	DESTROY(_planetDrawable);
	DESTROY(_atmosphereDrawable);
	//DESTROY(_airColor);	// this CTDs on loading savegames.. :(
	DESTROY(_materialParameters);
	DESTROY(_textureName);
	
	[[OOGraphicsResetManager sharedManager] unregisterClient:self];

	[super dealloc];
}


- (NSString*) descriptionComponents
{
	return [NSString stringWithFormat:@"position: %@ radius: %g m", HPVectorDescription([self position]), [self radius]];
}


- (void) setOrientation:(Quaternion) quat
{
	[super setOrientation: quat];
	_rotationAxis = vector_up_from_quaternion(quat);
}


- (double) radius
{
	return collision_radius;
}


- (OOStellarBodyType) planetType
{
	if (_miniature)  return STELLAR_TYPE_MINIATURE;
	if (_atmosphereDrawable != nil)  return STELLAR_TYPE_NORMAL_PLANET;
	return STELLAR_TYPE_MOON;
}


- (instancetype) miniatureVersion
{
	return [[[[self class] alloc] initAsMiniatureVersionOfPlanet:self] autorelease];
}


- (void) update:(OOTimeDelta) delta_t
{
	[super update:delta_t];
	
	if (EXPECT(!_miniature))
	{
		if (EXPECT_NOT(_atmosphereDrawable && cam_zero_distance < _mesopause2))
		{
			NSAssert(_airColor != nil, @"Expected a non-nil air colour for normal planet. Exiting.");
			double		alt = (sqrt(cam_zero_distance) - collision_radius) / kMesosphere;
			if (EXPECT_NOT(alt > 0 && alt <= 1.0))	// ensure aleph is clamped between 0 and 1
			{
				double	aleph = 1.0 - alt;
				double	aleph2 = aleph * aleph;
				
				// night sky, reddish flash on entering the atmosphere, low light pollution otherwhise
				OOColor	*mixColor = [OOColor colorWithRed:(EXPECT_NOT(alt > 0.98) ? 30.0f : 0.1f)
													green:0.1f
													 blue:0.1f
													alpha:aleph];
															  
				// occlusion rate: .9 is 18 degrees after the terminus, where twilight ends.
				// 1 is the terminus, 1.033 is 6 degrees before the terminus, where the sky begins to redden
				double rate = ([PLAYER occlusionLevel] - 0.97)/0.06; // from 0.97 to 1.03

				if (EXPECT(rate <= 1.0 && rate > 0.0))
				{
					mixColor = [mixColor blendedColorWithFraction:rate ofColor:_airColor];
					// TODO: properly calculated pink sky - needs to depend on sun's angular size,
					// and its angular height on the horizon.
					/*
					rate -= 0.7;
					if (rate >= 0.0) // pink sky!
					{
						rate = 0.5 - (fabs(rate - 0.15) / 0.3);	// at most a 50% blend!
						mixColor = [mixColor blendedColorWithFraction:rate ofColor:[OOColor colorWithRed:0.6
																								   green:0.1
																									blue:0.0
																								   alpha:aleph]];
					}
					*/
				}
				else
				{
					if (PLAYER->isSunlit && _airColor != nil) mixColor = _airColor;
				}
				[UNIVERSE setSkyColorRed:[mixColor redComponent] * aleph2
								   green:[mixColor greenComponent] * aleph2
									blue:[mixColor blueComponent] * aleph
								   alpha:aleph];
				[_atmosphereDrawable setRadius:collision_radius + (ATMOSPHERE_DEPTH * alt)];
			}
		}
		else if (EXPECT_NOT([_atmosphereDrawable radius] < collision_radius + ATMOSPHERE_DEPTH))
		{
			[_atmosphereDrawable setRadius:collision_radius + ATMOSPHERE_DEPTH];
		}
		
		double time = [UNIVERSE getTime];
		
		if (_shuttlesOnGround > 0 && time > _lastLaunchTime + _shuttleLaunchInterval)  [self launchShuttle];
	}
	
	quaternion_rotate_about_axis(&orientation, _rotationAxis, _rotationalVelocity * delta_t);

	[self orientationChanged];
	
	// FIXME: update atmosphere rotation
}


- (BOOL) isFinishedLoading
{
	OOMaterial *material = [self material];
	if (material != nil && ![material isFinishedLoading])  return NO;
	material = [self atmosphereMaterial];
	if (material != nil && ![material isFinishedLoading])  return NO;
	return YES;
}


- (void) drawImmediate:(bool)immediate translucent:(bool)translucent
{
	if (translucent || [UNIVERSE breakPatternHide])   return; // DON'T DRAW
	if (_miniature && ![self isFinishedLoading])  return; // For responsiveness, don't block to draw as miniature.
	
	if (![UNIVERSE viewFrustumIntersectsSphereAt:cameraRelativePosition withRadius:([self radius] + ATMOSPHERE_DEPTH)])
	{
		// Don't draw
		return;
	}
	
	if ([UNIVERSE wireframeGraphics])  OOGLWireframeModeOn();
	
	if (!_miniature)
	{
		[_planetDrawable calculateLevelOfDetailForViewDistance:cam_zero_distance];
		[_atmosphereDrawable setLevelOfDetail:[_planetDrawable levelOfDetail]];
	}
	
	[_planetDrawable renderOpaqueParts];
//#if NEW_ATMOSPHERE
	if (_atmosphereDrawable != nil)
	{
		[_atmosphereDrawable renderOpaqueParts];
	}
//#endif
	
	if ([UNIVERSE wireframeGraphics])  OOGLWireframeModeOff();
}


- (BOOL) checkCloseCollisionWith:(Entity *)other
{
	if (!other)
		return NO;
	if (other->isShip)
	{
		ShipEntity *ship = (ShipEntity *)other;
		if ([ship behaviour] == BEHAVIOUR_LAND_ON_PLANET)
		{
			return NO;
		}
	}
	
	return YES;
}


- (BOOL) planetHasStation
{
	// find the nearest station...
	ShipEntity	*station =  nil;
	station = [UNIVERSE nearestShipMatchingPredicate:IsStationPredicate
										   parameter:nil
									relativeToEntity:self];
	
	if (station && HPdistance([station position], position) < 4 * collision_radius) // there is a station in range.
	{
		return YES;
	}
	return NO;
}


- (void) launchShuttle
{
	if (_shuttlesOnGround == 0)  return;
	
	if (self != [UNIVERSE planet] && ![self planetHasStation])
	{
		// don't launch shuttles when no station is nearby.
		_shuttlesOnGround = 0;
		return;
	}
	
	Quaternion  q1;
	quaternion_set_random(&q1);
	float start_distance = collision_radius + 125.0f;
	HPVector launch_pos = HPvector_add(position, vectorToHPVector(vector_multiply_scalar(vector_forward_from_quaternion(q1), start_distance)));
	
	ShipEntity *shuttle_ship = [UNIVERSE newShipWithRole:@"shuttle"];   // retain count = 1
	if (shuttle_ship)
	{
		if ([[shuttle_ship crew] count] == 0)
		{
			[shuttle_ship setSingleCrewWithRole:@"trader"];
		}
		
		[shuttle_ship setPosition:launch_pos];
		[shuttle_ship setOrientation:q1];
		
		[shuttle_ship setScanClass: CLASS_NEUTRAL];
		[shuttle_ship setCargoFlag:CARGO_FLAG_FULL_PLENTIFUL];
		[shuttle_ship switchAITo:@"oolite-shuttleAI.js"];
		[UNIVERSE addEntity:shuttle_ship];	// STATUS_IN_FLIGHT, AI state GLOBAL
		_shuttlesOnGround--;
		_lastLaunchTime = [UNIVERSE getTime];
		
		[shuttle_ship release];
	}
}


- (void) welcomeShuttle:(ShipEntity *)shuttle
{
	_shuttlesOnGround++;
}


- (BOOL) isPlanet
{
	return YES;
}


- (BOOL) isVisible
{
	return YES;
}


- (double) rotationalVelocity
{
	return _rotationalVelocity;
}


- (void) setRotationalVelocity:(double) v
{
	if ([self hasAtmosphere])
	{
		// FIXME: change atmosphere rotation speed proportionally
	}
	_rotationalVelocity = v;
}


- (BOOL) hasAtmosphere
{
	return _atmosphereDrawable != nil;
}


// FIXME: need material model.
- (NSString *) textureFileName
{
	return [_planetDrawable textureName];
}


- (void)resetGraphicsState
{
	// reset the texture if graphics mode changes
	[self setUpPlanetFromTexture:_textureName];
}


- (void) setTextureFileName:(NSString *)textureName
{
	BOOL isMoon = _atmosphereDrawable == nil;
	
	OOTexture *diffuseMap = nil;
	OOTexture *normalMap = nil;
	NSDictionary *macros = nil;
	NSDictionary *materialDefaults = [ResourceManager materialDefaults];
	
#if OO_SHADERS
	OOGraphicsDetail detailLevel = [UNIVERSE detailLevel];
	BOOL shadersOn = detailLevel >= DETAIL_LEVEL_EXTRAS;
#else
	const BOOL shadersOn = NO;
#endif
	
	if (textureName != nil)
	{
		NSDictionary *spec = @{@"name": textureName, @"repeat_s": @"yes", @"min_filter": @"linear", @"cube_map": @"yes"};
		diffuseMap = [OOTexture textureWithConfiguration:spec];
		if (diffuseMap == nil)  return;		// OOTexture will have logged a file-not-found warning.
		if (shadersOn)  
		{
			[diffuseMap ensureFinishedLoading]; // only know if it is a cube map if it's loaded
			if ([diffuseMap isCubeMap])
			{
				macros = [materialDefaults oo_dictionaryForKey:isMoon ? @"moon-customized-cubemap-macros" : @"planet-customized-cubemap-macros"];
			}
			else
			{
				macros = [materialDefaults oo_dictionaryForKey:isMoon ? @"moon-customized-macros" : @"planet-customized-macros"];
			}
		}
		else textureName = @"dynamic";

		if (!isMoon)
		{
			/* Generate the atmosphere texture anyway */
			OOTexture *diffuseTmp = nil;
			OOTexture *atmosphere = nil;
			[OOPlanetTextureGenerator generatePlanetTexture:&diffuseTmp
									   secondaryTexture:NULL
										  andAtmosphere:&atmosphere
											   withInfo:_materialParameters];

			OOSingleTextureMaterial *dynamicMaterial = [[OOSingleTextureMaterial alloc] initWithName:@"dynamic" texture:atmosphere configuration:nil];
			[_atmosphereDrawable setMaterial:dynamicMaterial];
			[dynamicMaterial release];
		}
	}
	else
	{
		if (isMoon)
		{
			[OOPlanetTextureGenerator generatePlanetTexture:&diffuseMap
										   secondaryTexture:(detailLevel >= DETAIL_LEVEL_EXTRAS) ? &normalMap : NULL
												   withInfo:_materialParameters];
		}
		else
		{
			OOTexture *atmosphere = nil;
			[OOPlanetTextureGenerator generatePlanetTexture:&diffuseMap
										   secondaryTexture:(detailLevel >= DETAIL_LEVEL_EXTRAS) ? &normalMap : NULL
											  andAtmosphere:&atmosphere
												   withInfo:_materialParameters];
			
			OOSingleTextureMaterial *dynamicMaterial = [[OOSingleTextureMaterial alloc] initWithName:@"dynamic" texture:atmosphere configuration:nil];
			[_atmosphereDrawable setMaterial:dynamicMaterial];
			[dynamicMaterial release];
		}
		if (shadersOn)
		{
			macros = [materialDefaults oo_dictionaryForKey:isMoon ? @"moon-dynamic-macros" : @"planet-dynamic-macros"];
		}
		textureName = @"dynamic";
	}
	OOMaterial *material = nil;
	
#if OO_SHADERS
	if (shadersOn)
	{
		NSMutableDictionary *config = [[[materialDefaults oo_dictionaryForKey:@"planet-material"] mutableCopy] autorelease];
		config[@"_oo_texture_objects"] = @[diffuseMap, normalMap];
		
		material = [OOShaderMaterial shaderMaterialWithName:textureName
											  configuration:config
													 macros:macros
											  bindingTarget:self];
	}
#endif
	if (material == nil)
	{
		material = [[OOSingleTextureMaterial alloc] initWithName:textureName texture:diffuseMap configuration:nil];
		[material autorelease];
	}
	[_planetDrawable setMaterial:material];
}


- (BOOL) setUpPlanetFromTexture:(NSString *)textureName
{
	[self setTextureFileName:textureName];
	return YES;
}


- (OOMaterial *) material
{
	return [_planetDrawable material];
}


- (OOMaterial *) atmosphereMaterial
{
	return [_atmosphereDrawable material];
}

@end

#endif	// NEW_PLANETS
