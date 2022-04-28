all: main.c consumer.c producer.c
	gcc main.c -o main.out -Wall
	gcc consumer.c -o consumer.out -Wall
	gcc producer.c -o producer.out -Wall

main: main.c
	gcc main.c -o main.out -Wall

consumer: consumer.c
	gcc consumer.c -o consumer.out -Wall

producer: producer.c
	gcc producer.c -o producer.out -Wall

clean:
	rm -f *.out
