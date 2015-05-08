/*

OOCommodityMarket.h

Commodity price and quantity list for a particular station/system
Also used for the player ship's docked manifest

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

#import "OOCommodities.h"
#import "OOTypes.h"

@interface OOCommodityMarket: NSObject
{
@private
	NSMutableDictionary		*_commodityList;
	NSArray					*_sortedKeys;
}


@property (readonly, atomic) NSUInteger count;

- (void) setGood:(OOCommodityType)key withInfo:(NSDictionary *)info;

@property (readonly, copy) NSArray *goods;
- (NSDictionary *) dictionaryForScripting;

- (BOOL) setPrice:(OOCreditsQuantity)price forGood:(OOCommodityType)good;
- (BOOL) setQuantity:(OOCargoQuantity)quantity forGood:(OOCommodityType)good;
- (BOOL) addQuantity:(OOCargoQuantity)quantity forGood:(OOCommodityType)good;
- (BOOL) removeQuantity:(OOCargoQuantity)quantity forGood:(OOCommodityType)good;
- (void) removeAllGoods;
- (BOOL) setComment:(NSString *)comment forGood:(OOCommodityType)good;
- (BOOL) setShortComment:(NSString *)comment forGood:(OOCommodityType)good;

- (NSString *) nameForGood:(OOCommodityType)good;
- (NSString *) commentForGood:(OOCommodityType)good;
- (NSString *) shortCommentForGood:(OOCommodityType)good;
- (OOCreditsQuantity) priceForGood:(OOCommodityType)good;
- (OOCargoQuantity) quantityForGood:(OOCommodityType)good;
- (OOMassUnit) massUnitForGood:(OOCommodityType)good;
- (NSUInteger) exportLegalityForGood:(OOCommodityType)good;
- (NSUInteger) importLegalityForGood:(OOCommodityType)good;
- (OOCargoQuantity) capacityForGood:(OOCommodityType)good;
- (float) trumbleOpinionForGood:(OOCommodityType)good;

- (NSDictionary *) definitionForGood:(OOCommodityType)good;


@property (readonly, copy) NSArray *savePlayerAmounts;
- (void) loadPlayerAmounts:(NSArray *)amounts;

@property (readonly, copy) NSArray *saveStationAmounts;
- (void) loadStationAmounts:(NSArray *)amounts;

@end
