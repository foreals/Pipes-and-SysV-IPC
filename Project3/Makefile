all: master reverse upper

master: master.c
	clang -o master master.c -lpthread
reverse: reverse.c
	clang -o reverse reverse.c -lpthread
upper: upper.c
	clang -o upper upper.c -lpthread
clean:
	rm -f master *~
	rm -f reverse *~
	rm -f upper *~
