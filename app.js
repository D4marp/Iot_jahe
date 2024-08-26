const client = mqtt.connect('ws://broker.hivemq.com:8000/mqtt');

client.on('connect', () => {
    console.log('Connected to MQTT broker');
    client.subscribe('iot/#'); // Subscribe to all topics under 'iot/'
});

client.on('message', (topic, message) => {
    const value = message.toString();

    // Handle incoming messages based on topic
    if (topic === 'iot/suhu') {
        // Update temperature display
        document.getElementById('suhu').textContent = value;
    } else if (topic === 'iot/waktu') {
        // Update time display
        document.getElementById('waktu').textContent = value;
    } else if (topic === 'iot/kecepatan') {
        // Update speed display
        document.getElementById('kecepatan').textContent = value;
    } else if (topic === 'iot/power') {
        // Update power button state
        const powerButton = document.getElementById('powerButton');
        if (value === 'ON') {
            powerButton.classList.add('on');
            powerButton.textContent = 'ON';
        } else if (value === 'OFF') {
            powerButton.classList.remove('on');
            powerButton.textContent = 'OFF';
        }
    }
});

function adjustValue(control, increment) {
    const element = document.getElementById(control);
    let value = parseInt(element.textContent, 10);
    value += increment;
    if (value < 0) value = 0;  // Prevent negative values
    element.textContent = value;
    client.publish(`iot/${control}`, value.toString()); // Publish updated value to MQTT
}

function togglePower() {
    const powerButton = document.getElementById('powerButton');
    let newState;
    if (powerButton.classList.contains('on')) {
        newState = 'OFF';
    } else {
        newState = 'ON';
    }
    client.publish('iot/power', newState);
}
