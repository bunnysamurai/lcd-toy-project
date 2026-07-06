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
transparent_list+=(blue_tank_up.png blue_tank_down.png blue_tank_left.png blue_tank_right.png)
transparent_list+=(fire_dancer.png purple_ball.png)
transparent_list+=(water_glider_up.png water_glider_down.png water_glider_left.png water_glider_right.png)

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
opaque_list+=(digit_00_yellow.png fire_boots.png flippers.png ice_skates.png suction_boots.png)
opaque_list+=(ice.png ice_bottomleft.png ice_bottomright.png ice_topleft.png ice_topright.png)
opaque_list+=(push_floor_down.png push_floor_left.png push_floor_right.png push_floor_up.png)
opaque_list+=(campfire.png bomb.png)
opaque_list+=(blue_button.png brown_button.png green_button.png red_button.png)
opaque_list+=(green_button_clear.png green_button_wall.png)
opaque_list+=(bomb.png cloner_fire_dancer.png trap.png)

for f in ${opaque_list[@]}; do
    echo $f
	convert $f ${f%.png}
done
