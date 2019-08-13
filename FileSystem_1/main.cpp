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
void crearArchivo(QString);
QString getDireccion(QString);
QString getArchivo(QString);
bool existePLogica(QString, QString);

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
    cout << "Por favor escriba algunos comandos:" << endl;
    while(true){
        char input[400];
        printf(">> ");
        fgets(input,sizeof (input),stdin);
        YY_BUFFER_STATE buffer = yy_scan_string(input);
        if(yyparse() == 0){
            Graficador *g = new Graficador(raiz);
            g->generarImagen();
            //cout << "Cadena correcta" << endl;
            reconocerComando(raiz);
            memset(input,0,400);
        }else{
            cout << "Comando no reconocido" << endl;
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
                cout << "ERROR: Parametro -size ya agregado" << endl;
                flag = true;
                break; //ERROR
            }
            flagSize = true;
            valSize = n.valor.toInt();
            if(!(valSize > 0)){
                cout << "ERROR: Parametro -size menor a cero" << endl;
                break;
            }
        }else if(n.tipo_ == FIT){
            if(flagFit){
                cout << "ERROR: Parametro -fit ya agreagdo" << endl;
                flag = true;
                break; //ERROR
            }
            flagFit = true;
            valFit = n.valor;
        }else if(n.tipo_ == UNIT){
            if(flagUnit){
                cout << "ERROR: Parametro -unit ya agreagdo" << endl;
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
                cout << "ERROR: Valor del parametro -unit no reconocido" << endl;
                break;
            }
        }else if(n.tipo_ == PATH){
            if(flagPath){
                cout << "ERROR: Parametro -path ya agreagdo" << endl;
                flag = true;
                break;
            }
            flagPath = true;
            valPath = n.valor;//Quitarle comillas si tiene
            valPath = valPath.replace("\"","");
        }
    }

    if(!flag){//Flag para ver si hay parametros repetidos
        if(flagSize  && flagPath){//Parametros obligatorios
            printf("Comando mkdisk semanticamente correcto\n");
            MBR masterboot;

            int total_size = 1024;
            crearArchivo(valPath);
            masterboot.mbr_date_created = time(NULL);
            masterboot.mbr_disk_signature = (int)time(NULL);
            if(flagUnit){//Si hay parametro unit
                if(valUnit == 'm'){
                    masterboot.mbr_size = valSize * 1048576;
                    total_size = valSize * 1024;
                }
            }else{
                masterboot.mbr_size = valSize*1048576;
                total_size = valSize * 1024;
            }

            if(flagFit){

            }else{

            }

            for(int p = 0; p < 4; p++){
                masterboot.mbr_partition[p].part_status = '\0';
                masterboot.mbr_partition[p].part_type = '\0';
                masterboot.mbr_partition[p].part_fit = '\0';
                strcpy(masterboot.mbr_partition[p].part_name,"");
                masterboot.mbr_partition[p].part_size = 0;
                masterboot.mbr_partition[p].part_start = -1;
            }
            string comando = "dd if=/dev/zero of=\""+valPath.toStdString()+"\" bs=1024 count="+to_string(total_size);
            system(comando.c_str());
            FILE *fp = fopen(valPath.toStdString().c_str(),"rb+");
            fseek(fp,0,SEEK_SET);
            fwrite(&masterboot,sizeof(MBR),1,fp);
            fclose(fp);
        }else{
            //ERROR
        }
    }

}

void recorrerRMDISK(Nodo *raiz){
    QString valPath = raiz->hijos.at(0).valor;
    valPath = valPath.replace("\"","");

    FILE *fp;
    if((fp=fopen(valPath.toStdString().c_str(),"r"))){
        string opcion = "";
        cout << ">> ¿Seguro que desea eliminar el disco? Y/N : ";
        getline(cin,opcion);
        if(opcion.compare("Y") == 0 || opcion.compare("y") == 0){
            string comando = "rm \""+valPath.toStdString()+"\"";
            system(comando.c_str());
            cout << ">> Disco eliminado con exito" << endl;
        }else if(opcion.compare("N") || opcion.compare("n") == 0){
            cout << ">> Cancelado con exito" << endl;;
        }else{
            cout << ">> Opcion incorrecta" << endl;
        }
        fclose(fp);
    }else{
        cout << "No existe el disco a eliminar" << endl;
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

    int valSize = 0;
    char valUnit = 0;
    char valType = 0;
    QString valPath = "";
    QString valName = "";
    QString valFit = "";

    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        Nodo n = raiz->hijos.at(i);
        switch (n.tipo_)
        {
        case SIZE:
        {
            if(flagSize){
                //ERROR
                flag = true;
                break;
            }
            flagSize = true;
            valSize = n.valor.toInt();
            if(!(valSize > 0)){
                cout << "ERROR: parametro size menor a cero" << endl;
                flag = true;
                break;
            }
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
            string temp = n.valor.toStdString();
            valUnit = temp[0];
            if(valUnit == 'B' || valUnit == 'b'){
                valUnit = 'b';
            }else if(valUnit == 'K' || valUnit == 'k'){
                valUnit = 'k';
            }else if(valUnit == 'M' || valUnit == 'm'){
                valUnit = 'm';
            }else{
                cout << "ERROR: Valor del parametro unit no reconocido" << endl;
                flag = true;
                break;
            }

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
            valPath = n.valor;
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
            string temp = n.valor.toStdString();
            valType = temp[0];
            if(valType == 'P' || valType == 'p'){
                valType = 'P';
            }else if(valType == 'E' || valType == 'e'){
                valType = 'E';
            }else if(valType == 'L' || valType == 'l'){
                valType = 'L';
            }else{
                cout << "ERROR: Valor del parametro no type no reconocido" << endl;
                flag = true;
                break;
            }
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
            //valFit

            flagFit = true;
            valFit = n.hijos.at(0).valor;
            if(valFit == "bf"){
                valFit = "BF";
            }else if(valFit == "ff"){
                valFit = "FF";
            }else if(valFit == "wf"){
                valFit = "WF";
            }
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
            string tipo = n.hijos.at(0).tipo.toStdString();
            if(tipo.compare("ID") == 0){
                valName = n.hijos.at(0).valor;
            }else{
                valName = n.hijos.at(0).valor;
                valName = valName.replace("\"","");
            }
        }
            break;
        case ADD:
        {
            if(flagAdd){
                flag = true;
                //ERROR
                break;
            }
            flagAdd = true;
        }
            break;


        }
    }

    if(!flag){
        if(flagPath){
            string auxPath = valPath.toStdString();
            if(flagName){
                if(flagSize){
                    if(flagDelete || flagAdd){
                        cout << "ERROR: Parametro demas" << endl;
                    }else{
                        bool flagParticion = false;//Flag para ver si hay una particion disponible
                        int numeroParticion = 0;//Que particioon es
                        bool flagExtendida = false;//Flag para chequear si hay una particion extendida
                        FILE *fp;
                        MBR masterboot;
                        if((fp = fopen(auxPath.c_str(),"rb+"))){
                            fseek(fp,0,SEEK_SET);
                            fread(&masterboot,sizeof(MBR),1,fp);
                            //Revisar si alguna particion esta libre
                            for(int p = 0; p < 4; p++){
                                if(masterboot.mbr_partition[p].part_status == -1){//-----------
                                    numeroParticion = p;
                                    flagParticion = true;
                                }
                            }

                            /*-----Verificar particiones-------*/
                            if(!flagParticion && valType != 'L'){
                                cout << "ERROR: Ya existen 4 particiones, no se puede crear otra" << endl;
                                cout << "Elimine alguna para poder crear una" << endl;
                            }else{//Es un particion logica :o
                                if(!existePLogica(valPath,valName)){
                                    //Ver si ya hay una particion extendida

                                }else{
                                    cout << "Ya existe una particion con ese nombre" << endl;
                                }

                            }
                            fclose(fp);
                        }else{
                            cout << "ERROR no existe el disco" << endl;
                        }
                    }
                }else if(flagAdd){
                    if(flagSize || flagDelete){
                        cout << "ERROR: Parametros demas" << endl;
                    }
                }else if(flagDelete){
                    if(flagSize || flagAdd || flagFit || flagType){
                        cout << "ERROR: Parametros demas" << endl;
                    }else{
                        FILE *fp;
                        if((fp=fopen(auxPath.c_str(),"rb+"))){
                            fseek(fp,0,SEEK_SET);
                            MBR masterboot;
                            fread(&masterboot,sizeof(MBR),1,fp);
                            /*--------Buscamos una particion primaria-------*/
                            string auxName = valName.toStdString();
                            for(int i = 0; i < 4; i++){
                                if(strcmp(masterboot.mbr_partition[i].part_name,auxName.c_str()) == 0 && masterboot.mbr_partition[i].part_status == '1'){

                                }
                            }
                            fclose(fp);
                        }
                    }
                }
            }else {
                cout << "ERROR parametro -name no definido" << endl;
            }
        }else{
            cout << "ERROR parametro -path no definido" << endl;
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

void crearArchivo(QString direccion){
    QString aux = getDireccion(direccion);
    string comando = "sudo mkdir -p "+aux.toStdString();
    system(comando.c_str());
    string comando2 = "sudo chmod -R 777 "+aux.toStdString();
    system(comando2.c_str());
    string arch = direccion.toStdString();
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

bool existePLogica(QString direccion, QString nombre){
    int particionE = 0;
    FILE *fp = fopen(direccion.toStdString().c_str(),"rb+");
    fseek(fp,0,SEEK_SET);
    MBR masterboot;
    fread(&masterboot,sizeof(MBR),1,fp);
    for(int i = 0; i < 4; i++){
        if(masterboot.mbr_partition[i].part_status == '1'){
            if(strcmp(masterboot.mbr_partition[i].part_name,nombre.toStdString().c_str()) == 0){
                fclose(fp);
                return true;
            }else if(masterboot.mbr_partition[i].part_type == 'E'){
                particionE = i;
            }
        }
    }
    if(particionE!=-1){
        fseek(fp,masterboot.mbr_partition[particionE].part_start,SEEK_SET);
        EBR extendedboot;
        while((fread(&extendedboot,sizeof(EBR),1,fp)!=0)){
            if(strcmp(extendedboot.part_name,nombre.toStdString().c_str()) == 0){
                return true;
            }
            if(extendedboot.part_next == -1){
                break;
            }
        }
    }
    return false;
}
