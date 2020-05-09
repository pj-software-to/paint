TARGET_EXEC := paint
BUILD_DIR := ./build
BUILD_FILES := base.cpp canvas.cpp interpolation.cpp
VERSION := -std=c++11

paint:
	g++ $(BUILD_FILES) $(VERSION) -O2 `wx-config --cxxflags --libs` -o $(BUILD_DIR)/$(TARGET_EXEC)

debug:
	g++ $(BUILD_FILES) $(VERSION) -g `wx-config --cxxflags --libs` -o $(BUILD_DIR)/$(TARGET_EXEC)

gprof:
	g++ $(BUILD_FILES) $(VERSION) -pg `wx-config --cxxflags --libs` -o $(BUILD_DIR)/$(TARGET_EXEC)

clean:
	rm -f $(BUILD_DIR)/*
