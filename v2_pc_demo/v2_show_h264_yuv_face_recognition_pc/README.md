# v2_show_h264_yuv_face_recognition_pc 
  This is at pc platform to show h264 yuv face recognition demo.
## Dependent
rtsp 
萤石：rtsp://ａｄｍｉｎ：ｌｅｔｍｅｉｎ１@１９２．１６８．１．１６８:５５４/ｈ２６４/ch1(2)/main/av_stream
ｐｏｙａ：rtsp://ａｄｍｉｎ:ｌｅｔｍｅｉｎ１@１９２．１６８．１．１６８:５５４/１（２）
ＴＰ—Ｌｉｎｋ：rtsp://ａｄｍｉｎ:ｌｅｔｍｅｉｎ１@１９２．１６８．１．１６７:５５４/stream1(2)
```
1. QT5      Install  with ` $ sudo apt-get install qt5-default qtcreator `
2. libx264  Download with ` http://www.videolan.org/developers/x264.html `
                      or  ` $ git clone http://git.videolan.org/git/x264.git `
            Config   with ` $ ./configure --enable-static 
                                          --disable-asm   
                                          --prefix=/usr/local/ffmpeg   
                                          --arch=arm                   
                                          --host=arm-linux           
                                          --cross-prefix=arm-arago-linux-gnueabi-
                                          --extra-ldflags=-fPIC 
                          `
            Compile  with ` $ make
            Install  with ` $ make install

3. FFMPEG   Download with ` http://ffmpeg.org/download.html ` for ` ffmpeg-4.0.1.tar.bz2 `  
            Config   with ` $ ./configure --enable-shared 
                                          --disable-yasm 
                                          --enable-memalign-hack 
                                          --enable-libx264 
                                          --enable-gpl 
                                          --prefix=/usr/local/ffmpeg 
                                          --enable-cross-compile 
                                          --arch=arm 
                                          --target-os=linux 
                                          --cross-prefix=arm-arago-linux-gnueabi- 
                                          --cc=arm-arago-linux-gnueabi-gcc 
                                          --extra-cflags=-I/usr/local/ffmpeg/include 
                                          --extra-ldflags=-L/usr/local/ffmpeg/lib 
                                          --extra-libs=-ldl
                          `
            Compile  with ` $ make `
            Install  with ` $ make install `

4. OpenCv   Dependent On  ` $ sudo apt-get install build-essential 
                                                   cmake 
                                                   git 
                                                   pkg-config 
                                                   libgtk2.0-dev 
                                                   libavcodec-dev 
                                                   libavformat-dev 
                                                   libswscale-dev 
                                                   python-dev 
                                                   python-opencv 
                                                   python-numpy 
                                                   libtbb2 libtbb-dev 
                                                   libjpeg-dev 
                                                   libpng-dev 
                                                   libtiff-dev 
                                                   libjasper-dev 
                                                   libdc1394-22-dev
                          `
            Download with ` http://opencv.org/downloads.html `
            config   with ` $ unzip opencv_3.0.0.zip
                            $ cd opencv-3.0.0
                            $ mkdir release
                            $ cd release
                            $ cmake -D CMAKE_BUILD_TYPE=RELEASE 
                                    -D CMAKE_INSTALL_PREFIX=/usr/local 
                                    -D WITH_TBB=ON 
                                    -D BUILD_NEW_PYTHON_SUPPORT=ON 
                                    -D WITH_V4L=ON 
                                    -D WITH_QT=ON 
                                    -D WITH_OPENGL=ON 
                                    ..
                          `
            Compile  with ` $ make -j
            Install  with ` $ sudo make Install
            
            Config   with ` $ sudo /bin/bash -c 'echo "/usr/local/lib" > /etc/ld.so.conf.d/opencv.conf'
                            $ sudo ldconfig
                          `

5. Glog     Download with ` http://code.google.com/p/google-glog/downloads/list ` for ` glog-0.3.3.tar.gz `
            Config   with ` $ ./configure --preifx=mypath `
            Compile  With ` $ make `
            Install  with ` $ make install `

6. Protobuf Dependent On  ` $ sudo apt-get install autoconf
                                                   automake
                                                   libtool
                                                   make
                                                   g++
                                                   curl
                                                   unzip
                          `
            Download with ` $ git clone https://github.com/google/protobuf.git ` or ` https://github.com/google/protobuf/releases/lates `
            Config   with ` $ cd protobuf
                            $ git submodule update --init --recursive
                            $ ./autogen.sh
                            $ ./configure --prefix=/usr
                          `
            Compile  with ` $ make 
                            $ make check
                          `
            Install  with ` $ sudo make install `
            Config   with ` $ sudo ldconfig `

7. Dlib     Dependent On  ` $ sudo apt-get install libboost-all-dev 
                                                   build-essential 
                                                   cmake 
                                                   pkg-config 
                                                   libatlas-base-dev
                          `
            Download with ` $ git clone https://github.com/davisking/dlib.git `
            Config   with ` $ mkdir build
                            $ cd build
                            $ cmake ..
                          `
                          ********** May be use cmake args ***************
                          *** -DDLIB_USE_CUDA=0不使用cuda              ***
                          *** -DUSE_AVX_INSTRUCTIONS=1使用cpu的AVX加速 ***
                          
            Compile  with ` $ cmake --build . --config Release `
            Install  with ` $ sudo make install `
            Config   with ` $ sudo ldconfig `

8. gflags   Download with ` $ git clone https://github.com/gflags/gflags.git `                               
            Config   with ` $ cd gflags `
                          ` $ mkdir build && cd build `
                          ` $ cmake .. -DGFLAGS_NAMESPACE=google -DCMAKE_CXX_FLAGS=-fPIC .. `
            Compile  with ` $ make -j4 `
            Install  with ` $ sudo make install `

```                       
