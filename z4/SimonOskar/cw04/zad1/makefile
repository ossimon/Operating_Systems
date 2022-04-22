main: main.c exec.c
	gcc main.c -o main.out -Wall
	gcc exec.c -o exec.out -Wall

tests: main
	for action in ignore handler mask pending ; do \
		printf "$$action \n" >> raport2.txt ; \
		./main $$action >> raport2.txt ; \
		printf "\n" >> raport2.txt ; \
	done

clean:
	rm -f exec.out main.out