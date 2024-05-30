var client;
var host = "maqiatto.com";
var port = "8883";
var topicX = "catidifat@gmail.com/Xmove";
var topicY = "catidifat@gmail.com/Ymove";
var AutonomousTopic = "catidifat@gmail.com/AutonomousActivation";
var Connected = false;
var clientID;

document.getElementById('connectButton').addEventListener('click', startConnect);
document.getElementById('startButton').addEventListener('click', StartAutonomous);

const activeKeys = {}; // Object to store the state of each key

document.addEventListener('keydown', function(event) {
    const keyCode = event.keyCode;
    const key = document.querySelector(`div[data-key="${keyCode}"]`);

    if (key && !activeKeys[keyCode]) {
        if (keyCode === 87) {
            // W key
            activeKeys[keyCode] = { topic: topicY, value: 1 };
        } else if (keyCode === 65) {
            // A key
            activeKeys[keyCode] = { topic: topicX, value: -1 };
        } else if (keyCode === 83) {
            // S key
            activeKeys[keyCode] = { topic: topicY, value: -1 };
        } else if (keyCode === 68) {
            // D key
            activeKeys[keyCode] = { topic: topicX, value: 1 };
        }

        key.classList.add("activeKey"); // Add the "activeKey" class to the key
        updateMovement();
    }
});

document.addEventListener('keyup', function(event) {
    const keyCode = event.keyCode;
    const key = document.querySelector(`div[data-key="${keyCode}"]`);

    if (key && activeKeys[keyCode]) {
        delete activeKeys[keyCode];
        key.classList.remove("activeKey"); // Remove the "activeKey" class from the key
        updateMovement();
    }
});

function updateMovement() {
    let xValue = 0;
    let yValue = 0;

    for (const key in activeKeys) {
        if (activeKeys[key].topic === topicX) {
            xValue += activeKeys[key].value;
        } else if (activeKeys[key].topic === topicY) {
            yValue += activeKeys[key].value;
        }
    }

    publishMessage(topicX, xValue.toString());
    publishMessage(topicY, yValue.toString());
}

function startConnect() {
    if (!Connected) {
        clientID = "clientID_" + parseInt(Math.random() * 100);
        client = new Paho.MQTT.Client(host, Number(port), clientID);
        client.onConnectionLost = onConnectionLost;
        client.onMessageArrived = onMessageArrived;
        client.connect({
            userName: "catidifat@gmail.com",
            password: "Mybroker",
            onSuccess: onConnect,
            onFailure: onFail,
        });
    } else {
        startDisconnect();
    }
}

function onFail() {
    Connected = false;
    updateStatus();
}

function onConnect() {
    Connected = true;
    updateStatus();
    client.subscribe(topicX);
    client.subscribe(topicY);
    client.subscribe(AutonomousTopic);
}

function onMessageArrived(message) {
    console.log("Received message on topic: " + message.destinationName);
    console.log("Message payload: " + message.payloadString);
}

function onConnectionLost(responseObject) {
    Connected = false;
    updateStatus();
}

function startDisconnect() {
    client.disconnect();
    Connected = false;
    updateStatus();
}

function updateStatus() {
    var statusText = document.getElementById("statusText");
    if (Connected) {
        statusText.textContent = "Status: Active";
        statusText.style.color = "#4CAF50"; // Green color for active status
        document.getElementById("connectButton").textContent = "Disconnect";
    } else {
        statusText.textContent = "Status: Disconnected";
        statusText.style.color = "#F44336"; // Red color for inactive status
        document.getElementById("connectButton").textContent = "Connect";
    }
}

document.getElementById('modeSwitch').onchange = function () {
    var manualMenu = document.getElementById('manualMenu');
    var autonomousMenu = document.getElementById('autonomousMenu');
    if (this.checked) {
        manualMenu.classList.remove('active');
        autonomousMenu.classList.add('active');
        this.nextElementSibling.textContent = 'Autonomous';
    } else {
        autonomousMenu.classList.remove('active');
        manualMenu.classList.add('active');
        this.nextElementSibling.textContent = 'Manual';
    }
};
function StartAutonomous()
{
    publishMessage(AutonomousTopic, "Auto");
}
function init() {
    var xCenter = 150;
    var yCenter = 150;
    var stage = new createjs.Stage('joystick');

    var psp = new createjs.Shape();
    psp.graphics.beginFill('#999999').drawCircle(xCenter, yCenter, 50);
    psp.alpha = 0.25;

    var vertical = new createjs.Shape();
    var horizontal = new createjs.Shape();

    stage.addChild(psp);
    stage.addChild(vertical);
    stage.addChild(horizontal);
    createjs.Ticker.framerate = 120;
    createjs.Ticker.addEventListener('tick', stage);
    stage.update();

    var myElement = $('#joystick')[0];

    var mc = new Hammer(myElement);

    mc.on("panstart", function(ev) {
        var pos = $('#joystick').position();
        xCenter = psp.x;
        yCenter = psp.y;
        psp.alpha = 0.5;
        stage.update();
    });

    mc.on("panmove", function(ev) {
        var pos = $('#joystick').position();
        var coords = calculateCoords(ev.angle, ev.distance);
        var xScaled = coords.x / 100;
        var yScaled = coords.y / 100 * -1;
        publishMessage(topicX, xScaled.toString());
        publishMessage(topicY, yScaled.toString());
        psp.x = coords.x;
        psp.y = coords.y;
        psp.alpha = 0.5;
        stage.update();
    });

    mc.on("panend", function(ev) {
        psp.alpha = 0.25;
        createjs.Tween.get(psp).to({x:xCenter, y:yCenter}, 200, createjs.Ease.getPowInOut(2.5));
        publishMessage(topicY, "0");
        publishMessage(topicX, "0");
    });
}

function calculateCoords(angle, distance) {
    var coords = {};
    distance = Math.min(distance, 100);
    var rads = (angle * Math.PI) / 180.0;
    coords.x = distance * Math.cos(rads);
    coords.y = distance * Math.sin(rads);
    return coords;
}

function publishMessage(topic, message) {
    if (client && client.isConnected()) {
        var mqttMessage = new Paho.MQTT.Message(message);
        console.log("Sending a message : " + mqttMessage)
        mqttMessage.destinationName = topic;
        client.send(mqttMessage);
    }
}
