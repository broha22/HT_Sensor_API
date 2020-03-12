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
    sensorData: (state) => (id) => {
      const sensorReads = state.sensors[id].reads
      const dataCollection = {
        labels: [],
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
      if (!store.getters.updating) {
        let fn = () => {
          store.dispatch('updateSensors')
          store.commit('setTimer', setTimeout(fn, 1000))
        }
        fn()
      }
    },
    stopUpdating: (store) => {
      clearTimeout(store.getters.timer)
      store.commit('setTimer', null)
      store.commit('clearReads')
    },
    updateSensors: (store) => {
      store.getters.socket.emit('event', 'read')
      return new Promise((resolve) => {
        store.getters.socket.on('data', data => {
            const response = JSON.parse(data)
            const sensorMap = store.getters.sensors.map(o => o.id)
            for (let sensor of response) {
              const ind = sensorMap.indexOf(sensor.id)
              if (ind >= 0) {
                store.commit('updateSensor', {
                  ind: ind,
                  x: sensor.x,
                  y: sensor.y,
                  z: sensor.z,
                  time: sensor.time
                })
              } else {
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
            resolve()
          })
      })
    }
  },

  modules: {}
}

export default new Vuex.Store(store)