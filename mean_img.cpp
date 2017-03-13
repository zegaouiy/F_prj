#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
using namespace std;

void next_img(OCTET* img, char nom[250], int id, char suf[250], int n);

void update(int* mean, OCTET* img, int n);

void median(int* med, OCTET* img, int n, int m);

void mean_ref(OCTET* img, char nom[250], int id, char suf[250], int h, int w, int nbimg);

void do_med(OCTET* img, char nom[250], int id, char suf[250], int h, int w, int nbimg);
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

void median(int* med, OCTET* img, int n, int m)
{
  int i, j, b;
  
  for(i = 0; i < n; i++)
    {
      j = 0;
      b = (med[i * m + j] == -1) || (img[i] < med[i * m]);
      while(j < m && !b)
      {
	j++;
	b = (img[i] < med[i * m + j]) || (med[i * m + j] == -1);
	//cout << "img i = " << (int)img[i] << "   med i = " << med[i * m + j] << "  b = " << b << endl;
      }
      for(b = m - 1; b > j; b--)
	med[i * m + b] = med[i * m + b - 1];
      med[i * m + j] = img[i];
      
    }
}

void do_med(OCTET* img, char nom[250], int id, char suf[250], int h, int w, int nbimg)
{
  int i, *mean, *med, n = h*w;
  OCTET* out;

  allocation_tableau(med, int, 3 * n * nbimg);  
  allocation_tableau(out, OCTET, 3 * n);
  allocation_tableau(mean, int, 3 * n);

  for(i = 0; i < 3 * n * nbimg; i++)
    med[i] = -1;
  
  for(i = 1; i < nbimg; i++)
    {
      median(med, img, 3 * n, nbimg);
      next_img(img, nom, id + i, suf, n);
    }
  
  for(i = 0; i < 3 * n; i++)
    {
      out[i] = med[i * nbimg + nbimg/2];
    }

  ecrire_image_ppm("reference_med.ppm", out, h, w);
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

  do_med(ImgIn, nomImg, ind, suf, nH, nW, nbimg);

  return 1;
}
