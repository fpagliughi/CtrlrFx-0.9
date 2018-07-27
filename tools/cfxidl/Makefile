# Makefile for a simple YACC app w/ LEX lexer

app = cfxidl

RM = rm -f
LEX = flex

CPPFLAGS = -O0 -g

$(app): $(app).cpp $(app).y.cpp Func.o Iface.o lex.yy.o
	$(CXX) $(CPPFLAGS) -o $@ $^ -ly -lfl

$(app).y.cpp: $(app).y
	$(YACC) -d -o $@ $<
	mv $@.h $(app).y.h

lex.yy.c : $(app).l $(app).y.h
	$(LEX) $<

$(app).cpp: $(app).h

Iface.cpp: Iface.h Func.h
Func.cpp: Func.h

.PHONY clean:
clean:
	$(RM) *.o lex.yy.c $(app).y.cpp $(app).y.h *~

.PHONY distclean:
distclean: clean
	$(RM) $(app)


