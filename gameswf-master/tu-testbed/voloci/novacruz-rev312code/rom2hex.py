# tulrich 2011-07-26

# Hack to convert Main.rom to a hex format that can be used by avrdude.

import sys

for line in sys.stdin:
    line = line.strip()
    bytes = line[line.find(":") + 1 : ]
    if len(bytes) != 4:
        raise "unexpected format"
    #sys.stdout.write("0x%s,0x%s," % (bytes[2:4], bytes[0:2]))
    sys.stdout.write(chr(int(bytes[2:4], 16)))
    sys.stdout.write(chr(int(bytes[0:2], 16)))
