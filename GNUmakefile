bake:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" \
		            -DCMAKE_INSTALL_PREFIX="/usr" \
					-DQT_QMAKE_EXECUTABLE=qmake-qt5 ..; \
			  make -j3

check: bake
	cd build; ctest --output-on-failure

clean:
	rm -rf build


