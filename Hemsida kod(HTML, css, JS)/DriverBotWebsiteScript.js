// Variables for MQTT client connection
var client; // MQTT client object
var host = "maqiatto.com"; // MQTT broker hostname
var port = "8883"; // MQTT broker port
var topicX = "catidifat@gmail.com/Xmove"; // Topic for X-axis movement
var topicY = "catidifat@gmail.com/Ymove"; // Topic for Y-axis movement
var AutonomousTopic = "catidifat@gmail.com/AutonomousActivation"; // Topic for autonomous mode activation
var Connected = false; // Connection status flag
var clientID; // Unique client ID

// Event listeners for button clicks
document.getElementById('connectButton').addEventListener('click', startConnect); // Connect button
document.getElementById('startButton').addEventListener('click', StartAutonomous); // Start Autonomous button

var timer; // Timer variable
var startTime; // Variable to store the start time

// Object to store the state of each key (pressed or not)
const activeKeys = {};

// Event listener for keydown events
document.addEventListener('keydown', function(event) {
    const keyCode = event.keyCode; // Get the key code of the pressed key
    const key = document.querySelector(`div[data-key="${keyCode}"]`); // Find the corresponding key element

    // Check if the key is one of the control keys (W, A, S, D) and is not already pressed
    if (key && !activeKeys[keyCode]) {
        if (keyCode === 87) {
            // W key (forward)
            activeKeys[keyCode] = { topic: topicY, value: 1 };
        } else if (keyCode === 65) {
            // A key (left)
            activeKeys[keyCode] = { topic: topicX, value: -1 };
        } else if (keyCode === 83) {
            // S key (backward)
            activeKeys[keyCode] = { topic: topicY, value: -1 };
        } else if (keyCode === 68) {
            // D key (right)
            activeKeys[keyCode] = { topic: topicX, value: 1 };
        }

        key.classList.add("activeKey"); // Add the "activeKey" class to highlight the key
        updateMovement(); // Update the movement based on the active keys
    }
});

// Event listener for keyup events
document.addEventListener('keyup', function(event) {
    const keyCode = event.keyCode; // Get the key code of the released key
    const key = document.querySelector(`div[data-key="${keyCode}"]`); // Find the corresponding key element

    // Check if the key is one of the control keys (W, A, S, D) and is currently pressed
    if (key && activeKeys[keyCode]) {
        delete activeKeys[keyCode]; // Remove the key from the activeKeys object
        key.classList.remove("activeKey"); // Remove the "activeKey" class to un-highlight the key
        updateMovement(); // Update the movement based on the active keys
    }
});

// Function to update movement based on active keys
function updateMovement() {
    let xValue = 0; // Initialize x-axis value
    let yValue = 0; // Initialize y-axis value

    // Iterate through the active keys to calculate x and y values
    for (const key in activeKeys) {
        if (activeKeys[key].topic === topicX) {
            xValue += activeKeys[key].value;
        } else if (activeKeys[key].topic === topicY) {
            yValue += activeKeys[key].value;
        }
    }

    // Publish the updated x and y values
    publishMessage(topicX, xValue.toString());
    publishMessage(topicY, yValue.toString());
}

// Function to start the MQTT connection
function startConnect() {
    if (!Connected) {
        // Generate a unique client ID
        clientID = "clientID_" + parseInt(Math.random() * 100);
        // Create a new MQTT client
        client = new Paho.MQTT.Client(host, Number(port), clientID);
        // Set callback handlers
        client.onConnectionLost = onConnectionLost;
        client.onMessageArrived = onMessageArrived;
        // Connect to the MQTT broker
        client.connect({
            userName: "catidifat@gmail.com",
            password: "Mybroker",
            onSuccess: onConnect, // On successful connection
            onFailure: onFail, // On connection failure
        });
    } else {
        // Disconnect if already connected
        startDisconnect();
    }
}

// Function to handle connection failure
function onFail() {
    Connected = false; // Set connection status to false
    updateStatus(); // Update the status display
}

// Function to handle successful connection
function onConnect() {
    Connected = true; // Set connection status to true
    updateStatus(); // Update the status display
    // Subscribe to the control topics
    client.subscribe(topicX);
    client.subscribe(topicY);
    client.subscribe(AutonomousTopic);
}

// Function to handle incoming messages
function onMessageArrived(message) {
    console.log("Received message on topic: " + message.destinationName);
    console.log("Message payload: " + message.payloadString);
}

// Function to handle connection loss
function onConnectionLost(responseObject) {
    Connected = false; // Set connection status to false
    updateStatus(); // Update the status display
}

// Function to disconnect the client
function startDisconnect() {
    client.disconnect(); // Disconnect from the MQTT broker
    Connected = false; // Set connection status to false
    updateStatus(); // Update the status display
}

// Function to update connection status on the UI
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

// Event listener for mode switch (Manual/Autonomous)
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

// Function to start autonomous mode
function StartAutonomous() {
    var button = document.getElementById("startButton");
    if (button.textContent === "START") {
        button.textContent = "STOP";
        publishMessage(AutonomousTopic, "Auto");
        resetTimer();
        startTimer();
    } else {
        button.textContent = "START";
        publishMessage(AutonomousTopic, "Auto");
        stopTimer();
    }
}
// Function to start the timer
function startTimer() {
    startTime = Date.now(); // Record the start time
    timer = setInterval(updateTimer, 1000); // Update the timer every second
}

// Function to stop the timer
function stopTimer() {
    clearInterval(timer); // Stop the timer
    
}

// Function to reset the timer
function resetTimer() {
    document.getElementById("timerDisplay").textContent = "Time spent self driving: 0s"; // Reset the display
}

// Function to update the timer display
function updateTimer() {
    var currentTime = Date.now(); // Get the current time
    var elapsedTime = Math.floor((currentTime - startTime) / 1000); // Calculate the elapsed time in seconds
    document.getElementById("timerDisplay").textContent = "Time spent self driving: " + elapsedTime + "s"; // Update the display
}


// Function to initialize the joystick
function init() {
    var xCenter = 150;
    var yCenter = 150;
    var stage = new createjs.Stage('joystick');

    // Draw the joystick base
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

    // Event listener for joystick pan start
    mc.on("panstart", function(ev) {
        var pos = $('#joystick').position();
        xCenter = psp.x;
        yCenter = psp.y;
        psp.alpha = 0.5;
        stage.update();
    });

    // Event listener for joystick pan move
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

    // Event listener for joystick pan end
    mc.on("panend", function(ev) {
        psp.alpha = 0.25;
        createjs.Tween.get(psp).to({x:xCenter, y:yCenter}, 200, createjs.Ease.getPowInOut(2.5));
        publishMessage(topicY, "0");
        publishMessage(topicX, "0");
    });
}

// Function to calculate coordinates based on angle and distance
function calculateCoords(angle, distance) {
    var coords = {};
    distance = Math.min(distance, 100); // Limit the maximum distance
    var rads = (angle * Math.PI) / 180.0; // Convert angle to radians
    coords.x = distance * Math.cos(rads); // Calculate x coordinate
    coords.y = distance * Math.sin(rads); // Calculate y coordinate
    return coords;
}

// Function to publish messages to MQTT topics
function publishMessage(topic, message) {
    if (client && client.isConnected()) {
        var mqttMessage = new Paho.MQTT.Message(message);
        console.log("Sending a message : " + mqttMessage)
        mqttMessage.destinationName = topic;
        client.send(mqttMessage);
    }
}

// Function to generate a random hex color
function getRandomColor() {
    return '#' + Math.floor(Math.random()*16777215).toString(16);
}

// Function to generate a random colorful gradient background
function generateRandomGradient() {
    // Define a range of colorful non-white/bright colors
    var colorfulColors = ['#FF5733', '#C70039', '#900C3F', '#581845', '#DAF7A6', '#FFC300', '#FF5733', '#FF5733', '#FF5733'];

    // Select random colors from the range
    var color1 = colorfulColors[Math.floor(Math.random() * colorfulColors.length)];
    var color2 = colorfulColors[Math.floor(Math.random() * colorfulColors.length)];

    // Ensure colors are different
    while (color1 === color2) {
        color2 = colorfulColors[Math.floor(Math.random() * colorfulColors.length)];
    }

    // Generate random angle for gradient direction
    var angle = Math.floor(Math.random() * 360); // Random angle between 0 and 360 degrees

    // Create gradient background
    var gradient = 'linear-gradient(' + angle + 'deg, ' + color1 + ', ' + color2 + ')';

    // Apply gradient background to the body
    document.body.style.background = gradient;
}

// Function to update the status text color and outline color
function updateStatusColor(textColor, outlineColor) {
    var statusText = document.getElementById("statusText");
    statusText.style.color = textColor; // Set text color
    document.documentElement.style.setProperty('--outline-color', outlineColor); // Set outline color
}

// Event listener for window load event
window.onload = function() {
    // Generate random gradient background when the window is loaded
    generateRandomGradient();
    init();
};