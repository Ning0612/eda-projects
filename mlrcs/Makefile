# 編譯器和標誌
CXX = g++
CXXFLAGS = -std=c++11 -I$(GUROBI_HOME)/include
LDFLAGS = -L$(GUROBI_HOME)/lib -lgurobi_c++ -lgurobi120

# 目錄
SRC_DIR = src
BIN_DIR = .

# 程式名稱
TARGET = $(BIN_DIR)/mlrcs

# 原始碼文件
SRC = $(wildcard $(SRC_DIR)/*.cpp)

# 預設規則
all: $(TARGET)

# 編譯並連結
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# 清理
clean:
	rm -f $(TARGET)
