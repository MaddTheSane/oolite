/*

ProxyPlayerEntity.h

Ship entity which, in some respects, emulates a PlayerShip. In particular, at
this time it implements the extra shader bindable methods of PlayerShip.


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

#import "PlayerEntity.h"


@interface ProxyPlayerEntity: ShipEntity
{
@private
	OOMissileStatus			_missileStatus;
	OOFuelScoopStatus		_fuelScoopStatus;
	OOCompassMode			_compassMode;
	OOAlertCondition		_alertCondition;
	unsigned				_massLocked: 1,
							_atHyperspeed: 1,
							_dialIdentEngaged: 1;
}
@property NSInteger		tradeInFactor;
// Default: 0
@property NSUInteger	trumbleCount;
// Default: 0
@property (nonatomic) float			fuelLeakRate;
// Default: 1
@property GLfloat		dialForwardShield;
// Default: 1
@property GLfloat		dialAftShield;

- (void) copyValuesFromPlayer:(PlayerEntity *)player;

// Default: NO
@property  BOOL massLocked;

// Default: NO
@property  BOOL atHyperspeed;

// Default: MISSILE_STATUS_SAFE
@property  OOMissileStatus dialMissileStatus;

// Default: SCOOP_STATUS_NOT_INSTALLED or SCOOP_STATUS_OKAY depending on equipment.
@property  OOFuelScoopStatus dialFuelScoopStatus;

// Default: COMPASS_MODE_BASIC or COMPASS_MODE_PLANET depending on equipment.
@property  OOCompassMode compassMode;

// Default: NO
@property  BOOL dialIdentEngaged;

// Default: ALERT_CONDITION_DOCKED
@property  OOAlertCondition alertCondition;

@end


@interface Entity (ProxyPlayer)

// True for PlayerEntity or ProxyPlayerEntity.
@property (getter=isPlayerLikeShip, readonly) BOOL playerLikeShip;

@end
