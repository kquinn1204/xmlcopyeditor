build:
	g++ -o tests-main \
	  tests-main.cpp \
	  tests-readfile.cpp \
	  tests-replace.cpp \
	  tests-stringset.cpp \
	  tests-dita-models.cpp \
	  ../src/replace.cpp \
	  ../src/readfile.cpp \
	  ../src/ditatopicmodel.cpp \
	  ../src/ditamapmodel.cpp \
	  ../src/ditadetector.cpp \
	  -I/usr/include/libxml2 \
	  `wx-config --cxxflags` \
	  -lxml2 \
	  `wx-config --libs` \
	  -std=c++11
run:
	./tests-main
all: build run
clean:
	rm -f *.o tests-main
