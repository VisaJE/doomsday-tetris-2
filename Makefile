#---------------------
# feedback-system Makefile
#
# ---------------------

EXE = DoomsdayTetris2

#Put Rapidjson include folder here
CXXFLAGS = -IC:\Users\Eemil\Documents\Ohjelmointi\rapidjson\include -Wall -O2

LDFLAGS=-g
LDLIBS= -lSDL2main -lSDL2 -lSDL2_ttf

SRC_DIR = src
OBJ_DIR = obj
$(shell if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi)

SRC = $(wildcard $(SRC_DIR)/*.cpp)
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)		
$(info $$SRC is [${SRC}])

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)