# Un chatbot basado en MCP

(Español | [English](https://github.com/78/xiaozhi-esp32/blob/main/README.md) | [中文](README_zh.md) | [日本語](README_ja.md))

> Traducción al español del README original en inglés. Proyecto original: [78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32).

## Introducción

👉 [Humano: dale una cámara a la IA vs IA: descubre al instante que su dueño lleva tres días sin lavarse el pelo](https://www.bilibili.com/video/BV1bpjgzKEhd/)

👉 [Crea tu propia novia IA a mano, guía para principiantes](https://www.bilibili.com/video/BV1XnmFYLEJN/)

Como puerta de entrada a la interacción por voz, el chatbot de IA XiaoZhi aprovecha las capacidades de IA de modelos grandes como Qwen / DeepSeek y logra controlar varias terminales mediante el protocolo MCP.

<img src="docs/mcp-based-graph.jpg" alt="Controla todo con MCP" width="320">

## Actualizaciones recientes

- El proyecto ahora requiere ESP-IDF v6.0.1 o superior. Se recomienda la rama del SDK [ESP-IDF `release/v6.1`](https://github.com/espressif/esp-idf/tree/release/v6.1). ESP-IDF 5.x ya no es compatible. La matriz actual contiene 171 variantes; las compilaciones para ESP32-S31 requieren IDF 6.1 o superior.
- El código criptográfico de MQTT y BluFi migró a PSA Crypto. También se resolvieron las divisiones de componentes de IDF 6 y la compatibilidad con dependencias de terceros.
- Se reforzaron la concurrencia del pipeline de audio, la validación de paquetes MQTT y UDP, y la selección de la matriz de releases.
- ESP32-P4 Rev1 y Rev3 son compatibles con IDF 6 y ESP-SR 2.4.7.

### Funciones implementadas

- Redes Wi-Fi, Ethernet por cable, USB RNDIS y 4G Cat.1 con ML307/EC801E o NT26; las placas compatibles pueden cambiar entre Wi-Fi y 4G
- Despertar por voz sin conexión con [ESP-SR](https://github.com/espressif/esp-sr), con palabras de activación personalizables
- Dos transportes de comunicación: [WebSocket](docs/websocket.md) y [MQTT + UDP](docs/mqtt-udp.md)
- Transmisión de audio Opus con pipelines convencionales de ASR + LLM + TTS por streaming y modelos de voz de extremo a extremo en tiempo real; el hardware con AEC permite interacción full-dúplex en tiempo real
- Reconocimiento del hablante, identifica quién está hablando [3D Speaker](https://github.com/modelscope/3D-Speaker)
- Pantallas OLED / LCD con soporte de emojis y expresiones ricas, además de entrada de visión por cámara en las placas compatibles
- Indicador de batería y gestión de energía
- 39 idiomas de interfaz, con mensajes de voz localizados donde estén disponibles y inglés como alternativa
- Plataformas de chips ESP32, ESP32-C3, ESP32-C5, ESP32-C6, ESP32-S3 y ESP32-P4
- Aprovisionamiento de Wi-Fi mediante hotspot o BluFi
- MCP en el dispositivo para controlar el hardware (altavoz, LED, servos, GPIO, etc.)
- MCP en la nube para ampliar las capacidades del modelo grande (control del hogar inteligente, operación del escritorio de la PC, búsqueda de conocimiento, correo electrónico, etc.)
- Palabras de activación, fuentes, emojis y fondos de chat personalizables, con edición web en línea ([Generador de recursos personalizados](https://github.com/78/xiaozhi-assets-generator))

## Hardware

### Práctica DIY con protoboard

Consulta el tutorial en el documento de Feishu:

👉 ["Enciclopedia del chatbot de IA XiaoZhi"](https://ccnphfhqs21z.feishu.cn/wiki/F5krwD16viZoF0kKkvDcrZNYnhb?from=from_copylink)

Demo con protoboard:

![Demo con protoboard](docs/v1/wiring2.jpg)

### Compatible con 138 directorios de placas y 171 variantes de release (lista parcial)

- <a href="https://oshwhub.com/li-chuang-kai-fa-ban/li-chuang-shi-zhan-pai-esp32-s3-kai-fa-ban" target="_blank" title="LiChuang ESP32-S3 Development Board">Placa de desarrollo LiChuang ESP32-S3</a>
- <a href="https://github.com/espressif/esp-box" target="_blank" title="Espressif ESP32-S3-BOX-3">Espressif ESP32-S3-BOX-3</a>
- <a href="https://docs.m5stack.com/zh_CN/core/CoreS3" target="_blank" title="M5Stack CoreS3">M5Stack CoreS3</a>
- <a href="https://docs.m5stack.com/en/atom/Atomic%20Echo%20Base" target="_blank" title="AtomS3R + Echo Base">M5Stack AtomS3R + Echo Base</a>
- <a href="https://gf.bilibili.com/item/detail/1108782064" target="_blank" title="Magic Button 2.4">Botón Mágico 2.4</a>
- <a href="https://www.waveshare.net/shop/ESP32-S3-Touch-AMOLED-1.8.htm" target="_blank" title="Waveshare ESP32-S3-Touch-AMOLED-1.8">Waveshare ESP32-S3-Touch-AMOLED-1.8</a>
- <a href="https://github.com/Xinyuan-LilyGO/T-Circle-S3" target="_blank" title="LILYGO T-Circle-S3">LILYGO T-Circle-S3</a>
- <a href="https://oshwhub.com/tenclass01/xmini_c3" target="_blank" title="XiaGe Mini C3">XiaGe Mini C3</a>
- <a href="https://oshwhub.com/movecall/cuican-ai-pendant-lights-up-y" target="_blank" title="Movecall CuiCan ESP32S3">Colgante IA CuiCan</a>
- <a href="https://github.com/WMnologo/xingzhi-ai" target="_blank" title="WMnologo-Xingzhi-1.54">WMnologo-Xingzhi-1.54TFT</a>
- <a href="https://www.seeedstudio.com/SenseCAP-Watcher-W1-A-p-5979.html" target="_blank" title="SenseCAP Watcher">SenseCAP Watcher</a>
- <a href="https://www.bilibili.com/video/BV1BHJtz6E2S/" target="_blank" title="ESP-HI Low Cost Robot Dog">Perro robot ESP-HI de bajo costo</a>

<div style="display: flex; justify-content: space-between;">
<a href="docs/v1/lichuang-s3.jpg" target="_blank" title="LiChuang ESP32-S3 Development Board">
<img src="docs/v1/lichuang-s3.jpg" width="240" />
</a>
<a href="docs/v1/espbox3.jpg" target="_blank" title="Espressif ESP32-S3-BOX3">
<img src="docs/v1/espbox3.jpg" width="240" />
</a>
<a href="docs/v1/m5cores3.jpg" target="_blank" title="M5Stack CoreS3">
<img src="docs/v1/m5cores3.jpg" width="240" />
</a>
<a href="docs/v1/atoms3r.jpg" target="_blank" title="AtomS3R + Echo Base">
<img src="docs/v1/atoms3r.jpg" width="240" />
</a>
<a href="docs/v1/magiclick.jpg" target="_blank" title="Magic Button 2.4">
<img src="docs/v1/magiclick.jpg" width="240" />
</a>
<a href="docs/v1/waveshare.jpg" target="_blank" title="Waveshare ESP32-S3-Touch-AMOLED-1.8">
<img src="docs/v1/waveshare.jpg" width="240" />
</a>
<a href="docs/v1/lilygo-t-circle-s3.jpg" target="_blank" title="LILYGO T-Circle-S3">
<img src="docs/v1/lilygo-t-circle-s3.jpg" width="240" />
</a>
<a href="docs/v1/xmini-c3.jpg" target="_blank" title="XiaGe Mini C3">
<img src="docs/v1/xmini-c3.jpg" width="240" />
</a>
<a href="docs/v1/movecall-cuican-esp32s3.jpg" target="_blank" title="CuiCan">
<img src="docs/v1/movecall-cuican-esp32s3.jpg" width="240" />
</a>
<a href="docs/v1/wmnologo_xingzhi_1.54.jpg" target="_blank" title="WMnologo-Xingzhi-1.54">
<img src="docs/v1/wmnologo_xingzhi_1.54.jpg" width="240" />
</a>
<a href="docs/v1/sensecap_watcher.jpg" target="_blank" title="SenseCAP Watcher">
<img src="docs/v1/sensecap_watcher.jpg" width="240" />
</a>
<a href="docs/v1/esp-hi.jpg" target="_blank" title="ESP-HI Low Cost Robot Dog">
<img src="docs/v1/esp-hi.jpg" width="240" />
</a>
</div>

## Software

### Grabación del firmware

Para principiantes, se recomienda usar el firmware que se puede grabar sin configurar un entorno de desarrollo.

El firmware se conecta por defecto al servidor oficial de [xiaozhi.me](https://xiaozhi.me). Los usuarios personales pueden registrar una cuenta para usar gratis el modelo Qwen en tiempo real.

👉 [Guía de grabación de firmware para principiantes](https://ccnphfhqs21z.feishu.cn/wiki/Zpz4wXBtdimBrLk25WdcXzxcnNS)

### Entorno de desarrollo

- Cursor o VSCode
- Instala el plugin de ESP-IDF. El SDK mínimo es [ESP-IDF v6.0.1](https://github.com/espressif/esp-idf/releases/tag/v6.0.1); se recomienda [ESP-IDF `release/v6.1`](https://github.com/espressif/esp-idf/tree/release/v6.1). ESP-IDF 5.x no es compatible.
- Linux es mejor que Windows para compilar más rápido y tener menos problemas de controladores
- Este proyecto usa el estilo de código C++ de Google, por favor respétalo al enviar código

Usa la rama `release/v6.1` en lugar de la etiqueta de release `v6.1`: se reportó un fallo de inicialización de Wi-Fi en ESP32-C3 con esa etiqueta en [#2344](https://github.com/78/xiaozhi-esp32/issues/2344). Después de cambiar la revisión del SDK, ejecuta `git submodule update --init --recursive` en el directorio de ESP-IDF, carga su `export.sh` y ejecuta `idf.py fullclean` en este proyecto antes de volver a compilar.

### Documentación para desarrolladores

- [Guía de placas personalizadas](docs/custom-board.md) - Aprende a crear placas personalizadas para XiaoZhi AI
- [Uso de control IoT con el protocolo MCP](docs/mcp-usage.md) - Aprende a controlar dispositivos IoT mediante el protocolo MCP
- [Flujo de interacción del protocolo MCP](docs/mcp-protocol.md) - Implementación del protocolo MCP en el dispositivo
- [Documento del protocolo híbrido de comunicación MQTT + UDP](docs/mqtt-udp.md)
- [Documento detallado del protocolo de comunicación WebSocket](docs/websocket.md)

## Configuración del modelo grande

Si ya tienes un dispositivo chatbot de IA XiaoZhi y te conectaste al servidor oficial, puedes iniciar sesión en la consola de [xiaozhi.me](https://xiaozhi.me) para configurarlo.

👉 [Video tutorial de operación del backend (interfaz antigua)](https://www.bilibili.com/video/BV1jUCUY2EKM/)

## Proyectos de código abierto relacionados

Para desplegar el servidor en tu propia computadora, consulta los siguientes proyectos de código abierto:

- [xinnan-tech/xiaozhi-esp32-server](https://github.com/xinnan-tech/xiaozhi-esp32-server) Servidor en Python
- [joey-zhou/xiaozhi-esp32-server-java](https://github.com/joey-zhou/xiaozhi-esp32-server-java) Servidor en Java
- [AnimeAIChat/xiaozhi-server-go](https://github.com/AnimeAIChat/xiaozhi-server-go) Servidor en Golang
- [hackers365/xiaozhi-esp32-server-golang](https://github.com/hackers365/xiaozhi-esp32-server-golang) Servidor en Golang

Otros proyectos cliente que usan el protocolo de comunicación de XiaoZhi:

- [huangjunsen0406/py-xiaozhi](https://github.com/huangjunsen0406/py-xiaozhi) Cliente en Python
- [TOM88812/xiaozhi-android-client](https://github.com/TOM88812/xiaozhi-android-client) Cliente para Android
- [100askTeam/xiaozhi-linux](http://github.com/100askTeam/xiaozhi-linux) Cliente para Linux por 100ask
- [78/xiaozhi-sf32](https://github.com/78/xiaozhi-sf32) Firmware para chip Bluetooth de Sichuan
- [QuecPython/solution-xiaozhiAI](https://github.com/QuecPython/solution-xiaozhiAI) Firmware QuecPython de Quectel

Herramientas de recursos personalizados:

- [78/xiaozhi-assets-generator](https://github.com/78/xiaozhi-assets-generator) Generador de recursos personalizados (palabras de activación, fuentes, emojis, fondos)

## Sobre el proyecto

Este es un proyecto ESP32 de código abierto, publicado bajo la licencia MIT, que cualquiera puede usar gratis, incluso con fines comerciales.

Esperamos que este proyecto ayude a todos a entender el desarrollo de hardware con IA y a aplicar los modelos de lenguaje grandes, en rápida evolución, a dispositivos reales de hardware.

Si tienes ideas o sugerencias, no dudes en abrir Issues o unirte a nuestro [Discord](https://discord.gg/C759fGMBcZ) o grupo de QQ: 1095994019

## Historial de estrellas

<a href="https://star-history.com/#78/xiaozhi-esp32&Date">
<picture>
<source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date&theme=dark" />
<source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
<img alt="Star History Chart" src="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
</picture>
</a>
