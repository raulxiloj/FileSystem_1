#include <QCoreApplication>
#include <iostream>
#include <sys/stat.h>

#include "scanner.h"
#include "parser.h"
#include "graficador.h"

extern int yyparse();
extern Nodo *raiz; // Raiz del arbol

void imprimirEncabezado();
void escribaUnComando();
void reconocerComando(Nodo*);
void recorrerMKDISK(Nodo*);
void recorrerRMDISK(Nodo*);
void recorrerFDISK(Nodo*);
void recorrerMOUNT(Nodo*);
void recorrerUNMOUNT(Nodo*);
void recorrerREP(Nodo*);
QString getRuta(Nodo); //BORRAR
void crearArchivo(QString, QString);
QString getDireccion(QString);
QString getArchivo(QString);

using namespace std;

/*
 * Enumeracion que lista todos los comandos y sus parametros reconocidos por
 * el lenguaje que servira para el recorrido del arbol
*/
enum Choice
{
    MKDISK = 1,
    RMDISK = 2,
    FDISK = 3,
    MOUNT = 4,
    UNMOUNT = 5,
    REP = 6,
    PARAMETRO = 7,
    EXEC = 8,
    SIZE = 9,
    FIT = 10,
    UNIT = 11,
    PATH = 12,
    TYPE = 13,
    DELETE = 14,
    NAME = 15,
    ADD = 16,
    ID = 17,
};

/*
    Structs necesarios para guardar la informacion de los discos
    Los discos tendran informacion del MBR y espacio con particiones
*/

typedef struct {
    char part_status; //Indica si la particion esta activa o no
    char part_type; //Indica el tipo de particion
    char part_fit; //Tipo de ajuste de la particion
    int part_start; //Indica en que byte del disco inicia la particion
    int part_size; //Contiene el tamano de la particion en bytes
    char part_name[16]; //Nombre de la particion
} Partition;

typedef struct{
    int mbr_size; //Tamano total del disco en bytes
    time_t mbr_date_created; //Fecha y hora de creacion del disco
    int mbr_disk_signature; //Numero random, que identifica de forma unica cada disco
    char mbr_disk_fit; //Tipo de ajuste
    Partition mbr_partition[4]; //4 particiones
}MBR;

typedef struct{
    char part_status; //Indica si la particion esta activa o no
    char part_fit; //Tipo de ajuste
    int part_start; //Indica en que byte del disco inicia la particion
    int part_size; //Contiene el tamano total de la particion en bytes
    int part_next; //Byte en el que esta el proxima EBR. -1 si no hay siguiente
    char part_name[16]; //Nombre de la particion
}EBR;


/*
    FUNCION PRINCIPAL
*/
int main()
{
    imprimirEncabezado();
    escribaUnComando();
    return 0;
}

void imprimirEncabezado(){
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "|                           Sistema de archivos                             |" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "                                                       Raul Xiloj - 201612113" << endl;
    cout << endl;
}

void escribaUnComando(){
    cout << "Please type some commands" << endl;
    while(true){
        char input[400];
        printf(">> ");
        fgets(input,sizeof (input),stdin);
        YY_BUFFER_STATE buffer = yy_scan_string(input);
        if(yyparse() == 0){
            Graficador *g = new Graficador(raiz);
            g->generarImagen();
            cout << "Cadena correcta" << endl;
            reconocerComando(raiz);
            memset(input,0,400);
        }else{
            cout << "Cadena incorrecta" << endl;
        }
    }
}
/*
 * Analisis semantico, recorrer el arbol
*/
void reconocerComando(Nodo *raiz){
    switch (raiz->tipo_)
    {
    case MKDISK:
    {
        Nodo n = raiz->hijos.at(0);
        recorrerMKDISK(&n);
    }
        break;
    case RMDISK:
    {
        recorrerRMDISK(raiz);
    }
        break;
    case FDISK:
    {
        Nodo n = raiz->hijos.at(0);
        recorrerFDISK(&n);
    }
        break;
    case MOUNT:
    {
        Nodo n = raiz->hijos.at(0);
        recorrerMOUNT(&n);
    }
        break;
    case UNMOUNT:
    {
        recorrerUNMOUNT(raiz);
    }
        break;
    case REP:
        break;
    case EXEC:
        break;
    default: printf("Error no se reconoce el comando");

    }

}

void recorrerMKDISK(Nodo *raiz)
{
    bool flagSize = false;
    bool flagFit = false;
    bool flagUnit = false;
    bool flagPath = false;
    bool flag = false;
    int valSize = 0;
    QString valFit = "";
    char valUnit = 0;
    QString valPath = "";
    QString direccion = "";
    QString archivo = "";
    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        Nodo n = raiz->hijos.at(i);
        if(n.tipo_ == SIZE){
            if(flagSize){
                printf("ERROR: Parametro size ya agreagdo\n");
                flag = true;
                break; //ERROR
            }
            flagSize = true;
            valSize = n.valor.toInt();
        }else if(n.tipo_ == FIT){
            if(flagFit){
                printf("ERROR: Parametro fit ya agreagdo\n");
                flag = true;
                break; //ERROR
            }
            flagFit = true;
            valFit = n.valor;
        }else if(n.tipo_ == UNIT){
            if(flagUnit){
                printf("ERROR: Parametro unit ya agreagdo\n");
                flag = true;
                break;// ERROR;
            }
            flagUnit = true;
            string temp = n.valor.toStdString();
            valUnit = temp[0];
            if(valUnit == 'k'|| valUnit == 'K'){
                valUnit = 'k';
            }else if(valUnit == 'm' || valUnit == 'M'){
                valUnit = 'm';
            }else{
                printf("ERROR: Valor del parametro unit no reconocido");
                break;
            }
        }else if(n.tipo_ == PATH){
            if(flagPath){
                printf("ERROR: Parametro path ya agreagdo\n");
                flag = true;
                break; //ERROR
            }

            flagPath = true;
            valPath = n.valor;//Quitarle comillas si tiene
            valPath = valPath.replace("\"","");
            direccion = getDireccion(valPath);
            archivo = getArchivo(valPath);
        }
    }

    if(!flag){
        if(flagSize  && flagPath){//Parametros obligatorios
            printf("Comando mkdisk semanticamente correcto\n");
            MBR masterboot;
            crearArchivo(direccion, archivo);
            masterboot.mbr_date_created = time(NULL);
            masterboot.mbr_disk_signature = (int)time(NULL);
        }else{
            //ERROR
        }
    }

}

void recorrerRMDISK(Nodo *raiz){
    QString valPath = "";

    if(raiz->hijos.at(0).tipo == "AUX_RUTA"){
        valPath = getRuta(raiz->hijos.at(0));
    }else{
        valPath = raiz->hijos.at(0).valor;
    }

}

void recorrerFDISK(Nodo *raiz){
    bool flagSize = false;
    bool flagUnit = false;
    bool flagPath = false;
    bool flagType = false;
    bool flagFit = false;
    bool flagDelete = false;
    bool flagName = false;
    bool flagAdd = false;
    bool flag = false;

    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        switch (raiz->tipo_)
        {
        case SIZE:
        {
            if(flagSize){
                //ERROR
                flag = true;
                break;
            }
            flagSize = true;
        }
            break;
        case UNIT:
        {
            if(flagUnit){
                //ERROR
                flag = true;
                break;
            }
            flagUnit = true;
        }
            break;
        case PATH:
        {
            if(flagPath){
                flag = true;
                //ERROR
                break;
            }
            flagPath = true;
        }
            break;
        case TYPE:
        {
            if(flagType){
                flag = true;
               //ERROR
                break;
            }
            flagType = true;
        }
            break;
        case FIT:
        {
            if(flagFit)
            {
                flag = true;
                //ERROR
                break;
            }
            flagFit = true;
        }
            break;
        case DELETE:
        {
            if(flagDelete){
                flag = true;
                //ERROR
                break;
            }
            flagDelete = true;
        }
            break;
        case NAME:
        {
            if(flagName){
                flag = true;
                //ERROR
                break;
            }
            flagName = true;
        }
            break;
        case ADD:
        {
            if(flagAdd){
                flag = true;
                //ERROR
                break;
            }
            flagName = true;
        }
            break;


        }
    }

    if(!flag){
        if(flagSize && flagPath && flagName){ //Parametros obligatorios

        }else{
            //ERROR por falta de parametros
        }
    }
}

void recorrerMOUNT(Nodo *raiz){
    bool flagPath = false;
    bool flagName = false;
    bool flag = false;
    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        switch(raiz->tipo_)
        {
        case PATH:
        {
            if(flagPath){
                flag = true;
                //ERROR
                break;
            }
            flagPath = true;
        }
            break;
        case NAME:
        {
            if(flagName){
                flag = true;
                //ERROR
                break;
            }
            flagName = true;
        }
            break;
        }
    }

    if(flagName && flagPath){//Parametros obligatorios

    }

}

void recorrerUNMOUNT(Nodo *raiz){
    QString valID = raiz->hijos.at(0).valor;



}

void recorrerREP(Nodo *raiz){
    bool flagName = false;
    bool flagPath = false;
    bool flagID = false;
    bool flag = false;

    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        switch (raiz->tipo_) {
        case NAME:
        {
            if(flagName){
                //ERROR
                flag = true;
                break;
            }
            flagName = true;
        }
            break;
        case PATH:
        {
            if(flagPath){
                //ERROR
                flag = true;
                break;
            }
            flagPath = true;
        }
            break;
        case ID:
        {
            if(flagID){
                //ERROR
                flag = true;
                break;
            }
            flagID = true;
        }
            break;
        }
    }

    if(!flag){
        if(flagName && flagPath && flagID){

        }
    }


}

void recorrerEXEC(Nodo *raiz){
    if(raiz->hijos.count() > 0){ //Ruta con comillas

    }else{ //Ruta sin comillas

    }
}

QString getRuta(Nodo n){

    return "";
}

void crearArchivo(QString direccion, QString archivo){
    string comando = "sudo mkdir -p "+direccion.toStdString();
    system(comando.c_str());
    string comando2 = "sudo chmod -R 777 "+direccion.toStdString();
    system(comando2.c_str());
    string arch = direccion.toStdString() + archivo.toStdString();
    FILE *fp = fopen(arch.c_str(),"wb");
    if(fp != NULL) fclose(fp);
}

QString getDireccion(QString dir){
    string aux = dir.toStdString();
    string delimiter = "/";
    size_t pos = 0;
    string res = "";
    while((pos = aux.find(delimiter))!=string::npos){
        res += aux.substr(0,pos)+"/";
        aux.erase(0,pos + delimiter.length());
    }
    //res = res.substr(1,res.length());
    return QString::fromStdString(res);
}

QString getArchivo(QString dir){
    string aux = dir.toStdString();
    string delimiter = "/";
    size_t pos = 0;
    string res = "";
    while((pos = aux.find(delimiter))!=string::npos){
        res = aux.substr(0,pos);
        aux.erase(0,pos + delimiter.length());
    }
    return QString::fromStdString(aux);
}
