// Uses Declarative syntax to run commands inside a container.
pipeline {
    triggers {
        pollSCM("*/5 * * * *")
    }
    agent {
        kubernetes {
            yaml '''
apiVersion: v1
kind: Pod
spec:
  containers:
  - name: windows
    image: quay.imanuel.dev/dockerhub/library---ubuntu:21.10
    command:
    - cat
    tty: true
    env:
      - name: CONAN_REVISIONS_ENABLED
        value: 1
      - name: DEBIAN_FRONTEND
        value: noninteractive
      - name: TOOLCHAIN_SUFFIX
        value: posix
      - name: COMPILER_VERSION
        value: 10
  - name: linux
    image: quay.imanuel.dev/dockerhub/library---ubuntu:21.10
    command:
    - cat
    tty: true
    env:
      - name: CONAN_REVISIONS_ENABLED
        value: 1
      - name: DEBIAN_FRONTEND
        value: noninteractive
'''
            defaultContainer 'mingw'
        }
    }
    stages {
        stage('Build thingybrowser') {
            parallel {
                stage('Build for Windows') {
                    steps {
                        container('windows') {
                            sh 'apt-get update && apt-get upgrade -y'
                            sh 'apt-get install mingw-w64-x86-64-dev g++-mingw-w64-x86-64-posix gcc-mingw-w64-x86-64-posix binutils-mingw-w64-x86-64 build-essential cmake git wget rsync -y'
                            sh 'wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb'
                            sh 'dpkg -i conan-ubuntu-64.deb'

                            sh 'git config --global --add safe.directory /home/jenkins/agent/workspace/DerKnerd_thingybrowser_main'
                            sh 'git submodule update --init --recursive'
                            sh 'cmake -DCMAKE_TOOLCHAIN_FILE=./building/cmake/mingw-w64-x86_64.cmake -B ./build -S . -DCMAKE_BUILD_TYPE=Release -DMINGW=1'
                            sh 'cmake --build ./build --target thingybrowser'

                            archiveArtifacts './build/thingybrowser.exe'
                        }
                    }
                }
                stage('Build in Linux') {
                    steps {
                        container('linux') {
                            sh 'apt-get update -y'
                            sh 'apt-get install apt-utils -y'
                            sh 'apt-get install build-essential cmake git wget pkg-config rsync -y'
                            sh 'apt-get install libgtk-3-dev -y libgtk-3-0 libgtk-3-bin libgtk-3-common libgtk2.0-0 libgtk2.0-bin libgtk2.0-common # ubuntu-desktop'
                            sh 'wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb'
                            sh 'dpkg -i conan-ubuntu-64.deb'

                            sh 'git config --global --add safe.directory /home/jenkins/agent/workspace/DerKnerd_thingybrowser_main'
                            sh 'git submodule update --init --recursive'
                            sh 'cmake --build ./build --target thingybrowser'
                            sh 'cmake --build ./build --target thingybrowser'

                            archiveArtifacts './build/thingybrowser'
                        }
                    }
                }
            }
        }
    }
}
