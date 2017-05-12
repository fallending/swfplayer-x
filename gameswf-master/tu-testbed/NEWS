2009-08-08  Vitaly Alexeev <tishka92@yahoo.com>

The gameswf-2009-08-08 release of gameSWF is coming.
Improvements since the gameswf-2008-06-17 release are:

* added video handler for OpenGL ES
* added sound handler for OpenAL
* Array class implementation is optimized
* reverted fontlib, so text is antialiased now
* added network support in loadMovie()
* added SQLite plugin
* added File plugin
* added libpthread support
* added 'Mouse' action script class implementation
* fixed a lot of bugs


2008-06-17  Vitaly Alexeev <tishka92@yahoo.com>

The gameswf-2008-06-17 release of gameSWF is coming.
Improvements since the gameswf-2008-01-05 release are:

* gameSWF playes Flash9 with AS2.0
* gameSWF supports user defined flash components
* fixed detected memory leaks
* implemented sendAndLoad of LoadVars (with sample)
* implemented Sound.loadSound(url)
* implemented FSAA (hardware full screen antialiasing)
* Implemented the antialiasing of polygon edges. Very simple but quite pleasant
* call_method_args() is reverted at the instance of users
* reduced the texture memory usage
* fixed nested mask bug for opengl renderer
* implemented instanceof operator
* implemented FlashVars support
* started AVM2 implementation
* added implementation of the following AS2.0 classes
- Selection
- flash.geom.Matrix
- flash.geom.Point
- ColorTransform and Transform.


2008-01-05  Vitaly Alexeev <tishka92@yahoo.com>

The gameswf-2008-01-05 release of gameSWF is coming.
Improvements since the gameswf-2007-07-03 release are:

* Created libgameswf(.dll for Windows, .so for Linux)
* Added the supporting of loadable user plugins
* Added the supporting of Action Script object-oriented programming
- implemented the Subclass/Superclass
- implemented 'extends' opcode(0x69) 
- implemented 'super' object
- stabilized property stuff
* Fixed the all known memory leaks
* Tesselator shows more details now
* Implemented garbage collector 

* Added	implemenation of	
- 'Date' Action Script Class (partly)
- attachMovie() method
- drawing API
- __resolve, hasOwnProperty(), watch(), unwatch(), C_FUNCTION getter/setter
- pause() method of sound_handler
- gameSWF extension that allows to pass the user bootup options to Flash
  (through _global._bootup)
- getVersion() global function,
- sysInfo plugin
- Object.dump() method for debugging ActionScript
- A garbage-collection framework, with templated smart pointer class.
  A simple mark/sweep garbage collector that plugs into the framework.
  Work-in-progress.  Once weak ptrs are added, this might be usable by gameswf.
- the remaining of String methods

* Removed the deprecated call_method(), call_method0(), call_method_args(),
  call_method_parsed(). Better to use the user defined gameswf plugin
  
* Fixed
- the sound bug (thanks to David Ruzicka). When the movie playback is paused,
  currently played sample continues to play even then
- broadcaster, first we should process completely one event then another
- text fields can listen _global, key object can notify _global
- rewritten mcLoader to provide the loading single frame movie
- _width & _height bug, width of empty movieclip  must be 0

* Rewritten video implementation
- transparent video background
- reduced memory usage (video & audio queues contains the packed data now), 
- applied the hardware resample of video frame,
- applied the memory leak safe data

* Done the work around 3DS plugin
- applied lib3DS-1.3
- added play() & stop() methods to 3D model
- added spot light

2007-07-03  Vitaly Alexeev <tishka92@yahoo.com>

The gameswf-2007-07-03 release of GameSWF is coming. Improvements since
the gameswf-2006-03-29 release are:

* Allow use of network to open resource URLs (using HTTP protocol)
* Loading of a file is accelerated
* Added dynamic TTF font support in text fileds
* Added the following Action script classes support:
- XMLSocket()
- TextFormat()
- MovieClipLoader()
- Color(): getTransform() & setTransform() methods are added
* Added the following Action script function:
- unloadMovie()
- getter() & setter() support
- addProperty()
* Added the mouse cursor that is changed when points to the active object
* Added 3DS plugin that allow to render & advance 3D MAX files
- snapshot of movieclip may be used as texture(material) for 3D model
* Added support of the following tags:
- DefineFont3
- DefineShape4
- DefineFileAttribute
- DefineFontAlignZones
* Work on maintenance of compatibility with Flash 8 is done
* Fixed a lot of bugs
* Added new bugs :)

2007-03-29  Vitaly Alexeev <tishka92@yahoo.com>

The gameswf-2007-03-25 release of GameSWF is coming. Improvements since
the gameswf-2006-02-27 release are:

* Added video & MP3 audio support using FFMPEG library.
* Loading and displaying of the swf-file is running in different threads
that allows to display the first frame immideately. 
* Added keyPress event handler
* Added multievent support, like 
on(release, press, keyPress "<Space>") {...}
* Added input text fields support
* Added stream sound support
* Added delete & delete property operator support
* Fixed or added the following Action script classes:
- Array()
- Color()
- NetConnection()
- NetStream()
* Added MYSQL support
* Added:
- swapDepths(), 
- getDepths(),
- duplicateMovieClip(),
- createEmptyMovieClip(),
- removeMovieClip(),
- HitTest()
methods of sprite instance
* Added 0x43 opcode(decl object)
* Rewritten sound_handler_SDL to provide support video, 
SDL_mixer library now is not used
* Added SetFocus & KilFocus events handler
* Fixed many bugs
* Added new bugs :)
