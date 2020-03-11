const Net = require('net')
const Express = require('express')
const App = Express()
const Server = app.listen(3000)
const io = require('socket.io').listen(Server)
const Cors = require('cors')
const MySQL = require('mysql')
const BodyParser = require('body-parser')

App.use(BodyParser.urlencoded({ extended: false }))
App.use(BodyParser.json())

const DB = MySQL.createConnection({
  host: 'localhost',
  user: 'root',
  password: 'raspberry',
  port: 3306,
  database: 'sensor_data'
})

function queryDB(sql, args) {
  return new Promise((resolve, reject) => {
    DB.query(sql, args, (err, rows) => {
      if (err) { return reject(err) }
      resolve(rows)
    })
  })
}

App.use(Cors({ origin: true, credentials: true }))

const tcpClient = new Net.Socket()
tcpClient.connect(1337, '127.0.0.1', function() { /* ... */ })

const SensorLibrary = ["LSM", "NXP", "Bosch"]
const SensorType = ["Accelerometer", "Magnetometer", "Gyroscope"]

let JSONReads = []
let count = 0
let expected = 0
tcpClient.on('data', function (data) {
	if (count === 0 && data.search(/^START/gi)) {
    expected = parseInt(data.split(',')[1])
  } else { 
    let dataArray = data.split(',')
    JSONReads.push({
      library: SensorLibrary[parseInt(dataArray[0]) - 1],
      sensor: SensorType[parseInt(dataArray[1]) - 1],
      x: parseFloat(dataArray[2]),
      y: parseFloat(dataArray[3]),
      z: parseFloat(dataArray[4]),
      time: parseInt(dataArray[5]) * 60000 + parseInt(dataArray[6]) / 1000000 
    })
  }

  if (count === expected && data.search(/^END/gi)) {
    count = 0
    expected = 0
    io.emit(JSON.stringify(JSONReads))
    JSONReads = []
  } else {
    throw new Error("Socket Didn't send all the info :(")
  }
})

tcpClient.on('close', function () {
  /* ... */
})

io.on('connection', client => {
  client.on('event', data => { 
    let dataJSON = JSON.stringify(data)
    if (dataJSON.event === 'read') {
      tcpClient.write('read')
    }
  })
  client.on('disconnect', () => { /* … */ })
})