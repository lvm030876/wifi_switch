#ifndef setauto_h
#define setauto_h

const char* pirIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>вимикач</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
		<script>
            var cfg = ['mac','pirTime','pirLight', 'pirGroup'];

            function start(){
                function _from(x){
                    return xmlDoc.getElementsByTagName(cfg[x])[0].innerHTML
                }
                
                function _to(x){
                    document.getElementById(cfg[x]).innerText = _from(x);
                }
                
                function _too(x){
                    document.getElementById(cfg[x]).value = _from(x);
                }

                fetch("switch.xml")
                .then(response => response.text())
                .then(data => {
                    let parser = new DOMParser();
                    xmlDoc = parser.parseFromString(data,"text/xml");
		            _to(0);
                });

                fetch("pir.xml")
                .then(response => response.text())
                .then(data => {
                    let parser = new DOMParser();
                    xmlDoc = parser.parseFromString(data,"text/xml");
		            _too(1);
		            _too(2);
		            _too(3);
                })
            }
            function Set(){
                function _from(x){
                    return document.getElementById(cfg[x]).value;
                }

                let a = document.getElementById('pirTime').value;
                let b = document.getElementById('pirLight').value;
                fetch("mem?pirTime="+_from(1)+"&pirLight="+_from(2)+"&pirGroup="+_from(3))
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
#endif