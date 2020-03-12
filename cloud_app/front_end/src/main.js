import Vue from 'vue'
import App from './App.vue'
import store from './store'
import elm from 'element-ui'
import Vuei18n from 'vue-i18n'
import locale from 'element-ui/lib/locale/lang/en'
import router from './router'

import 'element-ui/lib/theme-chalk/index.css'
// import 'element-ui/lib/theme-chalk/reset.css'
Vue.config.productionTip = false

Vue.use(Vuei18n)
Vue.use(elm, { locale: locale })

new Vue({
  router,
  store,
  render: h => h(App),
}).$mount('#app')
