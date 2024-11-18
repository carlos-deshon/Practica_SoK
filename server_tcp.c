#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define PORT 5001
#define BUFFER_SIZE 1024

// Función para invertir una cadena de caracteres
void invertir_cadena(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; ++i) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

// Función para verificar si una cadena contiene números
int contiene_numeros(const char *str) {
    for (int i = 0; i < strlen(str); i++) {
        if (isdigit(str[i])) {
            return 1; // La cadena contiene números
        }
    }
    return 0; // La cadena no contiene números
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Crear el socket TCP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Vincular el socket al puerto
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al vincular");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        perror("Error al escuchar");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor en espera de conexiones en el puerto %d...\n", PORT);

    while (1) {
        // Aceptar una conexión entrante
        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
            perror("Error al aceptar la conexión");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        printf("Cliente conectado.\n");

        // Bucle para manejar la comunicación con el cliente
        while (1) {
            int n = read(new_socket, buffer, BUFFER_SIZE);
            if (n <= 0) break;  // Si no hay datos, cerrar la conexión

            buffer[n] = '\0';  // Terminar la cadena recibida

            // Verificar si la cadena contiene números
            if (contiene_numeros(buffer)) {
                strcpy(buffer, "Error: El mensaje no debe contener números.");
            } else {
                // Invertir el mensaje
                invertir_cadena(buffer);
            }

            // Enviar respuesta al cliente
            send(new_socket, buffer, strlen(buffer), 0);
            printf("Respuesta enviada: %s\n", buffer);
        }

        printf("Cliente desconectado.\n");
        close(new_socket);  // Cerrar la conexión con el cliente
    }

    close(server_fd);  // Cerrar el socket del servidor
    return 0;
}
