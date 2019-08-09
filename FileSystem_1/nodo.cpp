#include "nodo.h"

Nodo::Nodo(QString t, QString v){
    tipo =t;
    valor = v;
    hijos = QList<Nodo>();
}

void Nodo::add(Nodo n){
    hijos.append(n);
}
