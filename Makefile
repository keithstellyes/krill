main: main.o Shader.o ShaderProgram.o Buffer.o VertexDefinition.o VAO.o
	g++ main.o Shader.o ShaderProgram.o Buffer.o VertexDefinition.o VAO.o -o main -lSDL3 -lGL -lGLEW

main.o: src/main.cpp src/Shader.hpp src/Buffer.hpp src/VAO.hpp src/VertexDefinition.hpp
	g++ src/main.cpp -c

Shader.o: src/Shader.cpp src/Shader.hpp
	g++ src/Shader.cpp -c

ShaderProgram.o: src/ShaderProgram.cpp src/ShaderProgram.hpp src/Shader.hpp
	g++ src/ShaderProgram.cpp -c

Buffer.o: src/Buffer.cpp src/Buffer.hpp
	g++ src/Buffer.cpp -c

VertexDefinition.o: src/VertexDefinition.cpp src/VertexDefinition.hpp
	g++ src/VertexDefinition.cpp -c

VAO.o: src/VAO.hpp src/VAO.cpp src/Buffer.hpp
	g++ src/VAO.cpp -c

clean:
	rm -f *.o
