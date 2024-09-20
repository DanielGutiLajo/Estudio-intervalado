#include <stdbool.h> // bool
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


//* Organización de las tareas en el txt
//Nombre sin espacios    ultima fecha       siguiente fecha       gradp
FILE* tareas;

void extraerFecha(char fecha[], char linea[]) {
    int palabra_numero = 1; // Contador de palabras
    // Usar strtok para dividir la línea en palabras usando el espacio como delimitador
    fecha = strtok(linea, " \t\n"); // Eliminar espacios, tabulaciones y saltos de línea

    // Iterar por cada palabra en la línea
    while (fecha != NULL) {
        if (palabra_numero == 3) { // Si es la tercera palabra
            break;
        }
        // Pasar a la siguiente palabra
        fecha = strtok(NULL, " \t\n");
        palabra_numero++;
    }
}
void ordenarTareas() {
    //Ya debería estar en el principio de la linea
    char fechaProxima[50];
    char linea[512];
    fgets(linea, sizeof(linea), tareas);
    extraerFecha(fechaProxima, linea);

    fseek(tareas, 0, SEEK_SET);
    
    bool encontrada = false;
    while (fgets(linea, sizeof(linea), tareas) && !encontrada) {
        char* palabra;
        int palabra_numero = 1; // Contador de palabras
        // Usar strtok para dividir la línea en palabras usando el espacio como delimitador
        palabra = strtok(linea, " \t\n"); // Eliminar espacios, tabulaciones y saltos de línea

        // Iterar por cada palabra en la línea
        while (palabra != NULL) {
            if (palabra_numero == 3) { // Si es la tercera palabra
                if (strcmp(palabra, fechaProxima) == 0) // Comprobar si es igual a la palabra buscada
                    encontrada = true;
                break; // Salir del bucle ya que solo nos interesa la tercera palabra
            }
            // Pasar a la siguiente palabra
            palabra = strtok(NULL, " \t\n");
            palabra_numero++;
        }
    }
    
    fseek(tareas, 0, SEEK_SET);
}
//*Tiempo
time_t tiempo_actual;
struct tm* info_tiempo;
void actualizarTiempo() {
    // Obtener el tiempo actual del sistema
    time(&tiempo_actual);

    // Convertir el tiempo a una estructura de tiempo local
    info_tiempo = localtime(&tiempo_actual);
}
void irComienzoFecha(char fecha[]){
    fseek(tareas, 0, SEEK_SET);
    char linea[512];       // Buffer para almacenar cada línea

    bool encontrada = false;
    long posicion = ftell(tareas);
    // Leer el archivo línea por línea
    while (fgets(linea, sizeof(linea), tareas) && !encontrada) {
        char* palabra;
        int palabra_numero = 1; // Contador de palabras
        // Usar strtok para dividir la línea en palabras usando el espacio como delimitador
        palabra = strtok(linea, " \t\n"); // Eliminar espacios, tabulaciones y saltos de línea

        // Iterar por cada palabra en la línea
        while (palabra != NULL) {
            if (palabra_numero == 3) { // Si es la tercera palabra
                if (strcmp(palabra, fecha) == 0) // Comprobar si es igual a la palabra buscada
                    encontrada = true;
                else
                    posicion = ftell(tareas);
                break; // Salir del bucle ya que solo nos interesa la tercera palabra
            }
            // Pasar a la siguiente palabra
            palabra = strtok(NULL, " \t\n");
            palabra_numero++;
        }
    }
    fseek(tareas, posicion, SEEK_SET);
}
void escribirSiguienteFecha(struct tm fecha_original, int grado, char resultado[]) {
    int dias;
    if (grado == 1)
        dias = 1;
    else if (grado == 2)
        dias = 3;
    else if (grado == 3)
        dias = 7;
    else if (grado == 4)
        dias = 14;
    else if (grado == 5)
        dias = 30;
    else {
        dias = 30;
        for (int contador = grado; contador > 5; contador--)
            dias *= 2;
    }
    // Copia la fecha original para trabajar sobre la copia
    struct tm fecha_modificada = fecha_original;

    // Convertir la estructura tm a time_t
    time_t tiempo_modificado = mktime(&fecha_modificada);

    // Sumar los días (1 día = 86400 segundos)
    tiempo_modificado += dias * 86400;

    // Convertir de vuelta a la estructura tm
    struct tm* nueva_fecha = localtime(&tiempo_modificado);

    sprintf(resultado, "%02d/%02d/%d",
        nueva_fecha->tm_mday,
        nueva_fecha->tm_mon + 1,   // Mes (se suma 1 porque empieza en 0)
        nueva_fecha->tm_year + 1900); // Año (se suma 1900)
}
void irTareasHoy() {
    char fechaBuscar[100]; // La palabra que queremos buscar

    sprintf(fechaBuscar, " %02d/%02d/%d",
        info_tiempo->tm_mday,      // Día del mes (1-31)
        info_tiempo->tm_mon + 1,   // Mes (0-11, por eso se suma 1)
        info_tiempo->tm_year + 1900);
    
}

void escribirTareasHoy() {
    char linea[512];       // Buffer para almacenar cada línea
    char fechaBuscar[100]; // La palabra que queremos buscar

    sprintf(fechaBuscar, " %02d/%02d/%d",
        info_tiempo->tm_mday,      // Día del mes (1-31)
        info_tiempo->tm_mon + 1,   // Mes (0-11, por eso se suma 1)
        info_tiempo->tm_year + 1900);

    int numerador = 1;
    fgets(linea, sizeof(linea), tareas);
    printf("%d. %s \n", numerador, linea);
    bool escribiendo = true;
    while (fgets(linea, sizeof(linea), tareas) && escribiendo) {
        numerador++;
        char* palabra;
        int palabra_numero = 1; // Contador de palabras

        // Usar strtok para dividir la línea en palabras usando el espacio como delimitador
        palabra = strtok(linea, " \t\n"); // Eliminar espacios, tabulaciones y saltos de línea

        // Iterar por cada palabra en la línea
        while (palabra != NULL) {
            if (palabra_numero == 3) { // Si es la tercera palabra
                if (strcmp(palabra, fechaBuscar) == 0) // Comprobar si es igual a la palabra buscada
                    printf("%d. %s \n", numerador, linea);
                else
                    escribiendo = false;
                break; // Salir del bucle ya que solo nos interesa la tercera palabra
            }
            // Pasar a la siguiente palabra
            palabra = strtok(NULL, " \t\n");
            palabra_numero++;
        }
    }
    fseek(tareas, 0, SEEK_SET);
}

int main() {
    // Abrir el archivo de tareas
    tareas = fopen("tareas.txt", "r+");
    if (tareas == NULL) {
        // Si el archivo no existe, crearlo con "w+"
        printf("El archivo tareas.txt no existe, creando uno vacío...\n");
        tareas = fopen("tareas.txt", "w+");

        if (tareas == NULL) {
            // Si por algún motivo la creación falla
            printf("No se pudo crear el archivo.\n");
            return 1;
        }
    }

    printf("Hola, soy la máquina de estudio intervalado.\n");
    char cerrarPrograma = 'a';
    do {


        char respuestaAccion;
        do {
            printf("¿Qué deseas hacer?\n");
            printf("1. Agregar un estudio\n");
            printf("2. Ver todos los estuidos\n");
            printf("3. Ver los estudios de hoy\n");
            printf("4. Marcar como estudiada \n");

            scanf(" %c", &respuestaAccion);

            if (respuestaAccion != '1' && respuestaAccion != '2' && respuestaAccion != '3') {
                printf("Respuesta no esperada, vuelve a intentarlo\n");
            }
        } while (respuestaAccion != '1' && respuestaAccion != '2' && respuestaAccion != '3');

        actualizarTiempo();

        if (respuestaAccion = '1') {
            char respuestaBinaria;
            char nombre[256];
            do {
                printf("Cual es el nombre que le deseas dar al estudio?(menos de 256 caracteres y sin espacios)\n");
                scanf(" %s", nombre);

                do {
                    printf("El nombre eligido es %s?(Y,N)", nombre);
                    if (respuestaBinaria != 'Y' && respuestaBinaria != 'N')
                        printf("La respuesta dada no es valida repita");
                } while (respuestaBinaria != 'Y' && respuestaBinaria != 'N');

            } while (respuestaBinaria == 'N');

            // Mover el puntero al final del archivo
            fseek(tareas, 0, SEEK_END);
            long posicion = ftell(tareas);
            fprintf(tareas, "%s", nombre);
            fprintf(tareas, " %02d/%02d/%d",
                info_tiempo->tm_mday,      // Día del mes (1-31)
                info_tiempo->tm_mon + 1,   // Mes (0-11, por eso se suma 1)
                info_tiempo->tm_year + 1900);// Año (desde 1900, por eso se suma 1900)

            int grado;
            do {
                printf("Cual es el grado que le deseas dar al estudio?(0 estudiar manana, 1 dentro de tres días, 2 una semana, 3 dos semanas, 4 un mes ...)\n");
                scanf(" %s", grado);

                do {
                    printf("El grado eligido es %s?(Y,N)", grado);
                    if (respuestaBinaria != 'Y' && respuestaBinaria != 'N')
                        printf("La respuesta dada no es valida repita");
                } while (respuestaBinaria != 'Y' && respuestaBinaria != 'N');

            } while (respuestaBinaria == 'N');
            char fechaSiguiente[50];
            escribirSiguienteFecha(*info_tiempo, grado, fechaSiguiente);
            fprintf(tareas, " %s %d\n", fechaSiguiente, grado);
            fseek(tareas, posicion, SEEK_SET);
            ordenarTareas();
        }





        else if (respuestaAccion = '2') {
            char linea[512]; // Buffer para almacenar cada línea

            // Leer el archivo línea por línea y mostrarlo en pantalla
            while (fgets(linea, sizeof(linea), tareas)) {
                printf("%s", linea); // Mostrar la línea leída
            }
            fseek(tareas, 0, SEEK_SET);
        }








        else if (respuestaAccion = '3') {
            irTareasHoy();
            escribirTareasHoy();
        }


        else if (respuestaAccion = '4') {
            irTareasHoy();
            escribirTareasHoy();
            bool dentroRango = true;
            long posicion = 0;

            posicion = ftell(tareas);
            do {
                int tareaEstudiada = 0;
                printf("\n Que tarea quieres marcar como estudiada?(Escribe el numero)");
                irTareasHoy();
                scanf("%d", tareaEstudiada);
                char linea[512];       // Buffer para almacenar cada línea
                char fechaBuscar[100]; // La palabra que queremos buscar

                sprintf(fechaBuscar, " %02d/%02d/%d",
                    info_tiempo->tm_mday,      // Día del mes (1-31)
                    info_tiempo->tm_mon + 1,   // Mes (0-11, por eso se suma 1)
                    info_tiempo->tm_year + 1900);

                int numerador = 1;


                while (fgets(linea, sizeof(linea), tareas) && dentroRango && (numerador != tareaEstudiada)) {
                    posicion = ftell(tareas);
                    char* palabra;
                    int palabra_numero = 1; // Contador de palabras

                    // Usar strtok para dividir la línea en palabras usando el espacio como delimitador
                    palabra = strtok(linea, " \t\n"); // Eliminar espacios, tabulaciones y saltos de línea

                    // Iterar por cada palabra en la línea
                    while (palabra != NULL) {
                        if (palabra_numero == 3) { // Si es la tercera palabra
                            if (strcmp(palabra, fechaBuscar) != 0) // Comprobar si es igual a la palabra buscada
                                dentroRango = false;
                            break; // Salir del bucle ya que solo nos interesa la tercera palabra
                        }
                        // Pasar a la siguiente palabra
                        palabra = strtok(NULL, " \t\n");
                        palabra_numero++;
                    }
                    numerador++;
                }
                if (dentroRango)
                    printf("Ese numero no está disponible diga otro");
            } while (!dentroRango);
            fseek(tareas, posicion, SEEK_SET);


            char linea[512];
            char nombre[50], palabra2[50], fechaAntigua[50], gradoPrevio[50];

            fgets(linea, sizeof(linea), tareas);
            // Dividir la línea en palabras
            sscanf(linea, "%s %s %s %s", nombre, palabra2, fechaAntigua, gradoPrevio);

            int grado;
            printf("Que grado quieres asignarle a la tarea el actual es: %d (0 estudiar manana, 1 dentro de tres días, 2 una semana, 3 dos semanas, 4 un mes ...)", gradoPrevio);
            scanf(" %d", grado);

            char fechaSiguiente[50];
            escribirSiguienteFecha(*info_tiempo, grado, fechaSiguiente);
            // Reconstruir la línea con las modificaciones
            sprintf(linea, "%s %s %s %d\n", nombre, fechaAntigua, fechaSiguiente, grado);

            // Limpiar el resto de la línea si la nueva es más corta que la anterior
            fflush(tareas); // Asegura que la salida se escriba completamente

            fseek(tareas, posicion, SEEK_SET);

            ordenarTareas();
        }




        do {
            printf("Tarea finalizada desea cerrar el programa?(Y, N)");
            if (cerrarPrograma != 'Y' && cerrarPrograma != 'N')
                printf("La respuesta dada no es valida repita");
        } while (cerrarPrograma != 'Y' && cerrarPrograma != 'N');
    } while (cerrarPrograma != 'Y');
    fclose(tareas);
    return 0;
}
