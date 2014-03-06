#ifndef FORMATTER_HXX
#define FORMATTER_HXX

#include <QObject>
#include <QChar>
#include <QString>
#include <QRegularExpression>
#include <QTextStream>

class Formatter : public QObject
{
	Q_OBJECT
public:
	explicit Formatter(QObject *parent, QTextStream *in, QTextStream *out);
protected:
	QTextStream *in;
	QTextStream *out;
	QString fcs;
	QRegularExpression l, r;
	QString active;
	void set_rexp(QString l, QString r);
	void set_long_rexp(QString s);
	void drop_literals(QString *s);
	int process(QString s);
	int find_braces(QString *s);

signals:
	void about_to_quit();

public slots:
	void start();

};

#endif // FORMATTER_HXX
