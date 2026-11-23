// troko_panel.h — panel web del robot (ESP-IDF).
//
// Sirve el panel en http://<ip-del-robot>/ con esp_http_server:
//   /            -> la página (panel_html.h, generada desde panel-web/index.html)
//   /api/estado  -> JSON con hábitos, tareas, recordatorios, temporizador y pomodoro
//   /cmd y /api/cmd?texto=... -> ejecuta un comando (TrokoTools::EjecutarComando)
//
// Arranca solo desde TrokoTools::Register(). Si el servidor no arranca,
// el resto del robot sigue funcionando igual.
#pragma once

#include "troko_tools.h"

void TrokoPanelStart(TrokoTools* tools);
