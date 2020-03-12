/*
 * @Author: you@you.you
 * @Date:   Wednesday March 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Wednesday March 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */
import { Line, mixins } from 'vue-chartjs'
const { reactiveProp } = mixins

export default {
  extends: Line,
  mixins: [reactiveProp],
  props: ['options'],
  mounted () {
    // this.chartData is created in the mixin.
    // If you want to pass options please create a local options object
    this.renderChart(this.chartData, {
      maintainAspectRatio: false,
      legend: {
        labels: {
          fontSize: 12,
          fontColor: '#FFF'
        }
      },
      title: {
        fontSize: 12,
        fontColor: '#FFF',
        fontFamily: 'Open Sans'
      },
      layout: {
        padding: {
          left: 0,
          right: 0,
          bottom: 0,
          top: 0
        }
      },
      scales: {
        yAxes: [{
          ticks: {
            beginAtZero: false,
            fontSize: 12,
            fontColor: '#FFF'
          },
          gridLines: {
            color: '#FFF',
            zeroLineColor: '#fff',
            display: true // my new default options
          }
        }],
        xAxes: [{
          ticks: {
            beginAtZero: false,
            fontSize: 12,
            fontColor: '#FFF',
          },
          type: 'time',
          bounds: 'data',
          gridLines: {
            display: false
          }
        }]
      }
    })
  },
  methods: {
    update: function () {
      this.$data._chart.update()
    }
  }
}