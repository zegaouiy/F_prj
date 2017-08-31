#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>    
#include "morpho.cpp"
#include "fusion_window.cpp"
#include "self_crit.cpp"
#include "chk_data.cpp"
#include <omp.h>

using namespace std;

int main(int argc, char* argv[])
{
  char nomImg[250], suf[250], nomMask[250], name[250], outname[250];
  int nH, nW, nTaille, min, max, indice, nbimg, gt, day;
  
  OCTET *mask, *out, s;
  int* ImgIn;

  if (argc != 12) 
    {
      printf("Usage: Image indice .pgm Mask ImageRawPath taille_min taille_max s=10 nbimg?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg);
  sscanf (argv[2],"%d",&indice);
  sscanf (argv[3],"%s",suf) ;
  sscanf (argv[4],"%s",nomMask);
  sscanf (argv[5],"%d",&nH);
  sscanf (argv[6],"%d",&nW);
  sscanf (argv[7],"%d",&min);
  sscanf (argv[8],"%d",&max);
  sscanf (argv[9],"%d",&nbimg);
  sscanf (argv[10],"%d",&gt);
  sscanf (argv[11],"%d",&day);

  sprintf(name, "%s%d%s", nomImg, indice, suf);
  
  nTaille = nH * nW;
 
  allocation_tableau(mask, OCTET, nTaille);
  allocation_tableau(ImgIn, int, 3 * nTaille);
  
  lire_image_pgm(nomMask, mask, nH * nW);
 
  chk_window(nomImg, indice, suf, nbimg, ImgIn, mask, max, min, 80, 120, nH, nW, outname, gt, day);

  
  char ref[250], mask[250] = "second_mask.pgm", spec[250] = "nb_spec_", dif[250] = "dif_";
  int i, size_morph = 5, s = 10, min = 100, max = 10000;
  
  //#calcul image référence

  for (i = 0; i < nbday; i++)
    {
      indice = i * nbimg + first;
      ref="img_reference_day$1.ppm"
    ./do_ref "image" "$indice" ".ppm" $mask "$3"
    mv "reference_mean.ppm" $ref

    #spécification
    ./do_spec "image" "$indice" ".ppm" $mask $ref "$3"
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
for i in `seq 0 $(($1-1))`
do
    indice=$(($((i*$3))+$2))
    ./do_chk "carte_regions_" "$indice" ".raw" $mask "10656" "1998" $min $max "$3" "0" "$i" 
done


  return 1;
}
