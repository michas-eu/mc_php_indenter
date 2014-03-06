#include "formatter.hxx"

Formatter::Formatter(QObject *parent, QTextStream *in, QTextStream *out)
	: QObject(parent)
{
	this->in = in;
	this->out = out;
	this->fcs = "\t";
	this->active = "";
}

void Formatter::set_rexp(QString l, QString r) {
	this->l.setPattern(l);
	this->r.setPattern(r);
}

void Formatter::set_long_rexp(QString s) {
	QString l, r;
	l = s + "\\s*\\(.*\\)\\s*\\:";
	l.prepend("\\b");
	r = s + "\\;";
	r.prepend("\\bend");
	set_rexp(l,r);
}

void Formatter::drop_literals(QString *s) {
	QString nstr = "";
	const ushort *d = s->utf16();
	size_t i = 0;

	l_start:
		if (d[i] == 0) {
			goto l_end;
		} else if (active.isEmpty()) {
			goto l_empty;
		} else {
			goto l_busy;
		}
	l_empty:
		if (d[i] == '\'') {
			active = "'";
			goto l_inc;
		} else if (d[i] == '"') {
			active = "\"";
			goto l_inc;
		} else if (d[i] == '#') {
			nstr+= d[i];
			goto l_end;
		} else if (d[i] == '/') {
			goto l_slesh;
		} else {
			goto l_inc;
		}
	l_slesh:
		nstr+= d[i];
		i++;
		if (d[i] == '/') {
			nstr+= d[i];
			goto l_end;
		} else if (d[i] == '*') {
			active = "/*";
			goto l_inc;
		} else {
			goto l_start;
		}
	l_busy:
		if (d[i] == '\\') {
			goto l_block;
		} else if (active == QChar(d[i])) {
			active = "";
			goto l_inc;
		} else if (d[i] == '*' && active == "/*") {
			goto l_star;
		} else {
			goto l_skip;
		}
	l_block:
		i++;
		if (d[i] == 0) {
			goto l_end;
		} else {
			goto l_skip;
		}
	l_star:
		i++;
		if (d[i] == 0) {
			goto l_end;
		} else if (d[i] == '/') {
			nstr+= "*";
			active = "";
			goto l_inc;
		} else {
			goto l_skip;
		}
	l_inc:
		nstr+= d[i];
	l_skip:
		i++;
		goto l_start;
	l_end:

	*s = nstr;
}

int Formatter::process(QString s) {
	int ret = 0;
	drop_literals(&s);
	//*out << "xxxxxx" << s << "\n"; out->flush(); //For debugging.
	set_rexp("\\{","\\}");
	ret+= find_braces(&s);
	set_long_rexp("for");
	ret+= find_braces(&s);
	set_long_rexp("if");
	ret+= find_braces(&s);
	set_long_rexp("while");
	ret+= find_braces(&s);
	set_long_rexp("foreach");
	ret+= find_braces(&s);
	set_long_rexp("switch");
	ret+= find_braces(&s);
	return ret;
}

int Formatter::find_braces(QString *s) {
	int num;
	num = s->count(l);
	num-= s->count(r);
	return num;
}

void Formatter::start() {
	int ind = 0;
	int nind = 0;
	QString s;
	while (!in->atEnd()) {
		s = in->readLine().trimmed();
		nind = process(s);
		if (nind < 0) {
			ind+= nind;
		}
		*out << fcs.repeated(ind) << s << "\n";
		out->flush(); //For debugging.
		if (nind > 0) {
			ind+= nind;
		}
	}
	emit about_to_quit();
}
