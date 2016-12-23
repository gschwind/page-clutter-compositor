
#include "page.hxx"

int main(int argc, char** argv) {
	page::page main;
	main.init(&argc, &argv);
	main.run();
	return 0;
}

