
#include "page/page.hxx"

int main(int argc, char** argv) {
	page::page_t main;
	main.init(&argc, &argv);
	main.run();
	return 0;
}

