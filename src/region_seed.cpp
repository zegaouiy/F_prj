#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>    
using namespace std;

void h_map(OCTET* img, OCTET* mask, OCTET* out, int n);

void thresh(OCTET* img, OCTET s, int n);

void label(OCTET* img, OCTET* mask, int* map, vector<int>& sizes, OCTET tmin, int h, int w);

void region(OCTET* img, OCTET* mask, int size_max, int size_min, int h, int w, OCTET s);

//**************************************************************************

void find(OCTET* mask, int &m, int n)
{
  int i;
  m = 0;
  for(i = 0; i < n; i++)
    if(mask[i])
      m++;
}

void maxi(OCTET* img, int* map, int h, int w, int &mi, int &mj)
{
  int i, j, val = -1;

  mi = -1;
  mj = -1;
  
  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      if(img[i * w + j] > val && map[i * w + j] == 0)
	{
	  mi = i;
	  mj = j;
	  val = img[i * w + j];
	}
  
}

void max_i(int* img, int n, int &m)
{
  int i;
  m = 0;
    
  for(i = 0; i < n; i++)
    if(img[i] > m)
      m = img[i];
}


void write_label(int* map, int h, int w)
{
  int i, max, n = h * w;
  OCTET* out;
  
  allocation_tableau(out, OCTET, n);
  
  max_i(map, n, max);

  for(i = 0; i < n; i++)
    {
      out[i] = (OCTET)((float)map[i]/(float)max * 255.0);
      out[i] = 255 - out[i];
      if(map[i] == 0)
	out[i] = 0;
    }
  
  ecrire_image_pgm("label_seed.pgm", out, h, w);
}
  

void thresh(OCTET* img, OCTET s, int n)
{
  int i;
  
  for(i = 0; i < n;i++)
    {
      if(img[i] < s)
	img[i] = 0;
      else
	img[i] = 255;
    }
}


void h_map(OCTET* img, OCTET* mask, OCTET* out, int n)
{
  int i;
  float a;

  for(i = 0; i < n; i++)
    {
      float color[4][3] = { {20,20,255}, {20,255,20}, {255,255,20}, {255,20,20} };
      
      int id1;
      int id2;
      float val, dis = 0;  
      
      if(mask[i])
	{
	  val = ((float)(img[i]) / 255.0) * 3.0;
	  id1  = floor(val);            
	  id2  = id1+1;                   
	  dis = val - float(id1);
	  
	  out[3 * i]   = (color[id2][0] - color[id1][0]) * dis + color[id1][0];
	  out[3 * i + 1] = (color[id2][1] - color[id1][1]) * dis + color[id1][1];
	  out[3 * i + 2] = (color[id2][2] - color[id1][2]) * dis + color[id1][2];
	}
      else
	{
	  out[3 * i] = 0;
	  out[3 * i + 1] = 0;
	  out[3 * i + 2] = 0;
	}
    }
}

void label(OCTET* img, OCTET* mask, int* map, vector<int>& sizes, OCTET tmin, int h, int w)
{
  int  true_n, i, j, size, voisin, mi, mj, p_i, p_j, lab = 1, n = h * w;
  vector<int> done;
  OCTET* out, t;
  float adv = 0;

  find(mask, true_n, n);
  
  allocation_tableau(out, OCTET, n);

  for(i = 0; i < n; i++)
    map[i] = 0;

  sizes.clear();

  maxi(img, map, h, w, mi, mj);

  while(mi >= 0 && img[mi * w + mj] > 90)
    {
      if(tmin > img[mi * w + mj])
	t = 0;
      else
	t = img[mi * w + mj] - tmin;
      //cout << " thresh = " << (int)t << "   max = " << (int)img[mi * w + mj] << endl;          
      for(i = 0; i < n; i++)
	out[i] = img[i];

      thresh(out, t, n);
      
      done.clear();
      done.push_back(mi);
      done.push_back(mj);
	    
      size = 0;

      while(!done.empty())
	{
	  p_j = done.back();
	  done.pop_back();
	  p_i = done.back();
	  done.pop_back();
		
	  for(i = p_i - 1; i <= p_i +1; i++)
	    for(j = p_j - 1; j <= p_j +1; j++)
	      {
		voisin = i * w + j;
		if(mask[i*w + j] && map[voisin] == 0 && out[voisin] != 0)
		  {
		    done.push_back(i);
		    done.push_back(j);
			  
		    map[i * w + j] = lab;
		    size++;
		  }
	      }
	}
      sizes.push_back(size);
      lab++;
      adv += size;
      maxi(img, map, h, w, mi, mj);
      cout << " adv = " << adv/(float)true_n * 100.0 << endl;
    }
}
// for(i = 0; i < sizes.size(); i++)
//     cout << " sizes = " << sizes[i] << " || " ;
//cout << endl << endl;

		      
void region(OCTET* img, OCTET* mask, int size_max, int size_min, int h, int w, OCTET s)
{
  int i, j, val, rlab, *map, *tmap, *result, done = 0, nTaille = h * w;
  
  vector<int> sizes;
  
  allocation_tableau(map, int, nTaille);
  allocation_tableau(result, int, nTaille);
  
  //label binary image
  label(img, mask, map, sizes, s, h, w);
  cout << "region " << endl;
  for(i = 0; i < nTaille; i++)
    {
      //for each label in map
      val = map[i];
      
      if(val > 0 && (sizes[val - 1] < size_min || sizes[val - 1] > size_max))
	sizes[val - 1] = 0;
      else
	sizes[val - 1] = val;
    }

  // for(i = 0; i < nTaille; i++)
  //   if(map[i])
  //     map[i] = sizes[map[i] - 1];
  
  write_label(map, h, w);

  // sort(sizes.begin(), sizes.end());
  // cout << "sizes : " << sizes[sizes.size() - 1] << " " << sizes[sizes.size() - 2] << " " << sizes[sizes.size() - 3] << endl;
}
      

int main(int argc, char* argv[])
{
  char nomImg[250], nomMask[250];
  int nH, nW, nTaille, min, max;
  
  OCTET *ImgIn, *mask, *out, s;
  if (argc != 6) 
    {
      printf("Usage: ImageDif.pgm/ppm Mask taille_min taille_max?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg) ;
  sscanf (argv[2],"%s",nomMask);
  sscanf (argv[3],"%d",&min);
  sscanf (argv[4],"%d",&max);
  sscanf (argv[5],"%d",&s);

  lire_nb_lignes_colonnes_image_pgm(nomImg, &nH, &nW);
  nTaille = nH * nW;

  allocation_tableau(mask, OCTET, nTaille);
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);

  lire_image_pgm(nomImg, ImgIn, nTaille);
  lire_image_pgm(nomMask, mask, nH * nW);

  region(ImgIn, mask, max, min, nH, nW,s);
  
  //h_map(ImgIn, mask, out, nTaille);

  //ecrire_image_ppm("carte_chal.ppm", out, nH, nW);

  return 1;
}
