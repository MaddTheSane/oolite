/*

NSMutableDictionaryOOExtensions.m

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

#import <Foundation/Foundation.h>
#import "NSMutableDictionaryOOExtensions.h"

@implementation NSMutableDictionary (OOExtensions)

- (void)mergeEntriesFromDictionary:(NSDictionary *)otherDictionary
{
	NSEnumerator	*otherKeysEnum = nil;
	id				key = nil;
	 
	for (otherKeysEnum = [otherDictionary keyEnumerator]; (key = [otherKeysEnum nextObject]); )
	{
		if (!self[key])
			self[key] = otherDictionary[key];
		else
		{
			BOOL merged = NO;
			id thisObject = self[key];
			id otherObject = otherDictionary[key];
			
			if ([thisObject isKindOfClass:[NSDictionary class]]&&[otherObject isKindOfClass:[NSDictionary class]]&&(![thisObject isEqual:otherObject]))
			{
				NSMutableDictionary* mergeObject = [NSMutableDictionary dictionaryWithDictionary:(NSDictionary*)thisObject];
				[mergeObject mergeEntriesFromDictionary:(NSDictionary*)otherObject];
				self[key] = mergeObject;
				merged = YES;
			}
			
			if ([thisObject isKindOfClass:[NSArray class]]&&[otherObject isKindOfClass:[NSArray class]]&&(![thisObject isEqual:otherObject]))
			{
				NSMutableArray* mergeObject = [NSMutableArray arrayWithArray:(NSArray*)thisObject];
				[mergeObject addObjectsFromArray:(NSArray*)otherObject];
				self[key] = mergeObject;
				merged = YES;
			}
			
			if (!merged)
				self[key] = otherObject;
		}
	}	
}

@end
