// Bibliotecas necessárias
#include <string.h>                 // Para funções de string como strlen()
#include <time.h>                   // Para obter timestamp atual
#include "pico/stdlib.h"            // Biblioteca padrão do Pico (GPIO, tempo, etc.)
#include "pico/cyw43_arch.h"        // Driver WiFi para Pico W
#include "include/wifi_conn.h"      // Funções personalizadas de conexão WiFi
#include "include/mqtt_comm.h"      // Funções personalizadas para MQTT
#include "include/xor_cipher.h"     // Funções de cifra XOR
#include "config.h"                 // Arquivo de configuração com credenciais

int main() {
    // Inicializa todas as interfaces de I/O padrão (USB serial, etc.)
    stdio_init_all();
    
    // Conecta à rede WiFi usando credenciais do arquivo de configuração
    connect_to_wifi(WIFI_SSID, WIFI_PASSWORD);

    // Configura o cliente MQTT usando credenciais do arquivo de configuração
    mqtt_setup(MQTT_CLIENT_ID, MQTT_BROKER_IP, MQTT_USERNAME, MQTT_PASSWORD);
    
    // Aguarda 2 segundos para estabelecer conexão
    sleep_ms(2000);

    // Loop principal do programa
    while (true) {
        // Cria a mensagem JSON com valor e timestamp atual
        char buffer[64];
        float temperatura = 26.5;  // Valor de exemplo (poderia vir de um sensor)
        
        // Obtém o timestamp atual e formata a mensagem JSON
        sprintf(buffer, "{\"valor\":%.1f,\"ts\":%lu}", temperatura, (unsigned long)time(NULL));
        printf("Enviando: %s\n", buffer);
        
        // Buffer para mensagem criptografada
        uint8_t criptografada[64];
        
        // Criptografa a mensagem usando XOR com chave 42
        xor_encrypt((uint8_t *)buffer, criptografada, strlen(buffer), 42);
        
        // Publica a mensagem criptografada
        mqtt_comm_publish("escola/sala1/temperatura", criptografada, strlen(buffer));
        
        // Aguarda 5 segundos antes da próxima publicação
        sleep_ms(5000);
    }
    return 0;
}

/* 
 * Comandos de terminal para testar o MQTT:
 * 
 * Inicia o broker MQTT com logs detalhados:
 * mosquitto -c mosquitto.conf -v
 * 
 * Assina o tópico de temperatura (recebe mensagens):
 * mosquitto_sub -h localhost -p 1883 -t "escola/sala1/temperatura" -u "aluno" -P "senha123"
 * 
 * Publica mensagem de teste no tópico de temperatura:
 * mosquitto_pub -h localhost -p 1883 -t "escola/sala1/temperatura" -u "aluno" -P "senha123" -m "26.6"
 */