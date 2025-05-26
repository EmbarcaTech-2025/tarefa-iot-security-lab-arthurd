#include <string.h>                 // Para funções de string
#include <stdint.h>                 // Para tipos inteiros exatos
#include "pico/stdlib.h"            // Biblioteca padrão do Pico
#include "pico/cyw43_arch.h"        // Driver WiFi para Pico W
#include "include/wifi_conn.h"      // Funções de conexão WiFi
#include "include/mqtt_comm.h"      // Funções MQTT
#include "include/xor_cipher.h"     // Funções XOR
#include "config.h"                 // Arquivo de configuração com credenciais

// Variável para armazenar o último timestamp recebido
static uint32_t ultima_timestamp_recebida = 0;

// Modificamos a callback para processar a mensagem decifrada
void processar_mensagem_recebida(const char* mensagem) {
    // Processamento da mensagem decifrada
    float valor;
    uint32_t nova_timestamp;
    
    // Tentativa de extrair valor e timestamp do JSON
    if (sscanf(mensagem, "{\"valor\":%f,\"ts\":%lu}", &valor, &nova_timestamp) != 2) {
        printf("Erro no parse da mensagem: %s\n", mensagem);
        return;
    }
    
    // 2. Verificação de replay
    if (nova_timestamp > ultima_timestamp_recebida) {
        ultima_timestamp_recebida = nova_timestamp;
        printf("Nova leitura: %.2f (ts: %lu)\n", valor, nova_timestamp);
        
        // --> Processamento adicional aqui (lógica específica da aplicação) <--
        
    } else {
        printf("ALERTA! Replay detectado (ts: %lu <= %lu)\n", 
               nova_timestamp, ultima_timestamp_recebida);
    }
}

int main() {
    // Inicializa todas as interfaces de I/O padrão
    stdio_init_all();
    
    // Aguarda 2 segundos para dar tempo do terminal se conectar
    sleep_ms(2000);
    printf("Iniciando subscriber MQTT com proteção contra replay...\n");
    
    // Conecta à rede WiFi
    connect_to_wifi(WIFI_SSID, WIFI_PASSWORD);

    // Configura o cliente MQTT
    mqtt_setup(MQTT_CLIENT_ID, MQTT_BROKER_IP, MQTT_USERNAME, MQTT_PASSWORD);

    // Aguarda um momento para a conexão MQTT ser estabelecida
    sleep_ms(3000);
    
    // Inscreve no tópico, usando a chave 42 para decifragem
    mqtt_comm_subscribe("escola/sala1/temperatura", 42);
    
    // Loop principal - mantém o programa em execução
    while (true) {
        // O processamento de mensagens MQTT ocorre em callbacks
        sleep_ms(1000);
    }
    
    return 0;
}