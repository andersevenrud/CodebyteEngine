#!/bin/bash
	
	mkdir temp
	cd temp
	
	# drawlib and lodepng
	curl -o "Resources.zip" http://cameron.tylerkline.com/Resources.zip
	unzip Resources.zip
	cp -r "Resources" "../../bin/Resources"
	cd ../
	
	rm -Rf temp
	
	
