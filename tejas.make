# Alternative GNU Make project makefile autogenerated by Premake

ifndef config
  config=debug_macos64
endif

ifndef verbose
  SILENT = @
endif

.PHONY: clean prebuild

SHELLTYPE := posix
ifeq (.exe,$(findstring .exe,$(ComSpec)))
	SHELLTYPE := msdos
endif

# Configurations
# #############################################

FORCE_INCLUDE +=
ALL_CPPFLAGS += $(CPPFLAGS) -MD -MP $(DEFINES) $(INCLUDES)
ALL_RESFLAGS += $(RESFLAGS) $(DEFINES) $(INCLUDES)
LIBS +=
LDDEPS +=
LINKCMD = $(CXX) -o "$@" $(OBJECTS) $(RESOURCES) $(ALL_LDFLAGS) $(LIBS)
define PREBUILDCMDS
endef
define PRELINKCMDS
endef
define POSTBUILDCMDS
endef

ifeq ($(config),debug_macos64)
ifeq ($(origin CC), default)
  CC = clang
endif
ifeq ($(origin CXX), default)
  CXX = clang++
endif
ifeq ($(origin AR), default)
  AR = ar
endif
TARGETDIR = bin/Debug
TARGET = $(TARGETDIR)/tejas
OBJDIR = obj/macos64/Debug
DEFINES += -DDEBUG
INCLUDES +=
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -g -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -g -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -m64

else ifeq ($(config),debug_linux64)
RESCOMP = windres
TARGETDIR = bin/Debug
TARGET = $(TARGETDIR)/tejas
OBJDIR = obj/linux64/Debug
DEFINES += -DDEBUG
INCLUDES +=
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -g -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -g -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -static

else ifeq ($(config),debug_windows64)
RESCOMP = windres
TARGETDIR = bin/Debug
TARGET = $(TARGETDIR)/tejas.exe
OBJDIR = obj/windows64/Debug
DEFINES += -DDEBUG
INCLUDES +=
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -g -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -g -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -static

else ifeq ($(config),test_macos64)
ifeq ($(origin CC), default)
  CC = clang
endif
ifeq ($(origin CXX), default)
  CXX = clang++
endif
ifeq ($(origin AR), default)
  AR = llvm-ar
endif
TARGETDIR = bin/Test
TARGET = $(TARGETDIR)/tejas
OBJDIR = obj/macos64/Test
DEFINES += -DDEBUG
INCLUDES += -Isrc -Itest/lib/doctest
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -m64 -flto

else ifeq ($(config),test_linux64)
RESCOMP = windres
TARGETDIR = bin/Test
TARGET = $(TARGETDIR)/tejas
OBJDIR = obj/linux64/Test
DEFINES += -DDEBUG
INCLUDES += -Isrc -Itest/lib/doctest
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -s -static

else ifeq ($(config),test_windows64)
RESCOMP = windres
TARGETDIR = bin/Test
TARGET = $(TARGETDIR)/tejas.exe
OBJDIR = obj/windows64/Test
DEFINES += -DDEBUG
INCLUDES += -Isrc -Itest/lib/doctest
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -s -static

else ifeq ($(config),release_macos64)
ifeq ($(origin CC), default)
  CC = clang
endif
ifeq ($(origin CXX), default)
  CXX = clang++
endif
ifeq ($(origin AR), default)
  AR = llvm-ar
endif
TARGETDIR = bin/Release
TARGET = $(TARGETDIR)/tejas
OBJDIR = obj/macos64/Release
DEFINES += -DNDEBUG
INCLUDES +=
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -m64 -flto

else ifeq ($(config),release_linux64)
RESCOMP = windres
TARGETDIR = bin/Release
TARGET = $(TARGETDIR)/tejas
OBJDIR = obj/linux64/Release
DEFINES += -DNDEBUG
INCLUDES +=
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -s -static

else ifeq ($(config),release_windows64)
RESCOMP = windres
TARGETDIR = bin/Release
TARGET = $(TARGETDIR)/tejas.exe
OBJDIR = obj/windows64/Release
DEFINES += -DNDEBUG
INCLUDES +=
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -s -static

endif

# Per File Configurations
# #############################################


# File sets
# #############################################

GENERATED :=
OBJECTS :=

GENERATED += $(OBJDIR)/board.o
GENERATED += $(OBJDIR)/engine.o
GENERATED += $(OBJDIR)/move.o
GENERATED += $(OBJDIR)/movegen.o
GENERATED += $(OBJDIR)/utils.o
OBJECTS += $(OBJDIR)/board.o
OBJECTS += $(OBJDIR)/engine.o
OBJECTS += $(OBJDIR)/move.o
OBJECTS += $(OBJDIR)/movegen.o
OBJECTS += $(OBJDIR)/utils.o

ifeq ($(config),debug_macos64)
GENERATED += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/main.o

else ifeq ($(config),debug_linux64)
GENERATED += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/main.o

else ifeq ($(config),debug_windows64)
GENERATED += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/main.o

else ifeq ($(config),test_macos64)
GENERATED += $(OBJDIR)/test_board.o
GENERATED += $(OBJDIR)/test_main.o
GENERATED += $(OBJDIR)/test_move.o
GENERATED += $(OBJDIR)/test_movegen.o
OBJECTS += $(OBJDIR)/test_board.o
OBJECTS += $(OBJDIR)/test_main.o
OBJECTS += $(OBJDIR)/test_move.o
OBJECTS += $(OBJDIR)/test_movegen.o

else ifeq ($(config),test_linux64)
GENERATED += $(OBJDIR)/test_board.o
GENERATED += $(OBJDIR)/test_main.o
GENERATED += $(OBJDIR)/test_move.o
GENERATED += $(OBJDIR)/test_movegen.o
OBJECTS += $(OBJDIR)/test_board.o
OBJECTS += $(OBJDIR)/test_main.o
OBJECTS += $(OBJDIR)/test_move.o
OBJECTS += $(OBJDIR)/test_movegen.o

else ifeq ($(config),test_windows64)
GENERATED += $(OBJDIR)/test_board.o
GENERATED += $(OBJDIR)/test_main.o
GENERATED += $(OBJDIR)/test_move.o
GENERATED += $(OBJDIR)/test_movegen.o
OBJECTS += $(OBJDIR)/test_board.o
OBJECTS += $(OBJDIR)/test_main.o
OBJECTS += $(OBJDIR)/test_move.o
OBJECTS += $(OBJDIR)/test_movegen.o

else ifeq ($(config),release_macos64)
GENERATED += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/main.o

else ifeq ($(config),release_linux64)
GENERATED += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/main.o

else ifeq ($(config),release_windows64)
GENERATED += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/main.o

endif

# Rules
# #############################################

all: $(TARGET)
	@:

$(TARGET): $(GENERATED) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	$(PRELINKCMDS)
	@echo Linking tejas
	$(SILENT) $(LINKCMD)
	$(POSTBUILDCMDS)

$(TARGETDIR):
	@echo Creating $(TARGETDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(TARGETDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(TARGETDIR))
endif

$(OBJDIR):
	@echo Creating $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) mkdir -p $(OBJDIR)
else
	$(SILENT) mkdir $(subst /,\\,$(OBJDIR))
endif

clean:
	@echo Cleaning tejas
ifeq (posix,$(SHELLTYPE))
	$(SILENT) rm -f  $(TARGET)
	$(SILENT) rm -rf $(GENERATED)
	$(SILENT) rm -rf $(OBJDIR)
else
	$(SILENT) if exist $(subst /,\\,$(TARGET)) del $(subst /,\\,$(TARGET))
	$(SILENT) if exist $(subst /,\\,$(GENERATED)) del /s /q $(subst /,\\,$(GENERATED))
	$(SILENT) if exist $(subst /,\\,$(OBJDIR)) rmdir /s /q $(subst /,\\,$(OBJDIR))
endif

prebuild: | $(OBJDIR)
	$(PREBUILDCMDS)

ifneq (,$(PCH))
$(OBJECTS): $(GCH) | $(PCH_PLACEHOLDER)
$(GCH): $(PCH) | prebuild
	@echo $(notdir $<)
	$(SILENT) $(CXX) -x c++-header $(ALL_CXXFLAGS) -o "$@" -MF "$(@:%.gch=%.d)" -c "$<"
$(PCH_PLACEHOLDER): $(GCH) | $(OBJDIR)
ifeq (posix,$(SHELLTYPE))
	$(SILENT) touch "$@"
else
	$(SILENT) echo $null >> "$@"
endif
else
$(OBJECTS): | prebuild
endif


# File Rules
# #############################################

$(OBJDIR)/board.o: src/board.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/engine.o: src/engine.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/move.o: src/move.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/movegen.o: src/movegen.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/utils.o: src/utils.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

ifeq ($(config),debug_macos64)
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),debug_linux64)
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),debug_windows64)
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),test_macos64)
$(OBJDIR)/test_board.o: test/test_board.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_main.o: test/test_main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_move.o: test/test_move.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_movegen.o: test/test_movegen.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),test_linux64)
$(OBJDIR)/test_board.o: test/test_board.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_main.o: test/test_main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_move.o: test/test_move.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_movegen.o: test/test_movegen.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),test_windows64)
$(OBJDIR)/test_board.o: test/test_board.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_main.o: test/test_main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_move.o: test/test_move.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_movegen.o: test/test_movegen.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),release_macos64)
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),release_linux64)
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),release_windows64)
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

endif

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(PCH_PLACEHOLDER).d
endif