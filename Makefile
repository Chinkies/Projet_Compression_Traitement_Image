PROJECT_ROOT := .
MOSAIC_DIR := $(PROJECT_ROOT)/source/mosaic/src
SUPERPIXEL_DIR := $(PROJECT_ROOT)/source/super_pixel/src
LIB_DIR := $(MOSAIC_DIR)/lib
BUILD_DIR := $(PROJECT_ROOT)/build/mosaic
BIN_DIR := $(PROJECT_ROOT)/bin
TARGET := $(BIN_DIR)/mosaic

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
CPPFLAGS := -I$(MOSAIC_DIR) -I$(LIB_DIR) -I$(SUPERPIXEL_DIR)

SRCS := $(wildcard $(MOSAIC_DIR)/*.cpp) $(wildcard $(LIB_DIR)/*.cpp) $(wildcard $(SUPERPIXEL_DIR)/*.cpp)
OBJS := $(patsubst $(PROJECT_ROOT)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(PROJECT_ROOT)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

-include $(DEPS)