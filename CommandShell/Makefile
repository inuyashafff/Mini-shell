SOURCES=command.cpp  parser.cpp  shell.cpp  main.cpp
OBJS=$(patsubst %.cpp, %.o, $(SOURCES))
CPPFLAGS=-ggdb3 -Wall -Werror -pedantic -std=gnu++98

CommandShell: $(OBJS)
	g++ $(CPPFLAGS) -o CommandShell $(OBJS)
%.o: %.cpp shell.h  command.h  parser.h
	g++ $(CPPFLAGS) -c $<

clean:
	rm CommandShell *~ *.o
