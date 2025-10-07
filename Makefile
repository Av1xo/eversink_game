run:
	g++ stb_image.cpp main.cpp glad.c -o main -lglfw -ldl -lGL
	./main