const Net = require('net')
const Express = require('express')
const App = Express()
const server = App.listen(3000)
const io = require('socket.io').listen(server) //use web sockets to communicate with Front end
const Cors = require('cors')
const path = require('path')
const BodyParser = require('body-parser')

//Parse body as JSON (although I dont have anything that would use this yet)
App.use(BodyParser.urlencoded({ extended: false }))
App.use(BodyParser.json())

/* Allow cross site origin requests */
App.use(Cors({ origin: true, credentials: true }))

//Serve static built front end through express
App.use(Express.static('../front_end/dist'))
App.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, '../front_end/dist/index.html'))
})

//Start TCP Socket Client and connect to the RPi
const tcpClient = new Net.Socket()
tcpClient.connect(1337, 'pizero.local', function() { 
 /* ... */
})

//some const array maps
const SensorLibrary = ["LSM", "NXP", "Bosch"]
const SensorType = ["Accelerometer", "Magnetometer", "Gyroscope"]

let JSONReads = []
let runningText = ''
//wait for tcp client to send data
tcpClient.on('data', function (data) {
  //parse the incoming string and remove null chars
  const dataString = data.toString('utf8').replace(/\0/g, '')
  let endSplit = dataString.split('END') //split on the message terminator
  
  //accumulate text as messages dont come in line by line or message by mesage
  runningText += endSplit[0]

  //when an END is send then process the accumulated text
  if (endSplit.length > 1 || dataString.search('END') >= 0) {
    const rows = runningText.split('\n') //first split by line
    for (let r in rows) {
      const cols = rows[r].split(',') //then split into columns by comma
      
      //if the last row is reached then the data is ready to be sent
      if (r == rows.length - 1) {

        //broadcast the data through web sockets to all connected clients
        io.of('/').emit('data', JSON.stringify(JSONReads))
        
        //clear out all accumulator variables
        count = 0
        expected = 0
        JSONReads = []
        runningText = ''

        //add back second portion of data that was split by END
        if (endSplit.length > 1) {
          runningText += endSplit
        }

      //if its not the last row then parse columns and add them to a json object
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

//not implemented
tcpClient.on('close', function () {
  /* ... */
})

//listen for web socket connection
io.on('connection', client => {

  //listen for the client to send something
  client.on('event', data => {

    //if the client sends a read then route that to the TCP Socket
    if (data.trim() === 'read') {
      tcpClient.write('read')
    }
  })

  //Not implemented
  client.on('disconnect', () => { /* … */ })
})

//give some confirmation when express boots up all the way
App.on('listening', () => {
  console.log('Server Listening')
})
