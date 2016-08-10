/*

OOPixMapTextureLoader.h

Load a texture from a pixmap. The loader takes ownership of the pixmap.


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

#import "OOTextureGenerator.h"
#import "OOPixMap.h"


@interface OOPixMapTextureLoader: OOTextureGenerator
{
@private
	OOPixMap			_pixMap;
	uint32_t			_texOptions;
}

- (instancetype)initWithPath:(NSString *)path options:(uint32_t)options UNAVAILABLE_ATTRIBUTE;
- (instancetype) initWithPixMap:(OOPixMap)pixMap textureOptions:(uint32_t)options freeWhenDone:(BOOL)freeWhenDone NS_DESIGNATED_INITIALIZER;

@end
