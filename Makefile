INPUTDIR =./src/
CC=gcc

build: $(INPUTDIR)image_processor.c 
	$(CC) $(INPUTDIR)image_processor.c -o image_processor
	mkdir build
	mv image_processor ./build/ 
run:
	./build/image_processor
.PHONY: clean

clean:
	rm -rf ./build