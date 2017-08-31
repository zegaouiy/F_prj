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
  int h, w, n, min, max, indice, nbimg, gt, day;
  
  OCTET *mask, *out, img;
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
  
  char ref[250], mask[250] = "second_mask.pgm", spec[250] = "nb_spec_", dif[250] = "dif_";
  int i, size_morph = 5, s = 10, min = 100, max = 10000;
  
  lire_nb_lignes_colonnes_image_pgm(mask; &h; &w);
  n = h*w;

  lire_image_pgm("second_mask.pgm", mask, n);
  
  allocation_tableau(img, OCTET, 3*n);
  //#calcul image référence

  for (i = 0; i < nbday; i++)
    {
      indice = i * nbimg + first;
      sprintf(nomImg, "image%d.ppm", indice);
      lire_image_ppm(nomImg, img, n);
      nomImg = "image";
      mean_ref(img, nomImg, indice, ".ppm", h, w, nbimg, i);
     
      //#spécification
      sprintf(nomRef, "img_reference_day%d.ppm", i);
      spritnf(nomImg, "image%d.ppm", indice);
      lire_image_ppm(nomRef, ref, n);
      lire_image_ppm(nomImg, img, n);
      nomImg = "image";
      
      do_spec(img, ref, mask, nomImg, indice, ".ppm", int h, int w, int nbimg);
      
      //#différence
      spritnf(nomImg, "spec_%d.ppm", indice);
      lire_image_ppm(nomImg, img, n);
      nomImg = "spec_";

      do_diff(img, mask, ref, 5, nomImg, indice, ".ppm", h, w, nbimg);
    }	

  //regions map et red map
  spritnf(nomImg, "dif_%d.pgm", first);
  lire_image_ppm(nomImg, img, n);
  nomImg = "dif_";
  
  get_window_all(nomImg, first, ".pgm", nbimg, img, mask, max, min, 80, 160, h, w, 10, "image");
  /* 
     #---------------------------------------------
     #analyse des régions
     #---------------------------------------------
  */
  for(i = 0; i < nbday; i++)
    {
  indice = i * nbimg + first;
    ./do_chk "carte_regions_" "$indice" ".raw" $mask "10656" "1998" $min $max "$3" "0" "$i" 
      done
      

      return 1;
}
