/*

AI.h

Core NPC behaviour/artificial intelligence class.

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
#import "OOWeakReference.h"
#import "OOTypes.h"

#define AI_THINK_INTERVAL					0.125


@class ShipEntity;


@interface AI: OOWeakRefObject
{
@private
	id					_owner;						// OOWeakReference to the ShipEntity this is the AI for
	NSString			*ownerDesc;					// describes the object this is the AI for
	
	NSDictionary		*stateMachine;
	NSString			*stateMachineName;
	NSString			*currentState;
	NSMutableSet		*pendingMessages;
	
	NSMutableArray		*aiStack;
	
	OOTimeAbsolute		nextThinkTime;
	OOTimeDelta			thinkTimeInterval;
	
	NSString      *jsScript;
}

+ (AI *) currentlyRunningAI;
+ (NSString *) currentlyRunningAIDescription;

@property (readonly, copy, atomic) NSString *name;
@property (readonly, copy, atomic) NSString *associatedJS;
@property (copy, atomic) NSString *state;

- (void) setStateMachine:(NSString *)smName withJSScript:(NSString *)script;

- (void) setStateMachine:(NSString *)smName afterDelay:(NSTimeInterval)delay;
- (void) setState:(NSString *)stateName afterDelay:(NSTimeInterval)delay;

- (instancetype) initWithStateMachine:(NSString *) smName andState:(NSString *) stateName;

@property (strong, atomic) ShipEntity *owner;

- (void) preserveCurrentStateMachine;

- (void) restorePreviousStateMachine;

@property (readonly, atomic) BOOL hasSuspendedStateMachines;
- (void) exitStateMachineWithMessage:(NSString *)message;

@property (readonly, atomic) NSUInteger stackDepth;

- (void) reactToMessage:(NSString *) message;
// Immediately handle a message. This is the core dispatcher. DebugContext is a textual hint for diagnostics.
- (void) reactToMessage:(NSString *) message context:(NSString *)debugContext;

- (void) takeAction:(NSString *) action;

- (void) think;

- (void) message:(NSString *) ms;
- (void) dropMessage:(NSString *) ms;
@property (readonly, copy, atomic) NSSet *pendingMessages;
- (void) debugDumpPendingMessages;

@property (nonatomic) OOTimeAbsolute nextThinkTime;

@property OOTimeDelta thinkTimeInterval;

- (void) clearStack;

- (void) clearAllData;

- (void)dumpState;

@end
