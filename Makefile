all:
	rm -rf release
	mkdir release
	clang -lm -lpthread src/main.c -o release/urply

run:
	@ ./release/urply

install-foru:
	mv release/urply ~/.local/bin/

install:
	sudo mv release/urply /usr/local/bin/

remove-foru:
	rm ~/.local/bin/urply

remove:
	sudo rm /usr/local/bin/urply
