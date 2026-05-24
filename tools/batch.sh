#!/bin/bash

SRC=$1
CUR=`pwd`

pushd $SRC
for f in *png; do
	python $CUR/tetris_img_to_cpp.py $f tile_data > ${f%.png}.hpp
done
popd
