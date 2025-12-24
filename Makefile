# NeoC - NeoGeo Emulator
# Cross-platform Makefile for Mac, Linux, and Windows

PROJECT = neoc
VERSION = 0.1.0

# Detect OS
ifeq ($(OS),Windows_NT)
    PLATFORM = windows
    EXE = $(PROJECT).exe
    RM = del /Q
    MKDIR = mkdir
    PATHSEP = \\
else
    UNAME := $(shell uname -s)
    ifeq ($(UNAME),Darwin)
        PLATFORM = macos
    else
        PLATFORM = linux
    endif
    EXE = $(PROJECT)
    RM = rm -f
    MKDIR = mkdir -p
    PATHSEP = /
endif

# Directories
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -I$(INCDIR)
LDFLAGS =

# Platform-specific settings
ifeq ($(PLATFORM),macos)
    CFLAGS += -DPLATFORM_MACOS
    LDFLAGS += -framework Cocoa -framework OpenGL -framework AudioToolbox
endif

ifeq ($(PLATFORM),linux)
    CFLAGS += -DPLATFORM_LINUX
    LDFLAGS += -lGL -lX11 -lasound -lpthread -lm
endif

ifeq ($(PLATFORM),windows)
    CFLAGS += -DPLATFORM_WINDOWS
    LDFLAGS += -lopengl32 -lgdi32 -lwinmm -lws2_32
endif

# Build modes
ifdef DEBUG
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O3 -DNDEBUG -flto
    LDFLAGS += -flto
endif

# Source files
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Targets
.PHONY: all clean debug release dirs

all: dirs $(BINDIR)/$(EXE)

debug:
	$(MAKE) DEBUG=1

release:
	$(MAKE)

$(BINDIR)/$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Built $(EXE) for $(PLATFORM)"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

dirs:
	@$(MKDIR) $(OBJDIR) 2>/dev/null || true
	@$(MKDIR) $(BINDIR) 2>/dev/null || true

clean:
ifeq ($(PLATFORM),windows)
	-$(RM) $(OBJDIR)\*.o $(OBJDIR)\*.d $(BINDIR)\$(EXE) 2>nul
else
	-$(RM) $(OBJDIR)/*.o $(OBJDIR)/*.d $(BINDIR)/$(EXE)
endif

# Include dependencies
-include $(DEPS)

# Installation (Unix-like only)
ifneq ($(PLATFORM),windows)
PREFIX ?= /usr/local

install: $(BINDIR)/$(EXE)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(BINDIR)/$(EXE) $(DESTDIR)$(PREFIX)/bin/

uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(EXE)
endif
