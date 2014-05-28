NAME= BShisen
TYPE= APP
SRCS= BitmapControl.cpp Prefs.cpp Tiles.cpp ID_Win.cpp main.cpp board.cpp GameWindow.cpp Language.cpp Hall.cpp Shisen.cpp
RSRCS= Resources.rsrc
LIBS= be root media translation game
LIBPATHS=
SYSTEM_INCLUDE_PATHS=
LOCAL_INCLUDE_PATHS=
OPTIMIZE=SOME
#	specify any preprocessor symbols to be defined.  The symbols will not
#	have their values set automatically; you must supply the value (if any)
#	to use.  For example, setting DEFINES to "DEBUG=1" will cause the
#	compiler option "-DDEBUG=1" to be used.  Setting DEFINES to "DEBUG"
#	would pass "-DDEBUG" on the compiler's command line.
DEFINES=
#	specify special warning levels
#	if unspecified default warnings will be used
#	NONE = supress all warnings
#	ALL = enable all warnings
WARNINGS =
# Build with debugging symbols if set to TRUE
SYMBOLS=
COMPILER_FLAGS=-Woverloaded-virtual -funsigned-bitfields -Wwrite-strings 
LINKER_FLAGS=

## include the makefile-engine
DEVEL_DIRECTORY := \
	$(shell findpaths -r "makefile_engine" B_FIND_PATH_DEVELOP_DIRECTORY)
include $(DEVEL_DIRECTORY)/etc/makefile-engine
