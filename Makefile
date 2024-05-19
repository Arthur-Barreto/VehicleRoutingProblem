# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -O2
OMPFLAGS = -fopenmp

# Source and object files
SRC_DIR = src
OBJ_DIR = obj
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Targets
TARGETS = global_search parallel_search

# Default target
all: $(TARGETS)

# Create object directory if it doesn't exist
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# Compile global_search without OpenMP
global_search: $(OBJ_DIR)/global_search.o $(OBJ_DIR)/utils.o
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $^ -o $@

# Compile parallel_search with OpenMP
parallel_search: $(OBJ_DIR)/parallel_search.o $(OBJ_DIR)/utils.o
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) $^ -o $@

# Compile object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(OMPFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(TARGETS)