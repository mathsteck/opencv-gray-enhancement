all:
	g++ -g -o main main.cpp -I /usr/include/opencv `pkg-config opencv --libs`
