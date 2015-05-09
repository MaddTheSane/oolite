/*

ProxyPlayerEntity.m


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

#import "ProxyPlayerEntity.h"


@implementation ProxyPlayerEntity
@synthesize tradeInFactor = _tradeInFactor;
@synthesize trumbleCount = _trumbleCount;
@synthesize dialAftShield = _dialAftShield;
@synthesize dialForwardShield = _dialForwardShield;
@synthesize fuelLeakRate = _fuelLeakRate;

- (instancetype)initWithKey:(NSString *)key definition:(NSDictionary *)dict
{
	self = [super initWithKey:key definition:dict];
	if (self != nil)
	{
		[self setDialForwardShield:1.0f];
		[self setDialAftShield:1.0f];
		[self setDialFuelScoopStatus:[self hasScoop] ? SCOOP_STATUS_OKAY : SCOOP_STATUS_NOT_INSTALLED];
		[self setCompassMode:[self hasEquipmentItemProviding:@"EQ_ADVANCED_COMPASS"] ? COMPASS_MODE_PLANET : COMPASS_MODE_BASIC];
		[self setTradeInFactor:95];
	}
	
	return self;
}


- (void) copyValuesFromPlayer:(PlayerEntity *)player
{
	if (player == nil)  return;
	
	[self setFuelLeakRate:[player fuelLeakRate]];
	[self setMassLocked:[player massLocked]];
	[self setAtHyperspeed:[player atHyperspeed]];
	[self setDialForwardShield:[player dialForwardShield]];
	[self setDialAftShield:[player dialAftShield]];
	[self setDialMissileStatus:[player dialMissileStatus]];
	[self setDialFuelScoopStatus:[player dialFuelScoopStatus]];
	[self setCompassMode:[player compassMode]];
	[self setDialIdentEngaged:[player dialIdentEngaged]];
	[self setAlertCondition:[player alertCondition]];
	[self setTrumbleCount:[player trumbleCount]];
	[self setTradeInFactor:[player tradeInFactor]];

}


- (BOOL) isPlayerLikeShip
{
	return YES;
}


- (void) setFuelLeakRate:(float)value
{
	_fuelLeakRate = fmax(value, 0.0f);
}


- (BOOL) massLocked
{
	return _massLocked;
}

- (void) setMassLocked:(BOOL)value
{
	_massLocked = !!value;
}


- (BOOL) atHyperspeed
{
	return _atHyperspeed;
}

- (void) setAtHyperspeed:(BOOL)value
{
	_atHyperspeed = !!value;
}


@synthesize dialMissileStatus = _missileStatus;
@synthesize dialFuelScoopStatus = _fuelScoopStatus;
@synthesize compassMode = _compassMode;


- (BOOL) dialIdentEngaged
{
	return _dialIdentEngaged;
}

- (void) setDialIdentEngaged:(BOOL)value
{
	_dialIdentEngaged = !!value;
}

@synthesize alertCondition = _alertCondition;

// If you're here to add more properties, don't forget to update -copyValuesFromPlayer:.

@end


@implementation Entity (ProxyPlayer)

- (BOOL) isPlayerLikeShip
{
	return NO;
}

@end


@implementation PlayerEntity (ProxyPlayer)

- (BOOL) isPlayerLikeShip
{
	return YES;
}

@end
