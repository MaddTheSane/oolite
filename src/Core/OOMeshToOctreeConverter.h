/*

OOMeshToOctreeConverter.h

Class to manage the construction of octrees from triangle soups.


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
#import "OOMaths.h"


@class ShipEntity, Octree;


enum
{
	kOOMeshToOctreeConverterSmallDataCapacity = 16
};


@interface OOMeshToOctreeConverter: NSObject
{
@private
	struct OOMeshToOctreeConverterInternalData
	{
		Triangle			*triangles;
		uint_fast32_t		count;
		uint_fast32_t		capacity;
		uint_fast32_t		pendingCapacity;
		Triangle			smallData[kOOMeshToOctreeConverterSmallDataCapacity];
	}					_data;
}

- (instancetype) init UNAVAILABLE_ATTRIBUTE;
- (instancetype) initWithCapacity:(NSUInteger)capacity NS_DESIGNATED_INITIALIZER;
+ (instancetype) converterWithCapacity:(NSUInteger)capacity;

- (void) addTriangle:(Triangle)tri;

- (Octree *) findOctreeToDepth:(NSUInteger)depth;

@end
