#include <QCoreApplication>
#include <iostream>
#include <sys/stat.h>
#include <fstream>

#include "scanner.h"
#include "parser.h"
#include "graficador.h"
#include "listamount.h"

extern int yyparse();
extern Nodo *raiz; // Raiz del arbol

void imprimirEncabezado();
void reconocerComando(Nodo*);
void recorrerMKDISK(Nodo*);
void recorrerRMDISK(Nodo*);
void recorrerFDISK(Nodo*);
void recorrerMOUNT(Nodo*);
void recorrerUNMOUNT(Nodo*);
void recorrerREP(Nodo*);
void recorrerEXEC(Nodo*);
void crearArchivo(QString);
void crearParticionPrimaria(QString, QString, int, char, char);
void crearParticionExtendida(QString, QString, int, char, char);
void crearParticionLogica(QString, QString, int, char, char);
void eliminarParticion(QString, QString, QString);
void agregarQuitarParticion(QString, QString ,int ,char);
QString getDirectorio(QString);
QString getDireccion(QString);
bool existeParticion(QString, QString);
int buscarParticion_P_E(QString, QString);
int buscarParticion_L(QString, QString);
void leerComando(std::string);
void graficarMBR();
void graficarDisco(QString, QString, QString);
QString getExtension(QString);

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
    COMENTARIO = 18,
};

/*
    Structs necesarios para guardar la informacion de los discos
    Los discos tendran informacion del MBR y el MBR tendra el espacio de las particiones
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

ListaMount *lista = new ListaMount();

bool flag_global = true;

/*
    FUNCION PRINCIPAL
*/
int main()
{
    imprimirEncabezado();
    while(flag_global){
        char input[400];
        printf(">> ");
        fgets(input,sizeof (input),stdin);
        string aux = input;
        leerComando(aux);
        memset(input,0,400);
    }
}

void imprimirEncabezado()
{
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "|                           Sistema de archivos                             |" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;
    cout << "                                                       Raul Xiloj - 201612113" << endl;
    cout << endl;
    cout << "Por favor escriba algunos comandos:" << endl;
}

/*
 * Analisis semantico
 * Metodos para recorrer el arbol que genera cada comando y
 * obtener sus valores
*/
void reconocerComando(Nodo *raiz)
{
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
    {
        Nodo n = raiz->hijos.at(0);
        recorrerMOUNT(&n);
    }
        break;
    case EXEC:
    {
        recorrerEXEC(raiz);
    }
        break;
    case COMENTARIO:
    {

    }
        break;
    default: printf("Error no se reconoce el comando");

    }

}

void recorrerMKDISK(Nodo *raiz)
{
    /*Banderas para verificar cuando venga un parametro y si se repite*/
    bool flagSize = false;
    bool flagFit = false;
    bool flagUnit = false;
    bool flagPath = false;
    bool flag = false;//Si se repite un valor se activa esta bandera
    /*Variables para obtener los valores de cada nodo*/
    int valSize = 0;
    char valFit = 0;
    char valUnit = 0;
    QString valPath = "";

    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        Nodo n = raiz->hijos.at(i);
        switch (n.tipo_) {
        case SIZE:
        {
            if(flagSize){
                cout << "ERROR: Parametro -size ya definido" << endl;
                flag = true;
                break; //ERROR
            }
            flagSize = true;
            valSize = n.valor.toInt();
            if(!(valSize > 0)){
                flag = true;
                cout << "ERROR: Parametro -size menor a cero" << endl;
                break;
            }
        }
            break;
        case FIT:
        {
            if(flagFit){
                cout << "ERROR: Parametro -fit ya definido" << endl;
                flag = true;
                break; //ERROR
            }
            flagFit = true;
            valFit = n.valor.toStdString()[0];
            if(valFit == 'b'){
                valFit = 'B';
            }else if(valFit == 'f'){
                valFit = 'F';
            }else if(valFit == 'w'){
                valFit = 'W';
            }
        }
            break;
        case UNIT:
        {
            if(flagUnit){
                cout << "ERROR: Parametro -unit ya definido" << endl;
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
                flag = true;
                break;
            }
        }
            break;
        case PATH:
        {
            if(flagPath){
                cout << "ERROR: Parametro -path ya definido" << endl;
                flag = true;
                break;
            }
            flagPath = true;
            valPath = n.valor;//Quitarle comillas si tiene
            valPath = valPath.replace("\"","");
        }
            break;
        }

    }

    if(!flag){//Flag para ver si hay parametros repetidos
        if(flagPath){//Parametro path obligatorio
            if(flagSize){//Parametro size obligatorio
                MBR masterboot;
                int total_size = 1024;
                crearArchivo(valPath);
                masterboot.mbr_date_created = time(nullptr);//-------
                masterboot.mbr_disk_signature = (int)time(nullptr);//--------

                if(flagUnit){//Si hay parametro unit
                    if(valUnit == 'm'){
                        masterboot.mbr_size = valSize*1048576;
                        total_size = valSize * 1024;
                    }else{
                        masterboot.mbr_size = valSize * 1024;
                        total_size = valSize;
                    }
                }else{
                    masterboot.mbr_size = valSize*1048576;
                    total_size = valSize * 1024;
                }

                if(flagFit)//Si hay parametro fit
                    masterboot.mbr_disk_fit = valFit;
                else
                    masterboot.mbr_disk_fit = 'F';

                //Se inicializan las particiones en el MBR
                for(int p = 0; p < 4; p++){
                    masterboot.mbr_partition[p].part_status = 0;
                    masterboot.mbr_partition[p].part_type = 0;
                    masterboot.mbr_partition[p].part_fit = 0;
                    masterboot.mbr_partition[p].part_size = 0;
                    masterboot.mbr_partition[p].part_start = -1;
                    strcpy(masterboot.mbr_partition[p].part_name,"");
                }

                //Comando para genera un archivo de cierto tamano
                string comando = "dd if=/dev/zero of=\""+valPath.toStdString()+"\" bs=1024 count="+to_string(total_size);
                system(comando.c_str());
                FILE *fp = fopen(valPath.toStdString().c_str(),"rb+");
                fseek(fp,0,SEEK_SET);
                fwrite(&masterboot,sizeof(MBR),1,fp);
                fclose(fp);
            }else{
                cout << "ERROR Parametro -size no definido " << endl;
            }

        }else{
            cout << "ERROR Parametro -path  no definido" << endl;
        }
    }

}

void recorrerRMDISK(Nodo *raiz)
{
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
            cout << "Disco eliminado con exito" << endl;
        }else if(opcion.compare("N") || opcion.compare("n") == 0){
            cout << "Cancelado con exito" << endl;;
        }else{
            cout << "Opcion incorrecta" << endl;
        }
        fclose(fp);
    }else{
        cout << "No existe el disco a eliminar" << endl;
    }
}

void recorrerFDISK(Nodo *raiz)
{
    /*Banderas para verificar cuando venga un parametro y si se repite*/
    bool flagSize = false;
    bool flagUnit = false;
    bool flagPath = false;
    bool flagType = false;
    bool flagFit = false;
    bool flagDelete = false;
    bool flagName = false;
    bool flagAdd = false;
    bool flag = false;
    /*Variables para obtener los valores de cada nodo*/
    int valSize = 0;
    int valAdd = 0;
    char valUnit = 0;
    char valType = 0;
    char valFit = 0;
    QString valPath = "";
    QString valName = "";
    QString valDelete = "";

    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        Nodo n = raiz->hijos.at(i);
        switch (n.tipo_)
        {
        case SIZE:
        {
            if(flagSize){
                cout << "ERROR: Parametro -size ya definido" << endl;
                flag = true;
                break;
            }
            flagSize = true;
            valSize = n.valor.toInt();
            if(!(valSize > 0)){
                cout << "ERROR: parametro -size menor a cero" << endl;
                flag = true;
                break;
            }
        }
            break;
        case UNIT:
        {
            if(flagUnit){
                cout << "ERROR: Parametro -unit ya definido" << endl;
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
                cout << "ERROR: Parametro -path ya definido" << endl;
                flag = true;
                break;
            }
            flagPath = true;
            valPath = n.valor;
            valPath = valPath.replace("\"","");
        }
            break;
        case TYPE:
        {
            if(flagType){
                cout << "ERROR: Parametro -type ya definido" << endl;
                flag = true;
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
                cout << "ERROR: Valor del parametro -type no reconocido" << endl;
                flag = true;
                break;
            }
        }
            break;
        case FIT:
        {
            if(flagFit)
            {
                cout << "ERROR: Parametro -fit ya definido" << endl;
                flag = true;
                //ERROR
            }
            flagFit = true;
            valFit = n.hijos.at(0).valor.toStdString()[0];
            if(valFit == 'b'){
                valFit = 'B';
            }else if(valFit == 'f'){
                valFit = 'F';
            }else if(valFit == 'w'){
                valFit = 'W';
            }
        }
            break;
        case DELETE:
        {
            if(flagDelete){
                cout << "ERROR: Parametro -delete ya definido" << endl;
                flag = true;
                break;
            }
            flagDelete = true;
            valDelete = n.valor;
        }
            break;
        case NAME:
        {
            if(flagName){
                cout << "ERROR: Parametro -name ya definido" << endl;
                flag = true;
                break;
            }
            flagName = true;
            valName = n.valor;
            valName = valName.replace("\"", "");
        }
            break;
        case ADD:
        {
            if(flagAdd){
                cout << "ERROR: Parametro -add ya definido" << endl;
                flag = true;
                break;
            }
            flagAdd = true;
            valAdd = n.valor.toInt();
        }
            break;
        }
    }

    if(!flag){//Flag para ver si hay parametros repetidos
        if(flagPath){//Parametro obligatorio
            if(flagName){//Parametro obligatorio
                if(flagSize){//Parametro obligatorio al crear una particion
                    if(flagDelete || flagAdd){
                        cout << "ERROR: Parametro -delete|-add demas" << endl;
                    }else{
                        if(flagType){//Si especifica tipo de particion
                            if(valType == 'P'){
                                crearParticionPrimaria(valPath, valName, valSize, valFit, valUnit);
                            }else if(valType == 'E'){
                                crearParticionExtendida(valPath, valName,valSize, valFit, valUnit);
                            }else if(valType == 'L'){
                                crearParticionLogica(valPath,valName,valSize,valFit,valUnit);
                            }
                        }else{//Si no especifica se considera particion primaria
                            crearParticionPrimaria(valPath, valName, valSize, valFit, valUnit);
                        }
                    }
                }else if(flagAdd){
                    if(flagSize || flagDelete){
                        cout << "ERROR: Parametros -size|-delete demas" << endl;
                    }else{
                        if(flagUnit){
                            agregarQuitarParticion(valPath,valName,valAdd,valUnit);
                        }else{
                            cout << "ERROR parametro -unit no definido "<< endl;
                        }
                    }
                }else if(flagDelete){
                    if(flagSize || flagAdd || flagFit || flagType){
                        cout << "ERROR: Parametros demas" << endl;
                    }else{
                        eliminarParticion(valPath,valName,valDelete);
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
    QString valPath = "";
    QString valName = "";
    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        Nodo n = raiz->hijos.at(i);
        switch(n.tipo_)
        {
        case PATH:
        {
            if(flagPath){
                flag = true;
                cout << "ERROR: Parametro -path ya definido" << endl;
                break;
            }
            flagPath = true;
            valPath = n.valor;
        }
            break;
        case NAME:
        {
            if(flagName){
                flag = true;
                cout << "ERROR: Parametro -name ya definido" << endl;
                break;
            }
            flagName = true;
            valName = n.valor;
            valName = valName.replace("\"","");
        }
            break;
        }
    }

    if(!flag){
        if(flagPath){//Parametro obligatorio
            if(flagName){//Parametro obligtaorio
                int indexP = buscarParticion_P_E(valPath,valName);
                if(indexP != -1){
                    FILE *fp;
                    if((fp = fopen(valPath.toStdString().c_str(),"rb+"))){
                        MBR masterboot;
                        fseek(fp, 0, SEEK_SET);
                        fread(&masterboot, sizeof(MBR),1,fp);
                        masterboot.mbr_partition[indexP].part_status = '2';
                        fseek(fp,0,SEEK_SET);
                        fwrite(&masterboot,sizeof(MBR),1,fp);
                        fclose(fp);
                        int letra = lista->buscarLetra(valPath,valName);
                        if(letra != -1){
                            cout << "ERROR la particion ya esta montada" << endl;
                        }else{
                            int num = lista->buscarNumero(valPath, valName);
                            char auxLetra = static_cast<char>(letra);
                            string id = "vd";
                            id += auxLetra + to_string(num);
                            NodoMount *n = new NodoMount(valPath,valName,auxLetra,num);
                            lista->insertarNodo(n);
                            cout << "Particion " + valName.toStdString() + "montada con exito" << endl;
                        }
                    }else{
                        cout << "ERROR no se encuentra el disco" << endl;
                    }
                }else{

                }

            }else{
                cout << "ERROR parametro -name no definido" << endl;
            }
        }else{
            cout << "ERROR parametro -path no definido" << endl;
        }
    }

}

void recorrerUNMOUNT(Nodo *raiz){
    QString valID = raiz->hijos.at(0).valor;
    int eliminado = lista->eliminarNodo(valID);
    if(eliminado == 1)
        cout << "Unidad desmontada con exito" << endl;
    else
        cout << "ERROR no se encuentra esa unidad montada" << endl;
}

void recorrerREP(Nodo *raiz)
{
    bool flagName = false;
    bool flagPath = false;
    bool flagID = false;
    bool flag = false;
    QString valName = "";
    QString valPath = "";
    QString valID = "";

    for(int i = 0; i < raiz->hijos.count(); i++)
    {
        Nodo n = raiz->hijos.at(i);
        switch (n.tipo_)
        {
        case NAME:
        {
            if(flagName){
                cout << "ERROR parametro -name ya definido" << endl;
                flag = true;
                break;
            }
            flagName = true;
            valName = n.valor;
        }
            break;
        case PATH:
        {
            if(flagPath){
                cout << "ERROR parametro -path ya definido" << endl;
                flag = true;
                break;
            }
            flagPath = true;
            valPath = n.valor;
        }
            break;
        case ID:
        {
            if(flagID){
                cout << "ERROR parametro -id ya definido" << endl;
                flag = true;
                break;
            }
            flagID = true;
            valID = n.valor;
        }
            break;
        }

    }

    if(!flag){
        if(flagPath){
            if(flagName){
                if(flagID){
                    QString direccion = lista->getDireccion(valID);
                    QString ext = getExtension(valPath);
                    if(direccion != "null"){
                        QString directorio = getDirectorio(valPath);
                        string comando = "sudo mkdir -p \'"+directorio.toStdString()+"\'";
                        system(comando.c_str());
                        string comando2 = "sudo chmod -R 777 \'"+directorio.toStdString()+"\'";
                        system(comando2.c_str());
                        if(valName == "mbr"){
                            //graficarMBR();
                        }else{
                            graficarDisco(direccion,valPath,ext);
                        }
                    }else{
                        cout << "ERROR no se encuentra la particion" << endl;
                    }
                }else{
                    cout << "ERROR parametro -id no definido" << endl;
                }
            }else{
                cout << "ERROR parametro -name no definido" << endl;
            }
        }else{
            cout << "ERROR parametro -path no definido" << endl;
        }
    }


}

void recorrerEXEC(Nodo *raiz)
{
    QString valPath = raiz->hijos.at(0).valor;
    string auxPath = valPath.toStdString();
    string line;
    ifstream myfile(auxPath);
    if(myfile.is_open()){
        while(getline(myfile,line)){
            cout << line << endl;
            leerComando(line);
        }
        myfile.close();
    }else{
        cout << "ERROR no se encuentra el archivo" << endl;
    }
}

/*
 * Metodo que creara el directorio de carpetas hacia el archivo
 * dara permisos en las carpetas y creara el archivo.
 *@param QString direccion, ruta en donde se creara el archivo
*/
void crearArchivo(QString direccion){
    QString aux = getDirectorio(direccion);
    string comando = "sudo mkdir -p \'"+aux.toStdString()+"\'";
    system(comando.c_str());
    string comando2 = "sudo chmod -R 777 \'"+aux.toStdString()+"\'";
    system(comando2.c_str());
    string arch = direccion.toStdString();
    FILE *fp = fopen(arch.c_str(),"wb");
    if((fp = fopen(arch.c_str(),"wb")))
        fclose(fp);
    else
        cout << "Error al crear el archivo" << endl;
}

/* Funcion que devuleve el directorio en donde se creara el archivo
 * @param QString direccion: ruta donde se creara el archivo
*/
QString getDirectorio(QString direccion){
    string aux = direccion.toStdString();
    string delimiter = "/";
    size_t pos = 0;
    string res = "";
    while((pos = aux.find(delimiter))!=string::npos){
        res += aux.substr(0,pos)+"/";
        aux.erase(0,pos + delimiter.length());
    }
    return QString::fromStdString(res);
}

/* Metodo que se encarga de crear una particion primaria
 * @param QString direccion: direccion del disco
 * @param QString nombre: nombre de la particion
 * @param int size: tamano de la particion
 * @param char fit: ajuste de la particion
 * @param char unit: unidades del tamano (M K B)
 */
void crearParticionPrimaria(QString direccion, QString nombre, int size, char fit, char unit){
    char auxFit = 0;
    char auxUnit = 0;
    string auxPath = direccion.toStdString();
    int size_bytes = 1024;
    char buffer = '1';

    if(fit != 0)
        auxFit = fit;
    else
        auxFit = 'W';

    if(unit != 0){
        auxUnit = unit;
        if(auxUnit == 'b'){
            size_bytes = size;
        }else if(auxUnit == 'k'){
            size_bytes = size * 1024;
        }else{
            size_bytes = size*1024*1024;
        }
    }else{
        size_bytes = size * 1024;
    }

    FILE *fp;
    MBR masterboot;

    if((fp = fopen(auxPath.c_str(), "rb+"))){
        bool flagParticion = false;//Flag para ver si hay una particion disponible
        int numParticion = 0;//Que numero de particion es
        fseek(fp,0,SEEK_SET);
        fread(&masterboot,sizeof(MBR),1,fp);
        //Verificar si existe una particion disponible
        for(int i = 0; i < 4; i++){
            if(masterboot.mbr_partition[i].part_start == -1 || (masterboot.mbr_partition[i].part_status == 1 && masterboot.mbr_partition[i].part_size>=size_bytes)){
                flagParticion = true;
                numParticion = i;
                break;
            }
        }

        if(flagParticion){
            //Verificar el espacio libre del disco
            int espacioUsado = 0;
            for(int i = 0; i < 4; i++){
                espacioUsado += masterboot.mbr_partition[i].part_size;
            }
            cout << "Espacio disponible: " << (masterboot.mbr_size - espacioUsado) << " Bytes" << endl;
            cout << "Espacio necesario:  " << size_bytes << " Bytes" << endl;
            //Verificar que haya espacio suficiente para crear la particion
            if((masterboot.mbr_size - espacioUsado) >= size_bytes){
                if(!existeParticion(direccion,nombre)){
                    if(masterboot.mbr_disk_fit == 'F'){
                        masterboot.mbr_partition[numParticion].part_type = 'P';
                        masterboot.mbr_partition[numParticion].part_fit = auxFit;
                        //start
                        if(numParticion == 0){
                            masterboot.mbr_partition[numParticion].part_start = sizeof(masterboot);
                        }else{
                            masterboot.mbr_partition[numParticion].part_start = masterboot.mbr_partition[numParticion-1].part_start + masterboot.mbr_partition[numParticion-1].part_size;
                        }
                        masterboot.mbr_partition[numParticion].part_size = size_bytes;
                        masterboot.mbr_partition[numParticion].part_status = 0;
                        strcpy(masterboot.mbr_partition[numParticion].part_name,nombre.toStdString().c_str());
                        //Se guarda de nuevo el MBR
                        fseek(fp,0,SEEK_SET);
                        fwrite(&masterboot,sizeof(MBR),1,fp);
                        //Se guardan los bytes de la particion
                        fseek(fp,masterboot.mbr_partition[numParticion].part_start,SEEK_SET);
                        for(int i = 0; i < size_bytes; i++){
                            fwrite(&buffer,1,1,fp);
                        }
                        cout << "..." << endl;
                        cout << "Particion primaria creada con exito" << endl;
                    }else if(masterboot.mbr_disk_fit == 'B'){

                    }else if(masterboot.mbr_disk_fit == 'W'){

                    }
                }else{
                    cout << "ERROR ya existe una particion con ese nombre" << endl;
                }

            }else{
                cout << "ERROR la particion a crear excede el espacio libre" << endl;
            }
        }else{
            cout << "ERROR: Ya existen 4 particiones, no se puede crear otra" << endl;
            cout << "Elimine alguna para poder crear una" << endl;
        }
    fclose(fp);
    }else{
        cout << "ERROR no existe el disco" << endl;
    }
}

/* Metodo que se encarga de crear una particion extendida
 * @param QString direccion: direccion del disco
 * @param QString nombre: nombre de la particion
 * @param int size: tamano de la particion
 * @param char fit: ajuste de la particion
 * @param char unit: unidades del tamano (M K B)
*/
void crearParticionExtendida(QString direccion, QString nombre, int size, char fit, char unit){
    char auxFit = 0;
    char auxUnit = 0;
    string auxPath = direccion.toStdString();
    int size_bytes = 1024;
    char buffer = '1';

    if(fit != 0)
        auxFit = fit;
    else
        auxFit = 'W';

    if(unit != 0){
        auxUnit = unit;
        if(auxUnit == 'b'){
            size_bytes = size;
        }else if(auxUnit == 'k'){
            size_bytes = size * 1024;
        }else{
            size_bytes = size*1024*1024;
        }
    }else{
        size_bytes = size * 1024;
    }

    FILE *fp;
    MBR masterboot;
    if((fp = fopen(auxPath.c_str(), "rb+"))){
        bool flagParticion = false;//Flag para ver si hay una particion disponible
        bool flagExtendida = false;//Flag para ver si ya hay una particion extendida
        int numParticion = 0;//Que numero de particion es
        fseek(fp,0,SEEK_SET);
        fread(&masterboot,sizeof(MBR),1,fp);
        for(int i = 0; i < 4; i++){
            if (masterboot.mbr_partition[i].part_type == 'E'){
                flagExtendida = true;
                break;
            }
        }
        if(!flagExtendida){
            //Verificar si existe una particion disponible
            for(int i = 0; i < 4; i++){
                if(masterboot.mbr_partition[i].part_start == -1 || (masterboot.mbr_partition[i].part_status == 1 && masterboot.mbr_partition[i].part_size>=size_bytes)){
                    flagParticion = true;
                    numParticion = i;
                    break;
                }
            }
            if(flagParticion){
                //Verificar el espacio libre del disco
                int espacioUsado = 0;
                for(int i = 0; i < 4; i++){
                    espacioUsado += masterboot.mbr_partition[i].part_size;
                }
                cout << "Espacio disponible: " << (masterboot.mbr_size - espacioUsado) <<" Bytes"<< endl;
                cout << "Espacio necesario:  " << size_bytes << " Bytes" << endl;
                //Verificar que haya espacio suficiente para crear la particion
                if((masterboot.mbr_size - espacioUsado) >= size_bytes){
                    if(!(existeParticion(direccion,nombre))){
                        if(masterboot.mbr_disk_fit == 'F'){
                            masterboot.mbr_partition[numParticion].part_type = 'E';
                            masterboot.mbr_partition[numParticion].part_fit = auxFit;
                            //start
                            if(numParticion == 0){
                                masterboot.mbr_partition[numParticion].part_start = sizeof(masterboot);
                            }else{
                                masterboot.mbr_partition[numParticion].part_start =  masterboot.mbr_partition[numParticion-1].part_start + masterboot.mbr_partition[numParticion-1].part_size;
                            }
                            masterboot.mbr_partition[numParticion].part_size = size_bytes;
                            masterboot.mbr_partition[numParticion].part_status = 0;
                            strcpy(masterboot.mbr_partition[numParticion].part_name,nombre.toStdString().c_str());
                            //Se guarda de nuevo el MBR
                            fseek(fp,0,SEEK_SET);
                            fwrite(&masterboot,sizeof(MBR),1,fp);
                            //Se guarda la particion extendida
                            fseek(fp, masterboot.mbr_partition[numParticion].part_start,SEEK_SET);
                            EBR extendedBoot;
                            extendedBoot.part_fit = auxFit;
                            extendedBoot.part_status = 0;
                            extendedBoot.part_start = masterboot.mbr_partition[numParticion].part_start;
                            extendedBoot.part_size = 0;
                            extendedBoot.part_next = -1;
                            strcpy(extendedBoot.part_name, "");
                            fwrite(&extendedBoot,sizeof (EBR),1,fp);
                            for(int i = 0; i < (size_bytes - (int)sizeof(EBR)); i++){
                                fwrite(&buffer,1,1,fp);
                            }
                            cout << "..." << endl;
                            cout << "Particion extendida creada con exito" << endl;
                        }else if(masterboot.mbr_disk_fit == 'B'){

                        }else if(masterboot.mbr_disk_fit == 'W'){

                        }
                    }else{
                        cout << "ERROR ya existe una particion con ese nombre" << endl;
                    }
                }else{
                    cout << "ERROR la particion a crear excede el tamano libre" << endl;
                }
            }else{
                cout << "ERROR: Ya existen 4 particiones, no se puede crear otra" << endl;
                cout << "Elimine alguna para poder crear una" << endl;
            }
        }else{
            cout << "ERROR ya existe una particion extendida en este disco" << endl;
        }
    fclose(fp);
    }else{
        cout << "ERROR no existe el disco" << endl;
    }
}

/* Metodo que se encarga de crear una particion logica
 * @param QString direccion: direccion del disco
 * @param QString nombre: nombre de la particion
 * @param int size: tamano de la particion
 * @param char fit: ajuste de la particion
 * @param char unit: unidades del tamano (M K B)
*/
void crearParticionLogica(QString direccion, QString nombre, int size, char fit, char unit){
    char auxFit = 0;
    char auxUnit = 0;
    string auxPath = direccion.toStdString();
    int size_bytes = 1024;
    char buffer = '1';

    if(fit != 0)
        auxFit = fit;
    else
        auxFit = 'W';
    if(unit != 0){
        auxUnit = unit;
        if(auxUnit == 'b'){
            size_bytes = size;
        }else if(auxUnit == 'k'){
            size_bytes = size * 1024;
        }else{
            size_bytes = size*1024*1024;
        }
    }else{
        size_bytes = size * 1024;
    }

    FILE *fp;
    MBR masterboot;
    if((fp = fopen(auxPath.c_str(), "rb+"))){
        int numExtendida = -1;
        fseek(fp,0,SEEK_SET);
        fread(&masterboot,sizeof(MBR),1,fp);
        //Verificar si existe una particion extendida
        for(int i = 0; i < 4; i++){
            if(masterboot.mbr_partition[i].part_type == 'E'){
                numExtendida = i;
                break;
            }
        }
        if(numExtendida != -1){
            EBR extendedBoot;
            int cont = masterboot.mbr_partition[numExtendida].part_start;
            fseek(fp,cont,SEEK_SET);
            fread(&extendedBoot, sizeof(EBR),1,fp);
            if(extendedBoot.part_size == 0){//Si es la primera
                if(masterboot.mbr_partition[numExtendida].part_size < size_bytes){
                    cout << "ERROR la particion logica a crear excede el " << endl;
                    cout << "espacio disponible de la particion extendida" << endl;
                }else{
                    extendedBoot.part_status = 0;
                    extendedBoot.part_status = auxFit;
                    extendedBoot.part_start = ftell(fp) - sizeof(EBR); //Para regresar al inicio de la extendida
                    extendedBoot.part_size = size_bytes;
                    extendedBoot.part_next = -1;
                    strcpy(extendedBoot.part_name, nombre.toStdString().c_str());
                    fseek(fp, masterboot.mbr_partition[numExtendida].part_start ,SEEK_SET);
                    fwrite(&extendedBoot,sizeof(EBR),1,fp);
                    cout << "Particion logica creada con exito "<< endl;
                }
            }else{
                while((extendedBoot.part_next != -1) && (ftell(fp) < (masterboot.mbr_partition[numExtendida].part_size + masterboot.mbr_partition[numExtendida].part_start))){
                    fseek(fp,extendedBoot.part_next,SEEK_SET);
                    fread(&extendedBoot,sizeof(EBR),1,fp);
                }
                int espacioNecesario = extendedBoot.part_start + extendedBoot.part_size + size_bytes;
                if(espacioNecesario <= (masterboot.mbr_partition[numExtendida].part_size + masterboot.mbr_partition[numExtendida].part_start)){
                    extendedBoot.part_next = extendedBoot.part_start + extendedBoot.part_size;
                    //Escribimos el next del ultimo EBR
                    fseek(fp,ftell(fp) - sizeof (EBR),SEEK_SET);
                    fwrite(&extendedBoot, sizeof(EBR),1 ,fp);
                    fseek(fp,extendedBoot.part_start + extendedBoot.part_size, SEEK_SET);
                    extendedBoot.part_status = 0;
                    extendedBoot.part_fit = auxFit;
                    extendedBoot.part_start = ftell(fp);
                    extendedBoot.part_size = size_bytes;
                    extendedBoot.part_next = -1;
                    strcpy(extendedBoot.part_name,nombre.toStdString().c_str());
                    fwrite(&extendedBoot,sizeof(EBR),1,fp);
                    cout << "Particion logica creada con exito "<< endl;
                }else{

                    cout << "ERROR la particion logica a crear excede el" << endl;
                    cout << "espacio disponible de la particion extendida" << endl;
                }
            }
        }else{
            cout << "ERROR se necesita una particion extendida donde guardar la logica " << endl;
        }
    fclose(fp);
    }else{
        cout << "ERROR no existe el disco" << endl;
    }

}

/* Funcion que devuelve un booleano para comprobar la existencia del nombre de una particion en un disco
 * @param QString direccion: ruta del archivo
 * @param QString nombre: nombre de la particion
 * @return true = si existe | false = si no existe
*/
bool existeParticion(QString direccion, QString nombre){
    int extendida = -1;
    FILE *fp;
    if((fp = fopen(direccion.toStdString().c_str(),"rb+"))){
        MBR masterboot;
        fseek(fp,0,SEEK_SET);
        fread(&masterboot, sizeof(MBR),1,fp);
        for(int i = 0; i < 4; i++){
            if(strcmp(masterboot.mbr_partition[i].part_name,nombre.toStdString().c_str()) == 0){
                fclose(fp);
                return true;
            }else if(masterboot.mbr_partition[i].part_type == 'E'){
                extendida = i;
            }
        }
        if(extendida != -1){
            fseek(fp, masterboot.mbr_partition[extendida].part_start,SEEK_SET);
            EBR extendedBoot;
            while((fread(&extendedBoot,sizeof(EBR),1,fp))!=0 && (ftell(fp) < (masterboot.mbr_partition[extendida].part_size + masterboot.mbr_partition[extendida].part_start))){
                if(strcmp(extendedBoot.part_name,nombre.toStdString().c_str()) == 0){
                    fclose(fp);
                    return true;
                }
                if(extendedBoot.part_next == -1){
                    fclose(fp);
                    return false;
                }
            }
        }
    }
    fclose(fp);
    return false;
}

/* Metodo que se encarga de eliminar una particion de cualquier tipo
 * @param QString direccion: ruta del archivo
 * @param QString nombre: nombre de la particion
 * @param valD: tipo de eliminacion fast|full
*/
void eliminarParticion(QString direccion, QString nombre, QString typeDelete){
    string auxPath = direccion.toStdString();
    string auxNombre = nombre.toStdString();
    FILE *fp;
    if((fp = fopen(auxPath.c_str(), "rb+"))){
        //Verificar que la particion no este montada
        bool mount = lista->buscarNodo(direccion,nombre);
        if(!mount){
            MBR masterboot;
            fseek(fp,0,SEEK_SET);
            fread(&masterboot,sizeof (MBR),1,fp);
            int index = -1;
            int index_Extendida = -1;
            bool flagExtendida = false;
            string opcion = "";
            char buffer = '\0';
            //Buscamos la particion primaria/extendida
            for(int i = 0; i < 4; i++){
                if((strcmp(masterboot.mbr_partition[i].part_name, auxNombre.c_str())) == 0){
                    index = i;
                    if(masterboot.mbr_partition[i].part_type == 'E')
                        flagExtendida = true;
                    break;
                }else if(masterboot.mbr_partition[i].part_type == 'E'){
                    index_Extendida = i;
                }
            }
            cout << "¿Seguro que desea eliminar la particion? Y/N : " ;
            getline(cin, opcion);
            if(opcion.compare("Y") == 0 || opcion.compare("y") == 0){
                if(index != -1){//Si se encontro en las principales
                    if(!flagExtendida){//primaria
                        if(typeDelete == "fast"){
                            masterboot.mbr_partition[index].part_status = 1;
                            strcpy(masterboot.mbr_partition[index].part_name,"");
                            fseek(fp,0,SEEK_SET);
                            fwrite(&masterboot,sizeof(MBR),1,fp);
                        }else{//full
                            masterboot.mbr_partition[index].part_status = 1;
                            strcpy(masterboot.mbr_partition[index].part_name,"");
                            fseek(fp,0,SEEK_SET);
                            fwrite(&masterboot,sizeof(MBR),1,fp);
                            fseek(fp,masterboot.mbr_partition[index].part_start,SEEK_SET);
                            fwrite(&buffer,1,masterboot.mbr_partition[index].part_size,fp);
                            cout << "Particion primaria eliminada con exito" << endl;
                        }
                    }else{//extendida
                        if(typeDelete == "fast"){
                            masterboot.mbr_partition[index].part_status = 1;
                            strcpy(masterboot.mbr_partition[index].part_name,"");
                            fseek(fp,0,SEEK_SET);
                            fwrite(&masterboot,sizeof(MBR),1,fp);
                        }else{//full
                            masterboot.mbr_partition[index].part_status = 1;
                            strcpy(masterboot.mbr_partition[index].part_name,"");
                            fseek(fp,0,SEEK_SET);
                            fwrite(&masterboot,sizeof(MBR),1,fp);
                            fseek(fp,masterboot.mbr_partition[index].part_start,SEEK_SET);
                            fwrite(&buffer,1,masterboot.mbr_partition[index].part_size,fp);
                            cout << "Particion extendida eliminada con exito" << endl;
                        }
                    }
                }else{//Si es una particion logica
                    if(index_Extendida != -1){
                        bool flag = false;//Bandera para saber si existe
                        EBR extendedBoot;
                        fseek(fp,masterboot.mbr_partition[index_Extendida].part_start, SEEK_SET);
                        fread(&extendedBoot,sizeof(EBR),1,fp);
                        if(extendedBoot.part_size!=0){
                            fseek(fp, masterboot.mbr_partition[index_Extendida].part_start,SEEK_SET);
                            while((fread(&extendedBoot,sizeof(EBR),1,fp))!=0 && (ftell(fp) < (masterboot.mbr_partition[index_Extendida].part_start + masterboot.mbr_partition[index_Extendida].part_size))) {
                                if(strcmp(extendedBoot.part_name,nombre.toStdString().c_str()) == 0 && extendedBoot.part_status != 1){
                                    flag = true;
                                    break;
                                }else if(extendedBoot.part_next == -1){//Ya es la ultima y no se encontro
                                    break;
                                }
                            }
                        }
                        if(flag){
                            if(typeDelete == "fast"){
                                extendedBoot.part_status = 1;
                                strcpy(extendedBoot.part_name, "");
                                fseek(fp, ftell(fp)-sizeof(EBR),SEEK_SET);
                                fwrite(&extendedBoot,sizeof(EBR),1,fp);
                            }else{//full
                                extendedBoot.part_status = 1;
                                strcpy(extendedBoot.part_name, "");
                                fseek(fp, ftell(fp)-sizeof(EBR),SEEK_SET);
                                fwrite(&extendedBoot,sizeof(EBR),1,fp);
                                fwrite(&buffer,1,extendedBoot.part_size,fp);
                                cout << "Particion logica eliminada con exito" << endl;
                            }
                        }else{
                            cout << "ERROR no se encuentra la particion a eliminar" << endl;
                        }
                    }else{
                        cout << "ERROR no se encuentra la particion a eliminar" << endl;
                    }
                }
            }else if(opcion.compare("N") || opcion.compare("n") == 0){
                cout << "Cancelado con exito" << endl;;
            }else{
                cout << "Opcion incorrecta" << endl;
            }
        }else{
            cout << "ERROR desmote primero la particion para poder eliminarlo" << endl;
        }
    fclose(fp);
    }else{
        cout << "ERROR el disco donde se va a eliminar no existe" << endl;
    }
}

/* Metodo que se encarga de agregar o quitar espacio a una particion
 * @param QString direccion: ruta del archivo
 * @param QString nombre: nombre de la particion
 * @param units: unidades para agregar o quitar
*/
void agregarQuitarParticion(QString direccion, QString nombre, int add, char unit){
    string auxPath = direccion.toStdString();
    string auxNombre = nombre.toStdString();
    int size_Bytes = 0;
    QString tipo = "";

    if(add < 0)
        tipo = "add";

    if(tipo != "add")
        add = add*(-1);

    if(unit == 'm')
        size_Bytes = add * 1024 * 1024;
    else if(unit == 'k')
        size_Bytes = add * 1024;
    else
        size_Bytes = add;

    FILE *fp;
    if((fp = fopen(auxPath.c_str(), "rb+"))){
        //Verificar que la particion no este montada
        bool mount = lista->buscarNodo(direccion,nombre);
        if(!mount){
            MBR masterboot;
            fseek(fp,0,SEEK_SET);
            fread(&masterboot,sizeof(MBR),1,fp);
            int index = -1;
            int index_Extendida = -1;
            bool flagExtendida = false;
            //Buscamos la particion primaria/extendida
            for(int i = 0; i < 4; i++){
                if((strcmp(masterboot.mbr_partition[i].part_name, auxNombre.c_str())) == 0){
                    index = i;
                    if(masterboot.mbr_partition[i].part_type == 'E')
                        flagExtendida = true;
                    break;
                }else if(masterboot.mbr_partition[i].part_type == 'E'){
                    index_Extendida = i;
                }
            }
            if(index != -1){//Si se encontro en las principales
                if(!flagExtendida){//Primaria
                    if(tipo == "add"){//Agregar
                        //Verificar que exista espacio libre a la derecha
                        if(index == 3){
                            //No se puede
                        }else{
                            if(masterboot.mbr_partition[index + 1].part_status == 1){
                                if(masterboot.mbr_partition[index + 1].part_size >= size_Bytes){
                                    masterboot.mbr_partition[index].part_size = masterboot.mbr_partition[index].part_size + size_Bytes;
                                    masterboot.mbr_partition[index + 1].part_size = (masterboot.mbr_partition[index + 1].part_size - size_Bytes);
                                    masterboot.mbr_partition[index + 1].part_start = masterboot.mbr_partition[index + 1].part_start + size_Bytes;
                                    fseek(fp,0,SEEK_SET);
                                    fwrite(&masterboot,sizeof(MBR),1,fp);
                                    cout << "Se agrego espacio a la particion de manera exitosa" << endl;
                                }else{
                                    cout << "ERROR no es posible agregar espacio a la particion porque no hay suficiente espacio disponible a su derecha" << endl;
                                }
                            }else{
                                cout << "ERROR no es posible agregar espacio a la particion porque no hay espacio disponible a su derecha" << endl;
                            }
                        }
                    }else{//Quitar

                    }
                }else{//Extendida
                    if(tipo == "add"){//Agregar
                        //Verificar que exista espacio libre a la derecha
                        if(index == 3){
                            //No se puede
                        }else{
                            if(masterboot.mbr_partition[index + 1].part_status == 1){
                                if(masterboot.mbr_partition[index + 1].part_size >= size_Bytes){
                                    masterboot.mbr_partition[index].part_size = masterboot.mbr_partition[index].part_size + size_Bytes;
                                    masterboot.mbr_partition[index + 1].part_size = (masterboot.mbr_partition[index + 1].part_size - size_Bytes);
                                    masterboot.mbr_partition[index + 1].part_start = masterboot.mbr_partition[index + 1].part_start + size_Bytes;
                                    fseek(fp,0,SEEK_SET);
                                    fwrite(&masterboot,sizeof(MBR),1,fp);
                                    cout << "Se agrego espacio a la particion de manera exitosa" << endl;
                                }else{
                                    cout << "ERROR no es posible agregar espacio a la particion porque no hay suficiente espacio disponible a su derecha" << endl;
                                }
                            }else{
                                cout << "ERROR no es posible agregar espacio a la particion porque no hay espacio disponible a su derecha" << endl;
                            }
                        }
                    }else{//Quitar

                    }
                }
            }else{//Posiblemente logica
                if(index_Extendida != -1){

                }else{
                    cout << "ERROR no se encuentra la particion a redimensionar" << endl;
                }

            }
        }else{
             cout << "ERROR desmote primero la particion para poder redimensionar" << endl;
        }
    fclose(fp);
    }else{
        cout << "ERROR el disco donde se desea agregar/quitar unidades no existe" << endl;
    }
}

/* Funcion que devuelve un entero para comprobar la existencia de una particion primaria o extendida en un disco
 * @param QString direccion: ruta del archivo
 * @param QString nombre: nombre de la particion
 * @return -1 = no se encuentra | # de la particion asociada a ese nombre
*/
int buscarParticion_P_E(QString direccion, QString nombre){
    string auxPath = direccion.toStdString();
    string auxName = nombre.toStdString();
    FILE *fp;
    if((fp = fopen(auxPath.c_str(),"rb+"))){
        MBR masterboot;
        fseek(fp,0,SEEK_SET);
        fread(&masterboot,sizeof(MBR),1,fp);
        for(int i = 0; i < 4; i++){
            if(strcmp(masterboot.mbr_partition[i].part_name,auxName.c_str())){
                return i;
            }
        }

    }
    return -1;
}

/* Funcion que devuelve un entero para comprobar la existencia de una particion primaria o extendida en un disco
 * @param QString direccion: ruta del archivo
 * @param QString nombre: nombre de la particion
 * @return -1 = no se encuentra |
*/
int buscarParticion_L(QString direccion, QString nombre){
    string auxPath = direccion.toStdString();
    string auxName = nombre.toStdString();
    FILE *fp;
    if((fp = fopen(auxPath.c_str(),"rb+"))){
        int extendida = -1;
        MBR masterboot;
        fseek(fp,0,SEEK_SET);
        fread(&masterboot,sizeof(MBR),1,fp);
        for(int i = 0; i < 4; i++){
            if(masterboot.mbr_partition[i].part_type == 'E'){
                extendida = i;
                break;
            }
        }
        if(extendida != -1){
            EBR extendedBoot;
            fseek(fp, masterboot.mbr_partition[extendida].part_start,SEEK_SET);

        }
    }
    return -1;
}

/* Metodo para leer una linea tanto ingresada por el usuario como por un archi
 * @param string comando: linea a leer
*/
void leerComando(string comando){
    YY_BUFFER_STATE buffer = yy_scan_string(comando.c_str());
    if(yyparse() == 0){
        Graficador *g = new Graficador(raiz);
        g->generarImagen();
        reconocerComando(raiz);

    }else{
        cout << "Comando no reconocido" << endl;
    }
}

/* Metodo para graficar un disco con su estructura de las particiones */
void graficarDisco(QString direccion, QString destino, QString extension){
    string auxDir = direccion.toStdString();
    FILE *fp;
    FILE *graphDot;
    if((fp = fopen(auxDir.c_str(),"r"))){
        graphDot = fopen("grafica.dot","w");
        fprintf(graphDot,"digraph G{\n");
        fprintf(graphDot, "table [shape = box, label=<\n");
        fprintf(graphDot,"<table border=\'0\' cellborder=\'2\' width=\'600\' height=\"200\" color=\'LIGHTSTEELBLUE\'>\n");
        fprintf(graphDot, "<tr>");
        fprintf(graphDot, "<td height=\'200\' width=\'100\'> MBR </td>");
        MBR masterboot;
        fseek(graphDot,0,SEEK_SET);
        fread(&masterboot,sizeof(MBR),1,fp);
        int total = masterboot.mbr_size;
        int parcial = 0;
        double espacioUsado = 0;
        for(int i = 0; i < 4; i++){
            parcial = masterboot.mbr_partition[i].part_size;
            double porcentaje = (parcial*100)/total;
            double porcentaje2 = (porcentaje*500)/100;
            espacioUsado += porcentaje2;
            //Buscamos las particiones que no esten vacias
            if((masterboot.mbr_partition[i].part_start != -1) && (masterboot.mbr_partition[i].part_status != 0)){
                if(masterboot.mbr_partition[i].part_status != 1){
                    if(masterboot.mbr_partition[i].part_type == 'P'){
                        fprintf(graphDot, "<TD HEIGHT=\"200\" WIDTH=\"%f\">PRIMARIA <br/> Ocupado: %f%c</TD>\n",porcentaje2,porcentaje,'%');
                    }else{//Extendida
                        fprintf(graphDot,"<td height=\'200\' width=\'%f\'> \n",porcentaje2);
                        fprintf(graphDot,"<table border=\'0\' height=\'200\' width=\'%f\' cellborder=\'1\' >\n", porcentaje2);
                        fprintf(graphDot, "<tr>\n");
                        fprintf(graphDot,"<td height=\'50' width=\'%f\'>Extendida<\td>\n",porcentaje2);
                        fprintf(graphDot, "</tr>\n ");
                        fprintf(graphDot, "<tr>\n");
                        fseek(fp,masterboot.mbr_partition[i].part_start,SEEK_SET);
                        //EBR extendedBoot;
                    }

                }else{//Eliminada fast

                }
            }
        }
        fprintf(graphDot,"<td height='200' width=\'%f\'> ESPACIO LIBRE </td>",(100-espacioUsado));
        fprintf(graphDot, "</tr> \n </table> \n>];\n}");
        fclose(graphDot);
        fclose(fp);
        string comando = "dot -T"+extension.toStdString()+"grafica.dot -o "+destino.toStdString();
        system(comando.c_str());
    }else{
        cout << "ERROR al crear reporte, disco no encontrado" << endl;
    }
}

/* Funcion que retorna la extension de un archivo
 * @param QString direccion: ruta del archivo
 * @return .pdf, jpg, png | "error"
*/
QString getExtension(QString direccion){
    string aux = direccion.toStdString();
    string delimiter = ".";
    size_t pos = 0;
    while((pos = aux.find(delimiter))!=string::npos){
        aux.erase(0,pos+delimiter.length());
    }
    return QString::fromStdString(aux);
}
