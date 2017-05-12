-- launcher for presentation demos, requires only lua.exe

-- add -f to the string below to get fullscreen
options = " -w 1024 -h 768 -2"


function file_is_readable(file)
-- function to test whether the given file can be read from.
	local handle = openfile(file, "rb")
	if handle == nil then
		-- failure
		return nil
	else
		-- success
		closefile(handle)
		return 1
	end
end


function demo(chunkfile, texturefile)
-- launch the chunkdemo program, using the given files as input.
	if not file_is_readable(chunkfile) then
		write("can't open " .. chunkfile .. " -- did you download the\n");
		write("data?  See http://tulrich.com/geekstuff/chunklod.html\n");
	elseif not file_is_readable(texturefile) then
		write("can't open " .. texturefile .. " -- did you download the\n");
		write("data?  See http://tulrich.com/geekstuff/chunklod.html\n");
	else
		-- files are available.
		local cmd = "chunkdemo.exe " .. chunkfile .. " " .. texturefile .. options;
		write("running " .. cmd .. "...\n");
		execute(cmd);
	end
end


-- main script; show a menu, run command, repeat
while 1 do
	write("\n")
	write("1. crater\n")
	write("2. maui\n")
	write("3. riverblue\n")
	write("4. puget\n")
	write("5. hawaii\n")
	write("6. kauai\n")
	write("Q. quit\n")
	write("\n");

	local choice = read("*l")
	write("choice = " .. choice .. "\n")

	if (choice == "q" or choice == "Q") then
		exit(0)
	elseif (choice == "1" or choice == "") then
		demo("crater/crater.chu", "crater/crater.jpg")
	elseif (choice == "2") then
		demo("maui_2k.chu", "maui_4k_d5.tqt")
	elseif (choice == "3") then
		demo("riverblue_e1_d8.chu", "riverblue_16k_d7.tqt")
	elseif (choice == "4") then
		demo("puget_e4_d9.chu", "puget_32k_d9.tqt")
	elseif (choice == "5") then
		demo("island_e1_d9.chu", "island_16k_d7.tqt")
	elseif (choice == "6") then
		demo("kauai_e1_d9.chu", "kauai_8k_d6.tqt")
	end
end
