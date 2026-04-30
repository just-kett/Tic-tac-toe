CXX = g++ 
CXXFLAGS = -std=g++20 -Wall -Wextra -02

TARGET = game
SRCS = main.cpp gameRenderer.cpp gameLogger.cpp gameInteraction.cpp 
gameEngine.cpp gameConfig.cpp config.cpp
OBJS = $(SRCS:.cpp=.o)

all: $($TARGET)

$(TARGET): $(OBJS) 
    $(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
    $(CXX) $(CXXFLAGS) -c $< -o $@

ifeq ($(OS), Window_NT) 
    RM = del /Q
	TARGET_EXT = .exe
else 
    RM = rm -f
	TARGET_EXT = 
endif

clean:
    $(RM) *.o 
	$(RM) $(TARGET)$(TARGET_EXT)
