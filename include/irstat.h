#ifndef irstat_h
#define irstat_h

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
				fetch("switch.xml")
				.then(response => response.text())
				.then(data => {
					let parser = new DOMParser();
					xmlDoc = parser.parseFromString(data,"text/xml");
					document.getElementById('mac').innerText = xmlDoc.getElementsByTagName('mac')[0].innerHTML;
				});

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
#endif