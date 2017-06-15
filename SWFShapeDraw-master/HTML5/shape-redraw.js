var SWFShape = 
{
	init:function(context, steps)
	{
		var shape = {}
		shape.context = context
		shape.steps = steps

		shape.commands = []
		function println(msg)
		{
			console.log(msg)
			shape.commands.push(msg)
		}

		console.log(context)

		var rect = {left:0, top:0, right:0, bottom:0}
		function unionBoundsWithPoint(bounds, x, y)
		{
			bounds.left   = Math.min(x, bounds.left)
			bounds.right  = Math.max(x, bounds.right)
			bounds.top    = Math.min(y, bounds.top)
			bounds.bottom = Math.max(y, bounds.bottom)
		}

		for (var i = 0; i < steps.length; i++)
		{
			var method = steps[i][0]
			var params = steps[i][1]
			switch (method)
			{
				case "LINE_TO":case "MOVE_TO":
				{
					unionBoundsWithPoint(rect, params.x, params.y)
					break
				}

				case "CURVE_TO":
				{
					unionBoundsWithPoint(rect, params.controlX, params.controlY)
					unionBoundsWithPoint(rect, params.anchorX, params.anchorY)
					break
				}
			}
		}

		console.log(rect)

		var bounds = {x:rect.left, y:rect.top, width:rect.right - rect.left, height:rect.bottom - rect.top}
		println("var bounds = {x:" + bounds.x + ", y:" + bounds.y + ", width:" + bounds.width + ", height:" + bounds.height +"}")
		var canvas = context.canvas
		println("var canvas = context.canvas")

		var margin = 40
		println("var margin = " + margin)
		var scale = Math.min((canvas.width - margin) / bounds.width, (canvas.height - margin) / bounds.height)
		println("var scale = Math.min((canvas.width - margin) / bounds.width, (canvas.height - margin) / bounds.height)")
		context.scale(scale, scale)
		println("context.scale(scale, scale)")

		var translateX = -(bounds.x + bounds.width  / 2) + canvas.width  / 2 / scale
		println("var translateX = -(bounds.x + bounds.width  / 2) + canvas.width  / 2 / scale")
		var translateY = -(bounds.y + bounds.height / 2) + canvas.height / 2 / scale
		println("var translateY = -(bounds.y + bounds.height / 2) + canvas.height / 2 / scale")
		context.translate(translateX, translateY)
		println("context.translate(translateX, translateY)")

		function getRGBAColor(color, alpha)
		{
			var b = (color >> 0 ) & 0xFF
			var g = (color >> 8 ) & 0xFF
			var r = (color >> 16) & 0xFF
			return "rgba(" + r + "," + g + "," + b + "," + alpha + ")"
		}

		function createContextGradient(data)
		{
			var matrix = data.matrix
			var angle = Math.atan2(matrix.b, matrix.a)
			var scaleX = matrix.a / Math.cos(angle)
			var scaleY = matrix.d / Math.cos(angle)

			var width  = scaleX * 1638.4
			var height = scaleY * 1638.4

			var gradient
			if (data.type == "radial")
			{
				var endRadius = Math.max(width / 2, height / 2)
				var startCenter = {
					x:  width / 2 * Math.cos(angle) * data.focalPointRatio + matrix.tx,
					y: height / 2 * Math.sin(angle) * data.focalPointRatio + matrix.ty
				}

				var endCenter = {x:matrix.tx, y:matrix.ty}
				gradient = context.createRadialGradient(startCenter.x, startCenter.y, 0, endCenter.x, endCenter.y, endRadius)
				println("gradient = context.createRadialGradient(" + [startCenter.x, startCenter.y, 0, endCenter.x, endCenter.y, endRadius].join(", ") + ")")
			}
			else
			{
				var sp = {x:-819.2 * scaleX, y:0}
				var startPoint = {
					x:sp.x * Math.cos(angle) - sp.y * Math.sin(angle) + matrix.tx,
					y:sp.x * Math.sin(angle) + sp.y * Math.cos(angle) + matrix.ty
				}

				var ep = {x: 819.2 * scaleX, y:0}
				var endPoint = {
					x:ep.x * Math.cos(angle) - ep.y * Math.sin(angle) + matrix.tx,
					y:ep.x * Math.sin(angle) + ep.y * Math.cos(angle) + matrix.ty
				}

				gradient = context.createLinearGradient(startPoint.x, startPoint.y, endPoint.x, endPoint.y)
				println("gradient = context.createLinearGradient(" + [startPoint.x, startPoint.y, endPoint.x, endPoint.y].join(", ") + ")")
			}

			for (var i = 0; i < data.colors.length; i++)
			{
				gradient.addColorStop(data.ratios[i] / 255, getRGBAColor(data.colors[i], data.alphas[i]))
				println("gradient.addColorStop(" + data.ratios[i] / 255 + ", '" + getRGBAColor(data.colors[i], data.alphas[i]) + "')")
			}

			return gradient
		}

		var styleState

		var pathAvailable = false
		function flushCurrentContext()
		{
			if (pathAvailable == false) return

			switch(styleState)
			{
				case "SOLID_STROKE":case "GRADIENT_STROKE":
				{
					context.stroke()
					println("context.stroke()")
					break
				}

				case "SOLID_FILL":case "GRADIENT_FILL":
				{
					context.closePath()
					println("context.closePath()")
					context.fill()
					println("context.fill()")
					break
				}
			}

			pathAvailable = false
		}

		shape.begin = function()
		{
			println("var gradient")
		}

		function drawStep(method, params)
		{
			switch(method)
			{
				case "LINE_TO":
				{
					context.lineTo(params.x, params.y)
					println("context.lineTo(" + [params.x, params.y].join(", ") + ")")
					break
				}

				case "MOVE_TO":
				{
					context.moveTo(params.x, params.y)
					println("context.moveTo(" + [params.x, params.y].join(", ") + ")")
					break
				}

				case "CURVE_TO":
				{
					context.quadraticCurveTo(params.controlX, params.controlY, params.anchorX, params.anchorY)
					println("context.quadraticCurveTo(" + [params.controlX, params.controlY, params.anchorX, params.anchorY].join(", ") + ")")
					break
				}

				case "LINE_STYLE":
				{
					styleState = "SOLID_STROKE"
					context.lineCap = params.caps == "none"? "butt" : params.caps
					println("context.lineCap = '" + context.lineCap + "'")
					context.lineJoin = params.joints
					println("context.lineJoin = '" + context.lineJoin + "'")
					context.lineWidth = params.thickness
					println("context.lineWidth = " + context.lineWidth)
					context.miterLimit = params.miterLimit
					println("context.miterLimit = " + context.miterLimit)
					context.strokeStyle = getRGBAColor(params.color, params.alpha)
					println("context.strokeStyle = '" + getRGBAColor(params.color, params.alpha) + "'")
					context.beginPath()
					println("context.beginPath()")
					pathAvailable = true
					break
				}

				case "LINE_GRADIENT_STYLE":
				{
					styleState = "GRADIENT_STROKE"
					context.strokeStyle = createContextGradient(params)
					println("context.strokeStyle = gradient")
					context.beginPath()
					println("context.beginPath()")
					pathAvailable = true
					break
				}

				case "BEGIN_FILL":
				{
					styleState = "SOLID_FILL"
					context.fillStyle = getRGBAColor(params.color, params.alpha)
					println("context.fillStyle = '" + getRGBAColor(params.color, params.alpha) + "'")
					context.beginPath()
					println("context.beginPath()")
					pathAvailable = true
					break
				}

				case "BEGIN_GRADIENT_FILL":
				{
					styleState = "GRADIENT_FILL"
					context.fillStyle = createContextGradient(params)
					println("context.fillStyle = gradient")
					context.beginPath()
					println("context.beginPath()")
					pathAvailable = true
					break
				}

				case "END_FILL":
				{
					break
				}
			}
		}

		var currentIndex = 0
		shape.drawNextRound = function()
		{
			var index = currentIndex
			var complete = false

			while (index < steps.length)
			{
				var method = steps[index][0]
				var params = steps[index][1]
				switch(method)
				{
					case "LINE_STYLE":
					case "LINE_GRADIENT_STYLE":
					case "BEGIN_FILL":
					case "BEGIN_GRADIENT_FILL":
					case "END_FILL":
					{
						if (index > currentIndex)
						{
							complete = true
							break
						}
					}
				}

				if (complete) break

				drawStep(method, params)
				index++
				
				// if (styleState == "SOLID_STROKE" || styleState == "GRADIENT_STROKE")
				// {
				// 	if (method == "LINE_TO" || method == "CURVE_TO")
				// 	{
				// 		currentIndex = index
				// 		context.stroke()
				// 		println("context.stroke()")
				// 		return
				// 	}
				// }
			}

			flushCurrentContext()
			currentIndex = index
		}
		
		shape.stepsAvailable = function()
		{
			return currentIndex < steps.length
		}
		
		return shape
	}
}