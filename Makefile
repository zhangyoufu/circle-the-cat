# Note: Windows Command Prompt is not supported (mkdir, sdl-config, etc)

# Default Target
all :

# Compile Switch
DEBUG = FALSE
PROFILE = FALSE
STATIC = FALSE

# Project Specific
EXE = chatnoir
LIB = SDL_Image png z
CFLAGS += $(shell sdl-config --cflags)
#ifeq ($(DEBUG),TRUE)
LDFLAGS += -mconsole
#endif
ifeq ($(STATIC),TRUE)
LDFLAGS += -static $(shell sdl-config --static-libs)
else
LDFLAGS += $(shell sdl-config --libs)
endif
MODULE = $(patsubst src/%.c,%,$(wildcard src/*.c))
INCLUDE_PATH += 
LIBRARY_PATH += 
export SDL_STDIO_REDIRECT = 0

# Platform/Architecture Specific
ifeq ($(OS), Windows_NT)
EXE_EXTENSION = .exe
endif

EXE_FILE = $(EXE)$(EXE_EXTENSION)

# Compiler & Linker
CC = gcc
LD = gcc
CFLAGS += -c -Wall -Wextra
ifeq ($(DEBUG),TRUE)
CFLAGS += -ggdb3 -Wno-unused-parameter -Wno-unused-function
ifeq ($(PROFILE),TRUE)
CFLAGS += -pg
LDFLAGS += -pg
endif
else
CFLAGS += -Os
endif
CFLAGS += $(INCLUDE_PATH:%=-I%)
LDFLAGS += $(LIBRARY_PATH:%=-L%)

# Command Goal Check
ifneq ($(filter all clean,$(MAKECMDGOALS)),)
ifneq ($(words $(MAKECMDGOALS)),1)
$(error '$(MAKECMDGOALS)' target should be used standalone)
endif
endif

ifneq ($(filter-out all clean $(EXE) $(MODULE),$(MAKECMDGOALS)),)
$(error Unknown goal specified)
endif

# Build Files
OBJ = $(MODULE:%=obj/%.o)
DEP = $(MODULE:%=dep/%.d)

# Dependency
ifneq ($(filter all $(EXE),$(MAKECMDGOALS)),)
$(warning DEP_POLICY=ALL)
DEP_POLICY = ALL
endif

ifeq ($(MAKECMDGOALS),)
DEP_POLICY = ALL
endif

ifneq ($(DEP_POLICY),ALL)
ifneq ($(MAKECMDGOALS),clean)
DEP_POLICY = SPECIFIC
endif
endif

DEP_EXIST = $(filter $(DEP),$(wildcard dep/*.d))

ifeq ($(DEP_POLICY),ALL)
#$(warning Dependency: $(DEP_EXIST))
include $(DEP_EXIST)
endif

ifeq ($(DEP_POLICY),SPECIFIC)
# module goals specified
SPECIFIC_DEP = $(filter $(MAKECMDGOALS:%=dep/%.d),$(DEP_EXIST))
#$(warning Dependency: $(SPECIFIC_DEP))
export UPDATED_MODULE=$(SPECIFIC_DEP:dep/%.d=%)
include $(SPECIFIC_DEP)
endif

# Build Directory Goal
BUILD_DIR = obj dep
$(foreach dir,$(BUILD_DIR),$(eval $(dir) : ; @mkdir $$@))

# Special Goal
.PNONY : all clean $(EXE) $(MODULE)

all : $(EXE)

clean :
	@echo [CLEAN]
	@rm -rf $(EXE_FILE) $(BUILD_DIR)

# Top Level Goal
ifneq ($(EXE_EXTENSION),)
$(EXE) : $(EXE_FILE)
endif

$(EXE_FILE) : $(OBJ)
	@echo [LD] $(EXE)$(EXE_SUFFIX)
	@$(LD) -o $(EXE)$(EXE_SUFFIX) $(OBJ) $(LDFLAGS) $(LIB:%=-l%)

# Module Pseudo Goal
$(foreach module,$(MODULE),$(eval $(module) : obj/$(module).o dep/$(module).d))
ifdef MAKE_RESTARTS
$(foreach module,$(UPDATED_MODULE),$(eval $(module) : ; @true ))
endif

# Module Pattern Rule
obj/%.o : src/%.c | $(BUILD_DIR)
	@echo [obj rule] [GCC] $(@:obj/%.o=%)
	@$(CC) -c $(CFLAGS) -MP -MMD \
		-MF $(@:obj/%.o=dep/%.d) \
		-MT "$@ $(@:obj/%.o=dep/%.d)" \
		-o $@ \
		$<

dep/%.d : src/%.c | $(BUILD_DIR)
	@echo [dep rule] [CC] $(@:dep/%.d=%)
	@$(CC) -c $(CFLAGS) -MP -MMD \
		-MF $@ \
		-MT "$(@:dep/%.d=obj/%.o) $@" \
		-o $(@:dep/%.d=obj/%.o) \
		$<
