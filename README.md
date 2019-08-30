# Sistema de archivos 

Es una aplicacion en consola que simulara un sistema de archivos. Esta no tendra menus, sino que se utilizaran comandos. No distinguira entre mayúsculas y minúsculas. Solo se puede colocar un comando por línea. Ademas tambien se simulara un sistema RAID 1, para tener una copia de cada disco como respaldo. Toda la informacion se detalla mas en el archivo Fase1.pdf 

## Instalacion
- Descargar repositorio
- Abrir el archivo FileSystem_1.pro desde Qt Creator

## Comandos
- MKDISK: Creara un archivo binario, simulando un disco duro.
- RMKDISK: Borrara el archivo.
- FDISK: Administrara las operaciones que se hagan en una particion como crear, eliminar o quitar espacio.
- MOUNT: Montara una particion
- UNMOUNT: Desmontara una particion
- REP: Generara un reporte tipo imagen
- EXEC: Ejecutara un archivo con comandos ya escritos. 

## Tutorial
 - Cuando inicie el programa usted encontrara la siguiente consola donde tendra que escribir los comandos que se detallaron anteriormente. Recuerde que solo se puede ejecutar un comando por linea. 
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/Inicio.png)
 - Empezaremos con la creando de discos. Crearemos dos, el primero para crear particiones y el segundo para eliminarlo. Ambos  tendran un tamano de 50 MB
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/mkdisk_2.png)
 - Como la aplicacion cuenta con sistema RAID creara una copia de cada disco que tengamos.
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/mkdisk_1.png)
 - Eliminamos el segundo disco
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/rmdisk.png)
 - Aunque se haya eliminado un disco, siempre quedara una copia del mismo 
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/rmdisk_1.png)
 - Crearemos algunas particiones, 3 primarias, 1 extendida y 2 logicas. 
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/fdisk_2.png)
 - Eliminaremos la particion primaria 1 llamada Part1
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/fdisk_del.png)
 - Montaremos la particion 3 al sistema
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/mounti.png)
 - Y finalmente generaremos su respectivo reporte MBR y DISK
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/reports.png)
 - Reporte tipo disk
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/reporte1.jpg)
 - Reporte tipo MBR
 ![No se encuentra la imagen](https://github.com/raulxiloj/FileSystem_1/blob/master/Imagenes/reporte2.jpg)
 
## Herramientas utilizadas:
- C/C++ (Lenguaje de programacion) 
- Qt Creator (IDE)
- Linux Ubuntu (Sistema operativo) 
- Flex / Bison para el analizador
- Graphviz (Para los reportes)



## Autor 
Raul Xiloj
