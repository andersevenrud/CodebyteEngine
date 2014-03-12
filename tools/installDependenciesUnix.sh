#!/bin/bash
	
	rm -r ../dependencies
        mkdir ../bin
	mkdir ../dependencies
	mkdir ../dependencies/GL
	mkdir ../dependencies/src
	mkdir ../dependencies/libs
	rm -r temp
	mkdir temp
	cd temp

	# GLFW
	git clone https://github.com/glfw/glfw.git
	cd glfw
	cmake .
	make -j5
	cp -r include/GLFW/* "../../../dependencies/GL"
	cp "src/libglfw3.a" "../../../dependencies/libs/libglfw3.a"
	cd ../
	
	#curl https://bullet.googlecode.com/files/bullet-2.82-r2704.tgz - o "bullet.tgz"
	#tar -zxvf "bullet.tgz"
	#cd bullet
	#cmake .
	

	# GLEW
	git clone https://github.com/nigels-com/glew.git glew
	cd glew
	make extensions
	make -j5
	cp -r include/GL/* "../../../dependencies/GL/"
	cp "lib/libGLEW.a" "../../../dependencies/libs/libGLEW.a"
	cd ../
	
	# drawlib and lodepng
	curl -o "drawlib.h" http://cameron.tylerkline.com/drawlib.h
	curl -o "lodepng.h" http://lpi.googlecode.com/svn/trunk/lodepng.h
	curl -o "lodepng.cpp" http://lpi.googlecode.com/svn/trunk/lodepng.cpp
	cp "drawlib.h" "../../dependencies/drawlib.h"
	cp "lodepng.h" "../../dependencies/lodepng.h"
	cp "lodepng.cpp" "../../dependencies/src/lodepng.cpp"
	
	# GLM
	git clone https://github.com/g-truc/glm.git
	cd glm
	cp -r "glm" "../../../dependencies/glm"
	cd ../../
	
	rm -Rf temp
	
	
