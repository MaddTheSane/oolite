/*

OOOXZManager.h

Responsible for installing and uninstalling OXZs

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
#import "NSFileManagerOOExtensions.h"
#import "OOTypes.h"
#import "GuiDisplayGen.h"

typedef NS_ENUM(unsigned int, OXZDownloadStatus) {
	OXZ_DOWNLOAD_NONE = 0,
	OXZ_DOWNLOAD_STARTED = 1,
	OXZ_DOWNLOAD_RECEIVING = 2,
	OXZ_DOWNLOAD_COMPLETE = 10,
	OXZ_DOWNLOAD_ERROR = 99
};


typedef NS_ENUM(unsigned int, OXZInterfaceState) {
	OXZ_STATE_NODATA,
	OXZ_STATE_MAIN,
	OXZ_STATE_UPDATING,
	OXZ_STATE_PICK_INSTALL,
	OXZ_STATE_PICK_INSTALLED,
	OXZ_STATE_PICK_REMOVE,
	OXZ_STATE_INSTALLING,
	OXZ_STATE_DEPENDENCIES,
	OXZ_STATE_REMOVING,
	OXZ_STATE_TASKDONE,
	OXZ_STATE_RESTARTING,
	OXZ_STATE_SETFILTER,
	OXZ_STATE_EXTRACT,
	OXZ_STATE_EXTRACTDONE
};


@interface OOOXZManager : NSObject
{
@private
	NSArray 			*_oxzList;
	NSArray 			*_managedList;
	NSArray				*_filteredList;
	NSString			*_currentFilter;

	OXZInterfaceState	_interfaceState;
	BOOL				_interfaceShowingOXZDetail;
	BOOL				_changesMade;

	NSURLConnection		*_currentDownload;
	NSString			*_currentDownloadName;

	OXZDownloadStatus	_downloadStatus;
	NSUInteger			_downloadProgress;
	NSUInteger			_downloadExpected;
	NSFileHandle		*_fileWriter;
	NSUInteger			_item;

	BOOL				_downloadAllDependencies;

	NSUInteger			_offset;

	NSString			*_progressStatus;
	NSMutableSet		*_dependencyStack;
}

+ (OOOXZManager *) sharedManager;

@property (readonly, copy) NSString *installPath;

@property (readonly) BOOL updateManifests;
@property (readonly) BOOL cancelUpdate;

@property (readonly, copy) NSArray *manifests;
@property (readonly, copy) NSArray *managedOXZs;

- (void) gui;
@property (getter=isRestarting, readonly) BOOL restarting;
@property (getter=isAcceptingTextInput, readonly) BOOL acceptingTextInput;
@property (getter=isAcceptingGUIInput, readonly) BOOL acceptingGUIInput;

- (void) processSelection;
- (void) processTextInput:(NSString *)input;
- (void) refreshTextInput:(NSString *)input;
- (void) processFilterKey;
- (void) processShowInfoKey;
- (void) processExtractKey;
@property (readonly) OOGUIRow showInstallOptions;
@property (readonly) OOGUIRow showRemoveOptions;
- (void) showOptionsUpdate;
- (void) showOptionsPrev;
- (void) showOptionsNext;
- (void) processOptionsPrev;
- (void) processOptionsNext;

@end
