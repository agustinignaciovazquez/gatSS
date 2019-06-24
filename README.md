# gatSS

Trabajo Practico Criptografia y Seguridad
Secreto compartico y esteganografia en imagenes

## Compilación

#Prerequisitos: - Sistema Unix.
                - **C** Compiler.
                - **CMake**. 
               

**PASO 1:** $> chmod +x install.sh

**PASO 2:** $> ./install.sh (compila el proyecto con CMake)

## Ejecución

**PASO 1:** $> cd gatSS/target

**PASO 2:** $> ./gatSS <args> 

#Parametros:

  - **-h**: Muestra una lista de los parametros a utilizar.

  - **-d**: indica que se va a distribuir una imagen secreta en otras imágenes. 
 
  - **-r**:  indica que se va a recuperar una imagen secreta a partir de otras imágenes. 

  - **-s** *imagen*: El nombre imagen corresponde al nombre de un archivo de extensión .bmp. En el caso de que se haya elegido la opción (-d) este archivo debe existir ya que es la imagen a ocultar y debe ser una imagen en blanco y negro (8 bits por pixel) Si se eligió la opción (-r) éste archivo será el archivo de salida, con la imagen secreta revelada al finalizar el programa. 

  - **-m** *imagen*: El nombre imagen corresponde al nombre de un archivo con extensión .bmp. En el caso de que se haya elegido la opción (-d) este archivo es una imagen en blanco y negro que servirá como “marca de agua” para verificar todo el proceso. Debe ser de igual tamaño que la imagen secreta. En el caso de que se haya elegido la opción (-r) este archivo es una imagen en blanco y negro que contiene la transformación de la imagen de “marca de agua”.
  
  - **-k** *numero*: El número corresponde a la cantidad mínima de sombras necesarias para recuperar el secreto en un esquema (k, n). 
  
  - **-n** *numero*: El número corresponde a la cantidad total de sombras en las que se distribuirá el secreto en un esquema (k, n).
  
  - **-dir** *directorio*: El directorio donde se encuentran las imágenes en las que se distribuirá el secreto (en el caso de que se haya elegido la opción (-d)), o donde están las imágenes que contienen oculto el secreto ( en el caso de que se haya elegido la opción (-r)). Debe contener imágenes de extensión .bmp, de 24 bits por pixel. 
