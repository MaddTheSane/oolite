/*

ResourceManager.m

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

#import "ResourceManager.h"
#import "NSScannerOOExtensions.h"
#import "NSMutableDictionaryOOExtensions.h"
#import "NSStringOOExtensions.h"
#import "OOSound.h"
#import "OOCacheManager.h"
#import "Universe.h"
#import "OOStringParsing.h"
#import "OOPListParsing.h"
#import "MyOpenGLView.h"
#import "OOCollectionExtractors.h"
#import "OOLogOutputHandler.h"
#import "NSFileManagerOOExtensions.h"
#import "OldSchoolPropertyListWriting.h"
#import "OOOXZManager.h"

#import "OOJSScript.h"
#import "OOPListScript.h"

#import "OOManifestProperties.h"

static NSString * const kOOLogCacheUpToDate				= @"dataCache.upToDate";
static NSString * const kOOLogCacheExplicitFlush		= @"dataCache.rebuild.explicitFlush";
static NSString * const kOOLogCacheStalePaths			= @"dataCache.rebuild.pathsChanged";
static NSString * const kOOLogCacheStaleDates			= @"dataCache.rebuild.datesChanged";
static NSString * const kOOCacheSearchPathModDates		= @"search path modification dates";
static NSString * const kOOCacheKeySearchPaths			= @"search paths";
static NSString * const kOOCacheKeyModificationDates	= @"modification dates";



extern NSDictionary* ParseOOSScripts(NSString* script);


@interface ResourceManager (OOPrivate)

+ (void) checkOXPMessagesInPath:(NSString *)path;
+ (void) checkPotentialPath:(NSString *)path :(NSMutableArray *)searchPaths;
+ (BOOL) validateManifest:(NSDictionary*)manifest forOXP:(NSString *)path;
+ (BOOL) areRequirementsFulfilled:(NSDictionary*)requirements forOXP:(NSString *)path andFile:(NSString *)file;
+ (void) filterSearchPathsForConflicts:(NSMutableArray *)searchPaths;
+ (BOOL) filterSearchPathsForRequirements:(NSMutableArray *)searchPaths;
+ (BOOL) matchVersions:(NSDictionary *)rangeDict withVersion:(NSString *)version;
+ (void) addErrorWithKey:(NSString *)descriptionKey param1:(id)param1 param2:(id)param2;
+ (void) checkCacheUpToDateForPaths:(NSArray *)searchPaths;
+ (void) logPaths;
+ (void) mergeRoleCategories:(NSDictionary *)catData intoDictionary:(NSMutableDictionary *)category;

@end


static NSMutableArray	*sSearchPaths;
static BOOL				sUseAddOns = YES;
static BOOL				sFirstRun = YES;
static NSMutableArray	*sOXPsWithMessagesFound;
static NSMutableArray	*sExternalPaths;
static NSMutableArray	*sErrors;
static NSMutableDictionary *sOXPManifests;

// caches allow us to load any given file once only
//
static NSMutableDictionary *sSoundCache;
static NSMutableDictionary *sStringCache;



@implementation ResourceManager

+ (void) reset
{
	sFirstRun = YES;
	sUseAddOns = YES;
	DESTROY(sSearchPaths);
	DESTROY(sOXPsWithMessagesFound);
	DESTROY(sExternalPaths);
	DESTROY(sErrors);
	DESTROY(sOXPManifests);
}


+ (NSString *) errors
{
	NSArray					*error = nil;
	NSUInteger				i, count;
	NSMutableArray			*result = nil;
	NSString				*errStr = nil;
	
	count = [sErrors count];
	if (count == 0)  return nil;
	
	// Expand error messages. This is deferred for localizability.
	result = [NSMutableArray arrayWithCapacity:count];
	for (i = 0; i != count; ++i)
	{
		error = sErrors[i];
		errStr = [UNIVERSE descriptionForKey:[error oo_stringAtIndex:0]];
		if (errStr != nil)
		{
			errStr = [NSString stringWithFormat:errStr, error[1], error[2]];
			[result addObject:errStr];
		}
	}
	
	[sErrors release];
	sErrors = nil;
	
	return [result componentsJoinedByString:@"\n"];
}


+ (NSArray *)rootPaths
{
	static NSArray			*sRootPaths = nil;
	
	if (sRootPaths == nil)
	{
		// the paths are now in order of preference as per yesterday's talk. -- Kaks 2010-05-05
		
		sRootPaths = [[NSArray alloc] initWithObjects:[self builtInPath],

	  /* 1st path - ensures manually installed OXZs load later than
	   * managed ones, which may be useful for debugging/testing
	   * purposes. */
							[[OOOXZManager sharedManager] installPath],

#if OOLITE_MAC_OS_X
	/* 2nd mac path */		[[[[NSHomeDirectory() stringByAppendingPathComponent:@"Library"]
								stringByAppendingPathComponent:@"Application Support"]
								stringByAppendingPathComponent:@"Oolite"]
								stringByAppendingPathComponent:@"AddOns"],
	/* 3rd mac path */		[[[[NSBundle mainBundle] bundlePath]
								stringByDeletingLastPathComponent]
								stringByAppendingPathComponent:@"AddOns"],

#elif OOLITE_WINDOWS
	/* 2nd windows path */		@"../AddOns",
#else	
	/* 2nd *nix path */		@"AddOns",
#endif

#if !OOLITE_WINDOWS
	/*	3rd *nix path, 4th mac path */
							[[NSHomeDirectory()
								stringByAppendingPathComponent:@".Oolite"]
								stringByAppendingPathComponent:@"AddOns"],
#endif

		
						nil];
	}
	OOLog(@"paths.debug",@"%@",sRootPaths);
	return sRootPaths;
}


+ (NSString *)builtInPath
{
#if OOLITE_WINDOWS
	/*	[[NSBundle mainBundle] resourcePath] causes complaints under Windows,
		because we don't have a properly-built bundle.
	*/
	return @"Resources";
#else
	return [[NSBundle mainBundle] resourcePath];
#endif
}


+ (NSArray *)pathsWithAddOns
{
	if ([sSearchPaths count] > 0)  return sSearchPaths;
	
	[sErrors release];
	sErrors = nil;
	
	NSFileManager			*fmgr = [NSFileManager defaultManager];
	NSArray					*rootPaths = nil;
	NSMutableArray			*existingRootPaths = nil;
	NSEnumerator			*pathEnum = nil;
	NSString				*root = nil;
	NSDirectoryEnumerator	*dirEnum = nil;
	NSString				*subPath = nil;
	NSString				*path = nil;
	BOOL					isDirectory;
	
	// Copy those root paths that actually exist to search paths.
	rootPaths = [self rootPaths];
	existingRootPaths = [NSMutableArray arrayWithCapacity:[rootPaths count]];
	for (pathEnum = [rootPaths objectEnumerator]; (root = [pathEnum nextObject]); )
	{
		if ([fmgr fileExistsAtPath:root isDirectory:&isDirectory] && isDirectory)
		{
			[existingRootPaths addObject:root];
		}
	}
	
	// validate default search paths
	sSearchPaths = [NSMutableArray new];
	foreach(path, existingRootPaths)
	{
		[self checkPotentialPath:path :sSearchPaths];
	}
	
	// Iterate over root paths.
	for (pathEnum = [existingRootPaths objectEnumerator]; (root = [pathEnum nextObject]); )
	{
		// Iterate over each root path's contents.
		if ([fmgr fileExistsAtPath:root isDirectory:&isDirectory] && isDirectory)
		{
			for (dirEnum = [fmgr enumeratorAtPath:root]; (subPath = [dirEnum nextObject]); )
			{
				// Check if it's a directory.
				path = [root stringByAppendingPathComponent:subPath];
				if ([fmgr fileExistsAtPath:path isDirectory:&isDirectory])
				{
					if (isDirectory)
					{
						// If it is, is it an OXP?.
						if ([[[path pathExtension] lowercaseString] isEqualToString:@"oxp"])
						{
							[self checkPotentialPath:path :sSearchPaths];
							if ([sSearchPaths containsObject:path])  [self checkOXPMessagesInPath:path];
						}
						else
						{
							// If not, don't search subdirectories.
							[dirEnum skipDescendents];
						}
					}
					else
					{
						// If not a directory, is it an OXZ?
						if ([[[path pathExtension] lowercaseString] isEqualToString:@"oxz"])
						{
							[self checkPotentialPath:path :sSearchPaths];
							if ([sSearchPaths containsObject:path])  [self checkOXPMessagesInPath:path];
						}
					}
				}
			}
		}
	}
	
	for (pathEnum = [sExternalPaths objectEnumerator]; (path = [pathEnum nextObject]); )
	{
		[self checkPotentialPath:path :sSearchPaths];
		if ([sSearchPaths containsObject:path])  [self checkOXPMessagesInPath:path];
	}

	/* This is a conservative filter. It probably gets rid of more
	 * OXPs than it technically needs to in certain situations with
	 * dependency chains, but really any conflict here needs to be
	 * resolved by the user rather than Oolite. The point is to avoid
	 * loading OXPs which we shouldn't; if doing so takes out other
	 * OXPs which would have been safe, that's not important. */
	[self filterSearchPathsForConflicts:sSearchPaths];
	/* This one needs to be run repeatedly to be sure. Take the chain
	 * A depends on B depends on C. A and B are installed. A is
	 * checked first, and depends on B, which is thought to be
	 * okay. So A is kept. Then B is checked and removed. A must then
	 * be rechecked. This function therefore is run repeatedly until a
	 * run of it removes no further items.
	 *
	 * There may well be more elegant and efficient ways to do this
	 * but this is already fast enough for most purposes.
	 */
	while (![self filterSearchPathsForRequirements:sSearchPaths]) {}

	[self checkCacheUpToDateForPaths:sSearchPaths];
	
	return sSearchPaths;
}


+ (NSArray *)paths
{
	if (EXPECT_NOT(sSearchPaths == nil))
	{
		if (!sUseAddOns)
		{
			sSearchPaths = [[NSMutableArray alloc] init];
		}
	}
	return sUseAddOns ? [self pathsWithAddOns] : (NSArray *)@[[self builtInPath]];
}


+ (BOOL)useAddOns
{
	return sUseAddOns;
}


+ (void)setUseAddOns:(BOOL)useAddOns
{
	if (sFirstRun || sUseAddOns != useAddOns)
	{
		sFirstRun = NO;
		sUseAddOns = useAddOns;
		[ResourceManager clearCaches];
		
		OOCacheManager *cmgr = [OOCacheManager sharedCache];
		if (sUseAddOns)
		{
			[cmgr reloadAllCaches];
			[cmgr setAllowCacheWrites:YES];
		}
		else
		{
			[cmgr clearAllCaches];
			[cmgr setAllowCacheWrites:NO];
		}
		
		[self checkCacheUpToDateForPaths:[self paths]];
		[self logPaths];
	}
}


+ (void) addExternalPath:(NSString *)path
{
	if (sSearchPaths == nil)  sSearchPaths = [[NSMutableArray alloc] init];
	if (![sSearchPaths containsObject:path])
	{
		[sSearchPaths addObject:path];
		
		if (sExternalPaths == nil)  sExternalPaths = [[NSMutableArray alloc] init];
		[sExternalPaths addObject:path];
	}
}


+ (NSEnumerator *)pathEnumerator
{
	return [[self paths] objectEnumerator];
}


+ (NSEnumerator *)reversePathEnumerator
{
	return [[self paths] reverseObjectEnumerator];
}


+ (NSArray *)OXPsWithMessagesFound
{
	return [[sOXPsWithMessagesFound copy] autorelease];
}


+ (NSDictionary *)manifestForIdentifier:(NSString *)identifier
{
	return [sOXPManifests objectForKey:identifier];
}


+ (void) checkOXPMessagesInPath:(NSString *)path
{
	NSArray *OXPMessageArray = OOArrayFromFile([path stringByAppendingPathComponent:@"OXPMessages.plist"]);
	
	if ([OXPMessageArray count] > 0)
	{
		unsigned i;
		for (i = 0; i < [OXPMessageArray count]; i++)
		{
			NSString *oxpMessage = [OXPMessageArray oo_stringAtIndex:i];
			if (oxpMessage)
			{
				OOLog(@"oxp.message", @"%@: %@", path, oxpMessage);
			}
		}
		if (sOXPsWithMessagesFound == nil)  sOXPsWithMessagesFound = [[NSMutableArray alloc] init];
		[sOXPsWithMessagesFound addObject:[path lastPathComponent]];
	}
}


// Given a path to an assumed OXP (or other location where files are permissible), check for a requires.plist or manifest.plist and add to search paths if acceptable.
+ (void)checkPotentialPath:(NSString *)path :(NSMutableArray *)searchPaths
{
	NSDictionary			*requirements = nil;
	NSDictionary			*manifest = nil;
	BOOL					requirementsMet = YES;

	if (![[[path pathExtension] lowercaseString] isEqualToString:@"oxz"])
	{
		// OXZ format ignores requires.plist
		requirements = OODictionaryFromFile([path stringByAppendingPathComponent:@"requires.plist"]);
		requirementsMet = [self areRequirementsFulfilled:requirements forOXP:path andFile:@"requires.plist"];
	}
	if (!requirementsMet)
	{
		NSString *version = [[NSBundle mainBundle] infoDictionary][@"CFBundleVersion"];
		OOLog(@"oxp.versionMismatch", @"OXP %@ is incompatible with version %@ of Oolite.", path, version);
		[self addErrorWithKey:@"oxp-is-incompatible" param1:[path lastPathComponent] param2:version];
		return;
	}
	
	manifest = OODictionaryFromFile([path stringByAppendingPathComponent:@"manifest.plist"]);
	if (manifest == nil)
	{
		if ([[[path pathExtension] lowercaseString] isEqualToString:@"oxz"])
		{
			OOLog(@"oxp.noManifest", @"OXZ %@ has no manifest.plist", path);
			[self addErrorWithKey:@"oxz-lacks-manifest" param1:[path lastPathComponent] param2:nil];
			return;
		}
	}
	else
	{
		requirementsMet = [self validateManifest:manifest forOXP:path];
	}

	if (requirementsMet) 
	{
		[searchPaths addObject:path];
	}
}


+ (BOOL) validateManifest:(NSDictionary*)manifest forOXP:(NSString *)path
{
	if (EXPECT_NOT(sOXPManifests == nil))
	{
		sOXPManifests = [[NSMutableDictionary alloc] initWithCapacity:32];
	}
	
	BOOL 		OK = YES;
	NSString 	*identifier = [manifest oo_stringForKey:kOOManifestIdentifier defaultValue:nil];
	NSString 	*version = [manifest oo_stringForKey:kOOManifestVersion defaultValue:nil];
	NSString 	*required = [manifest oo_stringForKey:kOOManifestRequiredOoliteVersion defaultValue:nil];
	NSString	*title = [manifest oo_stringForKey:kOOManifestTitle defaultValue:nil];

	if (identifier == nil)
	{
		OOLog(@"oxp.noManifest", @"OXZ %@ manifest.plist has no '%@' field.", path, kOOManifestIdentifier);
		[self addErrorWithKey:@"oxp-manifest-incomplete" param1:title param2:kOOManifestIdentifier];
		OK = NO;
	}
	if (version == nil)
	{
		OOLog(@"oxp.noManifest", @"OXZ %@ manifest.plist has no '%@' field.", path, kOOManifestVersion);
		[self addErrorWithKey:@"oxp-manifest-incomplete" param1:title param2:kOOManifestVersion];
		OK = NO;
	}
	if (required == nil)
	{
		OOLog(@"oxp.noManifest", @"OXZ %@ manifest.plist has no '%@' field.", path, kOOManifestRequiredOoliteVersion);
		[self addErrorWithKey:@"oxp-manifest-incomplete" param1:title param2:kOOManifestRequiredOoliteVersion];
		OK = NO;
	}
	if (title == nil)
	{
		OOLog(@"oxp.noManifest", @"OXZ %@ manifest.plist has no '%@' field.", path, kOOManifestTitle);
		[self addErrorWithKey:@"oxp-manifest-incomplete" param1:title param2:kOOManifestTitle];
		OK = NO;
	}
	if (!OK)
	{
		return NO;
	}
	OK = [self checkVersionCompatibility:manifest forOXP:title];

	if (!OK)
	{
		NSString *version = [[NSBundle mainBundle] infoDictionary][@"CFBundleVersion"];
		OOLog(@"oxp.versionMismatch", @"OXP %@ is incompatible with version %@ of Oolite.", path, version);
		[self addErrorWithKey:@"oxp-is-incompatible" param1:[path lastPathComponent] param2:version];
		return NO;
	}

	NSDictionary *duplicate = sOXPManifests[identifier];
	if (duplicate != nil)
	{
		OOLog(@"oxp.duplicate", @"OXP %@ has the same identifier (%@) as %@ which has already been loaded.",path,identifier,[duplicate oo_stringForKey:kOOManifestFilePath]);
		[self addErrorWithKey:@"oxp-manifest-duplicate" param1:path param2:[duplicate oo_stringForKey:kOOManifestFilePath]];
		return NO;
	}
	NSMutableDictionary *mData = [NSMutableDictionary dictionaryWithDictionary:manifest];
	mData[kOOManifestFilePath] = path;
	// add an extra key
	sOXPManifests[identifier] = mData;
	return YES;
}


+ (BOOL) checkVersionCompatibility:(NSDictionary *)manifest forOXP:(NSString *)title
{
	NSString 	*required = [manifest oo_stringForKey:kOOManifestRequiredOoliteVersion defaultValue:nil];
	NSString *maxRequired = [manifest oo_stringForKey:kOOManifestMaximumOoliteVersion defaultValue:nil];
	if (maxRequired == nil)
	{
		return [self areRequirementsFulfilled:[NSDictionary dictionaryWithObjectsAndKeys:required, @"version", nil] forOXP:title andFile:@"manifest.plist"];
	}
	else
	{
		return [self areRequirementsFulfilled:[NSDictionary dictionaryWithObjectsAndKeys:required, @"version", maxRequired, @"max_version", nil] forOXP:title andFile:@"manifest.plist"];
	}
}


+ (BOOL) areRequirementsFulfilled:(NSDictionary*)requirements forOXP:(NSString *)path andFile:(NSString *)file
{
	BOOL				OK = YES;
	NSString			*requiredVersion = nil;
	NSString			*maxVersion = nil;
	unsigned			conditionsHandled = 0;
	static NSArray		*ooVersionComponents = nil;
	NSArray				*oxpVersionComponents = nil;
	
	if (requirements == nil)  return YES;
	
	if (ooVersionComponents == nil)
	{
		ooVersionComponents = ComponentsFromVersionString([[NSBundle mainBundle] infoDictionary][@"CFBundleVersion"]);
		[ooVersionComponents retain];
	}
	
	// Check "version" (minimum version)
	if (OK)
	{
		// Not oo_stringForKey:, because we need to be able to complain about non-strings.
		requiredVersion = requirements[@"version"];
		if (requiredVersion != nil)
		{
			++conditionsHandled;
			if ([requiredVersion isKindOfClass:[NSString class]])
			{
				oxpVersionComponents = ComponentsFromVersionString(requiredVersion);
				if (NSOrderedAscending == CompareVersions(ooVersionComponents, oxpVersionComponents))  OK = NO;
			}
			else
			{
				OOLog(@"requirements.wrongType", @"Expected %@ entry \"%@\" to be string, but got %@ in OXP %@.", file, @"version", [requirements class], [path lastPathComponent]);
				OK = NO;
			}
		}
	}
	
	// Check "max_version" (minimum max_version)
	if (OK)
	{
		// Not oo_stringForKey:, because we need to be able to complain about non-strings.
		maxVersion = requirements[@"max_version"];
		if (maxVersion != nil)
		{
			++conditionsHandled;
			if ([maxVersion isKindOfClass:[NSString class]])
			{
				oxpVersionComponents = ComponentsFromVersionString(maxVersion);
				if (NSOrderedDescending == CompareVersions(ooVersionComponents, oxpVersionComponents))  OK = NO;
			}
			else
			{
				OOLog(@"requirements.wrongType", @"Expected %@ entry \"%@\" to be string, but got %@ in OXP %@.", file, @"max_version", [requirements class], [path lastPathComponent]);
				OK = NO;
			}
		}
	}
	
	if (OK && conditionsHandled < [requirements count])
	{
		// There are unknown requirement keys - don't support. NOTE: this check was not made pre 1.69!
		OOLog(@"requirements.unknown", @"requires.plist for OXP %@ contains unknown keys, rejecting.", [path lastPathComponent]);
		OK = NO;
	}
	
	return OK;
}


+ (BOOL) manifestHasConflicts:(NSDictionary *)manifest logErrors:(BOOL)logErrors
{
	NSDictionary	*conflicting = nil;
	NSDictionary	*conflictManifest = nil;
	NSString		*conflictID = nil;
	NSArray			*conflicts = nil;
	
	conflicts = [manifest oo_arrayForKey:kOOManifestConflictOXPs defaultValue:nil];
	// if it has a non-empty conflict_oxps list 
	if (conflicts != nil && [conflicts count] > 0)
	{
		// iterate over that list
		foreach (conflicting, conflicts)
		{
			conflictID = [conflicting oo_stringForKey:kOOManifestRelationIdentifier];
			conflictManifest = [sOXPManifests objectForKey:conflictID];
			// if the other OXP is in the list
			if (conflictManifest != nil)
			{
				// then check versions
				if ([self matchVersions:conflicting withVersion:[conflictManifest oo_stringForKey:kOOManifestVersion]])
				{
					if (logErrors)
					{
						[self addErrorWithKey:@"oxp-conflict" param1:[manifest oo_stringForKey:kOOManifestTitle] param2:[conflictManifest oo_stringForKey:kOOManifestTitle]];
						OOLog(@"oxp.conflict",@"OXP %@ conflicts with %@ and was removed from the loading list",[[manifest oo_stringForKey:kOOManifestFilePath] lastPathComponent],[[conflictManifest oo_stringForKey:kOOManifestFilePath] lastPathComponent]);
					}
					return YES;
				}
			}
		}
	}
	return NO;
}


+ (void) filterSearchPathsForConflicts:(NSMutableArray *)searchPaths
{
	NSDictionary	*manifest = nil;
	NSString		*identifier = nil;
	NSArray			*identifiers = [sOXPManifests allKeys];

	// take a copy because we'll mutate the original
	// foreach identified add-on
	foreach (identifier, identifiers)
	{
		manifest = sOXPManifests[identifier];
		if (manifest != nil)
		{
			if ([self manifestHasConflicts:manifest logErrors:YES])
			{
				// then we have a conflict, so remove this path
				[searchPaths removeObject:[manifest oo_stringForKey:kOOManifestFilePath]];
				[sOXPManifests removeObjectForKey:identifier];
			}
		}
	}
}


+ (BOOL) manifestHasMissingDependencies:(NSDictionary *)manifest logErrors:(BOOL)logErrors
{
	NSDictionary	*required = nil;
	NSDictionary	*requiredManifest = nil;
	NSString		*requiredID = nil;
	NSArray			*requireds = nil;

	requireds = [manifest oo_arrayForKey:kOOManifestRequiresOXPs defaultValue:nil];
	// if it has a non-empty required_oxps list 
	if (requireds != nil && [requireds count] > 0)
	{
		// iterate over that list
		foreach (required, requireds)
		{
			requiredID = [required oo_stringForKey:kOOManifestRelationIdentifier];
			requiredManifest = [sOXPManifests objectForKey:requiredID];
			// if the other OXP is in the list
			BOOL requirementsMet = NO;
			if (requiredManifest != nil)
			{
				// then check versions
				if ([self matchVersions:required withVersion:[requiredManifest oo_stringForKey:kOOManifestVersion]])
				{
					requirementsMet = YES;
				}
			}
			if (!requirementsMet)
			{
				if (logErrors)
				{
					[self addErrorWithKey:@"oxp-required" param1:[manifest oo_stringForKey:kOOManifestTitle] param2:[required oo_stringForKey:kOOManifestRelationDescription defaultValue:[required oo_stringForKey:kOOManifestRelationIdentifier]]];
					OOLog(@"oxp.requirementMissing",@"OXP %@ had unmet requirements and was removed from the loading list",[[manifest oo_stringForKey:kOOManifestFilePath] lastPathComponent]);
				}
				return YES;
			}
		}
	}
	return NO;
}



+ (BOOL) filterSearchPathsForRequirements:(NSMutableArray *)searchPaths
{
	NSDictionary	*manifest = nil;
	NSString		*identifier = nil;
	NSArray			*identifiers = [sOXPManifests allKeys];

	BOOL			allMet = YES;

	// take a copy because we'll mutate the original
	// foreach identified add-on
	foreach (identifier, identifiers)
	{
		manifest = sOXPManifests[identifier];
		if (manifest != nil)
		{
			if ([self manifestHasMissingDependencies:manifest logErrors:YES])
			{
				// then we have a missing requirement, so remove this path
				[searchPaths removeObject:[manifest oo_stringForKey:kOOManifestFilePath]];
				[sOXPManifests removeObjectForKey:identifier];
				allMet = NO;
			}
		}
	}

	return allMet;
}


+ (BOOL) matchVersions:(NSDictionary *)rangeDict withVersion:(NSString *)version
{
	NSString	*minimum = [rangeDict oo_stringForKey:kOOManifestRelationVersion defaultValue:nil];
	NSString	*maximum = [rangeDict oo_stringForKey:kOOManifestRelationMaxVersion defaultValue:nil];
	NSArray		*isVersionComponents = ComponentsFromVersionString(version);
	NSArray		*reqVersionComponents = nil;
	if (minimum != nil)
	{
		reqVersionComponents = ComponentsFromVersionString(minimum);
		if (NSOrderedAscending == CompareVersions(isVersionComponents, reqVersionComponents))
		{
			// earlier than minimum version
			return NO;
		}
	}
	if (maximum != nil)
	{
		reqVersionComponents = ComponentsFromVersionString(maximum);
		if (NSOrderedDescending == CompareVersions(isVersionComponents, reqVersionComponents))
		{
			// later than maximum version
			return NO;
		}
	}
	// either version was okay, or no version info so an unconditional match
	return YES;
}



+ (void) addErrorWithKey:(NSString *)descriptionKey param1:(id)param1 param2:(id)param2
{
	if (descriptionKey != nil)
	{
		if (sErrors == nil)  sErrors = [[NSMutableArray alloc] init];
		[sErrors addObject:@[descriptionKey, param1 ?: (id)@"", param2 ?: (id)@""]];
	}
}


+ (void)checkCacheUpToDateForPaths:(NSArray *)searchPaths
{
	/*	Check if caches are up to date.
		The strategy is to use a two-entry cache. One entry is an array
		containing the search paths, the other an array of modification dates
		(in the same order). If either fails to match the correct settings,
		we delete both.
	*/
	OOCacheManager		*cacheMgr = [OOCacheManager sharedCache];
	NSFileManager		*fmgr = [NSFileManager defaultManager];
	BOOL				upToDate = YES;
	id					oldPaths = nil;
	NSMutableArray		*modDates = nil;
	NSEnumerator		*pathEnum = nil;
	NSString			*path = nil;
	id					modDate = nil;
	
	if (EXPECT_NOT([[NSUserDefaults standardUserDefaults] boolForKey:@"always-flush-cache"]))
	{
		OOLog(kOOLogCacheExplicitFlush, @"Cache explicitly flushed with always-flush-cache preference. Rebuilding from scratch.");
		upToDate = NO;
	}
	else if ([MyOpenGLView pollShiftKey])
	{
		OOLog(kOOLogCacheExplicitFlush, @"Cache explicitly flushed with shift key. Rebuilding from scratch.");
		upToDate = NO;
	}
	
	oldPaths = [cacheMgr objectForKey:kOOCacheKeySearchPaths inCache:kOOCacheSearchPathModDates];
	if (upToDate && ![oldPaths isEqual:searchPaths])
	{
		// OXPs added/removed
		if (oldPaths != nil) OOLog(kOOLogCacheStalePaths, @"Cache is stale (search paths have changed). Rebuilding from scratch.");
		upToDate = NO;
	}
	
	// Build modification date list. (We need this regardless of whether the search paths matched.)
	
	modDates = [NSMutableArray arrayWithCapacity:[searchPaths count]];
	for (pathEnum = [searchPaths objectEnumerator]; (path = [pathEnum nextObject]); )
	{
		modDate = [fmgr oo_fileAttributesAtPath:path traverseLink:YES][NSFileModificationDate];
		if (modDate != nil)
		{
			// Converts to double because I'm not sure the cache can deal with dates under GNUstep.
			modDate = @([modDate timeIntervalSince1970]);
			[modDates addObject:modDate];
		}
	}
		
	if (upToDate && ![[cacheMgr objectForKey:kOOCacheKeyModificationDates inCache:kOOCacheSearchPathModDates] isEqual:modDates])
	{
		OOLog(kOOLogCacheStaleDates, @"Cache is stale (modification dates have changed). Rebuilding from scratch.");
		upToDate = NO;
	}
	
	if (!upToDate)
	{
		[cacheMgr clearAllCaches];
		[cacheMgr setObject:searchPaths forKey:kOOCacheKeySearchPaths inCache:kOOCacheSearchPathModDates];
		[cacheMgr setObject:modDates forKey:kOOCacheKeyModificationDates inCache:kOOCacheSearchPathModDates];
	}
	else OOLog(kOOLogCacheUpToDate, @"Data cache is up to date.");
}


+ (NSDictionary *)dictionaryFromFilesNamed:(NSString *)fileName
								  inFolder:(NSString *)folderName
								  andMerge:(BOOL) mergeFiles
{
	return [ResourceManager dictionaryFromFilesNamed:fileName inFolder:folderName mergeMode:mergeFiles ? MERGE_BASIC : MERGE_NONE cache:YES];
}


+ (NSDictionary *)dictionaryFromFilesNamed:(NSString *)fileName
								  inFolder:(NSString *)folderName
								 mergeMode:(OOResourceMergeMode)mergeMode
									 cache:(BOOL)cache
{
	id				result = nil;
	NSMutableArray	*results = nil;
	NSString		*cacheKey = nil;
	NSString		*mergeType = nil;
	OOCacheManager	*cacheMgr = [OOCacheManager sharedCache];
	NSEnumerator	*enumerator = nil;
	NSString		*path = nil;
	NSString		*dictPath = nil;
	NSDictionary	*dict = nil;
	
	if (fileName == nil)  return nil;
	
	switch (mergeMode)
	{
		case MERGE_NONE:
			mergeType = @"none";
			break;
		
		case MERGE_BASIC:
			mergeType = @"basic";
			break;
		
		case MERGE_SMART:
			mergeType = @"smart";
			break;
	}
	if (mergeType == nil)
	{
		OOLog(kOOLogParameterError, @"Unknown dictionary merge mode %u for %@. (This is an internal programming error, please report it.)", mergeMode, fileName);
		return nil;
	}
	
	if (cache)
	{
	
		if (folderName != nil)
		{
			cacheKey = [NSString stringWithFormat:@"%@/%@ merge:%@", folderName, fileName, mergeType];
		}
		else
		{
			cacheKey = [NSString stringWithFormat:@"%@ merge:%@", fileName, mergeType];
		}
		result = [cacheMgr objectForKey:cacheKey inCache:@"dictionaries"];
		if (result != nil)  return result;
	}
	
	if (mergeMode == MERGE_NONE)
	{
		// Find "last" matching dictionary
		for (enumerator = [ResourceManager reversePathEnumerator]; (path = [enumerator nextObject]); )
		{
			if (folderName != nil)
			{
				dictPath = [[path stringByAppendingPathComponent:folderName] stringByAppendingPathComponent:fileName];
				dict = OODictionaryFromFile(dictPath);
				if (dict != nil)  break;
			}
			dictPath = [path stringByAppendingPathComponent:fileName];
			dict = OODictionaryFromFile(dictPath);
			if (dict != nil)  break;
		}
		result = dict;
	}
	else
	{
		// Find all matching dictionaries
		results = [NSMutableArray array];
		for (enumerator = [ResourceManager pathEnumerator]; (path = [enumerator nextObject]); )
		{
			dictPath = [path stringByAppendingPathComponent:fileName];
			dict = OODictionaryFromFile(dictPath);
			if (dict != nil)  [results addObject:dict];
			if (folderName != nil)
			{
				dictPath = [[path stringByAppendingPathComponent:folderName] stringByAppendingPathComponent:fileName];
				dict = OODictionaryFromFile(dictPath);
				if (dict != nil)  [results addObject:dict];
			}
		}
		
		if ([results count] == 0)  return nil;
		
		// Merge result
		result = [NSMutableDictionary dictionary];
		
		for (enumerator = [results objectEnumerator]; (dict = [enumerator nextObject]); )
		{
			if (mergeMode == MERGE_SMART)  [result mergeEntriesFromDictionary:dict];
			else  [result addEntriesFromDictionary:dict];
		}
		result = [[result copy] autorelease];	// Make immutable
	}
	
	if (cache && result != nil)  [cacheMgr setObject:result forKey:cacheKey inCache:@"dictionaries"];
	
	return result;
}


+ (NSArray *) arrayFromFilesNamed:(NSString *)fileName inFolder:(NSString *)folderName andMerge:(BOOL) mergeFiles
{
	return [self arrayFromFilesNamed:fileName inFolder:folderName andMerge:mergeFiles cache:YES];
}


+ (NSArray *) arrayFromFilesNamed:(NSString *)fileName inFolder:(NSString *)folderName andMerge:(BOOL) mergeFiles cache:(BOOL)useCache
{
	id				result = nil;
	NSMutableArray	*results = nil;
	NSString		*cacheKey = nil;
	OOCacheManager	*cache = [OOCacheManager sharedCache];
	NSEnumerator	*enumerator = nil;
	NSString		*path = nil;
	NSString		*arrayPath = nil;
	NSMutableArray	*array = nil;
	NSArray			*arrayNonEditable = nil;
	
	if (fileName == nil)  return nil;
	
	if (useCache)
	{
		cacheKey = [NSString stringWithFormat:@"%@%@ merge:%@", (folderName != nil) ? [folderName stringByAppendingString:@"/"] : (NSString *)@"", fileName, mergeFiles ? @"yes" : @"no"];
		result = [cache objectForKey:cacheKey inCache:@"arrays"];
		if (result != nil)  return result;
	}
	
	if (!mergeFiles)
	{
		// Find "last" matching array
		for (enumerator = [ResourceManager reversePathEnumerator]; (path = [enumerator nextObject]); )
		{
			if (folderName != nil)
			{
				arrayPath = [[path stringByAppendingPathComponent:folderName] stringByAppendingPathComponent:fileName];
				arrayNonEditable = OOArrayFromFile(arrayPath);
				if (arrayNonEditable != nil)  break;
			}
			arrayPath = [path stringByAppendingPathComponent:fileName];
			arrayNonEditable = OOArrayFromFile(arrayPath);
			if (arrayNonEditable != nil)  break;
		}
		result = arrayNonEditable;
	}
	else
	{
		// Find all matching arrays
		results = [NSMutableArray array];
		for (enumerator = [ResourceManager pathEnumerator]; (path = [enumerator nextObject]); )
		{
			arrayPath = [path stringByAppendingPathComponent:fileName];
			array = [[OOArrayFromFile(arrayPath) mutableCopy] autorelease];
			if (array != nil) [results addObject:array];
	
			// Special handling for arrays merging. Currently, equipment.plist only gets its objects merged.
			// A lookup index is required. For the equipment.plist items, this is the index corresponging to the
			// EQ_* string, which describes the role of an equipment item and is unique.
			if ([array count] != 0 && [array[0] isKindOfClass:[NSArray class]])
			{
				if ([[fileName lowercaseString] isEqualToString:@"equipment.plist"])
					[self handleEquipmentListMerging:results forLookupIndex:3]; // Index 3 is the role string (EQ_*).
			}
			if (folderName != nil)
			{
				arrayPath = [[path stringByAppendingPathComponent:folderName] stringByAppendingPathComponent:fileName];
				array = [[OOArrayFromFile(arrayPath) mutableCopy] autorelease];
				if (array != nil)  [results addObject:array];
				
				if ([array count] != 0 && [array[0] isKindOfClass:[NSArray class]])
				{
					if ([[fileName lowercaseString] isEqualToString:@"equipment.plist"])
						[self handleEquipmentListMerging:results forLookupIndex:3]; // Index 3 is the role string (EQ_*).
				}
			}
		}
		
		if ([results count] == 0)  return nil;
		
		// Merge result
		result = [NSMutableArray array];
		
		for (enumerator = [results objectEnumerator]; (array = [enumerator nextObject]); )
		{
			[result addObjectsFromArray:array];
		}
		result = [[result copy] autorelease];	// Make immutable
	}
	
	if (useCache && result != nil)  [cache setObject:result forKey:cacheKey inCache:@"arrays"];
	
	return [NSArray arrayWithArray:result];
}


// A method for handling merging of arrays. Currently used with the equipment.plist entries.
// The arrayToProcess array is scanned for repetitions of the item at lookup index location and, if found,
// the latest entry replaces the earliest.
+ (void) handleEquipmentListMerging: (NSMutableArray *)arrayToProcess forLookupIndex:(unsigned)lookupIndex
{
	NSUInteger i,j,k;
	NSMutableArray *refArray = arrayToProcess[[arrayToProcess count] - 1];
	
	// Any change to arrayRef will directly modify arrayToProcess.
	
	for (i = 0; i < [refArray count]; i++)
	{
		for (j = 0; j < [arrayToProcess count] - 1; j++)
		{
			NSUInteger count = [[arrayToProcess oo_arrayAtIndex:j] count];
			if (count == 0)  continue;
			
			for (k=0; k < count; k++)
			{
				id processValue = [[[arrayToProcess oo_arrayAtIndex:j] oo_arrayAtIndex:k] oo_objectAtIndex:lookupIndex defaultValue:nil];
				id refValue = [[refArray oo_arrayAtIndex:i] oo_objectAtIndex:lookupIndex defaultValue:nil];
				
				if ([processValue isEqual:refValue])
				{
					arrayToProcess[j][k] = refArray[i];
					[refArray removeObjectAtIndex:i];
				}
			}
		}
	}
	// arrayToProcess has been processed at this point. Any necessary merging has been done.
}


+ (NSDictionary *) whitelistDictionary
{
	static id whitelistDictionary = nil;
	
	if (whitelistDictionary == nil)
	{
		NSString *path = [[[ResourceManager builtInPath] stringByAppendingPathComponent:@"Config"] stringByAppendingPathComponent:@"whitelist.plist"];
		whitelistDictionary = [NSDictionary dictionaryWithContentsOfFile:path];
		if (whitelistDictionary == nil)  whitelistDictionary = [NSNull null];
		
		[whitelistDictionary retain];
	}
	
	if (whitelistDictionary == [NSNull null])  return nil;
	return whitelistDictionary;
}


static NSString *LogClassKeyRoot(NSString *key)
{
	NSRange dot = [key rangeOfString:@"."];
	if (dot.location != NSNotFound)
	{
		return [key substringToIndex:dot.location];
	}
	else
	{
		return key;
	}
}


+ (NSDictionary *) logControlDictionary
{
	// Load built-in copy of logcontrol.plist.
	NSString *path = [[[ResourceManager builtInPath] stringByAppendingPathComponent:@"Config"]
					  stringByAppendingPathComponent:@"logcontrol.plist"];
	NSMutableDictionary *logControl = [NSMutableDictionary dictionaryWithDictionary:OODictionaryFromFile(path)];
	if (logControl == nil)  logControl = [NSMutableDictionary dictionary];
	
	// Build list of root log message classes that appear in the built-in list.
	NSMutableSet *coreRoots = [NSMutableSet set];
	NSString *key = nil;
	foreachkey(key, logControl)
	{
		[coreRoots addObject:LogClassKeyRoot(key)];
	}
	
	NSArray *rootPaths = [self rootPaths];
	NSString *configPath = nil;
	NSDictionary *dict = nil;
	
	// Look for logcontrol.plists inside OXPs (but not in root paths). These are not allowed to define keys in hierarchies used by the build-in one.
	NSEnumerator *pathEnum = [self pathEnumerator];
	while ((path = [pathEnum nextObject]))
	{
		if ([rootPaths containsObject:path])  continue;
		
		configPath = [[path stringByAppendingPathComponent:@"Config"]
					  stringByAppendingPathComponent:@"logcontrol.plist"];
		dict = OODictionaryFromFile(configPath);
		if (dict == nil)
		{
			configPath = [path stringByAppendingPathComponent:@"logcontrol.plist"];
			dict = OODictionaryFromFile(configPath);
		}
		foreachkey (key, dict)
		{
			if (![coreRoots containsObject:LogClassKeyRoot(key)])
			{
				logControl[key] = dict[key];
			}
		}
	}
	
	// Now, look for logcontrol.plists in root paths, i.e. not within OXPs. These are allowed to override the built-in copy.
	pathEnum = [rootPaths objectEnumerator];
	while ((path = [pathEnum nextObject]))
	{
		configPath = [[path stringByAppendingPathComponent:@"Config"]
					  stringByAppendingPathComponent:@"logcontrol.plist"];
		dict = OODictionaryFromFile(configPath);
		if (dict == nil)
		{
			configPath = [path stringByAppendingPathComponent:@"logcontrol.plist"];
			dict = OODictionaryFromFile(configPath);
		}
		foreachkey (key, dict)
		{
			logControl[key] = dict[key];
		}
	}
	
	// Finally, look in preferences, which can override all of the above.
	dict = [[NSUserDefaults standardUserDefaults] dictionaryForKey:@"logging-enable"];
	if (dict != nil)  [logControl addEntriesFromDictionary:dict];
	
	return logControl;
}


+ (NSDictionary *) roleCategoriesDictionary
{
	NSMutableDictionary *roleCategories = [NSMutableDictionary dictionaryWithCapacity:16];

	NSString *path = nil;
	NSString *configPath = nil;
	NSDictionary *categories = nil;
	
	NSEnumerator *pathEnum = [self pathEnumerator];
	while ((path = [pathEnum nextObject]))
	{
		configPath = [[path stringByAppendingPathComponent:@"Config"]
					  stringByAppendingPathComponent:@"role-categories.plist"];
		categories = OODictionaryFromFile(configPath);
		if (categories != nil)
		{
			[ResourceManager mergeRoleCategories:categories intoDictionary:roleCategories];
		}
	}
	
	/* If the old pirate-victim-roles files exist, merge them in */
	NSArray *pirateVictims = [ResourceManager arrayFromFilesNamed:@"pirate-victim-roles.plist" inFolder:@"Config" andMerge:YES];
	[ResourceManager mergeRoleCategories:@{@"oolite-pirate-victim": pirateVictims} intoDictionary:roleCategories];

	return [[roleCategories copy] autorelease];
}


+ (void) mergeRoleCategories:(NSDictionary *)catData intoDictionary:(NSMutableDictionary *)categories
{
	NSMutableSet *contents = nil;
	NSArray *catDataEntry = nil;
	NSString *key;
	foreachkey(key, catData)
	{
		contents = categories[key];
		if (contents == nil)
		{
			contents = [NSMutableSet setWithCapacity:16];
			categories[key] = contents;
		}
		catDataEntry = [catData oo_arrayForKey:key];
		OOLog(@"shipData.load.roleCategories", @"Adding %ld entries for category %@", (unsigned long)[catDataEntry count], key);
		[contents addObjectsFromArray:catDataEntry];
	}
}


+ (NSDictionary *) shaderBindingTypesDictionary
{
	static id shaderBindingTypesDictionary = nil;
	
	if (shaderBindingTypesDictionary == nil)
	{
		NSAutoreleasePool *pool = [NSAutoreleasePool new];
		
		NSString *path = [[[ResourceManager builtInPath] stringByAppendingPathComponent:@"Config"] stringByAppendingPathComponent:@"shader-uniform-bindings.plist"];
		NSMutableDictionary *dict = [NSMutableDictionary dictionaryWithContentsOfFile:path];
		NSArray *keys = [dict allKeys];
		
		// Resolve all $inherit keys.
		unsigned changeCount = 0;
		do {
			changeCount = 0;
			NSString *key = nil;
			foreach (key, keys)
			{
				NSDictionary *value = [dict oo_dictionaryForKey:key];
				NSString *inheritKey = [value oo_stringForKey:@"$inherit"];
				if (inheritKey != nil)
				{
					changeCount++;
					NSMutableDictionary *mutableValue = [[value mutableCopy] autorelease];
					[mutableValue removeObjectForKey:@"$inherit"];
					NSDictionary *inherited = [dict oo_dictionaryForKey:inheritKey];
					if (inherited != nil)
					{
						[mutableValue addEntriesFromDictionary:inherited];
					}
					
					dict[key] = [[mutableValue copy] autorelease];
				}
			}
		} while (changeCount != 0);
		
		shaderBindingTypesDictionary = [dict copy];
		
		[pool release];
	}
	
	return shaderBindingTypesDictionary;
}


+ (NSString *) pathForFileNamed:(NSString *)fileName inFolder:(NSString *)folderName
{
	return [self pathForFileNamed:fileName inFolder:folderName cache:YES];
}


+ (NSString *) pathForFileNamed:(NSString *)fileName inFolder:(NSString *)folderName cache:(BOOL)useCache
{
	NSString		*result = nil;
	NSString		*cacheKey = nil;
	OOCacheManager	*cache = [OOCacheManager sharedCache];
	NSEnumerator	*pathEnum = nil;
	NSString		*path = nil;
	NSString		*filePath = nil;
	NSFileManager	*fmgr = nil;
	
	if (fileName == nil)  return nil;
	
	if (cache)
	{
		if (folderName != nil)  cacheKey = [NSString stringWithFormat:@"%@/%@", folderName, fileName];
		else  cacheKey = fileName;
		result = [cache objectForKey:cacheKey inCache:@"resolved paths"];
		if (result != nil)  return result;
	}
	
	// Search for file
	fmgr = [NSFileManager defaultManager];
	for (pathEnum = [[ResourceManager paths] reverseObjectEnumerator]; (path = [pathEnum nextObject]); )
	{
		filePath = [[path stringByAppendingPathComponent:folderName] stringByAppendingPathComponent:fileName];
		if ([fmgr oo_oxzFileExistsAtPath:filePath])
		{
			result = filePath;
			break;
		}
		
		filePath = [path stringByAppendingPathComponent:fileName];
		if ([fmgr oo_oxzFileExistsAtPath:filePath])
		{
			result = filePath;
			break;
		}
	}
	
	if (result != nil)
	{
		OOLog(@"resourceManager.foundFile", @"Found %@/%@ at %@", folderName, fileName, filePath);
		if (useCache)
		{
			[cache setObject:result forKey:cacheKey inCache:@"resolved paths"];
		}
	}
	return result;
}


+ (id) retrieveFileNamed:(NSString *)fileName
				inFolder:(NSString *)folderName
				   cache:(NSMutableDictionary **)ioCache
					 key:(NSString *)key
				   class:(Class)class
			usePathCache:(BOOL)useCache
{
	id				result = nil;
	NSString		*path = nil;
	
	if (ioCache)
	{
		if (key == nil)  key = [NSString stringWithFormat:@"%@:%@", folderName, fileName];
		if (*ioCache != nil)
		{
			// return the cached object, if any
			result = (*ioCache)[key];
			if (result)  return result;
		}
	}
	
	path = [self pathForFileNamed:fileName inFolder:folderName cache:useCache];
	if (path != nil)  result = [[[class alloc] initWithContentsOfFile:path] autorelease];
	
	if (result != nil && ioCache != NULL)
	{
		if (*ioCache == nil)  *ioCache = [[NSMutableDictionary alloc] init];
		(*ioCache)[key] = result;
	}
	
	return result;
}


+ (OOMusic *) ooMusicNamed:(NSString *)fileName inFolder:(NSString *)folderName
{
	return [self retrieveFileNamed:fileName
						  inFolder:folderName
							 cache:NULL	// Don't cache music objects; minimizing latency isn't really important.
							   key:[NSString stringWithFormat:@"OOMusic:%@:%@", folderName, fileName]
							 class:[OOMusic class]
					  usePathCache:YES];
}


+ (OOSound *) ooSoundNamed:(NSString *)fileName inFolder:(NSString *)folderName
{
	return [self retrieveFileNamed:fileName
						  inFolder:folderName
							 cache:&sSoundCache
							   key:[NSString stringWithFormat:@"OOSound:%@:%@", folderName, fileName]
							 class:[OOSound class]
					  usePathCache:YES];
}


+ (NSString *) stringFromFilesNamed:(NSString *)fileName inFolder:(NSString *)folderName
{
	return [self stringFromFilesNamed:fileName inFolder:folderName cache:YES];
}


+ (NSString *) stringFromFilesNamed:(NSString *)fileName inFolder:(NSString *)folderName cache:(BOOL)useCache
{
	id				result = nil;
	NSString		*path = nil;
	NSString		*key = nil;
	
	if (useCache)
	{
		key = [NSString stringWithFormat:@"%@:%@", folderName, fileName];
		if (sStringCache != nil)
		{
			// return the cached object, if any
			result = sStringCache[key];
			if (result)  return result;
		}
	}
	
	path = [self pathForFileNamed:fileName inFolder:folderName cache:useCache];
	if (path != nil)  result = [NSString stringWithContentsOfUnicodeFile:path];
	
	if (result != nil && useCache)
	{
		if (sStringCache == nil)  sStringCache = [[NSMutableDictionary alloc] init];
		sStringCache[key] = result;
	}
	
	return result;
}


+ (NSDictionary *)loadScripts
{
	NSMutableDictionary			*loadedScripts = nil;
	NSArray						*results = nil;
	NSArray						*paths = nil;
	NSEnumerator				*pathEnum = nil;
	NSString					*path = nil;
	NSEnumerator				*scriptEnum = nil;
	OOScript					*script = nil;
	NSString					*name = nil;
	NSAutoreleasePool			*pool = nil;
	
	OOLog(@"script.load.world.begin", @"Loading world scripts...");
	
	loadedScripts = [NSMutableDictionary dictionary];
	paths = [ResourceManager paths];
	for (pathEnum = [paths objectEnumerator]; (path = [pathEnum nextObject]); )
	{
		pool = [[NSAutoreleasePool alloc] init];
		
		@try
		{
			results = [OOScript worldScriptsAtPath:[path stringByAppendingPathComponent:@"Config"]];
			if (results == nil) results = [OOScript worldScriptsAtPath:path];
			if (results != nil)
			{
				for (scriptEnum = [results objectEnumerator]; (script = [scriptEnum nextObject]); )
				{
					name = [script name];
					if (name != nil)  loadedScripts[name] = script;
					else  OOLog(@"script.load.unnamed", @"Discarding anonymous script %@", script);
				}
			}
		}
		@catch (NSException *exception)
		{
			OOLog(@"script.load.exception", @"***** %s encountered exception %@ (%@) while trying to load script from %@ -- ignoring this location.", __PRETTY_FUNCTION__, [exception name], [exception reason], path);
			// Ignore exception and keep loading other scripts.
		}
		
		[pool release];
	}
	
	if (OOLogWillDisplayMessagesInClass(@"script.load.world.listAll"))
	{
		NSUInteger count = [loadedScripts count];
		if (count != 0)
		{
			NSMutableArray		*displayNames = nil;
			NSEnumerator		*scriptEnum = nil;
			OOScript			*script = nil;
			NSString			*displayString = nil;
			
			displayNames = [NSMutableArray arrayWithCapacity:count];
			
			for (scriptEnum = [loadedScripts objectEnumerator]; (script = [scriptEnum nextObject]); )
			{
				[displayNames addObject:[script displayName]];
			}
			
			displayString = [[displayNames sortedArrayUsingSelector:@selector(caseInsensitiveCompare:)] componentsJoinedByString:@"\n    "];
			OOLog(@"script.load.world.listAll", @"Loaded %lu world scripts:\n    %@", count, displayString);
		}
		else
		{
			OOLog(@"script.load.world.listAll", @"*** No world scripts loaded.");
		}
	}
	
	return loadedScripts;
}


+ (BOOL) writeDiagnosticData:(NSData *)data toFileNamed:(NSString *)name
{
	if (data == nil || name == nil)  return NO;
	
	NSString *directory = [self diagnosticFileLocation];
	if (directory == nil)  return NO;
	
	NSArray *nameComponents = [name componentsSeparatedByString:@"/"];
	NSUInteger count = [nameComponents count];
	if (count > 1)
	{
		name = [nameComponents lastObject];
		
		for (NSUInteger i = 0; i < count - 1; i++)
		{
			NSString *component = nameComponents[i];
			if ([component hasPrefix:@"."])
			{
				component = [@"!" stringByAppendingString:[component substringFromIndex:1]];
			}
			directory = [directory stringByAppendingPathComponent:component];
			[[NSFileManager defaultManager] oo_createDirectoryAtPath:directory attributes:nil];
		}
	}
	
	return [data writeToFile:[directory stringByAppendingPathComponent:name] atomically:YES];
}


+ (BOOL) writeDiagnosticString:(NSString *)string toFileNamed:(NSString *)name
{
	return [self writeDiagnosticData:[string dataUsingEncoding:NSUTF8StringEncoding] toFileNamed:name];
}


+ (BOOL) writeDiagnosticPList:(id)plist toFileNamed:(NSString *)name
{
	NSData *data = [plist oldSchoolPListFormatWithErrorDescription:NULL];
	if (data == nil)  [NSPropertyListSerialization dataFromPropertyList:plist format:NSPropertyListXMLFormat_v1_0 errorDescription:NULL];
	if (data == nil)  return NO;
	
	return [self writeDiagnosticData:data toFileNamed:name];
}


+ (NSDictionary *) materialDefaults
{
	return [self dictionaryFromFilesNamed:@"material-defaults.plist" inFolder:@"Config" andMerge:YES];
}


+ (BOOL)directoryExists:(NSString *)inPath create:(BOOL)inCreate
{
	BOOL				exists, directory;
	NSFileManager		*fmgr =  [NSFileManager defaultManager];
	
	exists = [fmgr fileExistsAtPath:inPath isDirectory:&directory];
	
	if (exists && !directory)
	{
		OOLog(@"resourceManager.write.buildPath.failed", @"Expected %@ to be a folder, but it is a file.", inPath);
		return NO;
	}
	if (!exists)
	{
		if (!inCreate) return NO;
		if (![fmgr oo_createDirectoryAtPath:inPath attributes:nil])
		{
			OOLog(@"resourceManager.write.buildPath.failed", @"Could not create folder %@.", inPath);
			return NO;
		}
	}
	
	return YES;
}


+ (NSString *) diagnosticFileLocation
{
	return OOLogHandlerGetLogBasePath();
}


+ (void) logPaths
{
	NSMutableArray			*displayPaths = nil;
	NSEnumerator			*pathEnum = nil;
	NSString				*path = nil;

	if (sUseAddOns)
	{
		// Prettify paths for logging.
		displayPaths = [NSMutableArray arrayWithCapacity:[sSearchPaths count]];
		for (pathEnum = [sSearchPaths objectEnumerator]; (path = [pathEnum nextObject]); )
		{
			[displayPaths addObject:[[path stringByStandardizingPath] stringByAbbreviatingWithTildeInPath]];
		}
		
		OOLog(@"searchPaths.dumpAll", @"Unrestricted mode - resource paths:\n    %@", [displayPaths componentsJoinedByString:@"\n    "]);
	}
	else
	{
		OOLog(@"searchPaths.dumpAll", @"Strict mode - resource path:\n    %@",
			[[[self builtInPath] stringByStandardizingPath] stringByAbbreviatingWithTildeInPath]);
	}
}


+ (void) clearCaches
{
	[sSoundCache release];
	sSoundCache = nil;
	[sStringCache release];
	sStringCache = nil;
}

@end
