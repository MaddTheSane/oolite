//
//  ImportExtension.swift
//  Oolite Mission Spotlight
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

private func configDictionary(withBaseURL baseURL: URL, name: String) -> [String: Any]? {
	var path = baseURL
	if #available(macOSApplicationExtension 13.0, *) {
		path.append(path: "Config", directoryHint: .isDirectory)
		path.append(path: name, directoryHint: .notDirectory)
	} else {
		path.appendPathComponent("Config")
		path.appendPathComponent(name)
	}
	guard let data = NSData.oo_data(withOXZFile: path.path) as Data?,
		  let plist = try? PropertyListSerialization.propertyList(from: data, format: nil) else {
		return nil
	}
	return plist as? [String: Any]
}

private func mergeShipData(shipData: [String: Any]?, overrides: [String: [String: Any]]?) -> [String: Any]? {
	func mergeShipDataEntry(base: [String: Any]?, override: [String: Any]) -> [String: Any]? {
		guard let base else {
			return override
		}
		var mutableEntry = base
		mutableEntry.merge(override, uniquingKeysWith: {$1})
		
		return mutableEntry
	}
	
	guard let overrides else {
		return shipData
	}
	guard let shipData else {
		return overrides
	}
	
	var mutableShipData = shipData
	for (key, overrideDict) in overrides {
		let baseDict = shipData[key] as? [String: Any]
		mutableShipData[key] = mergeShipDataEntry(base: baseDict, override: overrideDict)
	}
	
	return mutableShipData
}

// Stuff lifted from messy files in Oolite
private func OOParseRoles(from str: String?) -> [String: Any]? {
	// Split string at spaces, sanity checks, set-up.
	guard let str else {
		return nil
	}
	
	let tokens = scanTokens(from: str)
	guard !tokens.isEmpty else {
		return nil
	}
	
	var result = [String: Any]()
	result.reserveCapacity(tokens.count)
	
	// Scan tokens, looking for probabilities.
	for role1 in tokens {
		var role = role1
		var probability: Float = 1.0
		if role.range(of: "(") != nil {
			let scanner = Scanner(string: role)
			role = scanner.scanUpToString("(")!
			probability = scanner.scanFloat() ?? 1.0
			// Ignore rest of string
		}
		// shipKey roles start with [ so other roles can't
		if 0 <= probability && !role.hasPrefix("[") {
			result[role] = probability
		}
	}
	guard !result.isEmpty else {
		return nil
	}
	return result
}

private func scanTokens(from values: String?) -> [String] {
	guard let values else {
		return []
	}
	var result = [String]()
	let scanner = Scanner(string: values)
	let spaceSet = CharacterSet.whitespacesAndNewlines
	while !scanner.isAtEnd {
		var token: NSString? = nil
		scanner.ooliteScanCharacters(from: spaceSet, into: nil)
		if scanner.ooliteScanUpToCharacters(from: spaceSet, into: &token) {
			result.append(token! as String)
		}
	}
	
	return result
}

class ImportExtension: CSImportExtension {
    
    override func update(_ attributes: CSSearchableItemAttributeSet, forFileAt: URL) throws {
		if let manifest = configDictionary(withBaseURL: forFileAt, name: "manifest.plist") {
			if let title = manifest["title"] as? String {
				attributes.title = title
			}
			
			if let identifier = manifest["identifier"] as? String {
				attributes.identifier = identifier
			}
			if let version = manifest["version"] as? String {
				attributes.version = version
			}
			
			// Allow a string or array for author
			do {
				let author = manifest["author"]
				if let authStr = author as? String {
					attributes.authorNames = [authStr]
				} else if let authArr = author as? [String] {
					attributes.authorNames = authArr
				}
			}
			
			if let desc = manifest["description"] as? String {
				attributes.contentDescription = desc
			}
			do {
				var copyright = manifest["copyright"] as? String
				if copyright == nil {
					copyright = manifest["license"] as? String
				}
				if let copyright {
					attributes.copyright = copyright
				}
			}
			
			do {
				var url = manifest["download_url"] as? String
				if url == nil {
					url = manifest["information_url"] as? String
				}
				if let url, let actualURL = URL(string: url) {
					attributes.url = actualURL
				}
			}
			
			if let minVersion = manifest["required_oolite_version"] as? String,
			   let customKey = CSCustomAttributeKey(keyName: "org_aegidian_oolite_minversion") {
				attributes.setValue(minVersion as NSString, forCustomKey: customKey)
			}
		} else {
			// No manifest, look for requires.plist
			if let requires = configDictionary(withBaseURL: forFileAt, name: "requires.plist") {
				if let minVersion = requires["version"] as? String,
				   let customKey = CSCustomAttributeKey(keyName: "org_aegidian_oolite_minversion") {
					attributes.setValue(minVersion as NSString, forCustomKey: customKey)
				}
				
				if let maxVersion = requires["max_version"] as? String,
				   let customKey = CSCustomAttributeKey(keyName: "org_aegidian_oolite_maxversion") {
					attributes.setValue(maxVersion as NSString, forCustomKey: customKey)
				}
			}
			
			// Not "officially" supported, but exists in some OXPs.
			if let infoPList = configDictionary(withBaseURL: forFileAt, name: "Info.plist"),
			   let version = infoPList[kCFBundleVersionKey as String] as? String {
				attributes.version = version
			}
		}

		var shipData = configDictionary(withBaseURL: forFileAt, name: "shipdata.plist")
		let shipDataOverrides = configDictionary(withBaseURL: forFileAt, name: "shipdata-overrides.plist") as? [String: [String: Any]]
		shipData = mergeShipData(shipData: shipData, overrides: shipDataOverrides)
		
		if let shipData, !shipData.isEmpty {
			var shipNames = Set<String>()
			shipNames.reserveCapacity(shipData.count)
			var shipModels = Set<String>()
			shipModels.reserveCapacity(shipData.count)
			var shipRoles = Set<String>()
			
			if let attr = CSCustomAttributeKey(keyName: "org_aegidian_oolite_shipids") {
				attributes.setValue(Array(shipData.keys) as NSArray, forCustomKey: attr)
			}
			for ship in shipData.values {
				guard let ship = ship as? [String: Any] else {
					continue
				}
				if let name = ship["name"] as? String {
					shipNames.insert(name)
				}
				
				if let model = ship["model"] as? String {
					shipModels.insert(model)
				}
				if let role = ship["roles"] as? String,
				   let allRoles = OOParseRoles(from: role)?.keys {
					shipRoles.formUnion(allRoles)
				}
			}
			
			if let attr = CSCustomAttributeKey(keyName: "org_aegidian_oolite_shipclassnames") {
				attributes.setValue(Array(shipNames) as NSArray, forCustomKey: attr)
			}
			if let attr = CSCustomAttributeKey(keyName: "org_aegidian_oolite_shipmodels") {
				attributes.setValue(Array(shipModels) as NSArray, forCustomKey: attr)
			}
			if let attr = CSCustomAttributeKey(keyName: "org_aegidian_oolite_shiproles") {
				attributes.setValue(Array(shipRoles) as NSArray, forCustomKey: attr)
			}
		}

	}
}
