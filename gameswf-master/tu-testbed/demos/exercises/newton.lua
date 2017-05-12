-- some Newton's method experiments

function newton(f, fp, xc)
-- take a function, its derivative, and an initial guess, and do some Newton's method on it.
  for i = 1, 20 do
    print(i - 1, xc)
    local x_next = xc - f(xc) / fp(xc)
    if abs(f(xc)) < 1e-10 then return end
    xc = x_next;
  end
end


function newton_backtrack(f, fp, xc)
-- Newton's method, with backtracking.
  for i = 1, 20 do
    print(i - 1, xc)
    local x_next = xc - f(xc) / fp(xc)
    if abs(f(xc)) < 1e-10 then return end
    while abs(f(x_next)) > abs(f(xc)) do x_next = (x_next + xc) / 2 end    -- backtrack if we overshot.
    xc = x_next;
  end
end


function bisection(f, xk, zk)
  -- find root of f by recursive bisection.
  -- This is a demonstrably sucky algorithm.
  assert(f(xk) * f(zk) < 0)
  print(xk, zk)
  if (abs(f(xk)) < 1e-10) then return end
  local xk_next = (xk + zk) / 2
  local zk_next = xk
  if (f(xk_next) * f(zk) < 0) then zk_next = zk end
  return bisection(f, xk_next, zk_next)
end




function f(x)
--  return x*x - 1
  return atan(x)
end

--function fp(x)
--  return 2 * x - 2
--end

function fp(x)
  local x_minus = x - 1e-6
  local x_plus = x + 1e-6
  return (f(x_plus) - f(x_minus)) / 2e-6
end

xc = 1.41
newton(f, fp, xc)
print("\n")
newton_backtrack(f, fp, xc)


