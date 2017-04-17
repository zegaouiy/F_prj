#include <stdio.h>
#include "image_ppm.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>    
using namespace std;

void h_map(OCTET* img, OCTET* mask, OCTET* out, int n);

void thresh(OCTET* img, OCTET s, int n);

void label(OCTET* img, int* map, vector<int>& sizes, int h, int w);

void region(OCTET* img, int size_max, int size_min, int h, int w, OCTET s);

//**************************************************************************

void maxi(int* img, int n, int &m)
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
  
  maxi(map, n, max);

  for(i = 0; i < n; i++)
    out[i] = (OCTET)((float)map[i]/(float)max * 255.0);
  
  ecrire_image_pgm("label.pgm", out, h, w);
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

void label(OCTET* img, int* map, vector<int>& sizes, int h, int w)
{
  int i, k, j, l, p_i, p_j, size, voisin, lab = 1, n = h * w;
  vector<int> done;
  
  for(i = 0; i < n; i++)
    map[i] = 0;

  sizes.clear();

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      {
	if(!map[i * w + j] && img[i * w + j])
	  {
	    done.clear();
	    done.push_back(i);
	    done.push_back(j);
	    
	    size = 0;

	    while(!done.empty())
	      {
		p_j = done.back();
		done.pop_back();
		p_i = done.back();
		done.pop_back();
		
		for(k = p_i - 1; k <= p_i +1; k++)
		  for(l = p_j - 1; l <= p_j +1; l++)
		    {
		      voisin = k * w + l;
		      if(map[voisin] == 0 && img[voisin] != 0)
			{
			  done.push_back(k);
			  done.push_back(l);
			  
			  map[k * w + l] = lab;
			  size++;
		
			}
		    }
	      }
	    sizes.push_back(size);
	    lab++;
	    //cout << " size = " << sizes.size() << "  lab = " << lab << endl;
	  }
      }
// for(i = 0; i < sizes.size(); i++)
//     cout << " sizes = " << sizes[i] << " || " ;
  //cout << endl << endl;
}
		      
void region(OCTET* img, int size_max, int size_min, int h, int w, OCTET s)
{
  int i, j, val, rlab, *map, *tmap, *result, done = 0, nTaille = h * w;
  OCTET th = 127, *bin;
  vector<int> sizes, tsizes;
  
  allocation_tableau(map, int, nTaille);
  allocation_tableau(tmap, int, nTaille);
  allocation_tableau(result, int, nTaille);
  allocation_tableau(bin, OCTET, nTaille);
  
  rlab = 1;

  for(i = 0; i < nTaille; i++)
    map[i] = 0;

  while(!done)
    {
      //threshold image 
      for(i = 0; i < nTaille; i++)
	bin[i] = img[i];
      thresh(bin, th, nTaille); 

      //label binary image
      label(bin, tmap, tsizes, h, w);
      
      val = 1;//label value

      for(i = 0; i < nTaille; i++)
	{
	  //for each label in map
	  if(map[i] == val)
	    {
	      //check label size 
	      if(tsizes[tmap[i] - 1] >= size_max)
		{
		  //cout << "sup max" << endl;
		  if(sizes[val - 1] < size_max && sizes[val - 1] > size_min)
		    {
		      //update result
		      cout << "resultj = " << i << "  rlab = " << rlab << endl;
		      for(j = 0; j < nTaille; j++)
			if(map[j] == val)
			{
			  result[j] = rlab;
			}
		      rlab++;
		    }
		}
	      else
		done = 0;
	      val++;
	    }
	}
      
      // update map with new labels and sizes
      for(i = 0; i < nTaille; i++)
	map[i] = tmap[i];
      sizes = tsizes;
      
      th--;
      done = done || th == s;
    }
  
  for(i = 0; i < nTaille; i++)
    if(result[i] > 0)
      bin[i] = 255;
    else
      bin[i] = 0;
  
  ecrire_image_pgm("result.pgm", bin, h, w);

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

  region(ImgIn, max, min, nH, nW,s);

  //h_map(ImgIn, mask, out, nTaille);

  //ecrire_image_ppm("carte_chal.ppm", out, nH, nW);

  return 1;
}
