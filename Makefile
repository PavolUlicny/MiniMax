CC := gcc

BUILD ?= release
MODE ?= default

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

PROFILE_GENERATE_CFLAGS := -O3 -march=native -flto -fomit-frame-pointer -DNDEBUG -fno-plt -fprofile-generate
PROFILE_USE_CFLAGS := -O3 -march=native -flto -fomit-frame-pointer -DNDEBUG -fno-plt -fprofile-use -fprofile-correction

ifeq ($(BUILD),debug)
MODE_CFLAGS := $(DEBUG_CFLAGS)
else ifeq ($(MODE),pgo-generate)
MODE_CFLAGS := $(PROFILE_GENERATE_CFLAGS)
else ifeq ($(MODE),pgo-use)
MODE_CFLAGS := $(PROFILE_USE_CFLAGS)
else
MODE_CFLAGS := $(RELEASE_CFLAGS)
endif

CFLAGS := $(WARNINGS) $(BASE_CFLAGS) $(MODE_CFLAGS)
LDFLAGS := -flto

.PHONY: all clean run rebuild debug release pgo-generate pgo-use

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
	@$(MAKE) BUILD=debug MODE=default all

release:
	@$(MAKE) BUILD=release MODE=default all

pgo-generate:
	@$(MAKE) BUILD=pgo MODE=pgo-generate all

pgo-use:
	@$(MAKE) BUILD=pgo MODE=pgo-use all

.PHONY: pgo-train
pgo-train:
	@$(MAKE) pgo-generate
	./$(TARGET) --selfplay 2000 --quiet || true
	@$(MAKE) pgo-use

clean:
	@echo "[CLEAN] removing build artifacts"
	@rm -rf build $(TARGET)

-include $(DEPS)
