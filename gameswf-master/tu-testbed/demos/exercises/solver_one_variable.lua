-- Root-finder for non-linear equations w/ one variable -- Thatcher Ulrich <tu@tulrich.com>
--
-- This program has been donated to the Public Domain.
--
-- Written in Lua version 5, see http://www.lua.org
--



solver = {}


-- size of a "typical" root to our problem; helps us decide when to stop
local typx = 1


function solver.global_solve(f, df, x0)
	--[[
	solve for a root, x*, of the function f.
	f takes one number, and returns one number.
	df is either nil, or a function that is the derivative of f.
	x0 is an initial guess for x*.
	]]

	-- return solver.global_bisection(f, df, x0)
	return solver.global_backtracking(f, df, x0)
	-- return solver.interpolation(f, df, x0)
end


function solver.global_bisection(f, df, x, z)
	--[[
	look for a root of f in the interval [x,z]
	]]

	

	-- try something
	-- run some iterations of solver.local_step()
	-- if not converging, try a different region
end


function solver.global_backtracking(f, df, xc)
	--[[
	look for a root of f, starting at xc.
	Use backtracking.
	]]

	local	x_plus
	local	iteration_count = 0
	while 1 do
		x_plus = solver.local_step(f, df, xc)
		iteration_count = iteration_count + 1

		print("Newton step: " .. x_plus)

		while (math.abs(f(x_plus)) >= math.abs(f(xc))) do
			-- Our Newton step took us further from 0, so backtrack.
			x_plus = (x_plus + xc) / 2
			iteration_count = iteration_count + 1

			print("Backtrack  : " .. x_plus)
		end

		-- Are we there yet?
		if (math.abs(x_plus) <= 1e-5
			or math.abs(x_plus - xc) / math.max(typx, math.abs(x_plus)) <= 1e-7)
		then
			-- Looks like we're done.
			print("Solved     : f(" .. x_plus .. ") = " .. f(x_plus) .. " in " .. iteration_count .. " steps")
			return x_plus
		end

		xc = x_plus
	end
end


function solver.local_step(f, df, xc)
	--[[
	return the next iteration towards (hopefully) the root of f,
	from x0.
	]]

	-- Newton.
	local deriv = df(xc)
	local x_plus = xc
	if (math.abs(deriv) < 1e-6) then
		-- some fallback
	else
		x_plus = xc - f(xc) / deriv
	end

	return x_plus
end


-- some tests
local tests = {
	{
		f_string = "function (x) return math.sin(x) - math.cos(2*x) end",
		df_string = "function (x) return math.cos(x) + 2 * math.sin(2*x) end",  -- @@ check this derivative
		x0 = { 1 }
	},
	{
		f_string = "function (x) return x^3 - 7*x^2 + 11*x - 5 end",
		df_string = "function (x) return 3*x^2 - 14*x + 11 end",
		x0 = { 2, 7 }
	},
	{
		f_string = "function (x) return math.sin(x) - math.cos(x) end",
		df_string = "function (x) return math.cos(x) + math.sin(x) end",	-- @@ check this derivative
		x0 = { 1 }
	},
	{
		f_string = "function (x) return x^4 + 12*x^3 + 47*x^2 + 60*x + 24 end",
		df_string = "function (x) return 4*x^3 + 36*x^2 + 94*x + 60 end",
		x0 = { 0, 2 }
	}
}

function solver.run_test(test)
	--[[
	given a table with a function, run the solver on it.
	]]

	print("Test")
	print("f: " .. test.f_string)
	print("df: " .. test.df_string)

	local f = (loadstring("return " .. test.f_string))()
	local df = (loadstring("return " .. test.df_string))()

	local i
	for i = 1, table.getn(test.x0) do
		local x0 = test.x0[i]
		print("Solving, starting at " .. x0)

		-- do it
		solver.global_solve(f, df, x0)

		print("")	-- blank line
	end
end


table.foreachi(tests,
	function (_, test)
		solver.run_test(test)
		return nil
	end
)
