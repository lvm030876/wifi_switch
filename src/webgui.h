const char* upgradeIndex = R"=====(<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>)=====";

const char* helpIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
	</head>
	<body>
		<h1 style="color: #5e9ca0;"><span style="color: #000000;">Команди пристроя</span></h1>
		<div>
			<ul>
				<li><span style="color: #ff0000;">switch <span style="color: #000000;">- керування релейним блоком</span></span></li>
				<ul>
					<li><div>switch = switchA/switchB/switchC/pirProt - група освітлення увімкнути/вимкнути/інверсія/блокування датчику руху</div></li>
				</ul>
				<li><span style="color: #ff0000;">resalarm <span style="color: #000000;">- перезапуск спрацьованих датчиків</span></span></li>
				<li><span style="color: #ff0000;">switch.xml, switch.json</span></li>
				<li><span style="color: #ff0000;">ir.htm <span style="color: #000000;">- сторінка інфрачервоного керування</span></span></li>
				<li><span style="color: #ff0000;">rf.htm <span style="color: #000000;">- сторінка налаштунку радіо-модуля</span></span></li>
				<li><span style="color: #ff0000;">rf.json</span></li>
				<li><span style="color: #ff0000;">ip.htm <span style="color: #000000;">- сторінка налаштунку статичного адресу</span></span></li>
				<li><span style="color: #ff0000;">scanwifi.json</span></li>
				<li><span style="color: #ff0000;">mem <span style="color: #000000;">- збереження налаштувань</span></span></li>
				<ul>
					<li><div>ipStat - статична адреса</div></li>
					<li><div>pirTime - час спрацювання групи освітлення за датчиком руху</div></li>
					<li><div>pirLight - рівень освітлення спрацювання групи освітлення за датчиком руху</div></li>
					<li><div>pirGroup - група освітлення роботи за датчиком руху</div></li>
					<li><div>rfAOn - радіо-код увімкнути групу А</div></li>
					<li><div>rfВOn - радіо-код увімкнути групу В</div></li>
					<li><div>rfСOn - радіо-код увімкнути групу С</div></li>
					<li><div>rfOn - радіо-код увімкнути все освітлення</div></li>
					<li><div>rfOff - радіо-код вимкнути все освітлення</div></li>
					<li><div>ssid - назва wifi точки доступу</div></li>
					<li><div>pass - пароль доступу до wifi</div></li>
				</ul>
				<li><span style="color: #ff0000;">default <span style="color: #000000;">- онулення налаштувань пристрою</span></span></li>
				<li><span style="color: #ff0000;">reboot <span style="color: #000000;">- перезавантаження пристрою</span></span></li>
			</ul>
		</div>
	</body>
</html>
)=====";

const char* irIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>вимикач</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
		<script>
			var bekoTempSet	  =   18;
			var bekoHdrMark   = 6100;
			var bekoHdrSpace  = 7400;
			var bekoBitMark   =  600;
			var bekoOneSpace  = 1700;
			var bekoZeroSpace =  600;
			var bekoSwing = {
					close	: 0,
					flow	: 1,
					open	: 2
			};
			var bekoFan = {
					auto	: 0,
					speed3	: 1,
					speed2	: 2,
					speed1	: 3
			};
			var bekoMode = {
					auto	: 0,
					cool	: 1,
					dry		: 2,
					fan		: 3,
					heat	: 4,
			};

			function irBeko(powerOnF = false, modeF = bekoMode.auto, tempSetF = 21, fanF = bekoFan.auto, swingF = bekoSwing.flow, airFlowOnF = false, sleepOnF = false) {
				var raw 	= [];
				var irBit	= "";
				var cmd	= [255, 0, 255, 0, 255, 0, 6, 7, 8, 9, 42, 213];
				cmd[7]	= sleepOnF + (powerOnF << 1) + (swingF << 2) + (airFlowOnF << 4) + (fanF << 5);
				cmd[6]	= ~cmd[7] & 255;
				cmd[9]	= (tempSetF - 16) + (modeF << 5);
				cmd[8]	= ~cmd[9] & 255;
				cmd.forEach(element => irBit = irBit + bittostr(element));
				raw.push(bekoHdrMark, bekoHdrSpace);
				for (var i=0; i < irBit.length; i++) raw.push(bekoBitMark, (irBit.charAt(i) == "1")? bekoOneSpace: bekoZeroSpace);
				raw.push(bekoBitMark, bekoHdrSpace, bekoBitMark);
			return raw;
			}

			var saturnTempSet	=   18;
			var saturnHdrMark   = 9000;
			var saturnHdrSpace  = 4500;
			var saturnBitMark   =  600;
			var saturnOneSpace  = 1700;
			var saturnZeroSpace =  600;
			var termometr         = 25;
			var saturnFan = {
					fast	: 0,
					speed3	: 1,
					speed2	: 2,
					speed1	: 3,
					speed4	: 5,
			};
			var saturnMode = {
				recirculig	: 0,
				cooling		: 32,
				defrosting	: 64,
				heating		: 128,
				airing		: 192,
			};

			function irSaturn(powerOnF = false, modeF = saturnMode.defrosting, tempSetF = 21, fanF = saturnFan.speed1, swingOnF = false) {
				var raw 	= [];
				var irBit	= "";
				var summ	= 0;
				var nowDate = new Date();
				var cmd	= [195, 1, nowDate.getHours(), nowDate.getMinutes(), (fanF << 5), 0, modeF, (termometr - 22), 0, (powerOnF?32:0), 0, 5];
				cmd[1]	= ((tempSetF - 8) << 3) + (swingOnF?0:7);
				cmd.forEach(element => {
					irBit = irBit + bittostr(element);
					summ = summ + element;
				});
				irBit = irBit + bittostr(summ);
				raw.push(saturnHdrMark, saturnHdrSpace);
				for (var i=0; i < irBit.length; i++) (irBit.charAt(i) == "1")? raw.push(saturnBitMark, saturnOneSpace): raw.push(saturnBitMark, saturnZeroSpace);
				raw.push(saturnBitMark);
			return raw;
			}

			function reverseStr(x) {
				return x.split("").reverse().join("");
			}

			function bittostr(x) {
				return reverseStr((x + 256).toString(2).slice(-8));
			}

			tvOff = new Array(4550,4550,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,600,600,1700,600,600,600,600,600,600,600,600,600,600,600,600,600,1700,600,600,600,1700,600,1700,600,1700,600,1700,600,1700,600,1700,600);
			tvMute = new Array(4550,4550,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,1700,600,1700,600);
			tvVolUp = new Array(4550,4550,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,1700,600,1700,600,1700,600);
			tvVolDown = new Array(4550,4550,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,1700,600,600,600,600,600,600,600,600,600,600,600,1700,600,1700,600,600,600,1700,600,600,600,600,600,600,600,600,600,600,600,600,600,1700,600,600,600,1700,600,1700,600,1700,600,1700,600);

			function irSend(repeit, myarr) {
				fetch('/irsend?rep=' + repeit + '&khz=36&date=' + myarr.toString())
				.then(response => response.json())
				.then(response => response);
			}

			function TabClick(num) {
				for (var i=1; i<4; i++) {
					document.getElementById('TTab'+i).className = (i == num)? 'SelectedTab': 'Tab';
					document.getElementById('Tab'+i).style.display = (i == num)? 'block': 'none';
				}
			}

			function start() {
				fetch("switch.json")
					.then(response => response.json())
					.then(data => {
						document.getElementById('mac').innerText = data.switch.mac;
					})

				var allTab = document.querySelectorAll('.tab');
				// console.log(allTab[0]);
				allTab[0].classList.add("tabSel");
				for (var i = 0; i < allTab.length; i++) {
					allTab[i].addEventListener('click', function() {
						let old = document.querySelector('.tabSel');
						old.classList.remove("tabSel");
						document.getElementById('T' + old.id.substr(-3)).style.display = 'none';
						this.classList.add("tabSel");
						document.getElementById('T' + this.id.substr(-3)).style.display = 'block';
					});
				}
			}

			demo = () => {
				bekoTempSet = document.getElementById('bflying').valueAsNumber;
				document.getElementById('bflevel').innerText = "" + bekoTempSet + "/32";
				saturnTempSet = document.getElementById('sflying').valueAsNumber;
				document.getElementById('sflevel').innerText = "" + saturnTempSet + "/32";
			}
		</script>
	</head>
	<body onload='start()'>
		<div class='block header'>вимикач<hr><hr>
		</div>
		<div class='block' id='mac'>
		</div>
		<div class='block ctrl'>
			<div class="barTab">
				<div id="tab1" class="tab">телевізор</div>
				<div id="tab2" class="tab">SATURN</div>
				<div id="tab3" class="tab">EBKO</div>
			</div>
			<hr>	
			<fieldset id="Tab3" style="display: none;">
			<legend>кондиціонер EBKO</legend>
				<div style="display: grid; grid-gap: 10px;">
					<div style="display: grid; grid-template-columns: 1fr 70px;">
						<input id="bflying" type="range" min="16" max="32" value="18" oninput="demo()"> 
						<div id="bflevel">18/32</div>
					</div>
					<button onclick="irSend(1, irBeko(true, bekoMode.auto))">атоматично</button>
					<button onclick="irSend(1, irBeko(true, bekoMode.cool, bekoTempSet))">охолодження</button>
					<button onclick="irSend(1, irBeko(true, bekoMode.dry))">розмороження</button>
					<button onclick="irSend(1, irBeko(true, bekoMode.heat, bekoTempSet))">обігрів</button>
					<button onclick="irSend(1, irBeko(true, bekoMode.fan))">вентиляція</button>
					<button onclick="irSend(1, irBeko())">вимкнути</button>
				</div>
			</fieldset>
			<fieldset id="Tab2" style="display: none;">
			<legend>кондиціонер SATURN</legend>
				<div style="display: grid; grid-gap: 10px;">
					<div style="display: grid; grid-template-columns: 1fr 70px;">
						<input id="sflying" type="range" min="16" max="32" value="18" oninput="demo()"> 
						<div id="sflevel">18/32</div>
					</div>
					<button onclick="irSend(1, irSaturn(true, saturnMode.recirculig))">рецеркуляція</button>
					<button onclick="irSend(1, irSaturn(true, saturnMode.cooling, saturnTempSet))">охолодження</button>
					<button onclick="irSend(1, irSaturn(true, saturnMode.defrosting))">розмороження</button>
					<button onclick="irSend(1, irSaturn(true, saturnMode.heating, saturnTempSet))">обігрів</button>
					<button onclick="irSend(1, irSaturn(true, saturnMode.airing))">вентиляція</button>
					<button onclick="irSend(1, irSaturn())">вимкнути</button>
				</div>
			</fieldset>
			<fieldset id="Tab1">
			<legend>телевізор</legend>
				<div style="display: grid; grid-gap: 10px; grid-template-columns: 1fr 1fr 1fr;">
					<button onclick="irSend(3, tvOff)" style="grid-column: 1/4">вимкнути/увімкнути</button>
					<button onclick="irSend(3, tvVolUp)">гучність +</button>
					<button onclick="irSend(3, tvMute)">тихо</button>
					<button onclick="irSend(3, tvVolDown)">гучність -</button>
				</div>
			</fieldset>
		</div>
		<div class='block menu'>
            <div></div>
            <div></div>
			<button onclick="location.href = '/'">на головну</button>
		</div>
		<div class='block footer'>
			<hr>
				<div>
					<a href='mailto:lvm030876@gmail.com'>Valentyn Lobatenko</a>
					<a href='http://esp8266.ru/'>джерело надхнення</a>
				</div>
			(c))=====" __DATE__ R"=====(
		</div>
	</body>
</html>
)=====";

const char* rfIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>вимикач</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
        <script>
            var cfg = ['rfAOn', 'rfBOn', 'rfCOn', 'rfOn', 'rfOff'];
            var rfcode;

            function tick(){
                fetch("rf.json")
                .then(response => response.json())
                .then(data => {
		            if (data.rf.rfNew != rfcode){
                        document.getElementById("rfCode").value = data.rf.rfNew;
                        rfcode = data.rf.rfNew;
                    }
                    setTimeout(tick, 500);
                })
            }

            function start(){
		        tick();
                var allButtons = document.querySelectorAll('.rfcode>button');
                for (var i = 0; i < allButtons.length; i++) {
                    allButtons[i].addEventListener('click', function() {
                        let a = document.getElementById('rfCode').value;
                        fetch("mem?"+this.id+"="+a)
                            .then(response => response.json())
                            .then(data => this.innerText = a);
                    });
                }

				fetch("switch.json")
					.then(response => response.json())
					.then(data => {
					document.getElementById('mac').innerText = data.switch.mac;
                })

                fetch("rf.json")
                .then(response => response.json())
                .then(data => {
                    for (const element of cfg)
                        document.getElementById(element).innerText = data.rf[element]
                })
            }
        </script>
	</head>
	<body onload='start()'>
		<div class='block header'>вимикач<hr><hr>
		</div>
		<div class='block' id='mac'>
		</div>
		<div class='block ctrl'>
			<fieldset>
            <legend>Налаштування радіо пульту</legend>
                <div class="rfcode">
                    <div>Зафіксований код:</div>
                    <input id='rfCode' type='text'/>
                    <div>Код вмикання групи А:</div>
                    <button id='rfAOn'>0</button>
                    <div>Код вмикання групи В:</div>
                    <button id='rfBOn'>0</button>
                    <div>Код вмикання групи С:</div>
                    <button id='rfCOn'>0</button>
                    <div>Код вмикання:</div>
                    <button id='rfOn'>0</button>
                    <div>Код вимикання:</div>
                    <button id='rfOff'>0</button>
                </div>
			</fieldset>
		</div>
		<div class='block menu'>
            <div></div>
            <div></div>
			<button onclick="location.href = '/'">на головну</button>
		</div>
		<div class='block footer'>
			<hr>
				<div>
					<a href='mailto:lvm030876@gmail.com'>Valentyn Lobatenko</a>
					<a href='http://esp8266.ru/'>джерело надхнення</a>
				</div>
			(c))=====" __DATE__ R"=====(
		</div>
	</body>
</html>
)=====";

const char* pirIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>вимикач</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
		<script>
            var cfg = ['pirTime', 'pirLight', 'pirGroup'];
            function start(){
				fetch("switch.json")
					.then(response => response.json())
					.then(data => {
					document.getElementById('mac').innerText = data.switch.mac;
                })

                fetch("pir.json")
                .then(response => response.json())
                .then(data => {
                    for (const element of cfg)
                        document.getElementById(element).value = data.pir[element]
                })
            }
            function Set(){
                var _from = x => document.getElementById(cfg[x]).value;
                fetch("mem?pirTime="+_from(0)+"&pirLight="+_from(1)+"&pirGroup="+_from(2))
	            .then(response => {
                    console.log(response.json());
                    location.href = '/';
                    });
            }
        </script>
	</head>
	<body onload='start()'>
		<div class='block header'>вимикач<hr><hr>
		</div>
		<div class='block' id='mac'>
		</div>
		<div class='block ctrl'>
            <fieldset>
            <legend>Налаштування АДР</legend>
                <div class='setauto'>
                    <div>Група ламп:</div>
                    <input type='text' name='pirGroup' id='pirGroup'/>
                    <div>Час відпрацювання, сек:</div>
                    <input type='text' title='період часу на який вмикається група' name='pirTime' id='pirTime'/>
                    <div>Рівень освітлення, %:</div>
                    <input type='text' title='освітленність менше якої вмикається група' name='pirLight' id='pirLight'/>
                </div>
            </fieldset>
		</div>
		<div class='block menu'>
            <div></div>
			<button onclick="Set()">примінити</button>
			<button onclick="location.href = '/'">на головну</button>
		</div>
		<div class='block footer'>
			<hr>
				<div>
					<a href='mailto:lvm030876@gmail.com'>Valentyn Lobatenko</a>
					<a href='http://esp8266.ru/'>джерело надхнення</a>
				</div>
			(c))=====" __DATE__ R"=====(
		</div>
	</body>
</html>
)=====";

const char* ipIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>ролети</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
		<script>
			function selssid(ssid){
				document.getElementById('ssid').value = ssid;
			}
			function tick(){
				fetch("scanwifi.json")
					.then(response => response.json())
					.then(data => {
						var outstr = "";
						var [block] = document.getElementsByClassName("wifiscan");
						outstr += "<div>WiFi</div><div>MAC</div><div>dBm</div>";
						data.scan.forEach(i => {
							outstr += "<a href='javascript:selssid(\"" + i.ssid + "\")'>" + i.ssid + "</a>";
							outstr += "<div>" + i.mac + "</div>";
							outstr += "<div>" + i.rssi + "</div>";
						})
						block.innerHTML = outstr;
						setTimeout(tick, 500);
					})
			}
            function start(){
				tick();
                document.getElementById("ipStat").value = window.location.host.toString();
				fetch("switch.json")
					.then(response => response.json())
					.then(data => {
						document.getElementById('mac').innerText = data.switch.mac;
					})
            }
            function ipSet(){
                let ipStat = document.getElementById('ipStat').value;
                let ssid = document.getElementById('ssid').value;
                let pass = document.getElementById('pass').value;
				let s = "mem?";
				if (ipStat != "") s += "ipStat=" + ipStat + "&";
				if (ssid != "") s += "ssid=" + ssid + "&";
				if (pass != "") s += "pass=" + pass + "&";
                fetch(s)
	            .then(response => {
                    console.log(response.json());
                    if (ipStat != window.location.host.toString()) location.href = 'http://' + ipStat + '/';
					else location.href = '/'
                    });
            }
        </script>
	</head>
	<body onload='start()'>
		<div class='block header'>ролети<hr><hr>
		</div>
		<div class='block' id='mac'>
		</div>
		<div class='block ctrl'>
            <fieldset>
            <legend>WiFi налаштування</legend>
                <div class="wifiscan">
					<div>WiFi</div><div>MAC</div><div>dBm</div>
				</div>
                <div class="wificode">
                    <div>назва wifi:</div>
                    <input type='text' title='назва wifi' id='ssid'/>
                    <div>пароль wifi:</div>
                    <input type='password' title='пароль wifi' id='pass'/>
                    <div>IP адреса:</div>
                    <input type='text' title='статична IP адреса' name='ipStat' id='ipStat' pattern='\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}'/>
                </div>
            </fieldset>
		</div>
		<div class='block menu'>
			<button onclick="ipSet()">примінити</button>
			<button onclick="location.href = '/reboot'">reboot</button>
			<button onclick="location.href = '/'">на головну</button>
		</div>
		<div class='block footer'>
			<hr>
				<div>
					<a href='mailto:lvm030876@gmail.com'>Valentyn Lobatenko</a>
					<a href='http://esp8266.ru/'>джерело надхнення</a>
				</div>
			(c))=====" __DATE__ R"=====(
		</div>
	</body>
</html>
)=====";

const char* homeIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>вимикач</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.9, user-scalable=no, minimal-ui'>
		<script>
			function displaySwitch() {
				fetch("switch.json")
					.then(response => response.json())
					.then(data => {
						var arr = ['switchA','switchB','switchC'];
						var chClass = name => {
							var a = document.getElementById(name).classList;
							(data.switch[name] == 0)?a.add("cheng"):a.remove("cheng");
						}
						document.getElementById('mac').innerText = data.switch.mac;
						for (const element of arr) chClass(element);
						chClass('pirStatus');
						document.getElementById('pirStatus').innerText = data.switch.lightValue;
						setTimeout(displaySwitch, 500);
					})
			};

			function switch_do(group){
				fetch("switch?switch" + group + "=2");
			}	
		</script>
	</head>
	<body onload='displaySwitch()'>
		<div class='block header'>вимикач<hr><hr>
		</div>
		<div class='block' id='mac'>
		</div>
		<div class='block ctrl'>
			<fieldset>
			<legend>Блок керування</legend>
			<div class="keyboard">
				<button class="btn" id='switchA' onclick='switch_do("A")'>A</button>
				<button class="btn" id='switchB' onclick='switch_do("B")'>B</button>
				<button class="btn" id='switchC' onclick='switch_do("C")'>C</button>
			</div>
			<button class="pir" id="pirStatus" onclick='location.href = "/setauto.htm"'>0</button>
			</fieldset>
		</div>
		<div class='block menu'>
			<button onclick="location.href = '/ir.htm'">iч пульт</button>
			<button onclick="location.href = '/ip.htm'">wifi нал.</button>
			<button onclick="location.href = '/rf.htm'">rf нал.</button>
			<div></div>
			<div></div>
			<button onclick="location.href = '/help'">help</button>
		</div>
		<div class='block footer'>
			<hr>
				<div>
					<a href='mailto:lvm030876@gmail.com'>Valentyn Lobatenko</a>
					<a href='http://esp8266.ru/'>джерело надхнення</a>
				</div>
			(c))=====" __DATE__ R"=====(
		</div>
	</body>
</html>
)=====";

const char* style = R"=====(
hr{
font-size:8px;
border:0;
height:2px;
background:#333;
background-image:-webkit-linear-gradient(left, #ccc, #333, #ccc);
}
body{
margin:0;
background:#fff;
font-family:Arial, cursive;
font-style:italic;
}
.block{
margin:1px auto;
font-size:24px;
color:#E8FF66;
border:double 5px #2d2d2d;
width: 370px;
background:#0059B3;
padding:10px 10px 10px 10px;
border-radius: 5px;
text-align: center;
}
.header{
font-size: 32px;
text-shadow: 1px 1px 2px black, 0 0 1em red;
}
.ctrl{
text-align: left;
}
.ipStat{
font-size: 16px;
display: grid;
grid-gap: 10px;
grid-template-columns: 1fr 1fr;
}
.rfcode {
font-size: 16px;
display: grid;
grid-gap: 10px;
grid-template-columns: 1fr 100px;
}
.wificode {
font-size: 16px;
display: grid;
grid-gap: 10px;
grid-template-columns: 100px 1fr;
}
.wifiscan {
margin-bottom: 10px;
border: inset 3px #ffff00;
padding: 5px;
font-size: 12px;
display: grid;
grid-gap: 3px;
grid-template-columns: 1fr 1fr 40px;
}
.setauto {
font-size: 16px;
display: grid;
grid-gap: 10px;
grid-template-columns: 1fr 50px;
}
.menu{
display: grid;
grid-gap: 10px;
grid-template-columns: repeat(3, 1fr);
}
.footer{
display: grid;
grid-gap: 10px;
font-size:10px;
color:#003378;
}
a{
font-size:14px;
color:#E8FF66;
}
input{
background-color: yellow;
border-radius: 5px;
text-align: center;
}
button:visited, button{
width: 100%;
font-size:14px;
cursor:pointer;
display:inline-block;
font-weight:bold;
text-decoration:none;
white-space:nowrap;
border-radius:5px;
background-image: linear-gradient(rgba(255,255,255,.1), rgba(255,255,255,.05) 49%, rgba(0,0,0,.05) 51%, rgba(0,0,0,.1));
background-color:#1F2485;
color:#E8FF66;
border:1px solid rgba(0,0,0,0.3);
border-bottom-color: #4D8Eff;
box-shadow: inset 0 0.08em 0 rgba(255,255,255,0.7), inset 0 0 0.08em rgba(255,255,255,0.5);
height: 30px;
}
button:focus, button:hover{
background-color:#1F2485;
background-image: linear-gradient(rgba(255,255,255,0.5), rgba(255,255,255,0.2) 49%, rgba(0,0,0,0.05) 51%, rgba(0,0,0,0.15));
}
button:disabled{
color:#0059B3;
background-color:#0059B3;
background-image: linear-gradient(rgba(255,255,255,0.5), rgba(255,255,255,0.2) 49%, rgba(0,0,0,0.05) 51%, rgba(0,0,0,0.15));
}
.keyboard {
margin: 5px 8px 20px;
display: grid;
grid-gap: 10px;
grid-template-columns: repeat(3, 100px);
grid-template-rows: 150px;
}
.btn, .pir{
height: 150px;
font-size: 52px;
outline: none;
border-radius: 40%;
border: 7px solid rgb(255, 0, 0);
}
.pir{
height: 100px;
border-radius: 15px 15px 50% 50%;
border-style: groove;
}
.cheng{
border-color:rgb(0, 255, 0);
}
fieldset {
border-radius: 15px;
}
.barTab{
display: grid;
grid-template-columns: 1fr 1fr 1fr;
}
.tab{
text-align: center;
cursor: pointer;
font-size: 16px;
}
.tabSel{
color: #333;
}
)=====";