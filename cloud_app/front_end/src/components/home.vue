<template>
  <el-row>
    <el-col>
      <el-image style="width: 100%; height: 300px" src="will.jpg" fit="cover"></el-image>
    </el-col>
    <el-col>
      <el-row>
        <el-col class='header'>
          Head Tracking Dashboard
        </el-col>
        <el-col class='body'>
          This dashboard provides means for checking what sensors are connected to the device. It also allows you to collect some real time data on the device from the dashboard. This data is displayed in a graph on the web page. Additionally, some documentation is provided on this webpage. Below is a timeline for the completition of this system.
        </el-col>
      </el-row>
      <el-row class='timelinerow'>
        <el-col class='timelinecol'>
          <iframe class="timeline" src="https://plan.toggl.com/#pp/nGaSei3_Mv9KRz3sRseHDZci7waniSg8" />
        </el-col>
      </el-row>
    </el-col>
  </el-row>
</template>

<script>
export default {
  name: 'home',
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.header {
  padding-top: 15px;
  padding-left: 10px;
  text-align: left;
  font-size: 36px;
  color: rgb(151, 151, 151);
}
.body {
  text-align: left;
  padding: 10px;
  padding-left: 15px;
  padding-top: 5px;
  
}
.timeline {
  width: 100%;
  height: 100%;
  border: none;
  overflow: hidden;
}
.timelinecol {
  height: 300px;
}
</style>
