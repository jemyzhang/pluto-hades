#ifndef COMICBOOK_H
#define COMICBOOK_H

#include <QThread>

class ComicBook : public QThread
{
	Q_OBJECT

public:
	ComicBook(QObject *parent);
	~ComicBook();

private:
	
};

#endif // COMICBOOK_H
