/*
 
 OOVisualEffectEntity.h
 
 Entity subclass representing a visual effect with a custom mesh
 
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

#import "OOEntityWithDrawable.h"
#import "OOPlanetEntity.h"
#import "OOJSPropID.h"
#import "HeadUpDisplay.h"
#import "OOWeakReference.h"

@class	OOColor, OOMesh, OOScript, OOJSScript;


@interface OOVisualEffectEntity: OOEntityWithDrawable <OOSubEntity,OOBeaconEntity>
{
@private
	NSMutableArray			*subEntities;

	NSDictionary			*effectinfoDictionary;

	GLfloat					_profileRadius; // for frustum culling

	OOColor					*scanner_display_color1;
	OOColor					*scanner_display_color2;

	GLfloat         _hullHeatLevel;
	GLfloat         _shaderFloat1;
	GLfloat         _shaderFloat2;
	int             _shaderInt1;
	int             _shaderInt2;
	Vector          _shaderVector1;
	Vector          _shaderVector2;

	Vector _v_forward;
	Vector _v_up;
	Vector _v_right;

	OOJSScript				*script;
	NSDictionary			*scriptInfo;

	NSString				*_effectKey;

	BOOL            _haveExecutedSpawnAction;

	// beacons
	NSString				*_beaconCode;
	NSString				*_beaconLabel;
	OOWeakReference			*_prevBeacon;
	OOWeakReference			*_nextBeacon;
	id <OOHUDBeaconIcon>	_beaconDrawable;

	// scaling
	GLfloat scaleX;
	GLfloat scaleY;
	GLfloat scaleZ;

}

- (instancetype)initWithKey:(NSString *)key definition:(NSDictionary *) dict NS_DESIGNATED_INITIALIZER;
- (BOOL) setUpVisualEffectFromDictionary:(NSDictionary *) effectDict;

@property (atomic, strong) OOMesh *mesh;

@property (readonly, copy) NSString *effectKey;

@property (atomic, readonly) GLfloat frustumRadius;

- (void) clearSubEntities;
@property (atomic, readonly) BOOL setUpSubEntities;
- (void) removeSubEntity:(Entity<OOSubEntity> *)sub;
- (void) setNoDrawDistance;
@property (readonly, copy) NSArray *subEntities;
@property (atomic, readonly) NSUInteger subEntityCount;
@property (atomic, readonly, strong) NSEnumerator *visualEffectSubEntityEnumerator;
- (BOOL) hasSubEntity:(Entity<OOSubEntity> *)sub;

@property (atomic, readonly, strong) NSEnumerator *subEntityEnumerator;
@property (atomic, readonly, strong) NSEnumerator *effectSubEntityEnumerator;
@property (atomic, readonly, strong) NSEnumerator *flasherEnumerator;

- (void) orientationChanged;
@property (readonly) Vector forwardVector;
@property (readonly) Vector rightVector;
@property (readonly) Vector upVector;

@property (nonatomic, copy) OOColor *scannerDisplayColor1;
@property (nonatomic, copy) OOColor *scannerDisplayColor2;
- (GLfloat *) scannerDisplayColorForShip:(BOOL)flash :(OOColor *)scannerDisplayColor1 :(OOColor *)scannerDisplayColor2 NS_RETURNS_INNER_POINTER;

- (void) setScript:(NSString *)script_name;
- (OOJSScript *)script;
@property (atomic, readonly, copy) NSDictionary *scriptInfo;
- (void) doScriptEvent:(jsid)message;
- (void) remove;

@property (readonly, atomic) GLfloat scaleMax; // used for calculating frustum cull size
@property (nonatomic) GLfloat scaleX;
@property (nonatomic) GLfloat scaleY;
@property (nonatomic) GLfloat scaleZ;

// convenience for shaders
@property (nonatomic) GLfloat hullHeatLevel;
// shader properties
@property  GLfloat shaderFloat1;
@property  GLfloat shaderFloat2; 
@property  int shaderInt1; 
@property  int shaderInt2;
@property  Vector shaderVector1; 
@property  Vector shaderVector2; 


@property (atomic) BOOL isBreakPattern;

@property (readonly, copy) NSDictionary *effectInfoDictionary;


@end
