#!/bin/bash

function convert() {
	FILE=$1
	NAME=$2
	TRANSP="$3"
	python chippie_img_to_cpp.py $TRANSP $FILE $NAME > ../tiles/${FILE%.png}.hpp
}

# do transparent ones first
transparent_list="chippie_down.png chippie_left.png chippie_up.png chippie_right.png"

for f in $transparent_list; do
	convert $f ${f%.png} --transparent
done

# then the r
opaque_list="chip.png clear.png cyan_door.png cyan_key.png green_door.png green_key.png hint.png portal.png red_door.png red_key.png socket.png wall.png yellow_door.png yellow_key.png"
for f in $opaque_list; do
	convert $f ${f%.png}
done
