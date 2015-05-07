/*

OOCharacter.h

Represents an NPC person (as opposed to an NPC ship).

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
#import "OOTypes.h"
#import "legacy_random.h"
#import "OOJSPropID.h"

@class OOJSScript;


@interface OOCharacter: NSObject
{
@private
	NSString			*_name;
	NSString			*_shortDescription;
	OOSystemID			_originSystem;
	Random_Seed			_genSeed;
	int					_legalStatus;
	OOCreditsQuantity	_insuranceCredits;
	NSArray				*_scriptActions;
	OOJSScript			*_script;
}

- (instancetype) initWithRole:(NSString *)role andOriginalSystem:(OOSystemID)s;

+ (instancetype) characterWithRole:(NSString *)c_role andOriginalSystem:(OOSystemID)s;
+ (instancetype) randomCharacterWithRole:(NSString *)c_role andOriginalSystem:(OOSystemID)s;
+ (instancetype) characterWithDictionary:(NSDictionary *)c_dict;

@property (atomic, readonly, copy) NSString *planetOfOrigin;
@property (nonatomic, readonly) OOSystemID planetIDOfOrigin;
@property (atomic, readonly, copy) NSString *species;

- (void) basicSetUp;
- (BOOL) castInRole:(NSString *)role;

@property (copy) NSString *name;
@property (copy) NSString *shortDescription;
@property int legalStatus;
@property OOCreditsQuantity insuranceCredits;

@property (nonatomic, copy) NSArray *legacyScript;
@property (readonly, retain) OOJSScript *script;
- (void) setCharacterScript:(NSString *)scriptName;
- (void) doScriptEvent:(jsid)message;

- (NSDictionary *) infoForScripting;

@end
