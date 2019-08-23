#Archivo de prueba

#CREACION DE DISCOS
Mkdisk -size=50 -unit=M -path=/home/archivos/fase1/Disco1.disk
Mkdisk -unit=k -size=51200 -path=/home/archivos/fase1/Disco2.disk
mkDisk -size=10 -path=/home/archivos/fase1/Disco3.disk
mkdisk -size=51200 -path="/home/archivos/fase1/mis archivos/Disco4.disk" -unit=K
mkDisk -size=20 -path="/home/archivos/fase1/mis archivos/Disco5.disk" -unit=M

#Deberia dar error
mkdisk -param=x -size=30 -path=/home/archivos/fase1/Disco.disk

#ELIMINACION DE DISCOS
#El primero deberia dar error
rmDisk -path=/home/Disco3.disk
rmDisk -path=/home/archivos/fase1/Disco3.disk
RMDISK -path="/home/archivos/fase1/mis archivos/Disco4.disk"

#CREACION DE PARTICIONES
#Particiones en el disco1
fdisk -type=P -unit=K -name=Part1 -size=7680 -path=/home/archivos/fase1/Disco1.disk -fit=BF #7.5 MB
fdisk -type=E -unit=K -name=Part2 -size=7680 -path=/home/archivos/fase1/Disco1.disk -fit=FF
fdisk -type=E -unit=K -name=Part3 -size=7680 -path=/home/archivos/fase1/Disco1.disk -fit=WF #Deberia dar error
fdisk -type=P -unit=K -name=Part3 -size=7680 -path=/home/archivos/fase1/Disco1.disk -fit=WF
fdisk -type=P -unit=K -name=Part4 -size=7680 -path=/home/archivos/fase1/Disco1.disk -fit=BF
FDISK -type=L -unit=k -name=part5 -size=1280 -path=/home/archivos/fase1/Disco1.disk -fit=BF #1.25 MB
fdisk -type=L -unit=K -name=part6 -size=1280 -path=/home/archivos/fase1/Disco1.disk -fit=WF
fdisk -type=L -unit=K -name=part7 -size=1280 -path=/home/archivos/fase1/Disco1.disk -fit=wf
fdisk -type=L -unit=K -name=part8 -size=1280 -path=/home/archivos/fase1/Disco1.disk -fit=ff
fdisk -type=L -unit=K -name=part9 -size=1280 -path=/home/archivos/fase1/Disco1.disk -fit=bf
fdisk -type=L -unit=K -name=part10 -size=1280 -path=/home/archivos/fase1/Disco1.disk -fit=wf
fdisk -type=L -unit=K -name=part10 -size=1024 -path=/home/archivos/fase1/Disco1.disk -fit=BF #ERROR nombre y espacio

#Particiones en el disco2
fdisk -type=L -unit=k -name=Part11 -size=10240 -path=/home/archivos/fase1/Disco2.disk -fit=BF #ERRROR no hay una extendida
fdisk -type=L -unit=k -name=Part12 -size=10240 -path=/home/archivos/fase1/Disco2.disk -fit=BF #ERRROR no hay una extendida
fDisk -type=P -unit=K -name=Part11 -size=10240 -path=/home/archivos/fase1/Disco2.disk -fit=BF #10 MB
fDisk -type=P -unit=m -name=Part12 -size=5 -path=/home/archivos/fase1/Disco2.disk -fit=FF #5MB
fDisk -type=P -unit=K -name=Part13 -size=5120 -path=/home/archivos/fase1/Disco2.disk -fit=WF #5 MB
fdisk -type=E -unit=m -name=Part14 -size=20 -path=/home/archivos/fase1/Disco2.disk #20 MB
fdisk -type=L -unit=k -name=Part15 -size=1536 -path=/home/archivos/fase1/Disco2.disk #1.5 MB
fdisk -type=L -unit=k -name=Part16 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=BF
fdisk -type=L -unit=k -name=Part17 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=FF
fdisk -type=L -unit=k -name=Part18 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=BF
fdisk -type=L -unit=k -name=Part19 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=WF
fdisk -type=L -unit=k -name=Part20 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=BF
fdisk -type=L -unit=k -name=Part21 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=FF
fdisk -type=L -unit=k -name=Part22 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=BF
fdisk -type=L -unit=k -name=Part23 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=FF
fdisk -type=L -unit=k -name=Part24 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=BF
fdisk -type=L -unit=k -name=Part25 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=wF
fdisk -type=L -unit=k -name=Part26 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=BF
fdisk -type=L -unit=k -name=Part27 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=FF
fdisk -type=L -unit=k -name=Part28 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=FF
fdisk -type=L -unit=k -name=Part29 -size=1536 -path=/home/archivos/fase1/Disco2.disk -fit=wF

#Particiones en el disco 5
fdisk -type=E -unit=k -name=Part30 -size=5120 -path="/home/archivos/fase1/mis archivos/Disco5.disk" -fit=BF #5MB
fdisk -type=L -unit=k -name=Part31 -size=1024-path="/home/archivos/fase1/mis archivos/Disco5.disk" -fit=BF
fdisk -type=P -unit=k -name=Part32 -size=5120 -path="/home/archivos/fase1/mis archivos/Disco5.disk" -fit=BF
fdisk -type=L -unit=k -name=Part33 -size=1024 -path="/home/archivos/fase1/mis archivos/Disco5.disk" -fit=BF
fdisk -type=P -unit=k -name=Part34 -size=5120 -path="/home/archivos/fase1/mis archivos/Disco5.disk" -fit=BF
fdisk -type=P -unit=k -name=Part35 -size=5120 -path="/home/archivos/fase1/mis archivos/Disco5.disk" -fit=BF
fdisk -type=L -unit=k -name=Part36 -size=1024 -path="/home/archivos/fase1/mis archivos/Disco5.disk" -fit=BF
fdisk -type=L -unit=k -name=Part37 -size=1024 -path="/home/archivos/fase1/mis archivos/Disco5.disk" -fit=BF

#MONTAR PARTICIONES
mount -path=/home/archivos/fase1/Disco1.disk -name=Part1 #vda1
mount -path=/home/archivos/fase1/Disco3.disk -name=Part3 #ERROR no existe
mount -path=/home/archivos/fase1/Disco1.disk -name=Part2 #vda2
mount -path=/home/archivos/fase1/Disco1.disk -name=Part3 #vda3
mount -path=/home/archivos/fase1/Disco1.disk -name=Part4 #vda4
mount -path=/home/archivos/fase1/Disco1.disk -name=Part5 #vda5
mount -path=/home/archivos/fase1/Disco1.disk -name=Part6 #vda6
mount -path=/home/archivos/fase1/Disco1.disk -name=Part7 #vda7
mount -path=/home/archivos/fase1/Disco2.disk -name=Part13 #vdb1
mount -path=/home/archivos/fase1/Disco2.disk -name=Part14 #vdb2
mount -path=/home/archivos/fase1/Disco2.disk -name=Part15 #vdb3
mount -path=/home/archivos/fase1/Disco2.disk -name=Part16 #vdb4
mount -path=/home/archivos/fase1/Disco2.disk -name=Part17 #vdb5
mount -path="/home/archivos/fase1/mis archivos/Disco5.disk" -name=Part30 #vdc1
mount -path="/home/archivos/fase1/mis archivos/Disco5.disk" -name=Part31 #vdc2
mount -path="/home/archivos/fase1/mis archivos/Disco5.disk" -name=Part32 #vdc3

#DESMONTAR PARTICIONES
unmount -id=vda5
unmount -id=vda6
unmount -id=vdb5
unmount -id=vdc2
unmount -id=vdx1 #ERROR

#REPORTES
rep -id=vda1 -Path=/home/reports/reporte1.jpg -name=disk
rep -id=vda1 -Path=/home/reports/reporte2.jpg -name=mbr
rep -id=vdb1 -path=/home/reports/reporte3.jpg -name=disk
rep -id=vdb1 -path=/home/reports/reporte4.jpg -name=mbr
rep -id=vdc1 -path=/home/reports/reporte5.jpg -name=disk
rep -id=vdc1 -path=/home/reports/report6.jpg -name=mbr

exec -path=/home/raul/Desktop/archivo_de_prueba.sh 
