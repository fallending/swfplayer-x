//
//  VectorImageView.swift
//  SWFShapeDraw
//
//  Created by larryhou on 25/9/2015.
//  Copyright © 2015 larryhou. All rights reserved.
//
import Foundation
import UIKit

class VectorImageView:UIView
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
    
    struct GraphicsAction:OptionSet
    {
        let rawValue:Int
        init(rawValue:Int) { self.rawValue = rawValue }
        
        static let LineStyle         = GraphicsAction(rawValue: 1 << 0)
        static let LineGradientStyle = GraphicsAction(rawValue: 1 << 1)
        static let BeginFill         = GraphicsAction(rawValue: 1 << 2)
        static let BeginGradientFill = GraphicsAction(rawValue: 1 << 3)
        static let EndFill           = GraphicsAction(rawValue: 1 << 4)
        static let MoveTo            = GraphicsAction(rawValue: 1 << 5)
        static let LineTo            = GraphicsAction(rawValue: 1 << 6)
        static let CurveTo           = GraphicsAction(rawValue: 1 << 7)
        
        static let ChangeLineStyle:GraphicsAction = [GraphicsAction.LineStyle, GraphicsAction.LineGradientStyle]
        static let ChangeFillStyle:GraphicsAction = [GraphicsAction.BeginFill, GraphicsAction.BeginGradientFill]
        static let ChangeStyle:GraphicsAction = [GraphicsAction.ChangeLineStyle, GraphicsAction.ChangeFillStyle]
        
        static func from(_ method:String) -> GraphicsAction?
        {
            switch method
            {
                case "MOVE_TO"             :return GraphicsAction.MoveTo
                case "LINE_TO"             :return GraphicsAction.LineTo
                case "CURVE_TO"            :return GraphicsAction.CurveTo
                case "LINE_STYLE"          :return GraphicsAction.LineStyle
                case "LINE_GRADIENT_STYLE" :return GraphicsAction.LineGradientStyle
                case "BEGIN_FILL"          :return GraphicsAction.BeginFill
                case "BEGIN_GRADIENT_FILL" :return GraphicsAction.BeginGradientFill
                case "END_FILL"            :return GraphicsAction.EndFill
                default:return nil
            }
        }
    }
    
    fileprivate var state:StyleState!
    fileprivate var path:CGMutablePath!
    fileprivate var style:NSDictionary!
    fileprivate var lineWidth:CGFloat = 1.0
    fileprivate var lineJoin = CGLineJoin.round
    fileprivate var lineCap = CGLineCap.round
    fileprivate var miterLimit:CGFloat = 3.0
    
    var steps:[(method:String, params:NSDictionary)]!
    var iframe:CGRect!
    
    //MARK: import
    func importVectorGraphics(_ graphics:NSArray)
    {
        func union(_ rect:inout (left:CGFloat, top:CGFloat, right:CGFloat, bottom:CGFloat), x:CGFloat, y:CGFloat)
        {
            rect.left   = min(x, rect.left)
            rect.right  = max(x, rect.right)
            rect.top    = min(y, rect.top)
            rect.bottom = max(y, rect.bottom)
        }
        
        steps = []
        
        var rect:(left:CGFloat, top:CGFloat, right:CGFloat, bottom:CGFloat) = (0,0,0,0)
        for i in 0..<graphics.count
        {
            let data = graphics[i] as! NSArray
            let method = data.object(at: 0) as! String
            let params = data.object(at: 1) as! NSDictionary
            
            if let action = GraphicsAction.from(method)
            {
                steps.append((method, params))
                
                var x:CGFloat = 0.0, y:CGFloat = 0.0
                switch action
                {
                    case GraphicsAction.MoveTo, GraphicsAction.LineTo:
                        x = CGFloat(params.value(forKey: "x") as! Double)
                        y = CGFloat(params.value(forKey: "y") as! Double)
                        union(&rect, x: x, y: y)
                        
                    case GraphicsAction.CurveTo:
                        x = CGFloat(params.value(forKey: "anchorX") as! Double)
                        y = CGFloat(params.value(forKey: "anchorY") as! Double)
                        union(&rect, x: x, y: y)
                        
                        x = CGFloat(params.value(forKey: "controlX") as! Double)
                        y = CGFloat(params.value(forKey: "controlY") as! Double)
                        union(&rect, x: x, y: y)
                    default:break
                }
            }
            else
            {
                print("SKIP", method, params)
            }
        }
        
        iframe = CGRect(x: rect.left, y: rect.top, width: rect.right - rect.left, height: rect.bottom - rect.top)
    }
    
    //MARK: utils
    func getValue(_ params:NSDictionary, key:String) -> AnyObject?
    {
        let data = params.value(forKeyPath: key)
        if let data = data , data is NSNull
        {
            return nil
        }
        
        return data as AnyObject
    }
    
    func getCGFloat(_ params:NSDictionary, key:String, dftValue:CGFloat = 0.0) -> CGFloat
    {
        if let data = getValue(params, key: key)
        {
            return CGFloat(data as! Double)
            
        }
        
        return dftValue
    }
    
    func getUIColor(_ params:NSDictionary, colorKey:String, alphaKey:String) -> UIColor
    {
        let rgbColor:Int
        if let data = getValue(params, key: colorKey)
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
        if let data = getValue(params, key: alphaKey)
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
    
    //MARK: draw
    override func draw(_ rect: CGRect)
    {
        let context = UIGraphicsGetCurrentContext()
        
        let margin:CGFloat = 0.0
        let scale = min((rect.width - margin) / iframe.width, (rect.height - margin) / iframe.height)
        context.scaleBy(x: scale, y: scale)
        
        let translateX = -(iframe.origin.x + iframe.width  / 2) + rect.width  / 2 / scale
        let translateY = -(iframe.origin.y + iframe.height / 2) + rect.height / 2 / scale
        context.translateBy(x: translateX, y: translateY)
        
        for i in 0..<steps.count
        {
            let step = steps[i]
            if let action = GraphicsAction.from(step.method)
            {
                let params = step.params
                if GraphicsAction.ChangeStyle.contains(action) || action == GraphicsAction.EndFill
                {
                    flushContext(context)
                }
                
                switch action
                {
                    case GraphicsAction.LineStyle:
                        state = StyleState.SolidStroke
                        style = params
                        
                        path = CGMutablePath()
                        
                    case GraphicsAction.LineGradientStyle:
                        state = StyleState.GradientStroke
                        style = params
                        
                        path = CGMutablePath()
                        
                    case GraphicsAction.LineTo:
                        CGPathAddLineToPoint(path, nil,
                            getCGFloat(params, key: "x"), getCGFloat(params, key: "y"))
                        
                    case GraphicsAction.MoveTo:
                        CGPathMoveToPoint(path, nil,
                            getCGFloat(params, key: "x"), getCGFloat(params, key: "y"))
                        
                    case GraphicsAction.CurveTo:
                        CGPathAddQuadCurveToPoint(path, nil,
                            getCGFloat(params, key: "controlX"), getCGFloat(params, key: "controlY"),
                            getCGFloat(params, key: "anchorX"),  getCGFloat(params, key: "anchorY"))
                        
                    case GraphicsAction.BeginFill:
                        state = StyleState.SolidFill
                        style = params
                        
                        path = CGMutablePath()
                        
                    case GraphicsAction.BeginGradientFill:
                        state = StyleState.GradientFill
                        style = params
                        
                        path = CGMutablePath()
                        
                    case GraphicsAction.EndFill:break
                    default:break
                }
            }
        }
        
        flushContext(context)
    }
    
    func flushContext(_ context:CGContext?)
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
                context.addPath(path)
                CGContextClip(context)
                fillContextWithGradientStyle(context, params: style)
                context.restoreGState()
            }
            else
            {
                context.addPath(path)
                let color = getUIColor(style, colorKey: "color", alphaKey: "alpha")
                context.setFillColor(color.cgColor)
                CGContextFillPath(context)
            }
        }
        else
        if StyleState.Stroke.contains(state)
        {
            if (state == StyleState.GradientStroke)
            {
                context.saveGState()
                strokeContextWithGradientStyle(context, path: path)
                context.restoreGState()
            }
            else
            {
                context.addPath(path)
                setContextStrokeStyle(context, params: style)
                context.strokePath()
            }
        }
        
        path = nil
    }
    
    func setContextStrokeStyle(_ context:CGContext?, params:NSDictionary)
    {
        let color = getUIColor(params, colorKey: "color", alphaKey: "alpha")
        context.setStrokeColor(color.cgColor)
        
        lineWidth = getCGFloat(params, key: "thickness", dftValue: 0.0)
        context.setLineWidth(lineWidth)
        
        if let data = getValue(params, key: "caps")
        {
            let type = data as! String
            switch type
            {
                case "square": lineCap = CGLineCap.square
                case "round" : lineCap = CGLineCap.round
                default: lineCap = CGLineCap.butt
            }
        }
        else
        {
            lineCap = CGLineCap.butt
        }
        
        context.setLineCap(lineCap)
        
        if let data = getValue(params, key: "joints")
        {
            let type = data as! String
            switch type
            {
                case "bevel": lineJoin = CGLineJoin.bevel
                case "miter": lineJoin = CGLineJoin.miter
                default:lineJoin = CGLineJoin.round
            }
        }
        else
        {
            lineJoin = CGLineJoin.round
        }
        
        context.setLineJoin(lineJoin)
        
        miterLimit = getCGFloat(params, key: "miterLimit", dftValue: 3.0)
        context.setMiterLimit(miterLimit)
    }
    
    func strokeContextWithGradientStyle(_ context:CGContext?, path:CGMutablePath)
    {
        let gradientPath = CGPath(__byStroking: path, transform: nil, lineWidth: lineWidth, lineCap: lineCap, lineJoin: lineJoin, miterLimit: miterLimit)
        context.addPath(gradientPath)
        CGContextClip(context)
        
        fillContextWithGradientStyle(context, params: style)
    }
    
    func fillContextWithGradientStyle(_ context:CGContext?, params:NSDictionary)
    {
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
        
        var locations = (params.value(forKey: "ratios") as! [Int]).map({ CGFloat($0) / 0xFF })
        let gradient = CGGradient(colorsSpace: CGColorSpaceCreateDeviceRGB(), colors: rgbaColors.map({$0.cgColor}), locations: &locations)
        
        var matrix:(a:CGFloat, b:CGFloat, c:CGFloat, d:CGFloat, tx:CGFloat, ty:CGFloat) = (0,0,0,0,0,0)
        matrix.a  = getCGFloat(params, key: "matrix.a")
        matrix.b  = getCGFloat(params, key: "matrix.b")
        matrix.c  = getCGFloat(params, key: "matrix.c")
        matrix.d  = getCGFloat(params, key: "matrix.d")
        matrix.tx = getCGFloat(params, key: "matrix.tx")
        matrix.ty = getCGFloat(params, key: "matrix.ty")
        
        let angle = atan2(matrix.b, matrix.a)
        
        let scaleX = matrix.a / cos(angle)
        let scaleY = matrix.d / cos(angle)
        let width = scaleX * 1638.4, height = scaleY * 1638.4
        
        let focalPointRatio = getCGFloat(params, key: "focalPointRatio")
        let type = params.value(forKey: "type") as! String
        if type == "radial"
        {
            let radius = max(width / 2, height / 2);
            let startCenter = CGPoint(x: width / 2 * cos(angle) * focalPointRatio + matrix.tx,
                                         y: height / 2 * sin(angle) * focalPointRatio + matrix.ty)
            let endCenter = CGPoint(x: matrix.tx, y: matrix.ty)
            
            context.drawRadialGradient(gradient, startCenter: startCenter, startRadius: 0, endCenter: endCenter, endRadius: radius,
                options: [CGGradientDrawingOptions.drawsAfterEndLocation])
        }
        else
        {
            let sp = CGPoint(x: -819.2 * scaleX, y: 0)
            let startPoint = CGPoint(x: sp.x * cos(angle) - sp.y * sin(angle) + matrix.tx,
                                         y: sp.x * sin(angle) + sp.y * cos(angle) + matrix.ty)
            let ep = CGPoint( x: 819.2 * scaleX, y: 0)
            let endPoint = CGPoint(x: ep.x * cos(angle) - ep.y * sin(angle) + matrix.tx,
                                       y: ep.x * sin(angle) + ep.y * cos(angle) + matrix.ty)
            
            context.drawLinearGradient(gradient, start: startPoint, end: endPoint,
                options: [CGGradientDrawingOptions.drawsBeforeStartLocation, CGGradientDrawingOptions.drawsAfterEndLocation])
        }
    }
}
