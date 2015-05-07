/*

OOColor.h

An RGBA colour in device colour space.


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
#import "OOOpenGL.h"


typedef struct
{
	float			r, g, b, a;
} OORGBAComponents;


typedef struct
{
	float			h, s, b, a;
} OOHSBAComponents;


@interface OOColor: NSObject <NSCopying>
{
@private
	float			rgba[4];
}

+ (nonnull OOColor *) colorWithHue:(float)hue saturation:(float)saturation brightness:(float)brightness alpha:(float)alpha;	// Note: hue in 0..1
+ (nonnull OOColor *) colorWithRed:(float)red green:(float)green blue:(float)blue alpha:(float)alpha;
+ (nonnull OOColor *) colorWithWhite:(float)white alpha:(float)alpha;
+ (nonnull OOColor *) colorWithRGBAComponents:(OORGBAComponents)components;
+ (nonnull OOColor *) colorWithHSBAComponents:(OOHSBAComponents)components;	// Note: hue in 0..360

// Flexible color creator; takes a selector name, a string with components, or an array.
+ (nullable OOColor *) colorWithDescription:(nonnull id)description;

// Like +colorWithDescription:, but forces brightness of at least 0.5.
+ (nullable OOColor *) brightColorWithDescription:(nonnull id)description;

/*	Like +colorWithDescription:, but multiplies saturation by provided factor.
	If the colour is an HSV dictionary, it may specify a saturation greater
	than 1.0 to override the scaling.
*/
+ (nullable OOColor *) colorWithDescription:(nonnull id)description saturationFactor:(float)factor;

// Creates a colour given a string with components.
+ (nullable OOColor *) colorFromString:(nonnull NSString*) colorFloatString;

+ (nonnull OOColor *) blackColor;		// 0.0 white
+ (nonnull OOColor *) darkGrayColor;	// 0.333 white
+ (nonnull OOColor *) lightGrayColor;	// 0.667 white
+ (nonnull OOColor *) whiteColor;		// 1.0 white
+ (nonnull OOColor *) grayColor;		// 0.5 white
+ (nonnull OOColor *) redColor;			// 1.0, 0.0, 0.0 RGB
+ (nonnull OOColor *) greenColor;		// 0.0, 1.0, 0.0 RGB
+ (nonnull OOColor *) blueColor;		// 0.0, 0.0, 1.0 RGB
+ (nonnull OOColor *) cyanColor;		// 0.0, 1.0, 1.0 RGB
+ (nonnull OOColor *) yellowColor;		// 1.0, 1.0, 0.0 RGB
+ (nonnull OOColor *) magentaColor;		// 1.0, 0.0, 1.0 RGB
+ (nonnull OOColor *) orangeColor;		// 1.0, 0.5, 0.0 RGB
+ (nonnull OOColor *) purpleColor;		// 0.5, 0.0, 0.5 RGB
+ (nonnull OOColor *) brownColor;		// 0.6, 0.4, 0.2 RGB
+ (nonnull OOColor *) clearColor;		// 0.0 white, 0.0 alpha

//	Linear blend in working colour space (no attempt at gamma correction).
- (nonnull OOColor *) blendedColorWithFraction:(float)fraction ofColor:(nonnull OOColor *)color;

//	Get the red, green, or blue components.
@property (atomic, readonly) float redComponent;
@property (atomic, readonly) float greenComponent;
@property (atomic, readonly) float blueComponent;
- (void) getRed:(nonnull float *)red green:(nonnull float *)green blue:(nonnull float *)blue alpha:(nonnull float *)alpha;

@property (atomic, readonly) OORGBAComponents rgbaComponents;

@property (atomic, readonly, getter=isBlack) BOOL black;
@property (atomic, readonly, getter=isWhite) BOOL white;

/*	Get the components as hue, saturation, or brightness.
	
	IMPORTANT: for reasons of bugwards compatibility, these return hue values
	in the range [0, 360], but +colorWithedHue:... expects values in the
	range [0, 1].
*/
@property (atomic, readonly) float hueComponent;
@property (atomic, readonly) float saturationComponent;
@property (atomic, readonly) float brightnessComponent;
- (void) getHue:(nonnull float *)hue saturation:(nonnull float *)saturation brightness:(nonnull float *)brightness alpha:(nonnull float *)alpha;

@property (atomic, readonly) OOHSBAComponents hsbaComponents;


// Get the alpha component.
@property (atomic, readonly) float alphaComponent;

/*	Returns the colour, premultiplied by its alpha channel, and with an alpha
	of 1.0. If the reciever's alpha is 1.0, it will return itself.
*/
@property (atomic, readonly, assign, nonnull) OOColor *premultipliedColor;

// Multiply r, g and b components of a colour by specified factor, clamped to [0..1].
- (nonnull OOColor *) colorWithBrightnessFactor:(float)factor;

// r,g,b,a array in 0..1 range.
@property (atomic, readonly, copy, nonnull) NSArray *normalizedArray;

@property (atomic, readonly, copy, nonnull) NSString *rgbaDescription;
@property (atomic, readonly, copy, nonnull) NSString *hsbaDescription;

@end


NSString *__nonnull OORGBAComponentsDescription(OORGBAComponents components);
NSString *__nonnull OOHSBAComponentsDescription(OOHSBAComponents components);
