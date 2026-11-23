// troko_tools.cc — herramientas propias de Mi Robot (Xiaozhi, ESP-IDF).
// Lee troko_tools.h para la descripción y la integración.
#include "troko_tools.h"

#include <ctime>
#include <cstring>
#include <cstdlib>

#include "application.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_log.h"

#include <cJSON.h>

static const char* TAG = "Troko";

TrokoTools::TrokoTools(Display* display) : display_(display) {
    esp_timer_create_args_t args = {
        .callback = &TrokoTools::CheckTimerCb,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "troko_check",
        .skip_unhandled_events = true,
    };
    if (esp_timer_create(&args, &check_timer_) == ESP_OK) {
        esp_timer_start_periodic(check_timer_, 30 * 1000000);  // cada 30 s
    }
}

TrokoTools::~TrokoTools() {
    if (check_timer_) { esp_timer_stop(check_timer_); esp_timer_delete(check_timer_); }
    if (once_timer_) { esp_timer_stop(once_timer_); esp_timer_delete(once_timer_); }
}

void TrokoTools::RegistrarHerramienta(const std::string& nombre, const std::string& descripcion,
                                        const PropertyList& props, ToolCallback cb) {
    McpServer::GetInstance().AddTool(nombre, descripcion, props, std::move(cb));
}

// ---------------------------------------------------------------- HTTP ---
bool TrokoTools::HttpPostJson(const std::string& url, const std::string& body, std::string& out) {
    esp_http_client_config_t cfg = {};
    cfg.url = url.c_str();
    cfg.crt_bundle_attach = esp_crt_bundle_attach;
    cfg.timeout_ms = 15000;
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) return false;
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, body.c_str(), body.size());
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int len = esp_http_client_fetch_headers(client);
        char buf[1024];
        int total = 0;
        while (true) {
            int n = esp_http_client_read_response(client, buf, sizeof(buf) - 1);
            if (n <= 0) break;
            if (total + n < 8192) {
                buf[n] = 0;
                out.append(buf, n);
            }
            total += n;
            (void)len;
        }
    }
    int status = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);
    return err == ESP_OK && status == 200;
}

bool TrokoTools::HttpGet(const std::string& url, std::string& out) {
    esp_http_client_config_t cfg = {};
    cfg.url = url.c_str();
    cfg.crt_bundle_attach = esp_crt_bundle_attach;
    cfg.timeout_ms = 15000;
    esp_http_client_handle_t client = esp_http_client_init(&cfg);
    if (!client) return false;
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        char buf[1024];
        while (true) {
            int n = esp_http_client_read_response(client, buf, sizeof(buf) - 1);
            if (n <= 0) break;
            if (out.size() + (size_t)n < 8192) { buf[n] = 0; out.append(buf, n); }
        }
    }
    int status = esp_http_client_get_status_code(client);
    esp_http_client_cleanup(client);
    return err == ESP_OK && status == 200;
}

bool TrokoTools::TelegramEnviar(const std::string& texto) {
    if (std::string(TROKO_TELEGRAM_TOKEN).empty() || std::string(TROKO_TELEGRAM_CHAT_ID).empty()) {
        ESP_LOGW(TAG, "Telegram no configurado (troko_secrets.h)");
        return false;
    }
    std::string url = std::string("https://api.telegram.org/bot") + TROKO_TELEGRAM_TOKEN + "/sendMessage";
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "chat_id", TROKO_TELEGRAM_CHAT_ID);
    cJSON_AddStringToObject(root, "text", texto.c_str());
    char* body = cJSON_PrintUnformatted(root);
    std::string out;
    bool ok = HttpPostJson(url, body ? body : "{}", out);
    if (body) cJSON_free(body);
    cJSON_Delete(root);
    return ok;
}

// ------------------------------------------------------------- Guardado ---
int TrokoTools::Contar(const std::string& prefijo, int maximo) {
    int n = 0;
    for (int i = 0; i < maximo; i++) {
        if (!LeerSlot(prefijo, i).empty()) n++;
    }
    return n;
}

int TrokoTools::BuscarHueco(const std::string& prefijo, int maximo) {
    for (int i = 0; i < maximo; i++) {
        if (LeerSlot(prefijo, i).empty()) return i;
    }
    return -1;
}

std::string TrokoTools::LeerSlot(const std::string& prefijo, int i) {
    return settings_.GetString(prefijo + std::to_string(i));
}

void TrokoTools::GuardarSlot(const std::string& prefijo, int i, const std::string& valor) {
    settings_.SetString(prefijo + std::to_string(i), valor);
}

void TrokoTools::BorrarSlot(const std::string& prefijo, int i) {
    settings_.EraseKey(prefijo + std::to_string(i));
}

// -------------------------------------------------------------- Tiempo ---
long TrokoTools::ParseDuracion(const std::string& s) {
    if (s.empty()) return -1;
    char* end = nullptr;
    long v = std::strtol(s.c_str(), &end, 10);
    if (v <= 0 || end == s.c_str()) return -1;
    if (*end == 'h' || *end == 'H') return v * 3600;
    if (*end == 'm' || *end == 'M') return v * 60;
    if (*end == '\0') return v;  // segundos
    return -1;
}

long long TrokoTools::ParseFecha(const std::string& s) {
    if (s.empty()) return -1;
    // Duración ("10m", "2h") -> ahora + eso.
    long d = ParseDuracion(s);
    if (d > 0) return (long long)time(nullptr) + d;
    // ISO 8601: 2026-10-22T09:00:00-03:00 (el offset es opcional).
    if (s.size() >= 19 && s[4] == '-' && s[7] == '-' && (s[10] == 'T' || s[10] == ' ')) {
        std::tm tm = {};
        if (strptime(s.c_str(), "%Y-%m-%dT%H:%M:%S", &tm)) {
            long long epoch = (long long)timegm(&tm);
            // Offset de zona: -03:00 -> restar (-3h), es decir sumar 3h.
            if (s.size() >= 25 && (s[22] == '+' || s[22] == '-')) {
                int oh = std::atoi(s.substr(23, 2).c_str());
                int om = std::atoi(s.substr(26, 2).c_str());
                long long off = oh * 3600 + om * 60;
                epoch -= (s[22] == '-') ? -off : off;
            }
            return epoch;
        }
    }
    // "HH:MM" de hoy (o mañana si ya pasó).
    if (s.size() == 5 && s[2] == ':') {
        std::time_t now = time(nullptr);
        std::tm tm;
        localtime_r(&now, &tm);
        tm.tm_hour = std::atoi(s.substr(0, 2).c_str());
        tm.tm_min = std::atoi(s.substr(3, 2).c_str());
        tm.tm_sec = 0;
        std::time_t t = mktime(&tm);
        if (t <= now) t += 24 * 3600;
        return (long long)t;
    }
    return -1;
}

bool TrokoTools::RelojSincronizado() { return time(nullptr) > 1700000000; }

// -------------------------------------------------------------- Avisos ---
void TrokoTools::Avisar(const std::string& titulo, const std::string& mensaje, const char* emocion) {
    if (display_) {
        display_->ShowNotification(mensaje);
        display_->SetEmotion(emocion);
    }
    Application::GetInstance().Alert(titulo.c_str(), mensaje.c_str(), emocion);
}

void TrokoTools::CheckTimerCb(void* arg) {
    static_cast<TrokoTools*>(arg)->RevisarVencimientos();
}

void TrokoTools::OnceTimerCb(void* arg) {
    static_cast<TrokoTools*>(arg)->OnOnceTimer();
}

void TrokoTools::OnOnceTimer() {
    std::string label = once_label_;
    bool pomodoro = pomodoro_activo_;
    pomodoro_activo_ = false;
    once_label_.clear();
    // La voz y la pantalla se tocan desde la tarea principal, no desde el timer.
    Application::GetInstance().Schedule([this, label, pomodoro] {
        std::string msg = pomodoro ? ("Pomodoro terminado: " + label + ". Descansa 5 minutos.")
                                   : (label + " ¡Ya es la hora!");
        Avisar("Troko", msg, "happy");
        TelegramEnviar(msg);
    });
}

void TrokoTools::RevisarVencimientos() {
    if (!RelojSincronizado()) return;
    long long now = (long long)time(nullptr);
    // Alarma (un disparo).
    long long alarm = settings_.GetInt("alarm_epoch", 0);
    if (alarm > 0 && now >= alarm) {
        settings_.SetInt("alarm_epoch", 0);
        Application::GetInstance().Schedule([this] {
            Avisar("Troko", "¡Alarma! Ya es la hora.", "happy");
            TelegramEnviar("¡Alarma! Ya es la hora.");
        });
    }
    // Recordatorios (un disparo cada uno).
    for (int i = 0; i < TROKO_MAX_RECORDATORIOS; i++) {
        std::string slot = LeerSlot("rec", i);
        if (slot.empty()) continue;
        size_t sep = slot.find('|');
        if (sep == std::string::npos) continue;
        long long epoch = std::atoll(slot.substr(0, sep).c_str());
        if (epoch > 0 && now >= epoch) {
            std::string texto = slot.substr(sep + 1);
            BorrarSlot("rec", i);
            Application::GetInstance().Schedule([this, texto] {
                Avisar("Troko", "Recordatorio: " + texto, "happy");
                TelegramEnviar("Recordatorio: " + texto);
            });
        }
    }
}

// ------------------------------------------------------------ Registro ---
void TrokoTools::Register() {
    // --- Hábitos ---
    RegistrarHerramienta("self.troko.habit_add", "Añade un hábito diario (máx 15).",
        PropertyList({Property("title", kPropertyTypeString),
                      Property("time", kPropertyTypeString, std::string(""))}),
        [this](const PropertyList& p) -> ReturnValue {
            std::string title = p["title"].value<std::string>();
            std::string hora = p["time"].value<std::string>();
            int hueco = BuscarHueco("hab", TROKO_MAX_HABITOS);
            if (hueco < 0) return std::string("Ya tienes 15 hábitos, borra uno primero.");
            GuardarSlot("hab", hueco, title + "|" + hora + "|0");
            return std::string("Hábito guardado: ") + title;
        });
    RegistrarHerramienta("self.troko.habit_list", "Lista los hábitos y si están hechos hoy.",
        PropertyList(),
        [this](const PropertyList&) -> ReturnValue {
            std::string out;
            for (int i = 0; i < TROKO_MAX_HABITOS; i++) {
                std::string s = LeerSlot("hab", i);
                if (s.empty()) continue;
                size_t a = s.find('|'), b = s.rfind('|');
                std::string titulo = s.substr(0, a);
                std::string hora = (a != std::string::npos && b > a) ? s.substr(a + 1, b - a - 1) : "";
                bool hecho = b != std::string::npos && s.substr(b + 1) == "1";
                out += std::to_string(i) + ". " + titulo + (hora.empty() ? "" : " (" + hora + ")") +
                       (hecho ? " — hecho hoy\n" : " — pendiente\n");
            }
            return out.empty() ? std::string("No tienes hábitos guardados.") : out;
        });
    RegistrarHerramienta("self.troko.habit_complete", "Marca un hábito como hecho hoy, por su título.",
        PropertyList({Property("title", kPropertyTypeString)}),
        [this](const PropertyList& p) -> ReturnValue {
            std::string title = p["title"].value<std::string>();
            for (int i = 0; i < TROKO_MAX_HABITOS; i++) {
                std::string s = LeerSlot("hab", i);
                if (!s.empty() && s.substr(0, s.find('|')) == title) {
                    size_t b = s.rfind('|');
                    GuardarSlot("hab", i, s.substr(0, b + 1) + "1");
                    if (display_) display_->ShowNotification("¡Hábito completado!");
                    return std::string("¡Bien! Hábito completado: ") + title;
                }
            }
            return std::string("No encontré un hábito llamado ") + title;
        });

    // --- Tareas ---
    RegistrarHerramienta("self.troko.task_add", "Añade una tarea pendiente (máx 20).",
        PropertyList({Property("title", kPropertyTypeString)}),
        [this](const PropertyList& p) -> ReturnValue {
            std::string title = p["title"].value<std::string>();
            int hueco = BuscarHueco("task", TROKO_MAX_TAREAS);
            if (hueco < 0) return std::string("Ya tienes 20 tareas, completa una primero.");
            GuardarSlot("task", hueco, title);
            return std::string("Tarea guardada: ") + title;
        });
    RegistrarHerramienta("self.troko.task_list", "Lista las tareas pendientes con su número.",
        PropertyList(),
        [this](const PropertyList&) -> ReturnValue {
            std::string out;
            for (int i = 0; i < TROKO_MAX_TAREAS; i++) {
                std::string s = LeerSlot("task", i);
                if (!s.empty()) out += std::to_string(i) + ". " + s + "\n";
            }
            return out.empty() ? std::string("No tienes tareas pendientes.") : out;
        });
    RegistrarHerramienta("self.troko.task_complete", "Completa (borra) una tarea por su número.",
        PropertyList({Property("id", kPropertyTypeInteger, 0, 0, TROKO_MAX_TAREAS - 1)}),
        [this](const PropertyList& p) -> ReturnValue {
            int id = p["id"].value<int>();
            if (LeerSlot("task", id).empty()) return std::string("No hay tarea con ese número.");
            BorrarSlot("task", id);
            return std::string("Tarea completada.");
        });

    // --- Recordatorios y alarma ---
    RegistrarHerramienta("self.troko.reminder_add",
        "Añade un recordatorio (máx 8). due_at admite '10m', '2h', 'HH:MM' o fecha ISO.",
        PropertyList({Property("due_at", kPropertyTypeString), Property("text", kPropertyTypeString)}),
        [this](const PropertyList& p) -> ReturnValue {
            long long epoch = ParseFecha(p["due_at"].value<std::string>());
            if (epoch <= 0) return std::string("No entendí la fecha. Prueba '10m', '18:30' o una fecha completa.");
            if (!RelojSincronizado()) return std::string("El reloj aún no está sincronizado, inténtalo en un momento.");
            int hueco = BuscarHueco("rec", TROKO_MAX_RECORDATORIOS);
            if (hueco < 0) return std::string("Ya tienes 8 recordatorios, cancela uno primero.");
            GuardarSlot("rec", hueco, std::to_string(epoch) + "|" + p["text"].value<std::string>());
            return std::string("Recordatorio guardado.");
        });
    RegistrarHerramienta("self.troko.reminder_list", "Lista los recordatorios pendientes.",
        PropertyList(),
        [this](const PropertyList&) -> ReturnValue {
            std::string out;
            for (int i = 0; i < TROKO_MAX_RECORDATORIOS; i++) {
                std::string s = LeerSlot("rec", i);
                if (s.empty()) continue;
                size_t sep = s.find('|');
                long long epoch = std::atoll(s.substr(0, sep).c_str());
                char buf[32];
                std::time_t t = (std::time_t)epoch;
                std::tm tm;
                localtime_r(&t, &tm);
                std::strftime(buf, sizeof(buf), "%d/%m %H:%M", &tm);
                out += std::to_string(i) + ". " + buf + " — " + s.substr(sep + 1) + "\n";
            }
            return out.empty() ? std::string("No tienes recordatorios.") : out;
        });
    RegistrarHerramienta("self.troko.alarm_set", "Pone una alarma a una hora (24 h).",
        PropertyList({Property("hour", kPropertyTypeInteger, 7, 0, 23),
                      Property("minute", kPropertyTypeInteger, 0, 0, 59)}),
        [this](const PropertyList& p) -> ReturnValue {
            if (!RelojSincronizado()) return std::string("El reloj aún no está sincronizado, inténtalo en un momento.");
            std::time_t now = time(nullptr);
            std::tm tm;
            localtime_r(&now, &tm);
            tm.tm_hour = p["hour"].value<int>();
            tm.tm_min = p["minute"].value<int>();
            tm.tm_sec = 0;
            std::time_t t = mktime(&tm);
            if (t <= now) t += 24 * 3600;
            settings_.SetInt("alarm_epoch", (int32_t)t);
            char buf[32];
            std::strftime(buf, sizeof(buf), "%H:%M", &tm);
            return std::string("Alarma puesta a las ") + buf;
        });
    RegistrarHerramienta("self.troko.alarm_cancel", "Cancela la alarma.",
        PropertyList(),
        [this](const PropertyList&) -> ReturnValue {
            settings_.SetInt("alarm_epoch", 0);
            return std::string("Alarma cancelada.");
        });

    // --- Temporizador y Pomodoro ---
    auto start_once = [this](int seconds, const std::string& label, bool pomodoro) -> std::string {
        if (seconds < 1 || seconds > TROKO_MAX_TIMER_SEG) {
            return "El tiempo debe estar entre 1 segundo y 24 horas.";
        }
        if (once_timer_) { esp_timer_stop(once_timer_); esp_timer_delete(once_timer_); }
        esp_timer_create_args_t args = {
            .callback = &TrokoTools::OnceTimerCb,
            .arg = this,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "troko_once",
            .skip_unhandled_events = true,
        };
        if (esp_timer_create(&args, &once_timer_) != ESP_OK) return "No pude crear el temporizador.";
        once_label_ = label;
        pomodoro_activo_ = pomodoro;
        esp_timer_start_once(once_timer_, (uint64_t)seconds * 1000000);
        return label + " en marcha.";
    };
    RegistrarHerramienta("self.troko.timer_set", "Pone un temporizador (segundos, 1 a 86400).",
        PropertyList({Property("seconds", kPropertyTypeInteger, 60, 1, TROKO_MAX_TIMER_SEG),
                      Property("label", kPropertyTypeString, std::string("Temporizador"))}),
        [start_once](const PropertyList& p) -> ReturnValue {
            return start_once(p["seconds"].value<int>(), p["label"].value<std::string>(), false);
        });
    RegistrarHerramienta("self.troko.pomodoro_start", "Empieza un pomodoro de 25 minutos.",
        PropertyList(),
        [start_once](const PropertyList&) -> ReturnValue {
            return start_once(TROKO_POMODORO_SEG, "Pomodoro", true);
        });
    RegistrarHerramienta("self.troko.pomodoro_stop", "Para el temporizador o el pomodoro en curso.",
        PropertyList(),
        [this](const PropertyList&) -> ReturnValue {
            if (once_timer_) { esp_timer_stop(once_timer_); esp_timer_delete(once_timer_); }
            once_timer_ = nullptr;
            pomodoro_activo_ = false;
            once_label_.clear();
            return std::string("Temporizador parado.");
        });

    // --- Telegram ---
    RegistrarHerramienta("self.troko.telegram_send", "Envía un mensaje a tu Telegram.",
        PropertyList({Property("text", kPropertyTypeString)}),
        [this](const PropertyList& p) -> ReturnValue {
            std::string text = p["text"].value<std::string>();
            if (TelegramEnviar(text)) return std::string("Mensaje enviado a Telegram.");
            return std::string("No pude enviarlo. Revisa el token y el chat_id en troko_secrets.h.");
        });

    // --- Búsqueda web (Tavily) ---
    RegistrarHerramienta("self.troko.web_search", "Busca en internet con Tavily y devuelve la respuesta.",
        PropertyList({Property("query", kPropertyTypeString),
                      Property("max_results", kPropertyTypeInteger, 3, 1, 5)}),
        [this](const PropertyList& p) -> ReturnValue {
            if (std::string(TROKO_TAVILY_API_KEY).empty()) {
                return std::string("La búsqueda no está configurada. Pon tu clave de Tavily en troko_secrets.h.");
            }
            std::string query = p["query"].value<std::string>();
            cJSON* root = cJSON_CreateObject();
            cJSON_AddStringToObject(root, "api_key", TROKO_TAVILY_API_KEY);
            cJSON_AddStringToObject(root, "query", query.c_str());
            cJSON_AddNumberToObject(root, "max_results", p["max_results"].value<int>());
            cJSON_AddBoolToObject(root, "include_answer", true);
            char* body = cJSON_PrintUnformatted(root);
            std::string resp;
            bool ok = HttpPostJson("https://api.tavily.com/search", body ? body : "{}", resp);
            if (body) cJSON_free(body);
            cJSON_Delete(root);
            if (!ok) return std::string("La búsqueda falló, inténtalo de nuevo.");
            cJSON* json = cJSON_Parse(resp.c_str());
            if (!json) return std::string("La búsqueda devolvió una respuesta que no entendí.");
            std::string out;
            cJSON* answer = cJSON_GetObjectItemCaseSensitive(json, "answer");
            if (cJSON_IsString(answer)) out = answer->valuestring;
            cJSON* results = cJSON_GetObjectItemCaseSensitive(json, "results");
            if (cJSON_IsArray(results)) {
                int n = cJSON_GetArraySize(results);
                for (int i = 0; i < n && i < 3; i++) {
                    cJSON* r = cJSON_GetArrayItem(results, i);
                    cJSON* title = cJSON_GetObjectItemCaseSensitive(r, "title");
                    cJSON* content = cJSON_GetObjectItemCaseSensitive(r, "content");
                    if (cJSON_IsString(title)) { out += "\n- "; out += title->valuestring; }
                    if (cJSON_IsString(content)) { out += ": "; out += std::string(content->valuestring).substr(0, 200); }
                }
            }
            cJSON_Delete(json);
            return out.empty() ? std::string("No encontré nada sobre eso.") : out;
        });

    // --- Animación de pantalla ---
    RegistrarHerramienta("self.troko.show_animation",
        "Muestra una animación en la pantalla: 240, pelota, ovejas, calma, descubre, "
        "guino, enamorado, enojado, mareado, llorando, verguenza, pensando, riendo, "
        "triste, feliz, genial, oops, seguro, risa_apenada, shock o normal.",
        PropertyList({Property("name", kPropertyTypeString)}),
        [this](const PropertyList& p) -> ReturnValue {
            std::string name = p["name"].value<std::string>();
            // Nombre de animación -> emoción del paquete de assets de Xiaozhi.
            // Si el paquete no trae esa emoción, SetEmotion devuelve false
            // y la herramienta lo dice. Si añades tus GIF propios al
            // assets, cambia aquí el mapeo.
            const char* emocion = nullptr;
            if (name == "240" || name == "genial") emocion = "cool";
            else if (name == "pelota" || name == "feliz") emocion = "happy";
            else if (name == "ovejas") emocion = "sleepy";
            else if (name == "calma") emocion = "relaxed";
            else if (name == "descubre") emocion = "surprised";
            else if (name == "guino") emocion = "winking";
            else if (name == "enamorado") emocion = "loving";
            else if (name == "enojado") emocion = "angry";
            else if (name == "mareado") emocion = "dizzy";
            else if (name == "llorando") emocion = "crying";
            else if (name == "verguenza") emocion = "embarrassed";
            else if (name == "pensando") emocion = "thinking";
            else if (name == "riendo") emocion = "laughing";
            else if (name == "triste") emocion = "sad";
            else if (name == "normal") emocion = "neutral";
            // Las 4 nuevas no tienen emoción propia en el paquete de Xiaozhi:
            // se acercan a la más parecida que sí trae.
            else if (name == "oops" || name == "shock") emocion = "surprised";
            else if (name == "seguro") emocion = "cool";
            else if (name == "risa_apenada") emocion = "embarrassed";
            else return std::string("Animación no conocida. Prueba: 240, pelota, ovejas, calma, descubre, "
                                    "guino, enamorado, enojado, mareado, llorando, verguenza, pensando, "
                                    "riendo, triste, feliz, genial, oops, seguro, risa_apenada, shock o normal.");
            if (display_ && display_->SetEmotion(emocion)) {
                return std::string("Mostrando animación: ") + name;
            }
            return std::string("No pude mostrar esa animación en esta pantalla.");
        });

    // --- PC (companion en anexos/pc_companion.py) ---
    RegistrarHerramienta("self.troko.pc_status", "Pide el estado de tu PC (CPU, RAM, batería).",
        PropertyList(),
        [this](const PropertyList&) -> ReturnValue {
            if (std::string(TROKO_PC_URL).empty()) {
                return std::string("No hay PC configurada. Pon TROKO_PC_URL en troko_secrets.h.");
            }
            std::string resp;
            if (!HttpGet(std::string(TROKO_PC_URL) + "/status", resp)) {
                return std::string("No pude contactar con tu PC. ¿Está encendido el companion?");
            }
            return resp;
        });
    RegistrarHerramienta("self.troko.pc_clean_desktop", "Pide a tu PC que limpie el escritorio.",
        PropertyList(),
        [this](const PropertyList&) -> ReturnValue {
            if (std::string(TROKO_PC_URL).empty()) {
                return std::string("No hay PC configurada. Pon TROKO_PC_URL en troko_secrets.h.");
            }
            std::string resp;
            if (!HttpGet(std::string(TROKO_PC_URL) + "/clean", resp)) {
                return std::string("No pude contactar con tu PC. ¿Está encendido el companion?");
            }
            return resp.empty() ? std::string("Limpieza pedida.") : resp;
        });

    ESP_LOGI(TAG, "Herramientas Troko registradas");
}
