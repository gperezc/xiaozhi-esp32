// troko_tools.h — herramientas propias de Mi Robot para Xiaozhi (ESP-IDF).
//
// Qué es: registra herramientas MCP (las que la IA puede llamar por voz)
// con los nombres self.troko.*. Es el equivalente en ESP-IDF de
// arduino/Troko/funciones.h: mismos límites (15 hábitos, 20 tareas,
// 8 recordatorios, timer 1-86400 s, pomodoro 25 min) y mismo guardado
// en NVS, para que el robot recuerde aunque se apague.
//
// Cómo se integra (ver INTEGRACION.md):
//   1) Copia troko_tools.h, troko_tools.cc y troko_secrets.h
//      (creado desde troko_secrets.h.example) a
//      main/boards/wk-esp32s3-dev/ del proyecto Xiaozhi.
//      No hace falta tocar main/CMakeLists.txt: las fuentes de la placa
//      se recogen con GLOB (*.cc).
//   2) En wk-esp32s3-dev.cc, dentro de InitializeTools(), añade:
//        static TrokoTools troko_tools(GetDisplay());
//        troko_tools.Register();
//   3) No toques config.h ni los pines de la placa.
//
// Claves (Telegram, Tavily, PC): van en troko_secrets.h, NUNCA en el
// código que se comparte. Sin claves, esas herramientas responden que
// no están configuradas, y el resto sigue funcionando.
#pragma once

#include <string>
#include <vector>

#include <esp_timer.h>

#include "display/display.h"
#include "mcp_server.h"
#include "settings.h"

#if __has_include("troko_secrets.h")
#include "troko_secrets.h"
#endif

#ifndef TROKO_TELEGRAM_TOKEN
#define TROKO_TELEGRAM_TOKEN ""
#endif
#ifndef TROKO_TELEGRAM_CHAT_ID
#define TROKO_TELEGRAM_CHAT_ID ""
#endif
#ifndef TROKO_TAVILY_API_KEY
#define TROKO_TAVILY_API_KEY ""
#endif
#ifndef TROKO_PC_URL
#define TROKO_PC_URL ""
#endif

// Límites (los mismos que en la versión Arduino).
#define TROKO_MAX_HABITOS 15
#define TROKO_MAX_TAREAS 20
#define TROKO_MAX_RECORDATORIOS 8
#define TROKO_MAX_TIMER_SEG 86400
#define TROKO_POMODORO_SEG (25 * 60)

class TrokoTools {
public:
    explicit TrokoTools(Display* display);
    ~TrokoTools();

    // Registra todas las herramientas self.troko.* en el McpServer.
    void Register();

private:
    Display* display_ = nullptr;
    Settings settings_{"troko", true};

    esp_timer_handle_t check_timer_ = nullptr;  // revisa timers/alarmas/recordatorios
    esp_timer_handle_t once_timer_ = nullptr;   // timer o pomodoro en curso
    std::string once_label_ = "";
    bool pomodoro_activo_ = false;

    // --- HTTP (Telegram, Tavily, PC) ---
    // POST JSON a una URL https/http. Devuelve true si HTTP 200.
    bool HttpPostJson(const std::string& url, const std::string& body, std::string& out);
    bool HttpGet(const std::string& url, std::string& out);
    bool TelegramEnviar(const std::string& texto);

    // --- Guardado (NVS, namespace "troko") ---
    // Formato hábito: "titulo|HH:MM|0|1" (0 pendiente, 1 hecho hoy).
    // Formato tarea: "titulo". Formato recordatorio: "<epoch>|texto".
    int Contar(const std::string& prefijo, int maximo);
    int BuscarHueco(const std::string& prefijo, int maximo);
    std::string LeerSlot(const std::string& prefijo, int i);
    void GuardarSlot(const std::string& prefijo, int i, const std::string& valor);
    void BorrarSlot(const std::string& prefijo, int i);

    // --- Tiempo ---
    // "10m" / "2h" / "90" -> segundos desde ahora. -1 si no se entiende.
    long ParseDuracion(const std::string& s);
    // "18:30", "2026-10-22T09:00:00-03:00" o duración -> epoch (segundos Unix). -1 si no.
    long long ParseFecha(const std::string& s);
    bool RelojSincronizado();

    // --- Avisos ---
    void Avisar(const std::string& titulo, const std::string& mensaje, const char* emocion);
    void RevisarVencimientos();  // la llama el timer periódico
    static void CheckTimerCb(void* arg);
    static void OnceTimerCb(void* arg);
    void OnOnceTimer();

    void RegistrarHerramienta(const std::string& nombre, const std::string& descripcion,
                              const PropertyList& props, ToolCallback cb);
};
