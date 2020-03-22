#ifndef rfstat_h
#define rfstat_h

const char* rfIndex = R"=====(
<!DOCTYPE html>
<html>
	<head>
		<title>вимикач</title>
		<link rel='stylesheet' href='style.css'>
		<meta http-equiv='Content-Type' content='text/html; charset=UTF-8'>
		<meta name='viewport' content='width=device-width, initial-scale=0.7'>
        <script>
            var cfg = ['rfCode', 'rfAOn', 'rfBOn', 'rfCOn', 'rfOn', 'rfOff'];
            var rfcode;

            function tick(){
                fetch("rf.xml")
                .then(response => response.text())
                .then(data => {
                    let parser = new DOMParser();
                    xmlDoc = parser.parseFromString(data,"text/xml");// rfCode
		            let t = xmlDoc.getElementsByTagName("rfCode")[0].innerHTML
		            if (t != rfcode){
                        document.getElementById("rfCode").value = t;
                        rfcode = t;
                    }
                })
            }
            
            function _from(x){
                return xmlDoc.getElementsByTagName(x)[0].innerHTML
            }
            
            function _to(x){
                document.getElementById(x).innerText = _from(x);
            }

            function start(){
		        setInterval(tick, 500);
                var allButtons = document.querySelectorAll('.rfcode>button');
                for (var i = 0; i < allButtons.length; i++) {
                    allButtons[i].addEventListener('click', function() {
                        let a = document.getElementById('rfCode').value;
                        fetch("mem?"+this.id+"="+a)
                        .then(response => response.json())
                        .then(data => this.innerText = a);
                    });
                }

                fetch("switch.xml")
                .then(response => response.text())
                .then(data => {
                    let parser = new DOMParser();
                    xmlDoc = parser.parseFromString(data,"text/xml");
                    _to('mac');
                });

                fetch("rf.xml")
                .then(response => response.text())
                .then(data => {
                    let parser = new DOMParser();
                    xmlDoc = parser.parseFromString(data,"text/xml");
                    cfg.forEach(element => _to(element));
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

#endif