// atomix_tools.h — herramientas propias de Mi Robot para Xiaozhi (ESP-IDF).
//
// Qué es: registra herramientas MCP (las que la IA puede llamar por voz)
// con los nombres self.atomix.*. Límites: 15 hábitos, 20 tareas,
// 8 recordatorios, timer 1-86400 s, pomodoro 25 min. Los datos se guardan
// en NVS, para que el robot recuerde aunque se apague.
//
// Cómo se integra (ver INTEGRACION.md):
//   1) Copia atomix_tools.h, atomix_tools.cc, atomix_panel.h, atomix_panel.cc,
//      panel_html.h y atomix_secrets.h (creado desde atomix_secrets.h.example)
//      a main/boards/wk-esp32s3-dev/ del proyecto Xiaozhi.
//      No hace falta tocar main/CMakeLists.txt por los .cc: las fuentes de
//      la placa se recogen con GLOB (*.cc). Sí hay que añadir
//      esp_http_client y esp_http_server a main PRIV_REQUIRES.
//   2) En wk-esp32s3-dev.cc, dentro de InitializeTools(), añade:
//        static AtomixTools atomix_tools(GetDisplay());
//        atomix_tools.Register();
//      Register() también arranca el panel web del robot (puerto 80).
//   3) No toques config.h ni los pines de la placa.
//
// Claves (Telegram, Tavily, PC): van en atomix_secrets.h, NUNCA en el
// código que se comparte. Sin claves, esas herramientas responden que
// no están configuradas, y el resto sigue funcionando.
#pragma once

#include <string>
#include <vector>

#include <esp_timer.h>

#include "display/display.h"
#include "mcp_server.h"
#include "settings.h"

#if __has_include("atomix_secrets.h")
#include "atomix_secrets.h"
#endif

#ifndef ATOMIX_TELEGRAM_TOKEN
#define ATOMIX_TELEGRAM_TOKEN ""
#endif
#ifndef ATOMIX_TELEGRAM_CHAT_ID
#define ATOMIX_TELEGRAM_CHAT_ID ""
#endif
#ifndef ATOMIX_TAVILY_API_KEY
#define ATOMIX_TAVILY_API_KEY ""
#endif
#ifndef ATOMIX_PC_URL
#define ATOMIX_PC_URL ""
#endif

// Límites.
#define ATOMIX_MAX_HABITOS 15
#define ATOMIX_MAX_TAREAS 20
#define ATOMIX_MAX_RECORDATORIOS 8
#define ATOMIX_MAX_TIMER_SEG 86400
#define ATOMIX_POMODORO_SEG (25 * 60)

class AtomixTools {
public:
    explicit AtomixTools(Display* display);
    ~AtomixTools();

    // Registra todas las herramientas self.atomix.* en el McpServer
    // y arranca el panel web del robot (atomix_panel.cc).
    void Register();

    // Estado completo en JSON para el panel web (/api/estado).
    std::string EstadoJSON();
    // Ejecuta un comando de texto (los mismos que el panel web y el
    // asistente entienden) y devuelve la respuesta. Lo usa /api/cmd.
    std::string EjecutarComando(const std::string& texto);

private:
    Display* display_ = nullptr;
    Settings settings_{"atomix", true};

    esp_timer_handle_t check_timer_ = nullptr;  // revisa timers/alarmas/recordatorios
    esp_timer_handle_t once_timer_ = nullptr;   // timer o pomodoro en curso
    std::string once_label_ = "";
    bool pomodoro_activo_ = false;
    long once_total_seg_ = 0;       // duración total del timer en curso
    long once_restante_seg_ = 0;    // >0 cuando el timer está en pausa
    int64_t once_inicio_us_ = 0;    // esp_timer_get_time() al arrancar el timer

    // --- HTTP (Telegram, Tavily, PC) ---
    // POST JSON a una URL https/http. Devuelve true si HTTP 200.
    bool HttpPostJson(const std::string& url, const std::string& body, std::string& out);
    bool HttpGet(const std::string& url, std::string& out);
    bool TelegramEnviar(const std::string& texto);

    // --- Guardado (NVS, namespace "atomix") ---
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

    // --- Temporizador (con pausa) ---
    bool CrearOnceTimer(int seconds);   // crea y arranca once_timer_
    long TemporizadorRestanteSeg();     // segundos que quedan (0 si no hay)
    std::string PausarTemporizador();  // pausa, o reanuda si estaba en pausa
    std::string DetenerTemporizador(); // para el timer/pomodoro en curso
    std::string IniciarTemporizador(int seconds, const std::string& label, bool pomodoro);

    // --- Acciones que usan la voz y el panel web ---
    std::string MostrarAnimacion(const std::string& name);
    std::string BuscarWeb(const std::string& query, int max_results);

    // --- Avisos ---
    void Avisar(const std::string& titulo, const std::string& mensaje, const char* emocion);
    void RevisarVencimientos();  // la llama el timer periódico
    static void CheckTimerCb(void* arg);
    static void OnceTimerCb(void* arg);
    void OnOnceTimer();

    void RegistrarHerramienta(const std::string& nombre, const std::string& descripcion,
                              const PropertyList& props, ToolCallback cb);
};
