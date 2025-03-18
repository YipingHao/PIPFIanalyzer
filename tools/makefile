BASE = ../


ODIR = $(BASE)obj/
CODE = $(BASE)code/
TOOLS = $(BASE)tools/
TEST = $(BASE)test/

COFILE = $(ODIR)FIanalyzer.o $(ODIR)xyz.o
TestOFile = $(TEST)main.o

#TestOFile = $(TEST)main.o $(ODIR)lexical.o



libFIanalyzer.a:  $(COFILE)
	ar -rcv $(BASE)libFIanalyzer.a  $(COFILE)

#$(ODIR)lexical.o:$(CODE)lexical.cpp $(CODE)lexical.h
#	g++ -o $(ODIR)lexical.o -c -O3  $(CODE)lexical.cpp

$(ODIR)FIanalyzer.o:$(CODE)FIanalyzer.cpp $(CODE)FIanalyzer.h
	g++ -o $(ODIR)FIanalyzer.o -c -std=c++11 -O3  $(CODE)FIanalyzer.cpp

$(ODIR)xyz.o:$(CODE)xyz.cpp $(CODE)FIanalyzer.h
	g++ -o $(ODIR)xyz.o -c -std=c++11 -O3  $(CODE)xyz.cpp

LIBlex = -L $(BASE)extern/lib -lHyperAcc
#libHyperAcc.h

#test.exe: $(COFILE) $(TestOFile)
#	g++ -o $(TEST)test.exe $(COFILE) $(TestOFile)

test.exe: $(COFILE) $(TestOFile) $(CODE)libHyperAcc.h
	g++ -o $(TEST)test.exe $(COFILE) $(TEST)main.o $(LIBlex) -lpthread

#$(TEST)main.o: $(CODE)FIanalyzer.h $(TEST)main.cpp $(CODE)lexical.h
#	g++ -o $(TEST)main.o -c -O3 $(TEST)main.cpp

$(TEST)main.o: $(CODE)FIanalyzer.h $(TEST)main.cpp
	g++ -o $(TEST)main.o -c -O3 $(TEST)main.cpp


.PHONY: clean
clean:
	rm $(ODIR)*.o
	rm $(TEST)*.o







