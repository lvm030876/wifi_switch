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
				var cfg = ['mac','switchA','switchB','switchC','pirStatus','lightValue'];

				function xml_from(x){
					return xmlDoc.getElementsByTagName(cfg[x])[0].innerHTML
				}
				
				function xml_to(x, y){
					document.getElementById(cfg[x]).innerText = y;
				}
				
				function xml_to_css(x, y){
					let a = document.getElementById(cfg[x]).classList;
					y?a.add("cheng"):a.remove("cheng");
				}

				fetch("switch.xml")
					.then(response => response.text())
					.then(data => {
					let parser = new DOMParser();
					xmlDoc = parser.parseFromString(data,"text/xml");
					xml_to(0, xml_from(0));
					for (var i = 1; i < 4; i++) xml_to_css(i, (xml_from(i) == '0'));
					xml_to_css(4, (xml_from(4) == '0'));
					xml_to(4, xml_from(5));
					setTimeout('displaySwitch()',500);
					})
					.catch(() => {setTimeout('displaySwitch()',500)})
			};

			function switch_do(group){
				fetch("switch?switch"+group+"=2");
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