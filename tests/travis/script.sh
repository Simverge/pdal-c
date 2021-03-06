#!/bin/sh

gcc --version
g++ --version

PDALC_CMAKE_ARGS="-DPDALC_GCC_PARAM_GGC_MIN_HEAPSIZE=8192"

if [ "$SCAN" = "sonarcloud" ]; then
	curl -LsS https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip > build-wrapper-linux-x86.zip
	unzip build-wrapper-linux-x86.zip
	rm build-wrapper-linux-x86.zip
	export SONARCLOUD_DIR=${PWD}/build-wrapper-linux-x86

elif [ "$SCAN" = "coverity" ] && [ "$TRAVIS_BRANCH" = "master" ]; then
	curl -LsS -d "token=${COVERITY_TOKEN}&project=Simverge%2Fpdal-c" -X POST https://scan.coverity.com/download/cxx/linux64 > coverity_tool.tgz
	tar xaf coverity_tool.tgz
	rm coverity_tool.tgz
	mv cov-analysis-linux64-* cov-analysis-linux64
	export COVERITY_DIR=${PWD}/cov-analysis-linux64/bin

	PDALC_CMAKE_ARGS += "-DPDALC_ENABLE_CODE_COVERAGE=OFF"
	PDALC_CMAKE_ARGS += "-DPDALC_ENABLE_DOCS=OFF"
fi

export CI_PROJECT_DIR=/pdalc
export CI_PROJECT_NAME=pdal-c
export TARGET_PLATFORM=$DISTRO-`uname -m`-$BUILD_TYPE
echo "Building $CI_PROJECT_NAME ($TRAVIS_BRANCH-$TRAVIS_COMMIT) for $TARGET_PLATFORM"
rm -rf "$CI_PROJECT_DIR/build/$TARGET_PLATFORM" 
mkdir -p "$CI_PROJECT_DIR/build/$TARGET_PLATFORM"
cd "$CI_PROJECT_DIR/build/$TARGET_PLATFORM"

cmake -G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE=$BUILD_TYPE \
	${PDALC_CMAKE_ARGS} \
	"${CI_PROJECT_DIR}"

if [ "$SCAN" = "sonarcloud" ]; then
	${SONARCLOUD_DIR}/build-wrapper-linux-x86-64 --out-dir ${CI_PROJECT_DIR}/bw-output make
elif [ "$SCAN" = "coverity" ] && [ "$TRAVIS_BRANCH" = "master" ]; then
	${COVERITY_DIR}/cov-configure --gcc
	${COVERITY_DIR}/cov-build --dir ${CI_PROJECT_DIR}/cov-int make
	cd ${CI_PROJECT_DIR}
	tar czf cov-int.tgz cov-int

	curl --form token="${COVERITY_TOKEN}" \
		--form email="${COVERITY_EMAIL}" \
		--form file=@${CI_PROJECT_DIR}/cov-int.tgz \
		--form version="${TRAVIS_BRANCH}-$TRAVIS_COMMIT" \
		--form description="Automatic Coverity Scan build for ${TRAVIS_BRANCH}-$TRAVIS_COMMIT" \
		https://scan.coverity.com/builds?project=Simverge%2Fpdal-c
else
	echo "Running build without static analysis scans"
	make
fi
