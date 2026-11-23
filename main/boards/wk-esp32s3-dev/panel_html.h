#pragma once
// panel_html.h — HTML del panel web, generado desde panel-web/index.html.
// No editar a mano: edita panel-web/index.html y corre: python3 panel-web/generar.py
const char PANEL_HTML[] = R"PANEL_EOF(<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1, viewport-fit=cover">
<title>Atomix — Panel</title>
<style>
  :root{
    --bg:#050505; --card:#121212; --card2:#1a1a1a; --line:#262626;
    --txt:#ffffff; --muted:#9a9a9a; --lime:#b8f345; --lime-dim:#8fc72e;
    --purple:#a855f7; --purple-soft:rgba(168,85,247,.16);
    --radius:22px;
  }
  *{box-sizing:border-box; -webkit-tap-highlight-color:transparent;}
  html,body{margin:0; padding:0; background:#000;}
  body{
    font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,Helvetica,Arial,sans-serif;
    color:var(--txt); background:#000;
    display:flex; justify-content:center; min-height:100dvh;
  }
  #app{
    width:100%; max-width:430px; min-height:100dvh; background:var(--bg);
    border:2px solid var(--lime); border-radius:28px;
    display:flex; flex-direction:column; overflow:hidden;
    position:relative; padding-bottom:86px;
  }
  /* Barra de estado */
  #statusbar{
    display:flex; justify-content:space-between; align-items:center;
    padding:14px 20px 4px; font-size:15px; font-weight:600;
  }
  #statusbar .icons{display:flex; align-items:center; gap:8px;}
  /* Cabecera de pantalla */
  .screen{padding:10px 18px 0; display:none;}
  .screen.active{display:block;}
  .screen h1{font-size:29px; margin:6px 0 14px; display:flex; align-items:center; gap:12px;}
  .screen h1 .count{margin-left:auto; font-size:22px; font-weight:800;}
  .icon-chip{
    width:40px; height:40px; border-radius:12px; background:var(--purple-soft);
    display:inline-flex; align-items:center; justify-content:center; flex:none;
  }
  .icon-chip svg{width:24px; height:24px; stroke:var(--purple); fill:none; stroke-width:2; stroke-linecap:round; stroke-linejoin:round;}
  /* Segmentos Hoy / Semana / Todos */
  .seg{
    display:flex; background:var(--card2); border:1px solid var(--line);
    border-radius:999px; padding:4px; margin-bottom:18px;
  }
  .seg button{
    flex:1; border:0; background:transparent; color:var(--muted);
    font-size:16px; font-weight:600; padding:9px 0; border-radius:999px; cursor:pointer;
  }
  .seg button.active{background:var(--lime); color:#000;}
  /* Tarjetas */
  .card{
    background:var(--card); border:1px solid var(--line);
    border-radius:var(--radius); padding:20px; margin-bottom:14px;
  }
  .muted{color:var(--muted);}
  /* Anillo de progreso */
  .ring-wrap{display:flex; align-items:center; gap:18px;}
  .ring{position:relative; width:158px; height:158px; flex:none;}
  .ring svg{transform:rotate(-90deg);}
  .ring .ring-bg{stroke:#2b2b2b;}
  .ring .ring-fg{stroke:var(--lime); stroke-linecap:round; transition:stroke-dashoffset .6s ease;}
  .ring .ring-fg.purple{stroke:var(--purple);}
  .ring-center{
    position:absolute; inset:0; display:flex; flex-direction:column;
    align-items:center; justify-content:center; text-align:center;
  }
  .ring-center b{font-size:34px; line-height:1;}
  .ring-center span{color:var(--muted); font-size:15px; margin-top:4px;}
  .pct{font-size:26px; font-weight:800; color:var(--lime);}
  .pct small{display:block; font-size:14px; color:var(--muted); font-weight:500; margin-top:2px;}
  /* Mensaje con ojitos */
  .cheer{display:flex; gap:12px; align-items:center;}
  .cheer .mini-eyes{display:flex; gap:5px; flex:none;}
  .cheer .mini-eyes i{
    display:block; width:22px; height:26px; background:#fff; border-radius:50%;
    position:relative;
  }
  .cheer .mini-eyes i::after{
    content:""; position:absolute; width:8px; height:10px; background:#000;
    border-radius:50%; left:9px; top:10px;
  }
  .cheer p{margin:0; font-size:16px; line-height:1.35;}
  .cheer p b{font-size:17px;}
  /* Listas */
  .row{
    display:flex; align-items:center; gap:13px;
    background:var(--card); border:1px solid var(--line);
    border-radius:18px; padding:13px 14px; margin-bottom:11px; cursor:pointer;
  }
  .row .meta{flex:1; min-width:0;}
  .row .meta b{display:block; font-size:17px; font-weight:600; white-space:nowrap; overflow:hidden; text-overflow:ellipsis;}
  .row .meta span{color:var(--muted); font-size:14px;}
  .row.done .meta b{color:var(--muted);}
  .check{
    width:36px; height:36px; border-radius:50%; flex:none;
    border:2px solid #555; background:transparent; cursor:pointer;
    display:flex; align-items:center; justify-content:center; padding:0;
  }
  .check svg{width:20px; height:20px; stroke:#000; stroke-width:3; fill:none; stroke-linecap:round; stroke-linejoin:round;}
  .row.done .check{background:var(--lime); border-color:var(--lime);}
  .check.off{cursor:default;}
  /* Temporizador */
  .timer-ring{width:230px; height:230px; margin:6px auto 4px;}
  .timer-ring .ring-center b{font-size:46px;}
  .timer-ring .ring-center span{font-size:17px;}
  .timer-btns{display:flex; justify-content:center; gap:26px; margin:18px 0 8px;}
  .tbtn{
    width:66px; height:66px; border-radius:50%; cursor:pointer;
    border:2px solid var(--purple); background:rgba(168,85,247,.14);
    display:flex; align-items:center; justify-content:center;
  }
  .tbtn svg{width:26px; height:26px; fill:var(--purple);}
  .tbtn.stop{border-color:#555; background:transparent;}
  .tbtn.stop svg{fill:#fff;}
  .chips{display:flex; gap:8px; flex-wrap:wrap; justify-content:center; margin-top:14px;}
  .chip{
    border:1px solid var(--line); background:var(--card2); color:var(--txt);
    border-radius:999px; padding:9px 15px; font-size:15px; font-weight:600; cursor:pointer;
  }
  .chip:hover{border-color:var(--lime);}
  /* Recordatorio */
  .big-eyes{display:flex; justify-content:center; gap:16px; margin:18px 0 6px;}
  .big-eyes i{
    display:block; width:74px; height:88px; background:#fff; border-radius:50%;
    position:relative; animation:blink 5s infinite;
  }
  .big-eyes i::after{
    content:""; position:absolute; width:26px; height:32px; background:#000;
    border-radius:50%; left:30px; top:36px;
  }
  @keyframes blink{0%,92%,100%{transform:scaleY(1);} 95%{transform:scaleY(.08);}}
  .rem-title{text-align:center; font-size:17px; margin:10px 0 2px;}
  .rem-name{text-align:center; font-size:34px; font-weight:800; margin:2px 0;}
  .rem-dur{text-align:center; color:var(--muted); font-size:17px; margin:0 0 20px;}
  .rem-actions{display:flex; gap:9px;}
  .rem-actions button{
    flex:1; border-radius:14px; padding:14px 6px; font-size:15px; font-weight:700;
    cursor:pointer; border:1px solid var(--line); background:var(--card2); color:var(--txt);
  }
  .rem-actions button.primary{background:var(--lime); color:#000; border-color:var(--lime);}
  /* Comando libre */
  .cmdbox{display:flex; gap:8px; margin-top:6px;}
  .cmdbox input{
    flex:1; background:var(--card2); border:1px solid var(--line); color:var(--txt);
    border-radius:14px; padding:12px 14px; font-size:15px; outline:none;
  }
  .cmdbox button{
    background:var(--lime); color:#000; border:0; border-radius:14px;
    padding:0 18px; font-size:15px; font-weight:700; cursor:pointer;
  }
  /* Navegación inferior */
  #nav{
    position:fixed; bottom:0; left:50%; transform:translateX(-50%);
    width:100%; max-width:430px; background:rgba(10,10,10,.96);
    border-top:1px solid var(--line); border-left:2px solid var(--lime); border-right:2px solid var(--lime);
    border-bottom:2px solid var(--lime); border-radius:0 0 26px 26px;
    display:flex; padding:8px 4px calc(10px + env(safe-area-inset-bottom));
    z-index:10;
  }
  #nav button{
    flex:1; background:none; border:0; color:var(--muted); cursor:pointer;
    font-size:11px; font-weight:600; display:flex; flex-direction:column;
    align-items:center; gap:4px; padding:4px 0;
  }
  #nav button svg{width:24px; height:24px; stroke:currentColor; fill:none; stroke-width:2; stroke-linecap:round; stroke-linejoin:round;}
  #nav button.active{color:var(--lime);}
  /* Toast */
  #toast{
    position:fixed; left:50%; bottom:96px; transform:translateX(-50%) translateY(20px);
    background:#222; color:#fff; border:1px solid var(--line); border-radius:12px;
    padding:11px 16px; font-size:14px; max-width:86%; opacity:0; pointer-events:none;
    transition:opacity .25s, transform .25s; z-index:20; text-align:center;
  }
  #toast.show{opacity:1; transform:translateX(-50%) translateY(0);}
  .empty{color:var(--muted); text-align:center; padding:26px 10px; font-size:15px;}
  .dots{display:flex; gap:5px; margin-top:6px;}
  .dots i{width:9px; height:9px; border-radius:50%; background:#333; display:block;}
  .dots i.done{background:var(--lime);}
  .dots i.today{outline:2px solid var(--lime); outline-offset:1px;}
  .week-row{display:flex; align-items:center; justify-content:space-between; gap:10px;}
  .week-row b{font-size:16px; font-weight:600;}
</style>
</head>
<body>
<div id="app">
  <div id="statusbar">
    <span class="icons">
      <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="#fff" stroke-width="2" stroke-linecap="round"><path d="M5 12.55a11 11 0 0 1 14.08 0"/><path d="M8.53 16.11a6 6 0 0 1 6.95 0"/><circle cx="12" cy="19.5" r="1" fill="#fff"/></svg>
    </span>
    <span id="clock">20:28</span>
    <span class="icons">
      <svg width="28" height="20" viewBox="0 0 28 20"><rect x="1" y="4" width="22" height="12" rx="3" fill="none" stroke="#888" stroke-width="1.5"/><rect x="3" y="6" width="16" height="8" rx="1.5" fill="#b8f345"/><rect x="24.5" y="7.5" width="2.5" height="5" rx="1" fill="#888"/></svg>
    </span>
  </div>

  <!-- HÁBITOS -->
  <section class="screen active" id="screen-habitos">
    <h1>
      <span class="icon-chip"><svg viewBox="0 0 24 24"><path d="M4 20V10M10 20V4M16 20v-7M22 20H2"/></svg></span>
      Hábitos
    </h1>
    <div class="seg" id="habit-seg">
      <button class="active" data-view="hoy">Hoy</button>
      <button data-view="semana">Semana</button>
      <button data-view="todos">Todos</button>
    </div>
    <div class="card">
      <div class="ring-wrap">
        <div class="ring">
          <svg width="158" height="158" viewBox="0 0 158 158">
            <circle class="ring-bg" cx="79" cy="79" r="66" fill="none" stroke-width="12"/>
            <circle class="ring-fg" id="habit-ring" cx="79" cy="79" r="66" fill="none" stroke-width="12"
              stroke-dasharray="414.7" stroke-dashoffset="414.7"/>
          </svg>
          <div class="ring-center"><b id="habit-count">0/0</b><span>hábitos</span></div>
        </div>
        <div>
          <div class="pct"><span id="habit-pct">0%</span><small>completado</small></div>
        </div>
      </div>
    </div>
    <div class="card cheer">
      <span class="mini-eyes"><i></i><i></i></span>
      <p id="habit-msg"><b>¡Vas muy bien!</b><br><span class="muted">Aún te quedan hábitos por hoy.</span></p>
    </div>
    <div id="habit-extra"></div>
  </section>

  <!-- HOY -->
  <section class="screen" id="screen-hoy">
    <h1>
      <span class="icon-chip"><svg viewBox="0 0 24 24"><rect x="3" y="4" width="18" height="17" rx="2"/><path d="M16 2v4M8 2v4M3 9h18"/></svg></span>
      Hoy <span class="count" id="hoy-count">0/0</span>
    </h1>
    <div id="hoy-list"></div>
    <h1 style="font-size:22px; margin-top:18px;">Tareas</h1>
    <div id="tareas-list"></div>
    <div class="cmdbox">
      <input id="add-habito-nombre" type="text" placeholder="Nuevo hábito…" autocomplete="off">
      <input id="add-habito-hora" type="text" placeholder="21:00" style="flex:0 0 76px; text-align:center;" autocomplete="off">
      <button id="add-habito-btn">Añadir</button>
    </div>
    <div class="cmdbox">
      <input id="add-tarea-texto" type="text" placeholder="Nueva tarea…" autocomplete="off">
      <button id="add-tarea-btn">Añadir</button>
    </div>
    <div class="cmdbox">
      <input id="cmd-input" type="text" placeholder="Escribe un comando… (ayuda)" autocomplete="off">
      <button id="cmd-send">Enviar</button>
    </div>
  </section>

  <!-- TEMPORIZADOR -->
  <section class="screen" id="screen-temporizador">
    <h1 style="justify-content:center; text-align:center;">
      <span class="icon-chip"><svg viewBox="0 0 24 24"><path d="M2 4h6a4 4 0 0 1 4 4v12a3 3 0 0 0-3-3H2z"/><path d="M22 4h-6a4 4 0 0 0-4 4v12a3 3 0 0 1 3-3h7z"/></svg></span>
      <span id="timer-title">Temporizador</span>
    </h1>
    <div class="card" style="text-align:center;">
      <div class="ring timer-ring">
        <svg width="230" height="230" viewBox="0 0 230 230">
          <circle class="ring-bg" cx="115" cy="115" r="100" fill="none" stroke-width="13"/>
          <circle class="ring-fg purple" id="timer-ring" cx="115" cy="115" r="100" fill="none" stroke-width="13"
            stroke-dasharray="628.3" stroke-dashoffset="0"/>
        </svg>
        <div class="ring-center"><b id="timer-time">--:--</b><span id="timer-total">/ --:--</span></div>
      </div>
      <div class="timer-btns">
        <button class="tbtn" id="timer-pause" title="Pausa / seguir">
          <svg viewBox="0 0 24 24" id="pause-icon"><path d="M7 5h4v14H7zM13 5h4v14h-4z"/></svg>
        </button>
        <button class="tbtn stop" id="timer-stop" title="Detener">
          <svg viewBox="0 0 24 24"><rect x="6" y="6" width="12" height="12" rx="2"/></svg>
        </button>
      </div>
      <div class="chips">
        <button class="chip" data-cmd="temporizador 5m">5 min</button>
        <button class="chip" data-cmd="temporizador 10m">10 min</button>
        <button class="chip" data-cmd="temporizador 20m">20 min</button>
        <button class="chip" data-cmd="pomodoro">Pomodoro 25 min</button>
      </div>
      <p class="muted" id="timer-state" style="margin:14px 0 0; font-size:14px;">Sin temporizador activo</p>
    </div>
  </section>

  <!-- RECORDATORIOS -->
  <section class="screen" id="screen-recordatorios">
    <h1 style="color:var(--purple);">
      <svg width="30" height="30" viewBox="0 0 24 24" fill="var(--purple)"><path d="M12 22c1.1 0 2-.9 2-2h-4a2 2 0 0 0 2 2zm6-6v-5c0-3.07-1.63-5.64-4.5-6.32V4a1.5 1.5 0 0 0-3 0v.68C7.64 5.36 6 7.92 6 11v5l-2 2v1h16v-1l-2-2z"/></svg>
      Recordatorio
    </h1>
    <div id="rem-next"></div>
    <h1 style="font-size:22px; margin-top:18px;">Próximos</h1>
    <div id="rem-list"></div>
    <div class="cmdbox">
      <input id="add-rem-cuando" type="text" placeholder="10m" style="flex:0 0 76px; text-align:center;" autocomplete="off">
      <input id="add-rem-texto" type="text" placeholder="Nuevo recordatorio…" autocomplete="off">
      <button id="add-rem-btn">Añadir</button>
    </div>
  </section>
</div>

<nav id="nav">
  <button class="active" data-screen="habitos">
    <svg viewBox="0 0 24 24"><path d="M4 20V10M10 20V4M16 20v-7M22 20H2"/></svg>
    Hábitos
  </button>
  <button data-screen="hoy">
    <svg viewBox="0 0 24 24"><rect x="3" y="4" width="18" height="17" rx="2"/><path d="M16 2v4M8 2v4M3 9h18"/></svg>
    Hoy
  </button>
  <button data-screen="temporizador">
    <svg viewBox="0 0 24 24"><circle cx="12" cy="13" r="8"/><path d="M12 9v4l2.5 2.5M9 2h6"/></svg>
    Temporizador
  </button>
  <button data-screen="recordatorios">
    <svg viewBox="0 0 24 24"><path d="M18 8a6 6 0 0 0-12 0c0 7-3 9-3 9h18s-3-2-3-9"/><path d="M13.7 21a2 2 0 0 1-3.4 0"/></svg>
    Recordatorios
  </button>
</nav>

<div id="toast"></div>
<script>
"use strict";
// Datos de ejemplo: se usan solo si el robot no responde (vista previa).
const DEMO = {
  hora: "20:28",
  habitos: [
    {nombre: "Leer 20 min", hora: "21:00", hecho: true},
    {nombre: "Ejercicio 30 min", hora: "", hecho: true},
    {nombre: "Beber agua 8 vasos", hora: "", hecho: true},
    {nombre: "Meditar 10 min", hora: "", hecho: false},
    {nombre: "Estudiar CBC 2 horas", hora: "", hecho: false}
  ],
  tareas: [{id: 0, texto: "Repasar fichas 22 y 15"}, {id: 1, texto: "Comprar pilas AAA"}],
  recordatorios: [
    {texto: "Lectura 20 minutos", en_min: 0},
    {texto: "Sacar la ropa", en_min: 42}
  ],
  temporizador: {activo: true, pausado: false, restan_seg: 932, total_seg: 1200, texto: "Lectura"},
  pomodoro: {activo: false, restan_seg: 0, total_seg: 1500}
};

let state = null;       // último estado del robot (o DEMO)
let live = false;       // true cuando /api/estado responde
let habitView = "hoy";
let timerBase = null;   // {restan, at} para contar el temporizador entre refrescos

const $ = s => document.querySelector(s);
const $$ = s => document.querySelectorAll(s);
const CIRC_H = 414.7, CIRC_T = 628.3;

function toast(msg){
  const t = $("#toast"); t.textContent = msg; t.classList.add("show");
  clearTimeout(t._h); t._h = setTimeout(()=>t.classList.remove("show"), 2600);
}
function fmt(seg){
  seg = Math.max(0, Math.round(seg));
  return String(Math.floor(seg/60)).padStart(2,"0") + ":" + String(seg%60).padStart(2,"0");
}
function iconFor(name){
  const n = (name||"").toLowerCase();
  if (n.includes("leer") || n.includes("lectura") || n.includes("libro"))
    return '<svg viewBox="0 0 24 24"><path d="M2 4h6a4 4 0 0 1 4 4v12a3 3 0 0 0-3-3H2z"/><path d="M22 4h-6a4 4 0 0 0-4 4v12a3 3 0 0 1 3-3h7z"/></svg>';
  if (n.includes("ejercicio") || n.includes("gym") || n.includes("correr"))
    return '<svg viewBox="0 0 24 24"><path d="M6.5 6.5 17.5 17.5M2 12h4M18 12h4M6 7v10M18 7v10"/></svg>';
  if (n.includes("agua") || n.includes("beber"))
    return '<svg viewBox="0 0 24 24"><path d="M12 2.7 17.5 9a7.5 7.5 0 1 1-11 0z"/></svg>';
  if (n.includes("meditar") || n.includes("respir"))
    return '<svg viewBox="0 0 24 24"><path d="M12 21C7 16.5 3 13 3 8.9 3 6 5 4 7.5 4c1.7 0 3.3.9 4.5 2.4C13.2 4.9 14.8 4 16.5 4 19 4 21 6 21 8.9c0 4.1-4 7.6-9 12.1z"/></svg>';
  if (n.includes("estudiar") || n.includes("cbc") || n.includes("clase"))
    return '<svg viewBox="0 0 24 24"><path d="M22 10 12 5 2 10l10 5 10-5z"/><path d="M6 12v5c0 1.5 2.7 3 6 3s6-1.5 6-3v-5"/></svg>';
  return '<svg viewBox="0 0 24 24"><path d="M12 2l2.4 7.2H22l-6 4.6 2.3 7.2-6.3-4.5-6.3 4.5L8 13.8 2 9.2h7.6z"/></svg>';
}

// ---------- Render ----------
function renderHabits(){
  const h = state.habitos || [];
  const done = h.filter(x=>x.hecho).length, total = h.length;
  const pct = total ? Math.round(done/total*100) : 0;
  $("#habit-count").textContent = done + "/" + total;
  $("#habit-pct").textContent = pct + "%";
  $("#habit-ring").style.strokeDashoffset = CIRC_H * (1 - pct/100);
  const restan = total - done;
  let msg = pct === 100
    ? "<b>¡Día completado!</b><br><span class='muted'>Todos los hábitos hechos. A descansar.</span>"
    : pct >= 50
    ? "<b>¡Vas muy bien!</b><br><span class='muted'>Aún te quedan " + restan + " hábito" + (restan===1?"":"s") + ".</span>"
    : "<b>Buen comienzo.</b><br><span class='muted'>Te quedan " + restan + " hábito" + (restan===1?"":"s") + " por hoy.</span>";
  $("#habit-msg").innerHTML = msg;

  const extra = $("#habit-extra");
  if (habitView === "semana"){
    const day = (new Date().getDay()+6)%7; // 0 = lunes
    extra.innerHTML = '<div class="card">' + h.map(x=>{
      let dots = "";
      for (let d=0; d<7; d++)
        dots += '<i class="' + (d<day ? "" : d===day && x.hecho ? "done" : "") + (d===day ? " today" : "") + '"></i>';
      return '<div class="week-row" style="margin-bottom:12px;"><b>'+esc(x.nombre)+'</b><span class="dots">'+dots+'</span></div>';
    }).join("") + '<p class="muted" style="font-size:13px; margin:6px 0 0;">El punto marcado es hoy. El historial de días pasados se irá llenando solo.</p></div>';
  } else if (habitView === "todos"){
    extra.innerHTML = '<div class="card">' + (h.length ? h.map(x=>
      '<div class="week-row" style="margin-bottom:10px;"><b>'+esc(x.nombre)+'</b><span class="muted">'+esc(x.hora||"sin hora")+'</span></div>'
    ).join("") : '<p class="empty">Sin hábitos todavía.</p>') + '</div>';
  } else extra.innerHTML = "";
}
function renderHoy(){
  const h = state.habitos || [];
  const done = h.filter(x=>x.hecho).length;
  $("#hoy-count").textContent = done + "/" + h.length;
  $("#hoy-list").innerHTML = h.length ? h.map(x=>
    '<div class="row'+(x.hecho?" done":"")+'" data-habito="'+escAttr(x.nombre)+'">' +
      '<span class="icon-chip">'+iconFor(x.nombre)+'</span>' +
      '<span class="meta"><b>'+esc(x.nombre)+'</b><span>'+esc(x.hora||"Todo el día")+'</span></span>' +
      '<button class="check" aria-label="Marcar hecho"><svg viewBox="0 0 24 24"><path d="M20 6 9 17l-5-5"/></svg></button>' +
    '</div>').join("") : '<p class="empty">No tienes hábitos. Crea uno desde el robot o con: habito beber agua 10:00</p>';
  const t = state.tareas || [];
  $("#tareas-list").innerHTML = t.length ? t.map(x=>{
    const id = (x && typeof x === "object") ? x.id : 0;
    const txt = (x && typeof x === "object") ? x.texto : x;
    return '<div class="row"><span class="icon-chip">'+iconFor("tarea")+'</span><span class="meta"><b>'+esc(txt)+'</b></span>' +
      '<button class="check" data-task="'+id+'" aria-label="Completar tarea"><svg viewBox="0 0 24 24"><path d="M20 6 9 17l-5-5"/></svg></button></div>';
  }).join("") : '<p class="empty">Sin tareas pendientes.</p>';
}
function renderTimer(){
  const tm = state.temporizador || {}, pm = state.pomodoro || {};
  let restan = 0, total = 0, label = "Temporizador", activo = false, pausado = false;
  if (tm.activo || tm.pausado){
    activo = tm.activo; pausado = tm.pausado;
    total = tm.total_seg || 0;
    restan = timerBase ? Math.max(0, timerBase.restan - (Date.now()-timerBase.at)/1000) : tm.restan_seg;
    label = tm.texto || "Temporizador";
  } else if (pm.activo){
    activo = true; total = pm.total_seg || 1500;
    restan = timerBase ? Math.max(0, timerBase.restan - (Date.now()-timerBase.at)/1000) : pm.restan_seg;
    label = "Pomodoro";
  }
  $("#timer-title").textContent = label;
  $("#timer-time").textContent = (activo||pausado) ? fmt(restan) : "--:--";
  $("#timer-total").textContent = "/ " + (total ? fmt(total) : "--:--");
  const frac = total ? Math.max(0, Math.min(1, restan/total)) : 0;
  $("#timer-ring").style.strokeDashoffset = CIRC_T * (1 - frac);
  $("#pause-icon").innerHTML = pausado
    ? '<path d="M8 5v14l11-7z"/>'
    : '<path d="M7 5h4v14H7zM13 5h4v14h-4z"/>';
  $("#timer-state").textContent =
    pausado ? "En pausa — pulsa ▶ para seguir" :
    activo ? (pm.activo ? "Pomodoro en marcha. Concéntrate." : "Temporizador en marcha.") :
    "Sin temporizador activo";
}
function renderReminders(){
  const r = state.recordatorios || [];
  const next = r[0];
  $("#rem-next").innerHTML = next
    ? '<div class="card"><div class="big-eyes"><i></i><i></i></div>' +
      '<p class="rem-title">Es hora de tu hábito</p>' +
      '<p class="rem-name">'+esc(next.texto)+'</p>' +
      '<p class="rem-dur">'+(next.en_min>0 ? "en "+next.en_min+" minutos" : "ahora")+'</p>' +
      '<div class="rem-actions">' +
        '<button class="primary" data-rem="hecho">Ya lo hice</button>' +
        '<button data-rem="posponer">En 15 min</button>' +
        '<button data-rem="descartar">Ahora no</button>' +
      '</div></div>'
    : '<div class="card"><p class="empty">Sin recordatorios. Crea uno con: recordar 10m sacar la ropa</p></div>';
  $("#rem-list").innerHTML = r.length > 1 ? r.slice(1).map(x=>
    '<div class="row"><span class="icon-chip"><svg viewBox="0 0 24 24"><path d="M18 8a6 6 0 0 0-12 0c0 7-3 9-3 9h18s-3-2-3-9"/><path d="M13.7 21a2 2 0 0 1-3.4 0"/></svg></span>' +
    '<span class="meta"><b>'+esc(x.texto)+'</b><span>en '+x.en_min+' min</span></span></div>'
  ).join("") : "";
}
function renderAll(){ renderHabits(); renderHoy(); renderTimer(); renderReminders(); }
function esc(s){ return String(s??"").replace(/[&<>"]/g, c=>({"&":"&amp;","<":"&lt;",">":"&gt;",'"':"&quot;"}[c])); }
function escAttr(s){ return esc(s).replace(/'/g,"&#39;"); }

// ---------- Comandos al robot ----------
async function sendCmd(cmd){
  if (!live){ toast("Vista previa: el robot no está conectado"); renderAll(); return; }
  try{
    const r = await fetch("/cmd?texto=" + encodeURIComponent(cmd));
    const t = await r.text();
    toast(t.split("\n")[0].slice(0, 90) || "Hecho");
  }catch(e){ toast("No se pudo hablar con el robot"); }
  setTimeout(refresh, 400);
}

// ---------- Datos ----------
async function refresh(){
  try{
    const r = await fetch("/api/estado", {cache:"no-store"});
    if (!r.ok) throw 0;
    const data = await r.json();
    live = true;
    const tm = data.temporizador || {}, pm = data.pomodoro || {};
    const restan = (tm.activo||tm.pausado) ? tm.restan_seg : pm.activo ? pm.restan_seg : 0;
    if (restan !== (timerBase ? timerBase.restan : null)) timerBase = {restan, at: Date.now()};
    state = data;
  }catch(e){
    if (!state) state = DEMO; // sin robot: muestra el diseño con datos de ejemplo
    live = false;
  }
  renderAll();
}

// ---------- Navegación ----------
$$("#nav button").forEach(b=>b.addEventListener("click", ()=>{
  $$("#nav button").forEach(x=>x.classList.remove("active"));
  b.classList.add("active");
  $$(".screen").forEach(s=>s.classList.remove("active"));
  $("#screen-"+b.dataset.screen).classList.add("active");
  window.scrollTo(0,0);
}));
$$("#habit-seg button").forEach(b=>b.addEventListener("click", ()=>{
  $$("#habit-seg button").forEach(x=>x.classList.remove("active"));
  b.classList.add("active"); habitView = b.dataset.view; renderHabits();
}));
document.addEventListener("click", e=>{
  const taskBtn = e.target.closest("[data-task]");
  if (taskBtn){ sendCmd("tarea hecha " + taskBtn.dataset.task); return; }
  const row = e.target.closest(".row[data-habito]");
  if (row){ sendCmd("hecho " + row.dataset.habito); return; }
  const chip = e.target.closest(".chip");
  if (chip){ sendCmd(chip.dataset.cmd); return; }
  const rem = e.target.closest("[data-rem]");
  if (rem){
    const next = (state.recordatorios||[])[0];
    if (!next) return;
    const acc = rem.dataset.rem;
    if (acc === "posponer") sendCmd("recordatorio posponer " + next.texto + " 15");
    else if (acc === "descartar") sendCmd("recordatorio descartar " + next.texto);
    else sendCmd("recordatorio hecho " + next.texto);
    return;
  }
});
$("#timer-pause").addEventListener("click", ()=>sendCmd("temporizador pausa"));
$("#timer-stop").addEventListener("click", ()=>sendCmd("temporizador parar"));
$("#add-habito-btn").addEventListener("click", ()=>{
  const n = $("#add-habito-nombre"), h = $("#add-habito-hora");
  if (n.value.trim()){ sendCmd("habito " + n.value.trim() + (h.value.trim() ? " " + h.value.trim() : "")); n.value=""; h.value=""; }
});
$("#add-tarea-btn").addEventListener("click", ()=>{
  const i = $("#add-tarea-texto");
  if (i.value.trim()){ sendCmd("tarea " + i.value.trim()); i.value=""; }
});
$("#add-rem-btn").addEventListener("click", ()=>{
  const c = $("#add-rem-cuando"), t = $("#add-rem-texto");
  if (t.value.trim()){ sendCmd("recordar " + (c.value.trim() || "10m") + " " + t.value.trim()); t.value=""; c.value=""; }
});
$("#cmd-send").addEventListener("click", ()=>{
  const i = $("#cmd-input"); if (i.value.trim()){ sendCmd(i.value.trim()); i.value=""; }
});
$("#cmd-input").addEventListener("keydown", e=>{ if (e.key==="Enter") $("#cmd-send").click(); });

// Reloj de la barra de estado
function tickClock(){
  const d = new Date();
  $("#clock").textContent = String(d.getHours()).padStart(2,"0")+":"+String(d.getMinutes()).padStart(2,"0");
}
tickClock(); setInterval(tickClock, 20000);
setInterval(()=>{ if (state) renderTimer(); }, 1000); // el temporizador baja cada segundo
refresh();
setInterval(refresh, 5000);
</script>
</body>
</html>
)PANEL_EOF";
