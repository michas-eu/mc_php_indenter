#include <stdio.h>
#include <QCoreApplication>
#include <QTextStream>
#include <QTimer>

#include "formatter.hxx"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QTextStream in(stdin);
	QTextStream out(stdout);
	Formatter f(0,&in,&out);
	QTimer::singleShot(0,&f,SLOT(start()));
	a.connect(&f,SIGNAL(about_to_quit()),SLOT(quit()));
	return a.exec();
}
