// parse_bdm.h	-- Thatcher Ulrich <http://tulrich.com> 2007

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Parse a simple text format representing 2D polygon data.
//
// Input is in 'BDM' format.  BDM is a text file in ASCII format that defines
// any number of closed polygonal paths.  Each path consists of a line containing
// the vertex count, followed by that many lines containing the vertex coords.
// Blank lines are ignored.

#ifndef PARSE_BDM_H
#define PARSE_BDM_H

#include <stdio.h>
#include "base/container.h"

void parse_bdm(FILE* fp, array< array< float > >* paths);

#endif  // PARSE_BDM_H
