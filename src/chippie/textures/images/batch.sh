#!/bin/bash

function convert() {
	FILE=$1
	NAME=$2
	TRANSP="$3"
	python chippie_img_to_cpp.py $TRANSP $FILE $NAME > ../tiles/${FILE%.png}.hpp
}

# do transparent ones first
transparent_list=(chippie_down.png chippie_left.png chippie_up.png chippie_right.png)
transparent_list+=(chippie_down_girl.png chippie_left_girl.png chippie_up_girl.png chippie_right_girl.png)
transparent_list+=(centipede_down.png centipede_left.png centipede_up.png centipede_right.png)

for f in ${transparent_list[@]}; do
	echo $f
	convert $f ${f%.png} --transparent
done

# then the opaques
opaque_list=(digit_00.png digit_01.png digit_02.png)
opaque_list+=(digit_03.png digit_04.png digit_05.png)
opaque_list+=(digit_06.png digit_07.png digit_08.png)
opaque_list+=(digit_09.png)
opaque_list+=(chip.png clear.png cyan_door.png cyan_key.png green_door.png)
opaque_list+=(green_key.png hint.png portal.png red_door.png red_key.png)
opaque_list+=(socket.png wall.png yellow_door.png yellow_key.png)
opaque_list+=(dirt.png moving_block.png water_nosplash.png water_splash.png)

for f in ${opaque_list[@]}; do
    echo $f
	convert $f ${f%.png}
done
