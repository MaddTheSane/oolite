/*
OOShipGroup.h

A weak-referencing, mutable set of ships. Not thread safe.


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
#import "OOWeakReference.h"

@class ShipEntity;


@interface OOShipGroup: OOWeakRefObject
#if OOLITE_FAST_ENUMERATION
<NSFastEnumeration>
#endif
{
@private
	NSUInteger				_count, _capacity;
	unsigned long			_updateCount;
	OOWeakReference			**_members;
	OOWeakReference			*_leader;
	NSString				*_name;
	
	struct JSObject			*_jsSelf;
}

- (instancetype) init;
- (instancetype) initWithName:(NSString *)name NS_DESIGNATED_INITIALIZER;
+ (instancetype) groupWithName:(NSString *)name;
+ (instancetype) groupWithName:(NSString *)name leader:(ShipEntity *)leader;

@property (nonatomic, copy) NSString *name;

@property (atomic, strong) ShipEntity *leader;

@property (atomic, readonly, strong) NSEnumerator *objectEnumerator;
@property (atomic, readonly, strong) NSEnumerator *mutationSafeEnumerator;	// Enumerate over contents at time this is called, even if actual group is mutated.

@property (atomic, readonly, copy) NSSet *members;
@property (atomic, readonly, copy) NSArray *memberArray;	// arbitrary order
@property (atomic, readonly, copy) NSSet *membersExcludingLeader;
@property (atomic, readonly, copy) NSArray *memberArrayExcludingLeader;	// arbitrary order

- (BOOL) containsShip:(ShipEntity *)ship;
- (BOOL) addShip:(ShipEntity *)ship;
- (BOOL) removeShip:(ShipEntity *)ship;

@property (atomic, readonly) NSUInteger count;		// NOTE: this is O(n).
@property (atomic, readonly, getter=isEmpty) BOOL empty;

@end
