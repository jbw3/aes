# Makefile

#INCLUDES = -I<include path>
OBJDIR = obj

CXX = g++
CXXFLAGS = -std=c++1z #-stdlib=libc++ #$(INCLUDES)

#LDLIBS = -L<lib path> -l<lib name>
LDFLAGS = #-stdlib=libc++ #$(LDLIBS)

# DEPS = X.h

_OBJ = main.o
OBJ = $(patsubst %,$(OBJDIR)/%,$(_OBJ))

TARGET = test

.PHONY: all
all: init $(TARGET)

.PHONY: init
init:
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJDIR)/*.o $(TARGET)
