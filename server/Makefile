target = serv
CXX = g++ -Wall
CFLAGS = 
obj = server.o polling.o sockinit.o threadpool.o
lib = -lpthread

.PHONY : all
all : $(target)

.PHONY : debug
debug : CFLAGS += -DDEBUG -g
debug : $(target)

$(target) : $(obj) $(lib)
	$(CXX) $(CFLAGS) -o $@ $^ $(lib)
		
$(obj) : $(obj:.o=.cc)
	$(CXX) $(CFLAGS) -c -o $@ $(@:.o=.cc) $(lib)

.PHONY : clean
clean :
	@rm -f $(target) $(obj)
