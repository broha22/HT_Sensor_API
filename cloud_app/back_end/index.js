const Net = require('net')
const Express = require('express')
const App = Express()
const server = App.listen(3000)
const io = require('socket.io').listen(server)
const Cors = require('cors')
const path = require('path')
const BodyParser = require('body-parser')

App.use(BodyParser.urlencoded({ extended: false }))
App.use(BodyParser.json())

function queryDB(sql, args) {
  return new Promise((resolve, reject) => {
    DB.query(sql, args, (err, rows) => {
      if (err) { return reject(err) }
      resolve(rows)
    })
  })
}

App.use(Cors({ origin: true, credentials: true }))
App.use(Express.static('../front_end/dist'))
App.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '../front_end/dist/index.html'))
})

const tcpClient = new Net.Socket()
tcpClient.connect(1337, 'pizero.local', function() { 
 /* ... */
})

const SensorLibrary = ["LSM", "NXP", "Bosch"]
const SensorType = ["Accelerometer", "Magnetometer", "Gyroscope"]

let JSONReads = []
let runningText = ''
tcpClient.on('data', function (data) {
  const dataString = data.toString('utf8').replace(/\0/g, '')
  let endSplit = dataString.split('END')
  runningText += endSplit[0]
  if (endSplit.length > 1 || dataString.search('END') >= 0) {
    const rows = runningText.split('\n')
    for (let r in rows) {
      const cols = rows[r].split(',')
      if (r == rows.length - 1) {
        io.of('/').emit('data', JSON.stringify(JSONReads))

        count = 0
        expected = 0
        JSONReads = []
        runningText = ''
        if (endSplit.length > 1) {
          runningText += endSplit
        }
      } else if (r !== 0) {
        if (cols.length === 8) {
          JSONReads.push({
            id: parseInt(cols[7].trim()),
            library: SensorLibrary[parseInt(cols[0].trim()) - 1],
            sensor: SensorType[parseInt(cols[1].trim()) - 1],
            x: parseFloat(cols[2].trim()),
            y: parseFloat(cols[3].trim()),
            z: parseFloat(cols[4].trim()),
            time: Math.floor(parseInt(cols[5].trim()) * 1000 + parseInt(cols[6]) / 1000000)
          })
        }
      }
    }
  }
})

tcpClient.on('close', function () {
  /* ... */
})


io.on('connection', client => {
  // io.of('/').emit('test', 'Test')
  // client.emit('test', 'test')
  client.on('event', data => { 
    if (data.trim() === 'read') {
      tcpClient.write('read')
    }
  })
  // console.log(io.sockets.name)
  client.on('disconnect', () => { /* … */ })
})

App.on('listening', () => {
  console.log('Server Listening')
})
