/*

OOFlasherEntity.h

Flashing light attached to ships.


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

#import "OOLightParticleEntity.h"
#import "ShipEntity.h"


@interface OOFlasherEntity: OOLightParticleEntity <OOSubEntity>
{
@private
	float					_frequency;
	float					_phase;
	float					_wave;
	float         			_brightfraction;
	NSArray					*_colors;
	NSUInteger				_activeColor;
	
	OOTimeDelta				_time;
	
	BOOL					_active;
	BOOL					_justSwitched;
}

- (instancetype) initWithDiameter:(float)diameter UNAVAILABLE_ATTRIBUTE;
+ (instancetype) flasherWithDictionary:(NSDictionary *)dictionary;
- (instancetype) initWithDictionary:(NSDictionary *)dictionary NS_DESIGNATED_INITIALIZER;

@property (getter=isActive) BOOL active;

@property (atomic, readonly, copy) OOColor *color;
// setColor is defined by superclass

@property float frequency;

@property float phase;

@property float fraction;


@end


@interface Entity (OOFlasherEntityExtensions)

@property (atomic, getter=isFlasher, readonly) BOOL flasher;

@end

