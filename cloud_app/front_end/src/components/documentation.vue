<template>
  <el-col class='stage'>
    <el-row>
      <el-col class='mainheader'>
        Documentation
      </el-col>
    </el-row>
    <el-row>
      <el-col class='header'>
        Installation
      </el-col>
    </el-row>
    <el-row>
      <el-col class='body'>
        To get the head tracking system to work a few steps are necessary. The following list of steps can be used to configure a RPi Zero to correctly run the HeadTracking Sensor Libraries.
        <ul>
          <li>The first step is to acquire an RPi and get raspbian setup on the device. The library has been tested against an RPi Zero W and RPi 2. Currently on the RPi Zero W I am using version 10. It is recommended you intall this without the GUI to save on resources.</li>
          <li>Next the following additional libraries must be installed (install with 'apt-get install'):</li>
          <ul>
            <li>git</li>
            <li>wiringpi</li>
            <li>i2ctools</li>
          </ul>
          <li>Next clone the GitHub Repo (git clone https://github.com/broha22/HT_Sensor_API.git)</li>
          <li>Now cd into the repo and run sudo make install</li>
          <ul>
            <li>If this is the first time running make it may be neccessary to comment out the lines for stopping HTService</li>
          </ul>
          <li>You are now ready to begin using the HTSensor Library</li>
          <ul>
            <li>Examples for usage can be found in the example folder of the code repo</li>
          </ul>
        </ul>
      </el-col>
    </el-row>
    <el-row>
      <el-col class='header'>
        More Information
      </el-col>
    </el-row>
    <el-row>
      <el-col class='body'>
        More documentation and source code can be found at our <a href="https://github.com/broha22/HT_Sensor_API">GitHub repository</a>.
      </el-col>
    </el-row>
  </el-col>
</template>

<script>
export default {
  name: 'documentation'
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
.stage {
  text-align: left;
  padding-left: 15px;
}
.mainheader {
  padding-top: 20px;
  font-size: 42px;
}
.header {
  padding-top: 20px;
  font-size: 32px;
  color: rgb(151, 151, 151);
}
.body {
  padding-left: 5px;
}
</style>
