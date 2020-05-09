TARGET_EXEC := paint
BUILD_DIR := ./build
BUILD_FILES := base.cpp canvas.cpp interpolation.cpp

paint:
	g++ $(BUILD_FILES) -O2 `wx-config --cxxflags --libs` -o $(BUILD_DIR)/$(TARGET_EXEC)

debug:
	g++ $(BUILD_FILES) -g `wx-config --cxxflags --libs` -o $(BUILD_DIR)/$(TARGET_EXEC)

gprof:
	g++ $(BUILD_FILES) -pg `wx-config --cxxflags --libs` -o $(BUILD_DIR)/$(TARGET_EXEC)

clean:
	rm -f $(BUILD_DIR)/*
