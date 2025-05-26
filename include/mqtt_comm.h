#ifndef MQTT_COMM_H
#define MQTT_COMM_H

#include <stdint.h>
#include <stddef.h>

// Configuração da conexão MQTT
void mqtt_setup(const char *client_id, const char *broker_ip, const char *user, const char *pass);

// Publicação de mensagens
void mqtt_comm_publish(const char *topic, const uint8_t *data, size_t len);

// Subscrição a tópicos com decifragem automática
void mqtt_comm_subscribe(const char *topic, uint8_t decrypt_key);

#endif // MQTT_COMM_H