CXXFLAGS = -std=c++17 -O2

SRC = src/iptoasn.cpp
BIN = iptoasn

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	$(RM) $(BIN)
