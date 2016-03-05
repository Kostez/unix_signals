include_dir = ./includes
src_dir = ./src
bin_dir = ./bin

all: lab
main.o:
	gcc -c -I$(include_dir) $(src_dir)/main.c -o $(bin_dir)/main.o
common.o:
	gcc -c -I$(include_dir) $(src_dir)/common.c -o $(bin_dir)/common.o
lab: main.o common.o
	gcc -o $(bin_dir)/lab $(bin_dir)/main.o $(bin_dir)/common.o
clean:
	rm -rf $(bin_dir)/*.o
