/*
 * @Author: you@you.you
 * @Date:   Wednesday March 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Wednesday March 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

/* Sensor Structure
 id:
 library:
 type:
 reads: [
   {
     time
     x
     y
     z
   }
 ]
*/

//create state as function so it is always created with
//these values and not copied
const state = () => {
  return {
    sensors: [],
    socket: null,
    timer: null
  }
}

const store = {
  state: state(),
  getters: {
    sensors: (state) => {
      return state.sensors
    },
    socket: (state) => {
      return state.socket
    },
    updating: (state) => {
      if (state.timer) {
        return true
      } else {
        return false
      }
    },
    timer: (state) => {
      return state.timer
    },
    //Gets the data for the sensor in chart.js format
    sensorData: (state) => (id) => {
      const sensorReads = state.sensors[id].reads
      const dataCollection = {
        labels: [],
        //for each axis assign label and color then map the sensor data
        datasets: [{
          label: 'X',
          backgroundColor: '#688bff',
          borderColor: '#688bff',
          fill: false,
          data: sensorReads.map(o => { return { x: o.time, y: o.x } })
        },
        {
          label: 'Y',
          backgroundColor: '#455caa',
          borderColor: '#455caa',
          fill: false,
          data: sensorReads.map(o => { return { x: o.time, y: o.y } })
        },
        {
          label: 'Z',
          backgroundColor: '#26335f',
          borderColor: '#26335f',
          fill: false,
          data: sensorReads.map(o => { return { x: o.time, y: o.z } })
        }]
      }
      //return the chart.js format data
      return dataCollection
    }
  },
  mutations: {
    loadSocket: (state, payload) => {
      state.socket = payload
    },
    addSensor: (state, payload) => {
      state.sensors.push(payload)
    },
    updateSensor: (state, payload) => {
      const timeMap = state.sensors[payload.ind].reads.map(o => o.time)
      //check if the data already exists and only upload the new data
      if (timeMap.indexOf(payload.time) < 0) {
        state.sensors[payload.ind].reads.push({
          x: payload.x,
          y: payload.y,
          z: payload.z,
          time: payload.time
        })
      }
    },
    setTimer: (state, payload) => {
      state.timer = payload
    },
    clearReads: (state) => {
      for (let sensor of state.sensors) {
        sensor.reads = []
      }
    }
  },
  actions: {
    beginUpdating: (store) => {
      //set a dispatch timer to poll new datas
      if (!store.getters.updating) {
        let fn = () => {
          store.dispatch('updateSensors')
          store.commit('setTimer', setTimeout(fn, 1000))
        }
        fn()
      }
    },
    stopUpdating: (store) => {
      //stop the dispatch timer and remove the data
      clearTimeout(store.getters.timer)
      store.commit('setTimer', null)
      store.commit('clearReads')
    },
    updateSensors: (store) => {
      //send the read command to the web socket
      store.getters.socket.emit('event', 'read')

      //wrap the websocket response in a promise
      return new Promise((resolve) => {
        store.getters.socket.on('data', data => {

            //Parse string response form WS
            const response = JSON.parse(data)
            //Get current ids of sensors
            const sensorMap = store.getters.sensors.map(o => o.id)
            //loop through each sensor of WS response
            for (let sensor of response) {
              const ind = sensorMap.indexOf(sensor.id)
              if (ind >= 0) {
                //senosr exists so just add a read
                store.commit('updateSensor', {
                  ind: ind,
                  x: sensor.x,
                  y: sensor.y,
                  z: sensor.z,
                  time: sensor.time
                })
              } else {
                //sensor doesnt exist so create it
                store.commit('addSensor', {
                  id: sensor.id,
                  library: sensor.library,
                  type: sensor.sensor,
                  reads: [{
                    x: sensor.x,
                    y: sensor.y,
                    z: sensor.z,
                    time: sensor.time
                  }]
                })
              }
            }
            //resolve the promise because we went through all the data
            resolve()
          })
      })
    }
  },

  modules: {}
}

export default new Vuex.Store(store)