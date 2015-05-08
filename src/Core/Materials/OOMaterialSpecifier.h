/*

OOMaterialSpecifier.h

Key declarations and convenience methods for material specifiers.

 
Copyright (C) 2010-2013 Jens Ayton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#import "OOCocoa.h"

@class OOColor;


//	Convenience methods to extract properties from material dictionaries.
@interface NSDictionary (OOMateralProperties)

@property (readonly, copy) OOColor *oo_diffuseColor;
@property (readonly, copy) OOColor *oo_ambientColor;
@property (readonly, copy) OOColor *oo_specularColor;
@property (readonly, copy) OOColor *oo_specularModulateColor;
@property (readonly, copy) OOColor *oo_emissionColor;
@property (readonly, copy) OOColor *oo_emissionModulateColor;
@property (readonly, copy) OOColor *oo_illuminationModulateColor;

- (NSDictionary *) oo_diffuseMapSpecifierWithDefaultName:(NSString *)name;
@property (readonly, copy) NSDictionary *oo_combinedSpecularMapSpecifier;
@property (readonly, copy) NSDictionary *oo_specularColorMapSpecifier;
@property (readonly, copy) NSDictionary *oo_specularExponentMapSpecifier;
@property (readonly, copy) NSDictionary *oo_normalMapSpecifier;
@property (readonly, copy) NSDictionary *oo_parallaxMapSpecifier;
@property (readonly, copy) NSDictionary *oo_normalAndParallaxMapSpecifier;
@property (readonly, copy) NSDictionary *oo_emissionMapSpecifier;
@property (readonly, copy) NSDictionary *oo_illuminationMapSpecifier;
@property (readonly, copy) NSDictionary *oo_emissionAndIlluminationMapSpecifier;

@property (readonly) float oo_parallaxScale;
@property (readonly) float oo_parallaxBias;

@property (readonly) int oo_specularExponent;

@end


extern NSString * const kOOMaterialDiffuseColorName;
extern NSString * const kOOMaterialDiffuseColorLegacyName;
extern NSString * const kOOMaterialAmbientColorName;
extern NSString * const kOOMaterialAmbientColorLegacyName;
extern NSString * const kOOMaterialSpecularColorName;
extern NSString * const kOOMaterialSpecularColorLegacyName;
extern NSString * const kOOMaterialSpecularModulateColorName;
extern NSString * const kOOMaterialEmissionColorName;
extern NSString * const kOOMaterialEmissionColorLegacyName;
extern NSString * const kOOMaterialEmissionModulateColorName;
extern NSString * const kOOMaterialIlluminationModulateColorName;

extern NSString * const kOOMaterialDiffuseMapName;
extern NSString * const kOOMaterialSpecularColorMapName;
extern NSString * const kOOMaterialSpecularExponentMapName;
extern NSString * const kOOMaterialCombinedSpecularMapName;
extern NSString * const kOOMaterialNormalMapName;
extern NSString * const kOOMaterialParallaxMapName;
extern NSString * const kOOMaterialNormalAndParallaxMapName;
extern NSString * const kOOMaterialEmissionMapName;
extern NSString * const kOOMaterialIlluminationMapName;
extern NSString * const kOOMaterialEmissionAndIlluminationMapName;

extern NSString * const kOOMaterialParallaxScaleName;
extern NSString * const kOOMaterialParallaxBiasName;

extern NSString * const kOOMaterialSpecularExponentName;
extern NSString * const kOOMaterialSpecularExponentLegacyName;

extern NSString * const kOOMaterialLightMapsName;

#define kOOMaterialDefaultParallaxScale		(0.01f)
