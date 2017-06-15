//
//  SWFShapeDrawTests.swift
//  SWFShapeDrawTests
//
//  Created by larryhou on 20/8/2015.
//  Copyright © 2015 larryhou. All rights reserved.
//

import XCTest

class SWFShapeDrawTests: XCTestCase
{
    
    func testQuartzPerformance()
    {
        let bundle = Bundle(for: VectorImageView.self)
        let url = bundle.url(forResource: "graph", withExtension: "json")
        assert(url != nil)
        
        var steps:NSArray!
        if url != nil
        {
            let data = try? Data(contentsOf: url!)
            
            do
            {
                steps = try JSONSerialization.jsonObject(with: data!, options: JSONSerialization.ReadingOptions.allowFragments) as! NSArray
            }
            catch
            {
                assertionFailure("JSON parsing failed")
            }
        }
        
        let rect = UIScreen.main.bounds
        UIGraphicsBeginImageContextWithOptions(rect.size, false, UIScreen.main.scale)
        
        let vector = VectorImageView(frame:rect)
        vector.backgroundColor = UIColor(white: 0.9, alpha: 1.0)
        vector.importVectorGraphics(steps)
        self.measure
        {
            vector.draw(rect)
        }
    }
}
