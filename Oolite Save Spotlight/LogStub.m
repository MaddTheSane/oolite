//
//  LogStub.m
//  Oolite Save Spotlight
//
//  Created by C.W. Betts on 4/5/23.
//  Copyright © 2023 the Oolite team. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "NSScannerOOExtensions.h"
#import "OOCollectionExtractors.h"
#import "NSDataOOExtensions.h"

/* Disable OOLog. The only logging in the importer at the time of writing is
   "File not found" logging in NSDataOOExtensions, which is not an error. In
   general, we are unlikely to want logging from the importer.
 */
void OOLogWithFunctionFileAndLine(NSString *inMessageClass, const char *inFunction, const char *inFile, unsigned long inLine, NSString *inFormat, ...)
{
	
}


NSString * const kOOLogFileNotFound = @"";
