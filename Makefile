ROOT_PATH = $(abspath $(lastword $(MAKEFILE_LIST)))
SRC_DIR = $(dir $(ROOT_PATH))src
BUILD_DIR = $(dir $(ROOT_PATH))build
EX_DIR = $(dir $(ROOT_PATH))bin
TEST_DIR = $(dir $(ROOT_PATH))test
EX_NAME = oscill

CC = g++

ifeq ($(RELEASE),1)
  COMPILE_FLAGS = -O3 -w
endif
ifeq ($(DEBUG),1)
  COMPILE_FLAGS = -Og -g -Wall
endif

CFLAGS = -std=gnu++17 $(COMPILE_FLAGS)
LDFLAGS = -lm
SDLFLAGS = -lSDL2 -lSDL2_ttf $(sdl2-config --libs --cflags)

LAYOUT = $(wildcard $(SRC_DIR)/layout/*.cpp)
LAYOUT_OBJS = $(patsubst $(SRC_DIR)/layout/%.cpp,$(BUILD_DIR)/%.o,$(LAYOUT))
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

LAYOUT_DEPS = $(wildcard $(SRC_DIR)/layout/*.h)
DEPS = $(wildcard $(SRC_DIR)/*.h)

$(EX_NAME): $(EX_DIR)/$(EX_NAME)
$(EX_DIR)/$(EX_NAME): $(LAYOUT_OBJS) $(OBJS) $(DEPS)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) $(SDLFLAGS)
$(LAYOUT_OBJS): $(BUILD_DIR)/%.o : $(SRC_DIR)/layout/%.cpp $(LAYOUT_DEPS)
	$(CC) -c $< -o $@ $(CFLAGS)
$(OBJS): $(BUILD_DIR)/%.o : $(SRC_DIR)/%.cpp $(DEPS) $(LAYOUT_DEPS)
	$(CC) -c $< -o $@ $(CFLAGS)

test: $(TEST_DIR)/test
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TEST_DEPS = $(wildcard $(TEST_DIR)/*.h)
TEST_OBJS = $(patsubst $(TEST_DIR)/%.cpp,$(TEST_DIR)/%.o,$(TEST_SRCS))
$(TEST_DIR)/test: $(TEST_OBJS) $(TEST_DEPS)
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS) $(SDLFLAGS)
$(TEST_OBJS): $(TEST_DIR)/%.o : $(TEST_DIR)/%.cpp $(TEST_DEPS)
	$(CC) -c $< -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(BUILD_DIR)/*.o $(TEST_DIR)/*.o $(EX_DIR)/$(EX_NAME)
