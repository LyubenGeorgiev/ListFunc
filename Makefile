listFunc: main.cpp token.cpp return_value.cpp parser.cpp lexer.cpp interpreter.cpp ListFunc.cpp
	g++ -std=c++11 -O3 main.cpp token.cpp return_value.cpp parser.cpp lexer.cpp interpreter.cpp ListFunc.cpp -o listFunc

# main2.o: main2.cpp
# 	g++ -std=c++11 -c main2.cpp

# token.o: token.cpp
# 	g++ -std=c++11 -c token.cpp

# return_value.o: return_value.cpp
# 	g++ -std=c++11 -c return_value.cpp

# parser.o: parser.cpp
# 	g++ -std=c++11 -c parser.cpp

# lexer.o: lexer.cpp
# 	g++ -std=c++11 -c lexer.cpp

# interpreter.o: interpreter.cpp
# 	g++ -std=c++11 -c interpreter.cpp

# clean:
# 	rm *.o listFunc