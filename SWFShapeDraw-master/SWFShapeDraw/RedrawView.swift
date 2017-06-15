//
//  GraphView.swift
//  SWFShapeDraw
//
//  Created by larryhou on 22/8/2015.
//  Copyright © 2015 larryhou. All rights reserved.
//

import Foundation
import UIKit

class RedrawView:UIImageView
{
    struct StyleState:OptionSet
    {
        let rawValue:Int
        init(rawValue:Int) { self.rawValue = rawValue }
        
        static let SolidStroke    = StyleState(rawValue: 1 << 0)
        static let GradientStroke = StyleState(rawValue: 1 << 1)
        static let SolidFill      = StyleState(rawValue: 1 << 2)
        static let GradientFill   = StyleState(rawValue: 1 << 3)
        
        static let Stroke:StyleState = [StyleState.SolidStroke, StyleState.GradientStroke]
        static let Fill:StyleState   = [StyleState.SolidFill,   StyleState.GradientFill]
    }
    
    struct DrawAction:OptionSet
    {
        let rawValue:Int
        init(rawValue:Int) { self.rawValue = rawValue }
        
        static let LineStyle         = DrawAction(rawValue: 1 << 0)
        static let LineGradientStyle = DrawAction(rawValue: 1 << 1)
        static let BeginFill         = DrawAction(rawValue: 1 << 2)
        static let BeginGradientFill = DrawAction(rawValue: 1 << 3)
        static let EndFill           = DrawAction(rawValue: 1 << 4)
        static let MoveTo            = DrawAction(rawValue: 1 << 5)
        static let LineTo            = DrawAction(rawValue: 1 << 6)
        static let CurveTo           = DrawAction(rawValue: 1 << 7)
        
        static let ChangeLineStyle:DrawAction = [DrawAction.LineStyle, DrawAction.LineGradientStyle]
        static let ChangeFillStyle:DrawAction = [DrawAction.BeginFill, DrawAction.BeginGradientFill]
        static let ChangeStyle:DrawAction = [DrawAction.ChangeLineStyle, DrawAction.ChangeFillStyle]
        
        static func from(_ method:String) -> DrawAction?
        {
            switch method
            {
                case "MOVE_TO"             :return DrawAction.MoveTo
                case "LINE_TO"             :return DrawAction.LineTo
                case "CURVE_TO"            :return DrawAction.CurveTo
                case "LINE_STYLE"          :return DrawAction.LineStyle
                case "LINE_GRADIENT_STYLE" :return DrawAction.LineGradientStyle
                case "BEGIN_FILL"          :return DrawAction.BeginFill
                case "BEGIN_GRADIENT_FILL" :return DrawAction.BeginGradientFill
                case "END_FILL"            :return DrawAction.EndFill
                default:return nil
            }
        }
    }
    
    struct GradientStyleInfo
    {
        var type:String
        var gradient:CGGradient
        var matrix:(a:CGFloat, b:CGFloat, c:CGFloat, d:CGFloat, tx:CGFloat, ty:CGFloat)
        var focalPointRatio:CGFloat
    }
    
    fileprivate var state:StyleState!
    fileprivate var steps:[(method:String, params:NSDictionary)] = []
    
    fileprivate var path:CGMutablePath!
    fileprivate var style:NSDictionary!
    
    fileprivate var lineWidth:CGFloat = 1.0
    fileprivate var lineJoin = CGLineJoin.round
    fileprivate var lineCap = CGLineCap.round
    fileprivate var miterLimit:CGFloat = 3.0
    
    var irect:CGRect!
    
    var currentIndex:Int = 0
    var stepsAvaiable:Bool { return currentIndex < steps.count }
    
    func importSteps(_ data:[(method:String, params:NSDictionary)])
    {
        steps = data
        currentIndex = 0
    }
    
    func getUIColorCode(_ color:UIColor)->String
    {
        var r:CGFloat = 0, g:CGFloat = 0, b:CGFloat = 0, a:CGFloat = 0
        color.getRed(&r, green: &g, blue: &b, alpha: &a)
        return String(format: "UIColor(red:%.2f, green:%.2f, blue:%.2f, alpha:%.2f)", r, g, b, a)
    }
    
    func getGradientStyle(_ params:NSDictionary) -> GradientStyleInfo
    {
        let type = params.value(forKey: "type") as! String
        let colors = params.value(forKey: "colors") as! [Int]
        let alphas = params.value(forKey: "alphas") as! [Double]
        
        var rgbaColors:[UIColor] = []
        for i in 0..<colors.count
        {
            let rgbColor = colors[i]
            let b = (rgbColor >> 00) & 0xFF
            let g = (rgbColor >> 08) & 0xFF
            let r = (rgbColor >> 16) & 0xFF
            let color = UIColor(red: CGFloat(r)/0xFF, green: CGFloat(g)/0xFF, blue: CGFloat(b)/0xFF, alpha: CGFloat(alphas[i]))
            rgbaColors.append(color)
        }
        
        let ratios = params.value(forKey: "ratios") as! [Int]
        var locations = ratios.map({ CGFloat($0) / 0xFF })
        
        let gradient = CGGradient(colorsSpace: CGColorSpaceCreateDeviceRGB(), colors: rgbaColors.map({$0.cgColor}), locations: &locations)
        
        print("colors = [" + rgbaColors.map({ getUIColorCode($0) + ".CGColor" }).joined(separator: ",") + "]")
        print("locations = [" + locations.map({String(format:"%.4f", $0)}).joined(separator: ",") + "]")
        print("gradient = CGGradientCreateWithColors(CGColorSpaceCreateDeviceRGB(), colors, &locations)")
        
        var matrix:(a:CGFloat, b:CGFloat, c:CGFloat, d:CGFloat, tx:CGFloat, ty:CGFloat) = (0,0,0,0,0,0)
        matrix.a = CGFloat(params.value(forKeyPath: "matrix.a") as! Double)
        matrix.b = CGFloat(params.value(forKeyPath: "matrix.b") as! Double)
        matrix.c = CGFloat(params.value(forKeyPath: "matrix.c") as! Double)
        matrix.d = CGFloat(params.value(forKeyPath: "matrix.d") as! Double)
        matrix.tx = CGFloat(params.value(forKeyPath: "matrix.tx") as! Double)
        matrix.ty = CGFloat(params.value(forKeyPath: "matrix.ty") as! Double)
        
        let focalPointRatio = CGFloat(params.value(forKey: "focalPointRatio") as! Double)
        return GradientStyleInfo(type: type, gradient: gradient!, matrix: matrix, focalPointRatio: focalPointRatio)
    }
    
    func getCoord(_ params:NSDictionary, key:String) -> CGFloat
    {
        if let data = getUnifyValue(params, key: key)
        {
            return CGFloat(data as! Double)
            
        }
        
        return 0.0
    }
    
    func getColor(_ params:NSDictionary, colorKey:String, alphaKey:String) -> UIColor
    {
        let rgbColor:Int
        if let data = getUnifyValue(params, key: colorKey)
        {
            rgbColor = data as! Int
        }
        else
        {
            rgbColor = 0
        }
        
        let b = (rgbColor >> 00) & 0xFF
        let g = (rgbColor >> 08) & 0xFF
        let r = (rgbColor >> 16) & 0xFF
        
        let a:CGFloat
        if let data = getUnifyValue(params, key: alphaKey)
        {
            a = CGFloat(data as! Double)
        }
        else
        {
            a = 1.0
        }
        
        let color = UIColor(red: CGFloat(r)/0xFF, green: CGFloat(g)/0xFF, blue: CGFloat(b)/0xFF, alpha: a)
        return color
    }
    
    func drawNextRound()
    {
        let rect = bounds
        
        UIGraphicsBeginImageContextWithOptions(rect.size, false, UIScreen.main.scale)
        let context = UIGraphicsGetCurrentContext()
        if let image = self.image
        {
            image.draw(in: rect)
        }
        
        let margin:CGFloat = 20.0
        let scale = min((rect.width - margin) / irect.width, (rect.height - margin) / irect.height)
        context.scaleBy(x: scale, y: scale)
        
        let translateX = -(irect.origin.x + irect.width  / 2) + rect.width  / 2 / scale
        let translateY = -(irect.origin.y + irect.height / 2) + rect.height / 2 / scale
        context.translateBy(x: translateX, y: translateY)
        
        if currentIndex == 0
        {
            print("let context = UIGraphicsGetCurrentContext()")
            print("var gradient:CGGradient!")
            print("var path:CGMutablePath!")
            print("var locations:[CGFloat]")
            print("var colors:[CGColor]")
            print("")
            
            print("CGContextSaveGState(context)")
            print(String(format: "let scale = min((rect.width - %.2f) / %.2f, (rect.height - %.2f) / %.2f)", margin, irect.width, margin, irect.height))
            print(String(format: "CGContextScaleCTM(context, scale, scale)"))
            
            print(String(format: "let translateX = -(%.2f + %.2f / 2) + rect.width  / 2 / scale", irect.origin.x, irect.width ))
            print(String(format: "let translateY = -(%.2f + %.2f / 2) + rect.height / 2 / scale", irect.origin.y, irect.height ))
            print("CGContextTranslateCTM(context, translateX, translateY)")
            print("")
        }
        
        var index = currentIndex
        while index < steps.count
        {
            let step = steps[index]
            if let action = DrawAction.from(step.method)
            {
                if DrawAction.ChangeStyle.contains(action) || action == DrawAction.EndFill
                {
                    if index > currentIndex
                    {
                        break
                    }
                }
            }
            
            drawStep(context, step: step)
            index++
        }
        
        flushCurrentContext(context)
        
        self.image = UIGraphicsGetImageFromCurrentImageContext()
        
        UIGraphicsEndImageContext()
        
        currentIndex = index
        if currentIndex == steps.count
        {
            print("CGContextRestoreGState(context)")
        }
    }
    
    func drawStep(_ context:CGContext?, step:(method:String, params:NSDictionary))
    {
        let params = step.params
        guard let action = DrawAction.from(step.method) else
        {
            return
        }
        
        switch action
        {
            case DrawAction.LineStyle:
                state = StyleState.SolidStroke
                style = params
                
                print("// BEGIN-SOLID-STROKE")
                
                path = CGMutablePath()
                print("path = CGPathCreateMutable()")
            
            case DrawAction.LineGradientStyle:
                state = StyleState.GradientStroke
                style = params
                
                print("// BEGIN-GRADIENT-STROKE")
                
                path = CGMutablePath()
                print("path = CGPathCreateMutable()")
            
            case DrawAction.LineTo:
                CGPathAddLineToPoint(path, nil,
                    getCoord(params, key: "x"), getCoord(params, key: "y"))
                print(String(format:"CGPathAddLineToPoint(path, nil, %6.2f, %6.2f)", getCoord(params, key: "x"), getCoord(params, key: "y")))
            
            case DrawAction.MoveTo:
                CGPathMoveToPoint(path, nil,
                    getCoord(params, key: "x"), getCoord(params, key: "y"))
                print(String(format:"CGPathMoveToPoint(path, nil, %6.2f, %6.2f)", getCoord(params, key: "x"), getCoord(params, key: "y")))
            
            case DrawAction.CurveTo:
                CGPathAddQuadCurveToPoint(path, nil,
                    getCoord(params, key: "controlX"), getCoord(params, key: "controlY"),
                    getCoord(params, key: "anchorX"),  getCoord(params, key: "anchorY"))
                print(String(format:"CGPathAddQuadCurveToPoint(path, nil, %6.2f, %6.2f, %6.2f, %6.2f)",
                    getCoord(params, key: "controlX"), getCoord(params, key: "controlY"),
                    getCoord(params, key: "anchorX"),  getCoord(params, key: "anchorY")))
                
            case DrawAction.BeginFill:
                state = StyleState.SolidFill
                style = params
                
                print("// BEGIN-SOLID-FILL")
                
                path = CGMutablePath()
                print("path = CGPathCreateMutable()")
            
            case DrawAction.BeginGradientFill:
                state = StyleState.GradientFill
                style = params
                
                print("// BEGIN-GRADIENT-FILL")
                
                path = CGMutablePath()
                print("path = CGPathCreateMutable()")
            
            case DrawAction.EndFill:break
            default:break
        }
    }
    
    func flushCurrentContext(_ context:CGContext?)
    {
        if state == nil || path == nil
        {
            return
        }
        
        if StyleState.Fill.contains(state)
        {
            if state == StyleState.GradientFill
            {
                context.saveGState()
                print("CGContextSaveGState(context)")
                context.addPath(path)
                print("CGContextAddPath(context, path)")
                CGContextClip(context)
                print("CGContextClip(context)")
                fillContextGradientStyle(context, params: style)
                context.restoreGState()
                print("CGContextRestoreGState(context)")
                print("// END-GRADIENT-FILL")
            }
            else
            {
                context.addPath(path)
                print("CGContextAddPath(context, path)")
                let color = getColor(style, colorKey: "color", alphaKey: "alpha")
                context.setFillColor(color.cgColor)
                print(String(format: "CGContextSetFillColorWithColor(context, %@.CGColor)", getUIColorCode(color)))
                CGContextFillPath(context)
                print("CGContextFillPath(context)")
                print("// END-SOLID-FILL")
            }
        }
        else
        if StyleState.Stroke.contains(state)
        {
            if (state == StyleState.GradientStroke)
            {
                context.saveGState()
                print("CGContextSaveGState(context)")
                strokePathWithGradientStyle(context, path: path)
                context.restoreGState()
                print("CGContextRestoreGState(context)")
                print("// END-GRADIENT-STROKE")
            }
            else
            {
                context.addPath(path)
                print("CGContextAddPath(context, path)")
                setContextLineSolidColorStyle(context, params: style)
                context.strokePath()
                print("CGContextStrokePath(context)")
                print("// END-SOLID-STROKE")
            }
        }
        
        print("")
        path = nil
    }
    
    func setContextLineSolidColorStyle(_ context:CGContext?, params:NSDictionary)
    {
        let r:Int, g:Int, b:Int
        if let data = getUnifyValue(params, key: "color")
        {
            let rgbColor = data as! Int
            b = (rgbColor >> 00) & 0xFF
            g = (rgbColor >> 08) & 0xFF
            r = (rgbColor >> 16) & 0xFF
        }
        else
        {
            r = 0; g = 0; b = 0
        }
        
        var a:CGFloat = 1.0
        if let data = getUnifyValue(params, key: "alpha")
        {
            a = CGFloat(data as! Double)
        }
        
        let color = UIColor(red: CGFloat(r)/0xFF, green: CGFloat(g)/0xFF, blue: CGFloat(b)/0xFF, alpha: a)
        context.setStrokeColor(color.cgColor)
        print(String(format: "CGContextSetStrokeColorWithColor(context, %@.CGColor)", getUIColorCode(color) as NSString))
        
        if let data = getUnifyValue(params, key: "thickness")
        {
            lineWidth = CGFloat(data as! Double)
        }
        else
        {
            lineWidth = 0.0
        }
        
        context.setLineWidth(lineWidth)
        print(String(format: "CGContextSetLineWidth(context, %.2f)", lineWidth))
        
        if let data = getUnifyValue(params, key: "caps")
        {
            let type = data as! String
            switch type
            {
                case "square": lineCap = CGLineCap.square
                case "round": lineCap = CGLineCap.round
                default: lineCap = CGLineCap.butt
            }
        }
        else
        {
            lineCap = CGLineCap.butt
        }
        
        context.setLineCap(lineCap)
        print(String(format: "CGContextSetLineCap(context, CGLineCap(rawValue:%d)!)", lineCap.rawValue))
        
        if let data = getUnifyValue(params, key: "joints")
        {
            let type = data as! String
            switch type
            {
                case "bevel": lineJoin = CGLineJoin.bevel
                case "miter": lineJoin = CGLineJoin.miter
                case "round":fallthrough
                default:lineJoin = CGLineJoin.round
            }
        }
        else
        {
            lineJoin = CGLineJoin.round
        }
        
        context.setLineJoin(lineJoin)
        print(String(format: "CGContextSetLineJoin(context, CGLineJoin(rawValue:%d)!)", lineJoin.rawValue))
        
        if let data = getUnifyValue(params, key: "miterLimit")
        {
            miterLimit = CGFloat(data as! Double)
        }
        else
        {
            miterLimit = 3.0
        }
        
        context.setMiterLimit(miterLimit)
        print(String(format: "CGContextSetMiterLimit(context, %.2f)", miterLimit))
    }
    
    func getUnifyValue(_ params:NSDictionary, key:String) -> AnyObject?
    {
        let data = params.value(forKey: key)
        if let data = data , data is NSNull
        {
            return nil
        }
        
        return data
    }
    
    func strokePathWithGradientStyle(_ context:CGContext?, path:CGMutablePath)
    {
        let gradientPath = CGPath(__byStroking: path, transform: nil, lineWidth: lineWidth, lineCap: lineCap, lineJoin: lineJoin, miterLimit: miterLimit)
        context.addPath(gradientPath)
        print(String(format: "gradientPath = CGPathCreateCopyByStrokingPath(path, nil, %.2f, CGLineCap(rawValue:%d)!, CGLineJoin(rawValue:%d)!, %.2f)", lineWidth, lineCap.rawValue, lineJoin.rawValue, miterLimit))
        print("CGContextAddPath(context, gradientPath)")
        CGContextClip(context)
        print("CGContextClip(context)")
        
        fillContextGradientStyle(context, params: style)
    }
    
    func fillContextGradientStyle(_ context:CGContext?, params:NSDictionary)
    {
        let style = getGradientStyle(params)
        let matrix = style.matrix
        
        let angle = atan2(matrix.b, matrix.a)
        let scaleX = matrix.a / cos(angle)
        let scaleY = matrix.d / cos(angle)
        
        let width = scaleX * 1638.4, height = scaleY * 1638.4
        
        if style.type == "radial"
        {
            if width != height
            {
                //TODO: 椭圆情况做变形处理
            }
            
            let endRadius = max(width / 2, height / 2);
            let startCenter = CGPoint(x: width / 2 * cos(angle) * style.focalPointRatio + matrix.tx,
                                         y: height / 2 * sin(angle) * style.focalPointRatio + matrix.ty)
            let endCenter = CGPoint(x: matrix.tx, y: matrix.ty)
            
            context.drawRadialGradient(style.gradient, startCenter: startCenter, startRadius: 0, endCenter: endCenter, endRadius: endRadius, options: CGGradientDrawingOptions.drawsAfterEndLocation)
            print(String(format: "CGContextDrawRadialGradient(context, gradient, CGPointMake(%.2f, %.2f), 0, CGPointMake(%.2f, %.2f), %.2f, CGGradientDrawingOptions.DrawsAfterEndLocation)",
                startCenter.x, startCenter.y, endCenter.x, endCenter.y, endRadius))
        }
        else
        {
            let sp = CGPoint(x: -819.2 * scaleX, y: 0)
            let startPoint = CGPoint(x: sp.x * cos(angle) - sp.y * sin(angle) + matrix.tx,
                                         y: sp.x * sin(angle) + sp.y * cos(angle) + matrix.ty)
            let ep = CGPoint( x: 819.2 * scaleX, y: 0)
            let endPoint = CGPoint(x: ep.x * cos(angle) - ep.y * sin(angle) + matrix.tx,
                                       y: ep.x * sin(angle) + ep.y * cos(angle) + matrix.ty)
            
            let options:CGGradientDrawingOptions = [CGGradientDrawingOptions.drawsBeforeStartLocation, CGGradientDrawingOptions.drawsAfterEndLocation]
            context.drawLinearGradient(style.gradient, start: startPoint, end: endPoint, options: options)
            print(String(format: "CGContextDrawLinearGradient(context, gradient, CGPointMake(%.2f, %.2f), CGPointMake(%.2f, %.2f), CGGradientDrawingOptions(rawValue: %d))",
                startPoint.x, startPoint.y, endPoint.x, endPoint.y, options.rawValue))
        }
    }
}
