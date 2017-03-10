#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
using namespace std;

void next_img(OCTET* img, char nom[250], int id, char suf[250], int n);

void update(int* mean, OCTET* img, int n);

void mean_ref(OCTET* img, char nom[250], int id, char suf[250], int h, int w, int nbimg);

//********************************************************************************

void next_img(OCTET* img, char nom[250], int id, char suf[250], int n)
{
  char imnom[250];
  sprintf(imnom, "%s%d%s", nom, id, suf);

  lire_image_ppm(imnom, img, n);
}

void update(int* mean, OCTET* img, int n)
{
  int i;
  
  for(i = 0; i < 3 * n; i++)
    mean[i] += img[i];
}

void mean_ref(OCTET* img, char nom[250], int id, char suf[250], int h, int w, int nbimg)
{
  int i, *mean, n = h*w;
  OCTET* out;

  allocation_tableau(out, OCTET, 3 * n);
  allocation_tableau(mean, int, 3 * n);
  
  for(i = 1; i < nbimg; i++)
    {
      update(mean, img, n);
      next_img(img, nom, id + i, suf, n);
    }
  
  for(i = 0; i < 3 * n; i++)
    {
      mean[i] /= nbimg;
      out[i] = mean[i];
    }

  ecrire_image_ppm("reference_mean.ppm", out, h, w);
}

int main(int argc, char* argv[])
{
  char nomImg[250], nomMask[250], suf[250], fimg[250];;
  int nH, nW, nTaille, ind, nbimg;
  
  OCTET *ImgIn, *mask;
  if (argc != 6) 
    {
      printf("Usage: nomImg indice suffixe Mask nbimg?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg);
  sscanf (argv[2],"%d",&ind);
  sscanf (argv[3],"%s",suf);
  sscanf (argv[4],"%s",nomMask);
  sscanf (argv[5],"%d",&nbimg);

  sprintf(fimg, "%s%d%s", nomImg, ind, suf);
  cout << fimg << endl;
  ind++;

  lire_nb_lignes_colonnes_image_ppm(fimg, &nH, &nW);
  nTaille = nH * nW;
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);
  lire_image_ppm(fimg, ImgIn, nTaille);
  allocation_tableau(mask, OCTET, nTaille);
  lire_image_pgm(nomMask, mask, nH * nW);

  mean_ref(ImgIn, nomImg, ind, suf, nH, nW, nbimg);

  return 1;
}
