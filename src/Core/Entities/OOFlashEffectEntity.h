/*

OOFlashEffectEntity.h

Flashes during explosions and laser hits - not to be confused with flashers.


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


@interface OOFlashEffectEntity: OOLightParticleEntity 
{
@private
	float				_duration;
	float				_growthRate;
	float				_alpha;
}

+ (instancetype) explosionFlashFromEntity:(Entity *)entity;
+ (instancetype) laserFlashWithPosition:(HPVector)position velocity:(Vector)vel color:(OOColor *)color;

+ (void) setUpTexture;

@end
