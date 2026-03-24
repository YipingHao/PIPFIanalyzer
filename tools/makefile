BASE = ../


ODIR = $(BASE)obj/
CODE = $(BASE)code/
TOOLS = $(BASE)tools/
TEST = $(BASE)test/

COFILE = $(ODIR)sheet.o $(ODIR)analyzer.o $(ODIR)automata.o $(ODIR)dictionary.o $(ODIR)analyzerthread.o $(ODIR)printer.o
TestOFile = $(TEST)test.o $(TEST)main.o
HFILES = $(CODE)analyzer.h $(CODE)sheet.h $(CODE)automata.h $(CODE)dictionary.h $(CODE)template.h

OPTION ?= -O2
#OPTION = -O3
#EXTRA # =-Wall -Wextra -fno-strict-aliasing -fwrapv -fno-aggressive-loop-optimizations -Wconversion -ftrapv
#CC = icc
CC = g++
CFLAGS = $(OPTION) $(EXTRA)



#TestOFile = $(TEST)main.o $(ODIR)lexical.o



libFIanalyzer.a:  $(COFILE)
	ar -rcv $(BASE)libFIanalyzer.a  $(COFILE)

#$(ODIR)lexical.o:$(CODE)lexical.cpp $(CODE)lexical.h
#	g++ -o $(ODIR)lexical.o -c -O3  $(CODE)lexical.cpp

$(ODIR)sheet.o:$(CODE)sheet.cpp $(CODE)sheet.h
	$(CC) -o $(ODIR)sheet.o -c $(CFLAGS) $(CODE)sheet.cpp 

$(ODIR)analyzer.o:$(CODE)analyzer.cpp $(CODE)analyzer.h $(CODE)sheet.h $(CODE)automata.h $(CODE)dictionary.h $(CODE)template.h
	$(CC) -o $(ODIR)analyzer.o -c $(CFLAGS) $(CODE)analyzer.cpp 

$(ODIR)analyzerthread.o:$(CODE)analyzerthread.cpp $(CODE)analyzer.h $(CODE)sheet.h $(CODE)automata.h $(CODE)dictionary.h $(CODE)template.h
	$(CC) -o $(ODIR)analyzerthread.o -c $(CFLAGS) $(CODE)analyzerthread.cpp 

$(ODIR)printer.o:$(CODE)printer.cpp $(HFILES)
	$(CC) -o $(ODIR)printer.o -c $(CFLAGS) $(CODE)printer.cpp 


$(ODIR)automata.o:$(CODE)automata.cpp $(CODE)automata.h
	$(CC) -o $(ODIR)automata.o -c $(CFLAGS) $(CODE)automata.cpp 

$(ODIR)dictionary.o:$(CODE)dictionary.cpp $(CODE)dictionary.h $(CODE)automata.h 
	$(CC) -o $(ODIR)dictionary.o -c $(CFLAGS) $(CODE)dictionary.cpp 

test.exe: $(COFILE) $(TestOFile) 
	$(CC) -o $(TEST)test.exe $(COFILE) $(TestOFile) -lpthread

$(TEST)test.o: $(CODE)analyzer.h $(CODE)sheet.h $(CODE)automata.h $(CODE)dictionary.h $(CODE)template.h $(TEST)test.cpp
	$(CC) -o $(TEST)test.o -c $(CFLAGS) $(TEST)test.cpp

$(TEST)main.o: $(CODE)FIanalyzer.h $(TEST)main.cpp $(CODE)analyzer.h $(CODE)sheet.h $(CODE)automata.h $(CODE)dictionary.h $(CODE)template.h
	$(CC) -o $(TEST)main.o -c $(CFLAGS) $(TEST)main.cpp

#$(ODIR)FIanalyzer.o:$(CODE)FIanalyzer.cpp $(CODE)FIanalyzer.h
#	$(CC) -o $(ODIR)FIanalyzer.o -c -std=c++11 -O3  $(CODE)FIanalyzer.cpp

#$(ODIR)xyz.o:$(CODE)xyz.cpp $(CODE)FIanalyzer.h
#	g++ -o $(ODIR)xyz.o -c -std=c++11 -O3  $(CODE)xyz.cpp

#LIBlex = -L $(BASE)extern/lib -lHyperAcc
#libHyperAcc.h

#test.exe: $(COFILE) $(TestOFile)
#	g++ -o $(TEST)test.exe $(COFILE) $(TestOFile)

#test.exe: $(COFILE) $(TestOFile) $(CODE)libHyperAcc.h
#	g++ -o $(TEST)test.exe $(COFILE) $(TEST)main.o $(LIBlex) -lpthread

#$(TEST)main.o: $(CODE)FIanalyzer.h $(TEST)main.cpp $(CODE)lexical.h
#	g++ -o $(TEST)main.o -c -O3 $(TEST)main.cpp

#$(TEST)main.o: $(CODE)FIanalyzer.h $(TEST)main.cpp
#	g++ -o $(TEST)main.o -c -O3 $(TEST)main.cpp


.PHONY: clean
clean:
	rm $(ODIR)*.o
	rm $(TEST)*.o







