-- quick test of q-linear convergence.


iter = 1
x = 0.9
while (1 + x > 1 + 1/(2^24)) do
	print(iter .. ", x = " .. x)
	x = x * 0.9
	iter = iter + 1
end

