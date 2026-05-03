CXX = g++ 
CXXFLAGS = -std=c++20 -Wall -Wextra -O2

TARGET = game
SRCS = main.cpp gameRenderer.cpp gameLogger.cpp gameInteraction.cpp \
gameEngine.cpp gameConfig.cpp config.cpp gameLogic.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

ifeq ($(OS), Windows_NT) 
    RM = del /Q
	TARGET_EXT = .exe
else 
    RM = rm -f
	TARGET_EXT = 
endif

clean:
	$(RM) *.o 
	$(RM) $(TARGET)$(TARGET_EXT)
