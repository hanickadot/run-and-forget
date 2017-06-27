.PHONY: default all clean
	
CXXFLAGS := -std=c++14
LDFLAGS := -stdlib=libc++	

default: all
	
all: test
	
test: test.o
	$(CXX) $(LDFLAGS) $< -o $@
	
test.o: test.cpp run-and-forget.hpp
	$(CXX) -c $(CXXFLAGS) $< -o $@
	
clean:
	rm -f *.o test