CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
LDFLAGS =

ifeq ($(USE_GUROBI),1)
CXXFLAGS += -DUSE_GUROBI -I$(GUROBI_HOME)/include
LDFLAGS += -L$(GUROBI_HOME)/lib -lgurobi_c++ -lgurobi120
endif

SRC_DIR = src
BIN_DIR = .

TARGET = $(BIN_DIR)/mlrcs

SRC = $(wildcard $(SRC_DIR)/*.cpp)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
