const net = require('net');
const fs = require('fs');
const path = require('path');

const outputDir = './output';
let lastAccNum = 0;

// Ensure the output directory exists
if (!fs.existsSync(outputDir)) {
    fs.mkdirSync(outputDir);
}

function formatAccName(num) {
    var s = num.toString();
    while (s.length < 3) {
        s = '0' + s;
    }
    return s + '.csv';
}

function getUniqueAccOutputPath() {
    var outPath = path.join(outputDir, formatAccName(lastAccNum + 1));
    lastAccNum++;
    return outPath;
}

net.createServer(function (socket) {
    console.log('data connection started from ' + socket.remoteAddress);
    socket.setEncoding('utf8');

    var accOutPath = getUniqueAccOutputPath();
    var accWriter = fs.createWriteStream(accOutPath);

    // Write the header to the CSV file
    accWriter.write('timestamp,accX,accY,accZ,pressure,gyroX,gyroY,gyroZ\n');

    let buffer = '';

    socket.on('data', function (data) {
        buffer += data;

        let index;
        while ((index = buffer.indexOf('\n')) >= 0) {
            let line = buffer.slice(0, index);
            buffer = buffer.slice(index + 1);

            console.log('Received line:', line);

            let dataParts = line.split(',');
            if (dataParts.length == 8) { // Expecting 8 values: timestamp, accX, accY, accZ, pressure, gyroX, gyroY, gyroZ
                let sensorData = {
                    timestamp: parseInt(dataParts[0], 10),
                    accX: parseInt(dataParts[1], 10),
                    accY: parseInt(dataParts[2], 10),
                    accZ: parseInt(dataParts[3], 10),
                    pressure: parseInt(dataParts[4], 10),
                    gyroX: parseInt(dataParts[5], 10),
                    gyroY: parseInt(dataParts[6], 10),
                    gyroZ: parseInt(dataParts[7], 10)
                };
                accWriter.write(`${sensorData.timestamp},${sensorData.accX},${sensorData.accY},${sensorData.accZ},${sensorData.pressure},${sensorData.gyroX},${sensorData.gyroY},${sensorData.gyroZ}\n`);
            } else {
                console.error('Invalid sensor data format');
            }
        }
    });

    socket.on('end', function () {
        console.log('transmission complete, saved to ' + accOutPath);
        accWriter.end();
    });
}).listen(7123);