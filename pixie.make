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

INCLUDES +=
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
TARGET = $(TARGETDIR)/pixie
OBJDIR = obj/macos64/Debug
DEFINES += -DDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -g -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -g -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -m64

else ifeq ($(config),debug_linux64)
RESCOMP = windres
TARGETDIR = bin/Debug
TARGET = $(TARGETDIR)/pixie
OBJDIR = obj/linux64/Debug
DEFINES += -DDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -g -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -g -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -static

else ifeq ($(config),debug_windows64)
RESCOMP = windres
TARGETDIR = bin/Debug
TARGET = $(TARGETDIR)/pixie.exe
OBJDIR = obj/windows64/Debug
DEFINES += -DDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -g -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -g -std=c++20 -Wall -Wextra -march=native
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
TARGET = $(TARGETDIR)/pixie
OBJDIR = obj/macos64/Test
DEFINES += -DNDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -m64 -flto

else ifeq ($(config),test_linux64)
RESCOMP = windres
TARGETDIR = bin/Test
TARGET = $(TARGETDIR)/pixie
OBJDIR = obj/linux64/Test
DEFINES += -DNDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -s -static

else ifeq ($(config),test_windows64)
RESCOMP = windres
TARGETDIR = bin/Test
TARGET = $(TARGETDIR)/pixie.exe
OBJDIR = obj/windows64/Test
DEFINES += -DNDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -std=c++20 -Wall -Wextra -march=native
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
TARGET = $(TARGETDIR)/pixie
OBJDIR = obj/macos64/Release
DEFINES += -DNDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -m64 -flto

else ifeq ($(config),release_linux64)
RESCOMP = windres
TARGETDIR = bin/Release
TARGET = $(TARGETDIR)/pixie
OBJDIR = obj/linux64/Release
DEFINES += -DNDEBUG
ALL_CFLAGS += $(CFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -Wall -Wextra -march=native
ALL_CXXFLAGS += $(CXXFLAGS) $(ALL_CPPFLAGS) -m64 -Werror -flto -O3 -std=c++20 -Wall -Wextra -march=native
ALL_LDFLAGS += $(LDFLAGS) -L/usr/lib64 -m64 -flto -s -static

else ifeq ($(config),release_windows64)
RESCOMP = windres
TARGETDIR = bin/Release
TARGET = $(TARGETDIR)/pixie.exe
OBJDIR = obj/windows64/Release
DEFINES += -DNDEBUG
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
GENERATED += $(OBJDIR)/eval.o
GENERATED += $(OBJDIR)/fen.o
GENERATED += $(OBJDIR)/move.o
GENERATED += $(OBJDIR)/perft.o
GENERATED += $(OBJDIR)/position.o
GENERATED += $(OBJDIR)/search.o
GENERATED += $(OBJDIR)/threadpool.o
GENERATED += $(OBJDIR)/tt.o
GENERATED += $(OBJDIR)/utils.o
OBJECTS += $(OBJDIR)/board.o
OBJECTS += $(OBJDIR)/eval.o
OBJECTS += $(OBJDIR)/fen.o
OBJECTS += $(OBJDIR)/move.o
OBJECTS += $(OBJDIR)/perft.o
OBJECTS += $(OBJDIR)/position.o
OBJECTS += $(OBJDIR)/search.o
OBJECTS += $(OBJDIR)/threadpool.o
OBJECTS += $(OBJDIR)/tt.o
OBJECTS += $(OBJDIR)/utils.o

ifeq ($(config),debug_macos64)
GENERATED += $(OBJDIR)/bench.o
GENERATED += $(OBJDIR)/main.o
GENERATED += $(OBJDIR)/uci.o
OBJECTS += $(OBJDIR)/bench.o
OBJECTS += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/uci.o

else ifeq ($(config),debug_linux64)
GENERATED += $(OBJDIR)/bench.o
GENERATED += $(OBJDIR)/main.o
GENERATED += $(OBJDIR)/uci.o
OBJECTS += $(OBJDIR)/bench.o
OBJECTS += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/uci.o

else ifeq ($(config),debug_windows64)
GENERATED += $(OBJDIR)/bench.o
GENERATED += $(OBJDIR)/main.o
GENERATED += $(OBJDIR)/uci.o
OBJECTS += $(OBJDIR)/bench.o
OBJECTS += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/uci.o

else ifeq ($(config),test_macos64)
GENERATED += $(OBJDIR)/test_main.o
GENERATED += $(OBJDIR)/test_perft.o
GENERATED += $(OBJDIR)/test_position.o
GENERATED += $(OBJDIR)/test_tt.o
GENERATED += $(OBJDIR)/test_utils.o
OBJECTS += $(OBJDIR)/test_main.o
OBJECTS += $(OBJDIR)/test_perft.o
OBJECTS += $(OBJDIR)/test_position.o
OBJECTS += $(OBJDIR)/test_tt.o
OBJECTS += $(OBJDIR)/test_utils.o

else ifeq ($(config),test_linux64)
GENERATED += $(OBJDIR)/test_main.o
GENERATED += $(OBJDIR)/test_perft.o
GENERATED += $(OBJDIR)/test_position.o
GENERATED += $(OBJDIR)/test_tt.o
GENERATED += $(OBJDIR)/test_utils.o
OBJECTS += $(OBJDIR)/test_main.o
OBJECTS += $(OBJDIR)/test_perft.o
OBJECTS += $(OBJDIR)/test_position.o
OBJECTS += $(OBJDIR)/test_tt.o
OBJECTS += $(OBJDIR)/test_utils.o

else ifeq ($(config),test_windows64)
GENERATED += $(OBJDIR)/test_main.o
GENERATED += $(OBJDIR)/test_perft.o
GENERATED += $(OBJDIR)/test_position.o
GENERATED += $(OBJDIR)/test_tt.o
GENERATED += $(OBJDIR)/test_utils.o
OBJECTS += $(OBJDIR)/test_main.o
OBJECTS += $(OBJDIR)/test_perft.o
OBJECTS += $(OBJDIR)/test_position.o
OBJECTS += $(OBJDIR)/test_tt.o
OBJECTS += $(OBJDIR)/test_utils.o

else ifeq ($(config),release_macos64)
GENERATED += $(OBJDIR)/bench.o
GENERATED += $(OBJDIR)/main.o
GENERATED += $(OBJDIR)/uci.o
OBJECTS += $(OBJDIR)/bench.o
OBJECTS += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/uci.o

else ifeq ($(config),release_linux64)
GENERATED += $(OBJDIR)/bench.o
GENERATED += $(OBJDIR)/main.o
GENERATED += $(OBJDIR)/uci.o
OBJECTS += $(OBJDIR)/bench.o
OBJECTS += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/uci.o

else ifeq ($(config),release_windows64)
GENERATED += $(OBJDIR)/bench.o
GENERATED += $(OBJDIR)/main.o
GENERATED += $(OBJDIR)/uci.o
OBJECTS += $(OBJDIR)/bench.o
OBJECTS += $(OBJDIR)/main.o
OBJECTS += $(OBJDIR)/uci.o

endif

# Rules
# #############################################

all: $(TARGET)
	@:

$(TARGET): $(GENERATED) $(OBJECTS) $(LDDEPS) | $(TARGETDIR)
	$(PRELINKCMDS)
	@echo Linking pixie
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
	@echo Cleaning pixie
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
$(OBJDIR)/eval.o: src/eval.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/fen.o: src/fen.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/move.o: src/move.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/perft.o: src/perft.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/position.o: src/position.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/search.o: src/search.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/threadpool.o: src/threadpool.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/tt.o: src/tt.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/utils.o: src/utils.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

ifeq ($(config),debug_macos64)
$(OBJDIR)/bench.o: src/bench.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/uci.o: src/uci.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),debug_linux64)
$(OBJDIR)/bench.o: src/bench.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/uci.o: src/uci.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),debug_windows64)
$(OBJDIR)/bench.o: src/bench.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/uci.o: src/uci.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),test_macos64)
$(OBJDIR)/test_main.o: test/test_main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_perft.o: test/test_perft.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_position.o: test/test_position.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_tt.o: test/test_tt.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_utils.o: test/test_utils.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),test_linux64)
$(OBJDIR)/test_main.o: test/test_main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_perft.o: test/test_perft.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_position.o: test/test_position.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_tt.o: test/test_tt.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_utils.o: test/test_utils.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),test_windows64)
$(OBJDIR)/test_main.o: test/test_main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_perft.o: test/test_perft.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_position.o: test/test_position.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_tt.o: test/test_tt.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/test_utils.o: test/test_utils.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),release_macos64)
$(OBJDIR)/bench.o: src/bench.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/uci.o: src/uci.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),release_linux64)
$(OBJDIR)/bench.o: src/bench.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/uci.o: src/uci.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

else ifeq ($(config),release_windows64)
$(OBJDIR)/bench.o: src/bench.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/main.o: src/main.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"
$(OBJDIR)/uci.o: src/uci.cpp
	@echo "$(notdir $<)"
	$(SILENT) $(CXX) $(ALL_CXXFLAGS) $(FORCE_INCLUDE) -o "$@" -MF "$(@:%.o=%.d)" -c "$<"

endif

-include $(OBJECTS:%.o=%.d)
ifneq (,$(PCH))
  -include $(PCH_PLACEHOLDER).d
endif