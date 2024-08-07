#include<QApplication>
#include"SPlay.h"

int main(int argc, char* argv[]) {
	QApplication a(argc, argv);

	SPlay s;
	s.show();
	//s.testEfficientcy();
	return a.exec();
}
