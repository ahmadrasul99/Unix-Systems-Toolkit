all: mytoolkit.c
	gcc -g -lreadline -o mytoolkit mytoolkit.c
clean:
	rm mytoolkit
