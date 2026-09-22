const char ESTILO[] = R"PAGINA(<!DOCTYPE html><html lang="pt-br"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>RoboControl</title><style>
*{box-sizing:border-box}
body{margin:0;padding:22px 18px 30px;min-height:100vh;background:#0a1420;
background-image:
radial-gradient(ellipse 75% 55% at 6% 92%,rgba(232,133,44,.40),transparent 68%),
radial-gradient(ellipse 55% 45% at 95% 4%,rgba(70,120,180,.34),transparent 70%),
linear-gradient(175deg,#0b1a2c 0%,#0a1420 52%,#150f0e 100%);
background-attachment:fixed;color:#e6eef8;
font-family:ui-monospace,SFMono-Regular,Consolas,Menlo,monospace}
.topo{display:flex;justify-content:space-between;align-items:flex-start;gap:14px;
max-width:940px;margin:0 auto 26px}
.marca{font-family:system-ui,Segoe UI,Arial,sans-serif;font-size:27px;font-weight:700;
letter-spacing:-.02em;line-height:1}
.marca i{font-style:normal;color:#f0a94a}
.marca small{display:block;margin-top:7px;font-family:inherit;font-size:10px;
font-weight:500;letter-spacing:.24em;color:#7f97b4}
.lema{text-align:right;font-size:9.5px;letter-spacing:.2em;color:#6f89a8;
line-height:1.9;padding-top:5px}
.painel{position:relative;max-width:430px;margin:0 auto;padding:32px 22px 30px;
border-radius:22px;border:1px solid rgba(126,182,226,.28);
background:linear-gradient(158deg,rgba(20,42,66,.66),rgba(9,21,35,.5));
box-shadow:inset 0 1px 0 rgba(255,255,255,.05),0 24px 70px rgba(0,0,0,.5);
text-align:center}
.painel::before,.painel::after{content:"";position:absolute;width:28px;height:28px;
border-color:rgba(150,205,240,.65);border-style:solid}
.painel::before{top:-1px;left:-1px;border-width:2px 0 0 2px;border-radius:22px 0 0 0}
.painel::after{bottom:-1px;right:-1px;border-width:0 2px 2px 0;border-radius:0 0 22px 0}
.chip{display:inline-block;padding:5px 13px;margin-bottom:18px;border-radius:5px;
border:1px solid rgba(240,169,74,.45);background:rgba(240,169,74,.08);
color:#f0b975;font-size:10.5px;letter-spacing:.2em}
h1{margin:0 0 9px;font-size:19px;font-weight:600;letter-spacing:.2em;
text-transform:uppercase;color:#eaf2fb}
.ajuda{margin:0 0 24px;font-family:system-ui,Segoe UI,Arial,sans-serif;font-size:14.5px;
color:#93a9c2;line-height:1.55}
select{font-family:inherit;font-size:15px;padding:14px 12px;width:100%;
border-radius:11px;background:rgba(8,20,33,.85);color:#e6eef8;
border:1px solid rgba(126,182,226,.34);margin-bottom:14px;appearance:none;
text-align:center}
select:focus{outline:none;border-color:rgba(240,169,74,.8)}
.entrar{font-family:inherit;font-size:14px;font-weight:600;letter-spacing:.16em;
text-transform:uppercase;padding:15px 20px;width:100%;border-radius:11px;cursor:pointer;
border:2px solid rgba(240,169,74,.8);background:rgba(240,169,74,.12);color:#ffca85;
box-shadow:0 0 20px rgba(240,169,74,.16)}
.entrar:active{background:rgba(240,169,74,.32);box-shadow:0 0 28px rgba(240,169,74,.45)}
.grade{display:grid;grid-template-columns:repeat(3,1fr);gap:12px;
max-width:310px;margin:0 auto}
.grade span{display:block}
.grade button{aspect-ratio:1;width:100%;border-radius:19px;cursor:pointer;
display:flex;flex-direction:column;align-items:center;justify-content:center;gap:4px;
border:2px solid rgba(240,169,74,.75);background:rgba(240,169,74,.07);color:#ffca85;
box-shadow:0 0 18px rgba(240,169,74,.16),inset 0 0 22px rgba(240,169,74,.07);
font-family:inherit;font-size:9.5px;letter-spacing:.13em;text-transform:uppercase;
-webkit-user-select:none;user-select:none;touch-action:none;
transition:background .09s,box-shadow .09s,transform .09s}
.grade button:active{background:rgba(240,169,74,.34);
box-shadow:0 0 34px rgba(240,169,74,.55);transform:scale(.94)}
.grade svg{width:30px;height:30px;stroke:currentColor;stroke-width:2.6;fill:none;
stroke-linecap:round;stroke-linejoin:round}
.baixo svg{transform:rotate(180deg)}
.esq svg{transform:rotate(-90deg)}
.dir svg{transform:rotate(90deg)}
.parar{border-color:rgba(214,96,86,.75)!important;background:rgba(214,96,86,.09)!important;
color:#ffb0a6!important;box-shadow:0 0 16px rgba(214,96,86,.16)!important}
.parar:active{background:rgba(214,96,86,.38)!important;
box-shadow:0 0 32px rgba(214,96,86,.5)!important}
.parar b{display:block;width:13px;height:13px;border-radius:3px;background:currentColor}
#aviso{margin-top:22px;font-family:system-ui,Segoe UI,Arial,sans-serif;font-size:14.5px;
color:#93a9c2;min-height:21px}
#aviso.ocupado{color:#f0b975}
#assumir{margin-top:14px;font-family:inherit;font-size:11.5px;font-weight:600;
letter-spacing:.14em;text-transform:uppercase;padding:12px 22px;cursor:pointer;
border-radius:9px;border:1px solid rgba(240,169,74,.6);background:rgba(240,169,74,.12);
color:#f0b975}
#assumir:active{background:rgba(240,169,74,.34)}
.ajustes{margin-top:26px;padding-top:20px;border-top:1px solid rgba(126,182,226,.16)}
.ajustes .titulo{font-size:9.5px;letter-spacing:.22em;text-transform:uppercase;
color:#6f89a8;margin-bottom:11px}
.duplo{display:flex;gap:10px;justify-content:center}
.duplo button{flex:1;font-family:inherit;font-size:10px;font-weight:600;
letter-spacing:.12em;text-transform:uppercase;padding:11px 8px;cursor:pointer;
border-radius:9px;border:1px solid rgba(126,182,226,.34);background:rgba(8,20,33,.6);
color:#93a9c2}
.duplo button.ligado{border-color:rgba(240,169,74,.8);background:rgba(240,169,74,.16);
color:#ffca85}
.duplo button.ligado::after{content:" invertido";opacity:.75}
.duplo button:active{transform:scale(.97)}
.dica{margin-top:11px;font-family:system-ui,Segoe UI,Arial,sans-serif;font-size:12px;
color:#6f89a8;line-height:1.5}
.dica b{color:#93a9c2}
.broto{width:26px;height:26px;vertical-align:-5px;margin-right:7px}
.robo{position:fixed;left:16px;bottom:8px;width:172px;height:172px;
pointer-events:none;opacity:.97;z-index:-1}
@media(max-width:900px){.robo{width:128px;height:128px;opacity:.5;left:4px;bottom:0}}
@media(max-width:560px){.robo{display:none}}
.sair{display:inline-block;margin-top:22px;color:#6f8fb8;font-size:10.5px;
letter-spacing:.16em;text-transform:uppercase;text-decoration:none;
border-bottom:1px solid rgba(111,143,184,.4);padding-bottom:3px}
.rodape{max-width:940px;margin:28px auto 0;text-align:right;font-size:9.5px;
letter-spacing:.2em;color:#6f89a8;line-height:1.9}
@media(max-width:620px){
.topo{flex-direction:column;align-items:center;text-align:center}
.lema,.rodape{text-align:center}
.marca{font-size:23px}}
</style></head><body>
<div class="topo">
<div class="marca"><svg class="broto" viewBox="0 0 24 24"><path d="M12 21 v-8" stroke="#8fd07a" stroke-width="2" stroke-linecap="round" fill="none"/><path d="M12 14 q-7 -1 -8 -8 q8 0 8 8 z" fill="#8fd07a"/><path d="M12 15 q6 -2 7 -9 q-8 1 -7 9 z" fill="#6fb85c"/></svg>Robo<i>Control</i><small>CONTROLE SEU CARRINHO</small></div>
<div class="lema">PEQUENOS MOVIMENTOS<br>GRANDES DESCOBERTAS</div>
</div>
<div class="painel">
)PAGINA";

const char CONTROLE[] = R"PAGINA(<div class="grade">
<span></span>
<button data-acao="frente" class="cima"><svg viewBox="0 0 24 24"><polyline points="6 15 12 9 18 15"></polyline></svg>frente</button>
<span></span>
<button data-acao="esquerda" class="esq"><svg viewBox="0 0 24 24"><polyline points="6 15 12 9 18 15"></polyline></svg>esquerda</button>
<button data-acao="parar" class="parar"><b></b>parar</button>
<button data-acao="direita" class="dir"><svg viewBox="0 0 24 24"><polyline points="6 15 12 9 18 15"></polyline></svg>direita</button>
<span></span>
<button data-acao="tras" class="baixo"><svg viewBox="0 0 24 24"><polyline points="6 15 12 9 18 15"></polyline></svg>r&eacute;</button>
<span></span></div>
<div id="aviso"></div>
<button id="assumir" hidden>assumir o controle</button>
<a class="sair" href="/">trocar de motorista</a>
</div>
<div class="rodape">UM FUTURO<br>MAIS BRILHANTE</div>
<svg class="robo" viewBox="0 0 200 200"><defs><linearGradient id="gcorpo" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#e3b65c"/><stop offset="1" stop-color="#94701f"/></linearGradient><linearGradient id="golho" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#b3bcc9"/><stop offset="1" stop-color="#606b7c"/></linearGradient><radialGradient id="gbrilho" cx="50%" cy="55%" r="50%"><stop offset="0" stop-color="#f0a94a" stop-opacity=".30"/><stop offset="1" stop-color="#f0a94a" stop-opacity="0"/></radialGradient></defs><ellipse cx="100" cy="150" rx="96" ry="52" fill="url(#gbrilho)"/><rect x="18" y="140" width="56" height="46" rx="23" fill="#49525f" stroke="#2b323c" stroke-width="2"/><rect x="126" y="140" width="56" height="46" rx="23" fill="#49525f" stroke="#2b323c" stroke-width="2"/><circle cx="36" cy="163" r="8" fill="#333a45"/><circle cx="56" cy="163" r="8" fill="#333a45"/><circle cx="144" cy="163" r="8" fill="#333a45"/><circle cx="164" cy="163" r="8" fill="#333a45"/><rect x="30" y="104" width="16" height="44" rx="7" fill="#7b8697"/><rect x="154" y="104" width="16" height="44" rx="7" fill="#7b8697"/><rect x="52" y="88" width="96" height="66" rx="10" fill="url(#gcorpo)" stroke="#6d5215" stroke-width="2"/><rect x="64" y="102" width="72" height="36" rx="6" fill="#00000022" stroke="#7d5f1c" stroke-width="2"/><line x1="64" y1="120" x2="136" y2="120" stroke="#7d5f1c" stroke-width="2"/><circle cx="76" cy="111" r="3.5" fill="#ffd98a"/><circle cx="88" cy="111" r="3.5" fill="#c0873a"/><rect x="94" y="70" width="12" height="24" rx="5" fill="#7b8697"/><rect x="44" y="34" width="52" height="46" rx="23" fill="url(#golho)" stroke="#39414e" stroke-width="2"/><rect x="104" y="34" width="52" height="46" rx="23" fill="url(#golho)" stroke="#39414e" stroke-width="2"/><circle cx="70" cy="57" r="15" fill="#141c26"/><circle cx="130" cy="57" r="15" fill="#141c26"/><circle cx="70" cy="57" r="8" fill="#2a6ea8"/><circle cx="130" cy="57" r="8" fill="#2a6ea8"/><circle cx="65" cy="51" r="4" fill="#d8ebff" opacity=".92"/><circle cx="125" cy="51" r="4" fill="#d8ebff" opacity=".92"/><path d="M100 88 q-3 -16 -14 -22 q13 -1 16 12 q4 -14 17 -13 q-12 7 -15 23 z" fill="#5fae4e"/><rect x="97" y="84" width="4" height="10" rx="2" fill="#4a8c3c"/></svg>
<script>
var atual='';
var repetidor=null;
var aviso=document.getElementById('aviso');
var botaoAssumir=document.getElementById('assumir');
var teclas={ArrowUp:'frente',ArrowDown:'tras',ArrowLeft:'esquerda',ArrowRight:'direita'};
function mostrar(t){
var partes=t.split('|');
aviso.textContent=partes.length>1?partes[1]:t;
var ocupado=partes[0]=='ocupado';
aviso.className=ocupado?'ocupado':'';
botaoAssumir.hidden=!ocupado;}
function enviar(acao){
return fetch('/comando?u='+eu+'&a='+acao).then(function(r){return r.text()})
.then(mostrar)
.catch(function(){aviso.textContent='sem resposta do carrinho';});}
function manda(acao){
if(acao==atual)return;
atual=acao;
if(repetidor){clearInterval(repetidor);repetidor=null;}
enviar(acao);
if(acao!='parar'){repetidor=setInterval(function(){enviar(acao)},500);}}
function solta(){manda('parar')}
botaoAssumir.addEventListener('click',function(){atual='';manda('assumir')});
var botoes=document.querySelectorAll('.grade button');
for(var i=0;i<botoes.length;i++){(function(b){
var acao=b.getAttribute('data-acao');
b.addEventListener('mousedown',function(){manda(acao)});
b.addEventListener('touchstart',function(e){e.preventDefault();manda(acao)});
b.addEventListener('mouseup',solta);b.addEventListener('mouseleave',solta);
b.addEventListener('touchend',function(e){e.preventDefault();solta()});
})(botoes[i])}
document.addEventListener('keydown',function(e){
if(teclas[e.key]&&!e.repeat){e.preventDefault();manda(teclas[e.key])}});
document.addEventListener('keyup',function(e){
if(teclas[e.key]){e.preventDefault();solta()}});
window.addEventListener('blur',solta);
</script></body></html>
)PAGINA";

const char FIM_LOGIN[] = R"PAGINA(</div>
<div class="rodape">UM FUTURO<br>MAIS BRILHANTE</div>
<svg class="robo" viewBox="0 0 200 200"><defs><linearGradient id="gcorpo" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#e3b65c"/><stop offset="1" stop-color="#94701f"/></linearGradient><linearGradient id="golho" x1="0" y1="0" x2="0" y2="1"><stop offset="0" stop-color="#b3bcc9"/><stop offset="1" stop-color="#606b7c"/></linearGradient><radialGradient id="gbrilho" cx="50%" cy="55%" r="50%"><stop offset="0" stop-color="#f0a94a" stop-opacity=".30"/><stop offset="1" stop-color="#f0a94a" stop-opacity="0"/></radialGradient></defs><ellipse cx="100" cy="150" rx="96" ry="52" fill="url(#gbrilho)"/><rect x="18" y="140" width="56" height="46" rx="23" fill="#49525f" stroke="#2b323c" stroke-width="2"/><rect x="126" y="140" width="56" height="46" rx="23" fill="#49525f" stroke="#2b323c" stroke-width="2"/><circle cx="36" cy="163" r="8" fill="#333a45"/><circle cx="56" cy="163" r="8" fill="#333a45"/><circle cx="144" cy="163" r="8" fill="#333a45"/><circle cx="164" cy="163" r="8" fill="#333a45"/><rect x="30" y="104" width="16" height="44" rx="7" fill="#7b8697"/><rect x="154" y="104" width="16" height="44" rx="7" fill="#7b8697"/><rect x="52" y="88" width="96" height="66" rx="10" fill="url(#gcorpo)" stroke="#6d5215" stroke-width="2"/><rect x="64" y="102" width="72" height="36" rx="6" fill="#00000022" stroke="#7d5f1c" stroke-width="2"/><line x1="64" y1="120" x2="136" y2="120" stroke="#7d5f1c" stroke-width="2"/><circle cx="76" cy="111" r="3.5" fill="#ffd98a"/><circle cx="88" cy="111" r="3.5" fill="#c0873a"/><rect x="94" y="70" width="12" height="24" rx="5" fill="#7b8697"/><rect x="44" y="34" width="52" height="46" rx="23" fill="url(#golho)" stroke="#39414e" stroke-width="2"/><rect x="104" y="34" width="52" height="46" rx="23" fill="url(#golho)" stroke="#39414e" stroke-width="2"/><circle cx="70" cy="57" r="15" fill="#141c26"/><circle cx="130" cy="57" r="15" fill="#141c26"/><circle cx="70" cy="57" r="8" fill="#2a6ea8"/><circle cx="130" cy="57" r="8" fill="#2a6ea8"/><circle cx="65" cy="51" r="4" fill="#d8ebff" opacity=".92"/><circle cx="125" cy="51" r="4" fill="#d8ebff" opacity=".92"/><path d="M100 88 q-3 -16 -14 -22 q13 -1 16 12 q4 -14 17 -13 q-12 7 -15 23 z" fill="#5fae4e"/><rect x="97" y="84" width="4" height="10" rx="2" fill="#4a8c3c"/></svg>
</body></html>
)PAGINA";

void abrirPagina() {
  servidor.setContentLength(CONTENT_LENGTH_UNKNOWN);
  servidor.send(200, "text/html", "");
  servidor.sendContent(ESTILO);
  servidor.sendContent(String("<div class=\"chip\">") + nomeDaRede + "</div>");
}

void fecharPagina() {
  servidor.sendContent("");
}

void enviarLogin() {
  abrirPagina();
  servidor.sendContent("<h1>Quem vai dirigir?</h1>");
  servidor.sendContent("<p class=\"ajuda\">Escolha o seu nome na lista e entre.</p>");
  servidor.sendContent("<select id=\"quem\">");
  for (int i = 0; i < TOTAL_USUARIOS; i++) {
    servidor.sendContent(String("<option value=\"") + USUARIOS[i].usuario + "\">" +
                         USUARIOS[i].nome + "</option>");
  }
  servidor.sendContent("</select>");
  servidor.sendContent("<button class=\"entrar\" onclick=\"entrar()\">Entrar</button>");
  servidor.sendContent("<script>function entrar(){location.href='/carro/'"
                       "+document.getElementById('quem').value}</script>");
  servidor.sendContent(FIM_LOGIN);
  fecharPagina();
}

void enviarControle(String nome, String usuario) {
  abrirPagina();
  servidor.sendContent("<h1>Controle direto</h1>");
  servidor.sendContent("<p class=\"ajuda\">" + nome +
                       ", segure o bot&atilde;o ou use as setas do teclado."
                       " Soltou, o carrinho para.</p>");
  servidor.sendContent("<script>var eu='" + usuario + "';</script>");
  servidor.sendContent(CONTROLE);
  fecharPagina();
}

void enviarRecado(String titulo, String texto) {
  abrirPagina();
  servidor.sendContent("<h1>" + titulo + "</h1>");
  servidor.sendContent("<p class=\"ajuda\">" + texto + "</p>");
  servidor.sendContent("<a class=\"sair\" href=\"/\">voltar</a>");
  servidor.sendContent(FIM_LOGIN);
  fecharPagina();
}
