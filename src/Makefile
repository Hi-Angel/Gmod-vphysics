MODULE_NAME = vphysics_srv.so
PROJECT_NAME = vphysics

# Configuration (can only be "debug" or "release")
CONFIGURATION = release

# Path Configuration
# Doesn't matter whether it's mp or sp.
SOURCE_SDK = ../thirdparty/sourcesdk/mp/src
BULLET_SDK = ../bullet

PROJECT_DIR = .
OUT_DIR = ../build/bin/linux/$(CONFIGURATION)
OBJ_DIR = ../build/obj/linux/$(PROJECT_NAME)/$(CONFIGURATION)

# Compilation Configuration
INCLUDES = \
	-I../include 			\
	-I$(SOURCE_SDK)/public 		\
	-I$(SOURCE_SDK)/public/tier0 	\
	-I$(SOURCE_SDK)/public/tier1 	\
	-I$(BULLET_SDK)/src
	
STATICLIBDIRS = \
	-L../build/lib/linux/$(CONFIGURATION)

DYNAMICLIBS = \
	$(SOURCE_SDK)/lib/public/linux32/tier1.a 	\
	$(SOURCE_SDK)/lib/public/linux32/tier2.a 	\
	$(SOURCE_SDK)/lib/public/linux32/tier3.a 	\
	$(SOURCE_SDK)/lib/public/linux32/mathlib.a  \
	-lBulletSoftBody 	\
	-lBulletDynamics 	\
	-lBulletCollision	\
	-lLinearMath

CC = /usr/bin/g++
LD = /usr/bin/g++
ARCH = i386
DEFINES = -DLINUX -D__LINUX__ -D_LINUX -D__linux__ -DPOSIX -DGNUC -DARCH=$(ARCH) -Dsprintf_s=snprintf -Dstrcmpi=strcasecmp -D_alloca=alloca -Dstricmp=strcasecmp -D_stricmp=strcasecmp -Dstrcpy_s=strncpy -D_strnicmp=strncasecmp -Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf -D_alloca=alloca -Dstrcmpi=strcasecmp -DNO_MALLOC_OVERRIDE
CFLAGS += $(INCLUDES) $(DEFINES) -fpermissive -fPIC -w -msse2 -m32 -march=$(ARCH) -g
LDFLAGS += -m32 -msse2 -lm -ldl $(STATICLIBDIRS) $(DYNAMICLIBS) -shared

ifeq ($(CONFIGURATION), debug)
	# Optimize but don't affect debugging experience. (-Og, only on G++ 4.8 and above)
	#CFLAGS += -Og
	CFLAGS += -D_DEBUG -O0
endif

ifeq ($(CONFIGURATION), release)
	CFLAGS += -O2 -DNDEBUG
endif

SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))

# Commands
RM = rm -f
CP = cp -f
MKDIR = mkdir -p
MAKE = make

build: dirs $(MODULE_NAME)

dirs:
	@-$(MKDIR) $(OUT_DIR)
	@-$(MKDIR) $(OBJ_DIR)

$(MODULE_NAME): $(OBJECTS)
	@echo " + Linking..."
	@$(LD) -o $(OUT_DIR)/$@ $(OBJECTS) $(LDFLAGS)
	@echo " + Module created at $(OUT_DIR)/$@"

$(OBJ_DIR)/%.o: %.cpp
	@echo " + Compiling '$<'"
	@$(CC) $(CFLAGS) -o $@ -c $<
	
clean:
	@$(RM) $(OBJECTS)
	@$(RM) $(OUT_DIR)/$(MODULE_NAME)
	@echo " + Clean!"
  
