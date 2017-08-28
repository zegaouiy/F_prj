#!/bin/sh
#1 nombre jour; 2 indice premiere image ; 3 fréquence + horaires ; 
#out : courbes nombre bateaux ; carte de fréquentation/jour ; total journée

mask="second_mask.pgm"
spec="nb_spec_"
size_morph="5"
dif="dif_"
s=10
min="100"
max="10000"

#calcul image référence
for i in `seq 1 $1`
do
    indice=$(($((i*$3))+$2))
    echo "indice = $indice"
    ref="img_reference_day$1.ppm"
    ./do_ref "image" "$indice" ".ppm" $mask "$3"
    mv "reference_mean.ppm" $ref

    #spécification
    ./do_spec "image" "$indice" ".ppm" $mask "$3" $ref
    echo "end spec"

    #différence
    ./do_dif $spec "$indice" ".ppm" $mask $ref $size_morph "$3"
    echo "end dif"
done

#régions map et red map
./do_regions $dif "$2" ".pgm" $mask "out" "100" "1000000" "$s" "$(($3*$1))" 

#---------------------------------------------
#analyse des régions
#---------------------------------------------
for i in `seq 1 $1`
do
    indice=$(($((i*$3))+$2))
    ./do_chk "carte_regions" "$indice" ".raw" $mask "1998" "10656" $min $max "$3" "0" "$i" 
done
