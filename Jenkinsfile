node {
   def cmakeTool
   stage('Preparation') {
      // Get some code from a GitHub repository
      checkout scm
      
      cmakeTool = tool name: 'InSearchPath', type: 'hudson.plugins.cmake.CmakeTool'
      
      sh "rm -rf build"
      sh "mkdir -p build"
   }
   stage('Configure') {
      dir("build") {
          sh "${cmakeTool} .."
      }
      
   }
   
   stage('Build') {
      dir("build") {
          sh "make  storm"
      }
      
   }
   
   stage('Build Tests') {
      dir("build") {
          sh "make -j 4 tests"
      }
      
   }
   
   stage('Test') {
       	dir("build") {
       	    sh "make check"
	}
   }
   
   stage('Archive') {
      archiveArtifacts artifacts: 'build/bin/*', onlyIfSuccessful: true
      archiveArtifacts artifacts: 'build/lib/*', onlyIfSuccessful: true
      archiveArtifacts artifacts: 'build/include/*', onlyIfSuccessful: true
   }
}
