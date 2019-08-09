#ifndef NODO_H
#define NODO_H

#include <qstring.h>
#include <QList>

class Nodo
{
public:
    Nodo(QString tipo, QString valor);
    QString tipo;
    QString valor;
    int linea;
    int columna;
    QList<Nodo> hijos;
    QString cadenaDot;
    void add(Nodo n);
};

#endif // NODO_H
