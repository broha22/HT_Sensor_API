/*
 * @Author: you@you.you
 * @Date:   Wednesday March 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Wednesday March 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
<template>
  <el-col class='sensorTable'>
    <el-row v-for='sensor in sensors' :key='sensor.id' class='sensorRow'>
      <el-col :span='8' class='sensorLib'>
        {{sensor.library}}
      </el-col>
      <el-col :span='8' class='sensorType'>
        {{sensor.type}}
      </el-col>
      <el-col :span='8' class='logButton'>
        <el-button @click='logSensor(sensor.id)' class='logButton' :type='buttonType(sensor.id)' plain :disabled='loadedGraphs.length > 0 && loadedGraphs[0] !== sensor.id'>{{buttonText(sensor.id)}}</el-button>
        <!-- <el-button @click='hideGraph(sensor.id)' class='logButton' :style='`display:${(loadedGraphs.indexOf(sensor.id) < 0)?"none":"inline"}`' type='warning' plain>Hide</el-button> -->
      </el-col>
      <el-col class='graph' ref='graphs' :id='`${sensor.id}-graph`'>
        <line-chart :chart-data="$store.getters.sensorData(sensor.id)" ref='chart' style='position: absolute; left: 5%;width: 90%; height: 67%;'></line-chart>
      </el-col>
    </el-row>
  </el-col>
</template>

<script>
import LineChart from '@/components/line-chart.js'

export default {
  name: 'sensors',
  components: {
    LineChart
  },
  data () {
    return {
      loadedGraphs: []
    }
  },
  computed: {
    sensors: {
      get () {
        return this.$store.getters.sensors
      }
    }
  },
  methods: {
    logSensor: function (id) {
      if (this.loadedGraphs.indexOf(id) < 0) {
        for (let graph of this.$refs.graphs) {
          if (graph.$attrs.id === id + '-graph') {
            graph.$el.style.display = 'block'
          }
        }
        this.loadedGraphs.push(id)
        this.$store.dispatch('beginUpdating')
      } else {
        this.loadedGraphs.splice(this.loadedGraphs.indexOf(id), 1)
        this.$store.dispatch('stopUpdating')
        for (let graph of this.$refs.graphs) {
          graph.$el.style.display = 'none'
        }
      }
    },
    buttonText: function (id) {
      if (this.loadedGraphs.indexOf(id) >= 0) {
        return 'Stop'
      } else {
        return 'Log Data'
      }
    },
    buttonType: function (id) {
      if (this.loadedGraphs.indexOf(id) >= 0) {
        return 'danger'
      } else {
        return 'primary'
      }
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.sensorTable {
  overflow-y: scroll;
}
.sensorRow {
  padding-top: 20px;
  padding-right: 0px;
  padding-left: 0px;
  border-bottom: 1px solid rgba(170,170,170,0.3);
}
.logButton {
  text-align: left;
  bottom: 10px;
}
.sensorType {
  text-align: center;
}
.sensorLib {
  text-align: right;
}
.graph {
  height: 300px;
  /* margin-bottom: 80px; */
  background-color: rgb(97, 97, 97);
  padding: 0;
  padding-top: 20px;
  display: none;
}
.logButton {
  padding: 10px;
  margin-top: -10px;
}
</style>
