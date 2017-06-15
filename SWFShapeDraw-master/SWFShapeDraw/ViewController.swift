//
//  ViewController.swift
//  SWFShapeDraw
//
//  Created by larryhou on 20/8/2015.
//  Copyright © 2015 larryhou. All rights reserved.
//

import UIKit
import Foundation

class ViewController: UIViewController, UIScrollViewDelegate
{
    fileprivate var shape:RedrawView!
    fileprivate var tapGuesture:UITapGestureRecognizer!

    @IBOutlet weak var imageView: UIImageView!
    
    override func viewDidLoad()
    {
        super.viewDidLoad()
        let view = self.view as! UIScrollView
        view.backgroundColor = UIColor(white: 0.9, alpha: 1.0)
        view.delegate = self
        view.maximumZoomScale = 2.0
        view.minimumZoomScale = 1.0
        
        tapGuesture = UITapGestureRecognizer()
        tapGuesture.addTarget(self, action: "tapAction:")
        tapGuesture.numberOfTapsRequired = 2
        view.addGestureRecognizer(tapGuesture)
        
        var steps:NSArray!
        let jurl = Bundle.main.url(forResource: "graph", withExtension: "json")
        if jurl != nil
        {
            let data = try? Data(contentsOf: jurl!)
            
            do
            {
                steps = try JSONSerialization.jsonObject(with: data!, options: JSONSerialization.ReadingOptions.allowFragments) as! NSArray
            }
            catch
            {
                print(error)
                return
            }
        }
        
        shape = RedrawView(frame:view.frame);
        shape.backgroundColor = UIColor.clear
        view.addSubview(shape)
        
//        let vector = VectorImageView(frame:view.frame)
//        vector.backgroundColor = UIColor(white: 0.9, alpha: 1.0)
//        vector.importVectorGraphics(steps)
//        view.addSubview(vector)
        
        drawShape(steps)
    }
    
    func tapAction(_ guesture:UITapGestureRecognizer)
    {
        if !shape.stepsAvaiable
        {
            shape.currentIndex = 0
            shape.image = nil
            
            startDrawingAnimation()
        }
    }
    
    func unionBoundsWithPoint(_ bounds:inout (left:CGFloat, top:CGFloat, right:CGFloat, bottom:CGFloat), x:CGFloat, y:CGFloat)
    {
        bounds.left   = min(x, bounds.left)
        bounds.right  = max(x, bounds.right)
        bounds.top    = min(y, bounds.top)
        bounds.bottom = max(y, bounds.bottom)
    }
    
    func drawShape(_ steps:NSArray)
    {
        var list:[(method:String, params:NSDictionary)] = []
        var bounds:(left:CGFloat, top:CGFloat, right:CGFloat, bottom:CGFloat) = (0,0,0,0)
        
        for i in 0..<steps.count
        {
            let data = steps[i] as! NSArray
            let method = data.object(at: 0) as! String
            let params = data.object(at: 1) as! NSDictionary
            list.append((method, params))
            
            var x:CGFloat = 0.0, y:CGFloat = 0.0
            switch method
            {
                case "MOVE_TO", "LINE_TO":
                    x = CGFloat(params.value(forKey: "x") as! Double)
                    y = CGFloat(params.value(forKey: "y") as! Double)
                    unionBoundsWithPoint(&bounds, x: x, y: y)
                    
                case "CURVE_TO":
                    x = CGFloat(params.value(forKey: "anchorX") as! Double)
                    y = CGFloat(params.value(forKey: "anchorY") as! Double)
                    unionBoundsWithPoint(&bounds, x: x, y: y)
                    
                    x = CGFloat(params.value(forKey: "controlX") as! Double)
                    y = CGFloat(params.value(forKey: "controlY") as! Double)
                    unionBoundsWithPoint(&bounds, x: x, y: y)
                default:break
            }
        }
        
        shape.irect = CGRect(x: bounds.left, y: bounds.top, width: bounds.right - bounds.left, height: bounds.bottom - bounds.top)
        shape.importSteps(list)
        
        startDrawingAnimation()
    }
    
    func startDrawingAnimation()
    {
        Timer.scheduledTimer(timeInterval: 1.0 / 25, target: self, selector: "timeTickUpdate:", userInfo: nil, repeats: true)
    }
    
    func timeTickUpdate(_ timer:Timer)
    {
        if shape.stepsAvaiable
        {
            shape.drawNextRound()
        }
        else
        {
            timer.invalidate()
        }
    }
    
    //MARK: zoom
    func viewForZooming(in scrollView: UIScrollView) -> UIView?
    {
        return shape
    }

    override func didReceiveMemoryWarning()
    {
        super.didReceiveMemoryWarning()
    }
    
    deinit
    {
        tapGuesture.removeTarget(self, action: "tapAction:")
    }
}

