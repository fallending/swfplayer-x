dumbuild
========

dumbuild is a build tool for building C++ programs, written in
C++.  The goals include:

* Small, self-contained and trivial to bootstrap.  The code is Public
  Domain.  Easy to embed in larger open-source projects -- no external
  dependencies, no license restrictions.

* Simple and sane config language (based on JSON syntax).  Emphasize a
  literal, declarative style.

* Written in garden-variety C++ -- hackable and extendable by C++
  programmers.

* Support Windows, Linux and Mac OSX out of the box.

* Reliable and fast.  May be usable as your everyday build tool.

Getting dumbuild
================

The code is here:
http://tu-testbed.svn.sourceforge.net/viewvc/tu-testbed/trunk/tu-testbed/dumbuild/

You can grab it out of SVN or just poke around in there if you're
curious.

When it's more mature I'll provide .zip and .tar.gz archives.

A prebuilt statically-linked windows executable is here:
http://tulrich.com/geekstuff/dumbuild/dmb.exe

Building dumbuild
=================

dumbuild bootstraps itself from its wrapper script, the first time you
run it.  On Windows:

> dmb.bat

On Linux etc:

# ./dmb

If all goes well, you should get dmb.exe (Windows) or dmb (Linux etc)
in the subdirectory dmb-out/bootstrap .

Usage
=====

On Windows:

> dmb.bat [options] [target-name]

On Linux etc:

# ./dmb [options] [target-name]

 ----
The target is found by looking for a build.dmb file in the specified
target's path, and then looking for the named target within that
build.dmb.  For example:

  dmb libs/base/math

will try to parse the file \"libs/base/build.dmb\" for a target
named \"math\", and build it according to the target's
specification.  If not specified, the default target is \"default\".

options include:

  -h           Print usage.

  -C <dir>     Change to the specified directory before starting work.
               This should have the effect of invoking dmb from that
               directory.

  -c <config>  Specify the name of a build configuration (i.e. compiler 
               & mode).  Supplied configurations in the default root.dmb
               include
                  gcc-debug
                  gcc-release
                  vc8-debug
                  vc8-release
	                 vc9-debug
	                 vc9-release
               If not specified, the default configuration is 
               \"default\", which tries to do some auto-detection for
               an appropriate build configuration.

  -r           Rebuild all, whether or not source files have changed.

  -v           Verbose.  Does a lot of extra logging.

  --test       Run internal unit tests.

The project root directory is located by searching upward from the
current directory for a file named \"root.dmb\".  root.dmb may
contain project-wide defaults.  Target paths may be specified in
relation to the project root.

The build output goes in <project-root>/dmb-out/<config-name>
 ----

Without any options, "dmb" builds the target "default" using the
default configuration "default".

Example:

> dmb -c vc8-release

Scans build.dmb for the default target, and builds it using the
vc8-release configuration.

Files:

root.dmb -- put this in the root directory of your project.  All the
  pathnames in dumbuild config are relative to this root.  This is
  also where you probably want to put the compiler configuration
  (command-line templates and such).
  [[http://tu-testbed.svn.sourceforge.net/viewvc/tu-testbed/trunk/tu-testbed/dumbuild/root.dmb?view=markup][Example]]

build.dmb -- these files specify your actual "targets" (i.e. libs and
  executables).  Put these throughout your project tree, wherever you
  need them.  The recommended pattern is to put one in each source
  directory.  You can have more than one target in a single directory;
  this is normal if you want some additional structure inside the
  directory.  A target can actually pull sources from outside the
  directory its build.dmb file is in, though this is more intended for
  special circumstances, like building sources from a foreign code
  tree.
  [[http://tu-testbed.svn.sourceforge.net/viewvc/tu-testbed/trunk/tu-testbed/dumbuild/build.dmb?view=markup][Example]]

Build Language
==============

The build language syntax is JSON ("Javascript Object Notation").

TODO explain semantics

Alternatives
============

There are many build tools in the world -- why another one?  Partly
for fun, but mostly to scratch an itch: there is no ideal single build
tool for a small or medium-sized open source project that wants to
support both Windows and non-Windows platforms.  Here's a rundown of
my take on the top contenders:

[[http://redmine.jamplex.org/projects/show/jamplus][JamPlus]] is
closest to the ideal.  The minor knocks on it are that it's not
trivial to bootstrap, and in my opinion the config language is hard to
use.  People use it for big commercial projects and it has a
reputation for excellent speed.

[[http://www.scons.org/][Scons]] is full of features and has a
reasonable config language (based on Python).  But, it's written in
Python, it's not tiny, and unfortunately it can be egregiously slow on
big projects.  I don't think it's a great choice for embedding in a
small or medium-sized open-source project, mainly because it would be
bigger than the typical host project, and it has a dependency on
Python.

GNU make is what I have used for a long time for my own open-source
stuff.  It's fast, fairly ubiquitous, and I'm used to it.  I don't
mind it too much, but it's not the most elegant thing, the header
dependency checking is terrible, and it seems to perplex most Windows
programmers.

Project file generators like cmake and gyp have increased in
popularity, and have some adherents.  Personally I have a deep-seated
dislike for anything that relies on Visual Studio as a build tool.
Same with Xcode on Mac.  I prefer to use the same non-GUI build tool
across multiple platforms.


Notes
=====

* I call it "dumbuild" because I want the implementation to be as
  straightforward as possible, and I want it to be simple to use.
  It's also a reminder that it doesn't have to be perfect and it
  doesn't need every possible build tool feature.

* dumbuild is written in vanilla C++.  There are a couple small .bat
  and bash scripts for bootstrapping, but the build engine is C++ with
  no external dependencies other than STL and a few C library calls.

  I'm using the Google C++ Style Guide:
  http://google-styleguide.googlecode.com/svn/trunk/cppguide.xml

* The config format tries to cover the necessities, but favors
  simplicity over power or generality.

* I want dumbuild to be usable as an everyday build tool.  That means
  it must be fast, dependency checking must be reliable, and the build
  tools should be invoked hermetically; i.e. dumbuild does not let
  your environment variables leak into the build.

* In service of fast/reliable builds, it uses SHA1 content hashes.
  When something changes, dumbuild quickly and accurately determines
  what files need to be recompiled and relinked.

* dumbuild is Lib and Exe oriented.  It doesn't dignify individual
  .obj files as distinct targets, which helps keep the configured
  dependency graph within reason, and I think is a better match for
  the way programmers think about their projects.  The two main target
  types are "lib" (library) and "exe" (executable), which can have any
  number of source files, and can depend on any number of other
  targets.

* I'm using jsoncpp to parse JSON.  It's working OK.  It's Public
  Domain, which I require, and it's reasonable C++.  I tweaked it
  slightly to remove exceptions and to allow a trailing comma after
  the last element in an object or array.  TODO: the source code is
  bigger than necessary; I have to fight the urge not to delete the
  optional custom allocators and containers and whatnot.

* TODO: need a way to handle generated code.  My plan is to put a very
  simple template-based generator in dumbuild for basic needs.  For
  more complex things I will probably add a "command" target that runs
  an external program/script and declares its inputs and outputs.

* TODO: need to parallelize building of independent targets.

* TODO: need a way to implement GNU-style "configure" functionality.
  The key motivation here is to make it usable on Windows.

* TODO: '#:' syntax for inhibiting dep_libs path prepend is hacky

* TODO: gcc doesn't echo the source files as it compiles them, but
  msvc does.  Need to echo ourselves, or figure out the option that
  makes gcc echo.

* TODO: in a big list of src's, if there's a compile failure in the
  middle, none of the src hashes are updated, so the whole set of
  files has to be recompiled.  Would be nice to figure out which
  .obj's can get their hashes updated, so they don't have to be
  recompiled after the coder fixes the problem with the failed file.

  Actually, this is important for correctness too since if a source file
  has a new obj, but doesn't get an updated hash, and then the coder
  reverts the source to its previous state, the dep will match the old
  source but the real .obj file is new!  So dumbuild will think it
  doesn't need to rebuild the .obj

  (Could trivially fix this by only compiling one src file per
  invocation of the compiler, but that could be a performance
  regression.)

