<template>
  <el-container class="app">
    <el-header class='header'>
      <span class='headerText'>Head Tracking Dashboard</span>
    </el-header>
    <el-main class='main' ref='main'>
      <el-row class='mainRow'>
        <el-col :span='5' class='menuCol'>
          <sideMenu />
        </el-col>
        <el-col :span='19' class='routerCol'>
          <router-view />
        </el-col>
      </el-row>
    </el-main>
    <el-footer class='footer'>
    </el-footer>
  </el-container>
</template>

<script>
import io from 'socket.io-client'
import sideMenu from '@/components/side_menu.vue'

export default {
  name: 'App',
  components: {
    sideMenu
  },
  created () {
    this.$store.commit('loadSocket', io('http://localhost:3000'))
    this.$store.dispatch('updateSensors').then(() => {
      this.$store.commit('clearReads')
    })
  }
}
</script>
<style>
html,body {
    width: 100%;
    height: 100%;
    margin: 0px;
    padding: 0px;
    overflow-x: hidden; 
}
</style>
<style scoped>
.app {
  position: absolute;
  top: 0;
  left: 0;
  height: 100%;
  width: 100%;
  padding: 0;
  margin: 0;
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  overflow: hidden !important;
}
.header {
  position: absolute;
  top: 0;
  left: 0;
  height: 60px;
  color: white;
  background-color: #2c3e50;
  text-align: left;
  font-size: 20px;
  padding-top: 15px;
  width: 100%;
}
.footer {
  position: absolute;
  top: calc(100% - 15px);
  left: 0px;
  width: 100%;
  padding: 0;
  margin: 0;
  height: 15px !important;
  color: white;
  background-color: #2c3e50;
}
.main {
  position: absolute;
  top: 60px;
  left: 0px;
  width: 100%;
  height: calc(100% - 75px) !important;
  padding: 0 !important;
  margin: 0 !important;
  overflow: hidden;
  background-color: white;
}
.mainRow {
  padding: 0 !important;
  margin: 0 !important;
  height: 100% !important;
}
.menuCol {
  padding: 0 !important;
  margin: 0 !important;
  height: 100%;
}
.routerCol {
  overflow: scroll;
  height: 100%;
}
</style>
