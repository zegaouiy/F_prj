#!/bin/sh
#1 nombre jour; 2 indice premiere image ; 3 fréquence + horaires ; 
#out : courbes nombre bateaux ; carte de fréquentation/jour ; total journée

mask="second_mask.pgm"
spec="nb_spec_"
size_morph="5"
dif="dif_"
s=10

#calcul image référence
indice=$(($1*52+$2))
echo "indice = $indice"
ref="img_reference_day$1.ppm"
./do_ref "image" "$indice" ".ppm" $mask "52"
mv "reference_mean.ppm" $ref

#spécification
./do_spec "image" "$indice" ".ppm" $mask "52" $ref
echo "end spec"

#différence
./do_dif $spec "$indice" ".ppm" $mask $ref $size_morph "52"
echo "end dif"

#régions map et red map
./do_regions $dif "$indice" ".pgm" $mask "out" "100" "1000000" "$s" "52" 
