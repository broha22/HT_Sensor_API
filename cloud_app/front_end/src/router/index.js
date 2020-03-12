/*
 * @Author: you@you.you
 * @Date:   Wednesday March 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Wednesday March 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */

import Vue from 'vue'
import Router from 'vue-router'

//Import components to route to
import home from '@/components/home.vue'
import docs from '@/components/documentation.vue'
import sensors from '@/components/sensors.vue'

//tell vue to use router
Vue.use(Router)

//export the routing paths
export default new Router({
  routes: [
    {
      path: '/',
      name: 'home',
      component: home
    },
    {
      path: '/docs',
      name: 'docs',
      component: docs
    },
    {
      path: '/sensors',
      name: 'sensors',
      component: sensors
    }
  ]
})