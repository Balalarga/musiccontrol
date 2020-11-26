
function sendMessage() {
    var nickname = document.getElementById("inputNick").value;
    var msg = document.getElementById("inputText").value;
    var strToSend = nickname + ": " + msg;
    if ( websocket != null )
    {
        document.getElementById("inputText").value = "";
        websocket.send( strToSend );
        console.log( "string sent :", '"'+strToSend+'"' );
    }
}

var websocket = null;

function initWebSocket() {
    try {
        if (typeof MozWebSocket == 'function')
            WebSocket = MozWebSocket;
        if ( websocket && websocket.readyState == 1 )
            websocket.close();
        var wsUri = "ws://" + location.hostname+":5413";
        websocket = new WebSocket( wsUri );
        websocket.onopen = function (evt) {
            document.getElementById("disconnectButton").disabled = false;
            document.getElementById("sendButton").disabled = false;
        };
        websocket.onclose = function (evt) {
            document.getElementById("disconnectButton").disabled = true;
            document.getElementById("sendButton").disabled = true;
        };
        websocket.onmessage = function (evt) {
            console.log( "Message received :", evt.data );
        };
        websocket.onerror = function (evt) {
        };
    } catch (exception) {
        console.log(exception)
        throw exception
    }
}

function stopWebSocket() {
    if (websocket)
        websocket.close();
}

function checkSocket() {
    if (websocket != null) {
        var stateStr;
        switch (websocket.readyState) {
            case 0: {
                stateStr = "CONNECTING";
                break;
            }
            case 1: {
                stateStr = "OPEN";
                break;
            }
            case 2: {
                stateStr = "CLOSING";
                break;
            }
            case 3: {
                stateStr = "CLOSED";
                break;
            }
            default: {
                stateStr = "UNKNOW";
                break;
            }
        }
    } else {
        console.log("WebSocket is null");
    }
}