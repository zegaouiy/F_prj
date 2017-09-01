#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "image_ppm.h"
#include <omp.h>
using namespace std;

void h_map(OCTET* img, OCTET* mask, OCTET* out, int n);

void thresh(OCTET* img, OCTET s, int n);

void label(OCTET* img, OCTET* mask, int* map, vector<int>& sizes, OCTET tmin, int h, int w);

void region(OCTET* img, OCTET* mask, int size_max, int size_min, int h, int w, OCTET s);

void get_max(OCTET* img, OCTET* mask, int* map, OCTET* maxs, int n, int nlab);

void geo_val(OCTET* img, OCTET* mask, int* map, OCTET* maxs, int n, int nlab);

void red_zone(OCTET* img, OCTET* mask, int* map, vector<int>& sizes, OCTET tmin, int h, int w);

void get_window(OCTET* img, OCTET* mask, int size_max, int size_min, int wh, int ww, int h, int w, OCTET s);

void red_filter(int* red_map, int* center, vector<int> red_sizes, vector<int>& red_tab, int h, int w);

void barycentre(int* red_map, vector<int> red_tab, int h, int w);

void fusion(OCTET* crit_size, OCTET* crit_ratio, OCTET* crit_dens, int* map, int* red_map, int *tmp_map, vector<int> sizes, vector<int>& regions, vector<int> red_tab, int size_max, int size_min, int wh, int ww, int h, int w);
  
void crit_size(OCTET* mask, int* map, int* red_map, vector<int> sizes, vector<int> red_sizes, char* nom, int ind, int h, int w);

void crit_dens(OCTET* mask, int* map, int* red_map, vector<int> sizes, vector<int> red_sizes, char* nom, int ind, int h, int w);

void app_mask(OCTET* img, OCTET* mask, int n)
{
  for(int i = 0; i < n; i++)
    {
      if(mask[i] != 255)
	for(int j = 0; j < 3; j ++)
	  img[3 * i + j] = 0;
    }
}

void app_maskg(OCTET* img, OCTET* mask, int n)
{
  for(int i = 0; i < n; i++)
    {
      if(mask[i] != 255)
	img[i] = 0;
    }
}


#include "chk_data.cpp"
#include "morpho.cpp"
#include "lib_regions.cpp"
#include "self_crit.cpp"
#include "fusion_window.cpp"




int main(int argc, char* argv[])
{
  OCTET *mask, *out, *img, *ref;
  int* ImgIn, first, nbday, indice, nbimg;

  // if (argc != 4) 
  //   {
  //     printf("Usage: Image indice .pgm Mask ImageRawPath taille_min taille_max s=10 nbimg?\n"); 
  //     exit (1) ;
  //   }
   
  // sscanf (argv[1],"%d",&nbday);
  // sscanf (argv[2],"%d",&first);
  // sscanf (argv[3],"%d",&nbimg) ;

  cout << "nombre de jour?    premier indice?     nombre d'img/jour" << endl;
  cin >> nbday;
  cin >> first;
  cin >> nbimg;
  
  char nomImg[250], nomRef[250], nomMask[250] = "second_mask.pgm", spec[250] = "nb_spec_", dif[250] = "dif_";
  int i, size_morph = 5, s = 10, min = 100, max = 10000, h, w, n;
  
  lire_nb_lignes_colonnes_image_pgm(nomMask, &h, &w);
  n = h*w;

  cout << h << " " << w << endl;

  allocation_tableau(mask, OCTET, n);
  allocation_tableau(out, OCTET, n);
  allocation_tableau(img, OCTET, 3*n);
  allocation_tableau(ref, OCTET, 3*n);

  lire_image_pgm("second_mask.pgm", mask, n);
  //#calcul image référence
  
  for (i = 0; i < nbday; i++)
    {
      indice = i * nbimg + first;
      sprintf(nomImg, "image%d.ppm", indice);
      lire_image_ppm(nomImg, img, n);
      sprintf(nomImg, "image");
      mean_ref(img, nomImg, indice, ".ppm", h, w, nbimg, i);
      cout << "ref done" << endl;
     
      //#spécification
      sprintf(nomRef, "img_reference_day%d.ppm", i);
      sprintf(nomImg, "image%d.ppm", indice);
      lire_image_ppm(nomRef, ref, n);
      lire_image_ppm(nomImg, img, n);
      sprintf(nomImg, "image");
      
      do_spec(img, ref, mask, nomImg, indice, ".ppm", h, w, nbimg);
      //#différence
      sprintf(nomImg, "spec_%d.ppm", indice+1);
      lire_image_ppm(nomImg, img, n);
      sprintf(nomImg, "spec_");

      do_diff(img, mask, ref, 3, nomImg, indice, ".ppm", h, w, nbimg);
      cout << "dif done " << endl;
    }	

  //regions map et red map
  sprintf(nomImg, "dif_%d.pgm", first);
  lire_image_pgm(nomImg, out, n);
  sprintf(nomImg, "dif_");
  cout << "region start" << endl;
  
  get_window_all(nomImg, first, ".pgm", nbimg, out, mask, max, min, 8, 12, h, w, 10, "image");
  /* 
     #---------------------------------------------
     #analyse des régions
     #---------------------------------------------
  */
  int *map;
  allocation_tableau(map, int, n);
  for(i = 0; i < nbday; i++)
    {
      indice = i * nbimg + first;
      chk_window("carte_regions_", indice, ".raw", nbimg, map, mask, max, min, 80, 120, h, w, "image", 0, i);
    }
      

  return 1;
}
