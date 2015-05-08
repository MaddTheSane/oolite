/*

OOPlanetDrawable.h

Draw a ball, such as might be used to represent a planet.

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

#import "OODrawable.h"
#import "OOMaths.h"

@class OOMaterial;


@interface OOPlanetDrawable: OODrawable <NSCopying>
{
@private
	OOMaterial				*_material;
	BOOL					_isAtmosphere;
	float					_radius;
	OOMatrix				_transform;
	unsigned				_lod;
}

+ (instancetype) planetWithTextureName:(NSString *)textureName radius:(float)radius;
+ (instancetype) atmosphereWithRadius:(float)radius;

- (instancetype) initAsAtmosphere;

@property (strong) OOMaterial *material;

@property (copy) NSString *textureName;

// Radius, in game metres.
@property  float radius;

// Level of detail, [0..1]. Granularity is implementation-defined.
@property  float levelOfDetail;
- (void) calculateLevelOfDetailForViewDistance:(float)distance;

// depth-buffer hack
- (void) renderTranslucentPartsOnOpaquePass;

@end
