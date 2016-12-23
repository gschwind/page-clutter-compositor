
#include <page-core.hxx>

int main(int argc, char** argv) {
	page::page_core main;
	main.init(&argc, &argv);
	main.run();
	return 0;
}

