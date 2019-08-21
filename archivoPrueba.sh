#Crea un disco de 3000 Kb en la carpeta home
Mkdisk -Size=3000 -unit=K -path=/home/user/Disco1.disk
#No es necesario utilizar comillas para la ruta en este caso
#ya que la ruta no tiene ningun espacio en blanco
Mkdisk -path=/home/user/Disco2.disk -Unit=K -size=3000
#Se ponen comillas por la carpeta "mis discos" ya que tiene espacios en blanco
mkdisk -size=5 -unit=M -path="/home/mis discos/Disco3.disk"
#Creara un disco de 10Mb ya que no hay parametro unit
Mkdisk -size=10 -path="/home/mis discos/Disco4.disk"
#Elimina Disco4.disk
rmDisk -path="/home/mis discos/Disco4.disk"
#Crea una particion primaria llamada Particion1 de 300 kb
#con el peor ajuste en el disco Disco1.disk
fdisk -size=300 -path=/home/user/Disco1.disk -name=Particion1
#Crea una particion extendida dentro de Disco2 de 300 kb
#Tiene el peor ajuste
fdisk -type=E -path=/home/user/Disco2.disk -Unit=K -name=Particion2 -size=300
#Crea una particion extendida dentro del Disco 3 de 200 kb
fdisk -type=E -path="/home/mis discos/Disco3.disk" -Unit=K -name=Particion2 -size=300
#Crea una particion logica con el mejor ajuste, llamada Particion3
#de 1 Mb en el Disco 3
fdisk -size=10 -type=L -unit=k -fit=BF -path="/home/mis discos/Disco3.disk" -name="Particion3"
