CC := gcc

BUILD ?= release

SRCDIR := src
OBJDIR := build/$(BUILD)
TARGET := ttt

SOURCES := \
	$(SRCDIR)/main.c \
	$(SRCDIR)/TicTacToe/tic_tac_toe.c \
	$(SRCDIR)/MiniMax/mini_max.c

OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

WARNINGS := -Wall -Wextra
BASE_CFLAGS := -std=c11 -MMD -MP -pipe

DEBUG_CFLAGS := -O0 -g
RELEASE_CFLAGS := -O3 -march=native -flto -fomit-frame-pointer -DNDEBUG -fno-plt

ifeq ($(BUILD),debug)
MODE_CFLAGS := $(DEBUG_CFLAGS)
else
MODE_CFLAGS := $(RELEASE_CFLAGS)
endif

CFLAGS := $(WARNINGS) $(BASE_CFLAGS) $(MODE_CFLAGS)
LDFLAGS := -flto

.PHONY: all clean run rebuild debug release

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "[LINK ] $@"
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo "[CC   ] $<"
	@$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

rebuild: clean all

debug:
	@$(MAKE) BUILD=debug all

release:
	@$(MAKE) BUILD=release all

clean:
	@echo "[CLEAN] removing build artifacts"
	@rm -rf build $(TARGET)

-include $(DEPS)
