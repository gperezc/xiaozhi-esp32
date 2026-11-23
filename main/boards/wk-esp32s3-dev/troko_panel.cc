// troko_panel.cc — servidor web del panel del robot (ESP-IDF).
// Lee troko_panel.h. La página es panel_html.h, generada con:
//   python3 panel-web/generar.py
#include "troko_panel.h"
#include "panel_html.h"

#include <esp_http_server.h>
#include <esp_log.h>

static const char* TAG = "TrokoPanel";
static TrokoTools* g_tools = nullptr;

static esp_err_t PanelRootHandler(httpd_req_t* req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, PANEL_HTML, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t PanelEstadoHandler(httpd_req_t* req) {
    if (!g_tools) return httpd_resp_send_500(req);
    std::string json = g_tools->EstadoJSON();
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json.c_str(), json.size());
}

static esp_err_t PanelCmdHandler(httpd_req_t* req) {
    if (!g_tools) return httpd_resp_send_500(req);
    char query[512];
    std::string texto;
    if (httpd_req_get_url_query_str(req, query, sizeof(query)) == ESP_OK) {
        char val[256];
        if (httpd_query_key_value(query, "texto", val, sizeof(val)) == ESP_OK) texto = val;
    }
    std::string resp = g_tools->EjecutarComando(texto);
    httpd_resp_set_type(req, "text/plain");
    return httpd_resp_send(req, resp.c_str(), resp.size());
}

void TrokoPanelStart(TrokoTools* tools) {
    g_tools = tools;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.stack_size = 8192;
    config.max_uri_handlers = 8;
    httpd_handle_t server = nullptr;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGW(TAG, "No pude arrancar el servidor web del panel");
        return;
    }
    httpd_uri_t root = {.uri = "/", .method = HTTP_GET, .handler = PanelRootHandler, .user_ctx = nullptr};
    httpd_uri_t estado = {.uri = "/api/estado", .method = HTTP_GET, .handler = PanelEstadoHandler, .user_ctx = nullptr};
    httpd_uri_t cmd = {.uri = "/cmd", .method = HTTP_GET, .handler = PanelCmdHandler, .user_ctx = nullptr};
    httpd_uri_t cmd_api = {.uri = "/api/cmd", .method = HTTP_GET, .handler = PanelCmdHandler, .user_ctx = nullptr};
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &estado);
    httpd_register_uri_handler(server, &cmd);
    httpd_register_uri_handler(server, &cmd_api);
    ESP_LOGI(TAG, "Panel web en http://<ip-del-robot>/");
}
