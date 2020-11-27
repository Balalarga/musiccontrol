// Websocket logic
var websocket = null;
window.onload = setInterval(initWebSocket, 1000)
function initWebSocket() {
    if(!websocket)
        try {
            if (typeof MozWebSocket == 'function')
                WebSocket = MozWebSocket;
            if ( websocket && websocket.readyState == 1 )
                websocket.close();
            var wsUri = "ws://"+location.hostname+":5413";
            websocket = new WebSocket( wsUri );
            websocket.onopen = function (evt) {
                console.log("Connected to player")
            };
            websocket.onclose = function (evt) {
                stopWebSocket();
            };
            websocket.onmessage = getMessage;
            websocket.onerror = function (evt) {
                console.log(evt)
            };
            setInterval(tick, 100);
        } catch (exception) {
            console.log(exception)
        }
}

function getMessage(event){
    const fr = new FileReader();
    fr.onload = function() {
        const data = JSON.parse(this.result)
        console.log(data)
        if(data.hasOwnProperty("trackTime")){
            console.log("trackTime setup")
            total_duration.textContent = formatTime(data.trackTime)
            seek_slider.max = data.trackTime
            console.log("Slider max = " + seek_slider.max)
        }
        if(data.hasOwnProperty("currentTrack")){
            console.log("currentTrack setup")
            track_name.textContent = data.currentTrack
        }
        if(data.hasOwnProperty("volume")){
            console.log("volume setup")
            volume_slider.value = data.volume
        }
        if(data.hasOwnProperty("playing")){
            console.log("playing setup")
            isPlaying = data.playing
            if(isPlaying)
                playpause_btn.innerHTML = '<i class="fa fa-pause-circle fa-5x"></i>';
            else
                playpause_btn.innerHTML = '<i class="fa fa-play-circle fa-5x"></i>';
        }
        if(data.hasOwnProperty("position")){
            console.log("position setup")
            seek_slider.value = data.position
            timer = data.position
            console.log("Timer = " + timer)
            console.log("Slider = " + seek_slider.value)
            console.log("Position = " + data.position)
        }
    };
    fr.readAsText(event.data);
}

function stopWebSocket() {
    isPlaying = false;
    if (websocket)
        websocket.close();
}

let track_name = document.querySelector(".track-name");
  
let playpause_btn = document.querySelector(".playpause-track");
  
let seek_slider = document.querySelector(".seek_slider");
let volume_slider = document.querySelector(".volume_slider");
let curr_time = document.querySelector(".current-time");
let total_duration = document.querySelector(".total-duration");

let isPlaying = false;
let timer = 0;
  
function playpauseTrack() {
    if (!isPlaying) playTrack();
    else pauseTrack();
}

function tick(){
    if(isPlaying){
        timer += 10;
        console.log("ticks " + timer)
        seek_slider.value = Math.ceil(timer)
        curr_time.textContent = formatTime(seek_slider.value)
        console.log("Ser slider = "+Math.ceil(timer))
    }
}

function playTrack() { 
    var obj = {
        command:"play"
    }
    rawData = new Blob([JSON.stringify(obj)])
    if(websocket)
        websocket.send(rawData)
    isPlaying = true;

    playpause_btn.innerHTML = '<i class="fa fa-pause-circle fa-5x"></i>'; 
}
  
function pauseTrack() { 
    var obj = {
        command:"pause"
    }
    rawData = new Blob([JSON.stringify(obj)])
    if(websocket)
        websocket.send(rawData)
    isPlaying = false; 

    playpause_btn.innerHTML = '<i class="fa fa-play-circle fa-5x"></i>';
}
  
function nextTrack() { 
    var obj = {
        command:"next"
    }
    rawData = new Blob([JSON.stringify(obj)])
    if(websocket)
        websocket.send(rawData)
} 
  
function prevTrack() {
    var obj = {
        command:"prev"
    }
    rawData = new Blob([JSON.stringify(obj)])
    if(websocket)
        websocket.send(rawData)
}

function setVolume(){
    var obj = {
        command:"volume",
        value: volume_slider.value
    }
    console.log("Volume changed " + volume_slider.value)
    rawData = new Blob([JSON.stringify(obj)])
    if(websocket)
        websocket.send(rawData)
}

function seekTo(){
    var obj = {
        command:"position",
        value: seek_slider.value
    }
    timer = seek_slider.value;
    console.log("Position changed " + seek_slider.value)
    rawData = new Blob([JSON.stringify(obj)])
    if(websocket)
        websocket.send(rawData)
}
function formatTime(mils){
    let seconds = Math.floor(mils / 1000);
    let minutes = Math.floor(seconds / 60);
    seconds -= minutes * 60;
    return "{0}:{1}".format(minutes, seconds)
}
String.prototype.format = function() {
    a = this;
    for (k in arguments) {
      a = a.replace("{" + k + "}", arguments[k])
    }
    return a
}