/*
 * @Author: you@you.you
 * @Date:   Wednesday March 11th 2020
 * @Last Modified By:  Brogan Miner
 * @Last Modified Time:  Wednesday March 11th 2020
 * @Copyright:  (c) Oregon State University 2020
 */


<template>
  <el-menu :default-active="testRoute()" class="menu" @select='route'>
    <el-menu-item index="1">
      <i class="el-icon-s-home"></i>
      <span>Home</span>
    </el-menu-item>
    <el-menu-item index="2">
      <i class="el-icon-menu"></i>
      <span>Sensors</span>
    </el-menu-item>
    <el-menu-item index="3">
      <i class="el-icon-document"></i>
      <span>Documentation</span>
    </el-menu-item>
  </el-menu>
</template>

<script>
export default {
  name: 'Menu',
  methods: {
    route: function (index) {
      if (index == 1) {
        this.$router.push('/')
      } else if (index == 2) {
        this.$router.push('/sensors')
      } else if (index == 3) {
        this.$router.push('/docs')
      }
    },
    testRoute: function () {
      if (this.$route.path.includes('sensors')) {
        return '2'
      } else if (this.$route.path.includes('docs')) {
        return '3'
      } else {
        return '1'
      }
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  .menu {
    min-height: 100%;
    text-align: left;
  }
</style>