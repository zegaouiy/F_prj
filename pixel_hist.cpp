#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
using namespace std;

void init_cord(int* cord, int nbpixel);

void next_img(OCTET* img, char nom[250], int id, char suf[250], int n);

void update_vals(OCTET* img, OCTET* vals, int* cord, int id, int w, int nbpixel, int nbimg);

void print_tab_nb(OCTET* vals, int nbpixel, int nbimg);

void print_tab_rgb(OCTET* vals, int nbpixel, int nbimg);

void pixel_histo(OCTET* img, char nom[250], char suf[250], int ind, int n, int w, int nbimg, int nbpixel);

//*************************************************************************************

void init_cord(int* cord, int nbpixel)
{
  for(int i = 0; i < nbpixel; i++)
    {
      cout << " tp cord?" << endl;
      cin >> cord[i*2];
      cin >> cord[i*2 + 1];
    }
}

void next_img(OCTET* img, char nom[250], int id, char suf[250], int n)
{
  char imnom[250];
  sprintf(imnom, "%s%d%s", nom, id, suf);

  lire_image_ppm(imnom, img, n);
}

void update_vals(OCTET* img, OCTET* vals, int* cord, int id, int w, int nbpixel, int nbimg)
{
  int i, j;

  for(i = 0; i < nbpixel; i++)
    {
      vals[3 * (i * nbimg + id)] = img[3 * (cord[i * 2] * w + cord[i * 2 + 1])];
      vals[3 * (i * nbimg + id) + 1] = img[3 * (cord[i * 2] * w + cord[i * 2 + 1]) + 1];
      vals[3 * (i * nbimg + id) + 2] = img[3 * (cord[i * 2] * w + cord[i * 2 + 1]) + 2];
    }
}

void print_tab_nb(OCTET* vals, int nbpixel, int nbimg)
{
  int i, j;
  ofstream lum;
  char nom[250];
  float r, g, b;
  
  for(i = 0; i < nbpixel; i++)
    {
      sprintf(nom, "repart_%d", i);
      lum.open(nom);
      
      for(j = 0; j < nbimg; j++)
	{
	  r = vals[3 * (i * nbimg + j)];
	  g = vals[3 * (i * nbimg + j) + 1];
	  b = vals[3 * (i * nbimg + j) + 2];
	  
	  lum << j << " " << 0.3 * r + 0.6 * g + 0.1 * b << endl;
	}
      lum.close();
    }
}

void print_tab_rgb(OCTET* vals, int nbpixel, int nbimg)
{
  int i, j;
  ofstream r, g, b;
  char nom[250];
  
  for(i = 0; i < nbpixel; i++)
    {
      sprintf(nom, "red_%d", i);
      r.open(nom);
      sprintf(nom, "green_%d", i);
      g.open(nom);
      sprintf(nom, "blue_%d", i);
      b.open(nom);
      for(j = 0; j < nbimg; j++)
	{
	  r << j << " " << (int)vals[3 * (i * nbimg + j)] << endl;
	  g << j << " " << (int)vals[3 * (i * nbimg + j) + 1] << endl;
	  b << j << " " << (int)vals[3 * (i * nbimg + j) + 2] << endl;
	}
      r.close();
      g.close();
      b.close();
    }
}

void pixel_histo(OCTET* img, char nom[250], char suf[250], int ind, int n, int w, int nbimg, int nbpixel)
{
  int i, *cord;
  OCTET *vals;

  allocation_tableau(cord, int, 2 * nbpixel);
  allocation_tableau(vals, OCTET, 3 * nbimg * nbpixel);

  init_cord(cord, nbpixel);

  for(i = 1; i < nbimg + 1; i++)
    {
      update_vals(img, vals, cord, i - 1,  w,  nbpixel, nbimg);
      next_img(img, nom, ind + i, suf, n);
    }

  print_tab_nb(vals, nbpixel, nbimg);
  print_tab_rgb(vals, nbpixel, nbimg);
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

  pixel_histo(ImgIn, nomImg, suf, ind, nTaille, nW, nbimg, 10);

  return 1;
}
