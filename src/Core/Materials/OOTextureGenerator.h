/*

OOTextureGenerator.h

A texture "loader" which doesn't require an input file.


Copyright (C) 2007-2013 Jens Ayton

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

#import "OOTextureLoader.h"


typedef struct
{
	float			r, g, b;
} FloatRGB;


typedef struct
{
	float			r, g, b, a;
} FloatRGBA;


@interface OOTextureGenerator: OOTextureLoader

// Generators, unlike normal loaders, get to specify their own flags and other settings.
@property (atomic, readonly) uint32_t textureOptions;	// Default: kOOTextureDefaultOptions
@property (atomic, readonly) GLfloat anisotropy;			// Default: kOOTextureDefaultAnisotropy
@property (atomic, readonly) GLfloat lodBias;			// Default: kOOTextureDefaultLODBias

// Key for in-memory cache; nil for no cache.
@property (atomic, readonly, copy) NSString *cacheKey;

// For use by OOTexture.
@property (atomic, readonly) BOOL enqueue;

@end
