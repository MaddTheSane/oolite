//
//  ImportExtension.swift
//  Oolite Save Spotlight
//
//  Created by C.W. Betts on 4/5/23.
//  Copyright © 2023 the Oolite team. All rights reserved.
//

import CoreSpotlight


private func GetBundlePropertyList(named inPListName: String) throws -> Any {
	let bundle = Bundle(for: ImportExtension.self)
	guard let url = bundle.url(forResource: inPListName, withExtension: "plist") else {
		throw CocoaError(.fileNoSuchFile)
	}
	let data = try Data(contentsOf: url)
	return try PropertyListSerialization.propertyList(from: data, options: [], format: nil)
}

class ImportExtension: CSImportExtension {
	
	override func update(_ attributes: CSSearchableItemAttributeSet, forFileAt: URL) throws {
		let content = try NSDictionary.init(contentsOf: forFileAt, error: ())
		
		if let playerName = content["player_name"] as? String {
			attributes.title = playerName
		}
		if let shipDesc = content["ship_desc"] as? NSString,
		   let customKey = CSCustomAttributeKey(keyName: "org_aegidian_oolite_shipids") {
			attributes.setValue(shipDesc, forCustomKey: customKey)
		}
		
		if let shipDesc = content["ship_name"] as? NSString,
		   let customKey = CSCustomAttributeKey(keyName: "org_aegidian_oolite_shipclassnames") {
			attributes.setValue(shipDesc, forCustomKey: customKey)
		}
		
		if let shipDesc = content["current_system_name"] as? NSString,
		   let customKey = CSCustomAttributeKey(keyName: "org_aegidian_oolite_systemname") {
			attributes.setValue(shipDesc, forCustomKey: customKey)
		}
		
		if let shipDesc = content["comm_log"] as? [String], shipDesc.count > 0 {
			attributes.textContent = shipDesc.joined(separator: "\n")
		}
		
		if let killCount = content["ship_kills"] as? Int,
		   killCount > 0 {
			
			if let preRatings = (try? GetBundlePropertyList(named: "Values")) as? [String: Any],
			   let ratings = preRatings["ratings"] as? [String] {
				var rating = 0
				let kRequiredKills = [0x0008,  0x0010,  0x0020,  0x0040,  0x0080,  0x0200,  0x0A00,  0x1900]
				
				while (rating < 8 && kRequiredKills[rating] <= killCount) {
					rating += 1
				}
				if let customKey = CSCustomAttributeKey(keyName: "org_aegidian_oolite_combatrating") {
					attributes.setValue(ratings[rating] as NSString, forCustomKey: customKey)
				}
			}
		}
	}
}
