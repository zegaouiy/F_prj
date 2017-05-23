#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>    
#include "morpho.cpp"

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

void fusion(int* map, int* red_map, int* tmp_map, vector<int> sizes, vector<int>& regions, vector<int> red_tab, int size_max, int size_min, int wh, int ww, int h, int w);

void crit_size(OCTET* mask, int* map, int* red_map, vector<int> sizes, vector<int> red_sizes, char* nom, int ind, int h, int w);

void crit_dens(OCTET* mask, int* map, int* red_map, vector<int> sizes, vector<int> red_sizes, char* nom, int ind, int h, int w);

void crit_ratio(OCTET* mask, int* map, int* red_map, vector<int> sizes, vector<int> red_sizes, char* nom, int ind, int h, int w);

void max_i(int* img, int n, int &m);
void max_i(float* img, int n, float &m);
void min_i(int* img, int n, int &m);
void min_i(float* img, int n, float &m);

//void draw_window(OCTET* img, vector<int> regions, int wh, int ww, int h, int w);

//void write_label(int* map, OCTET* img, OCTET* mask, int h, int w, int nlab);
//**************************************************************************

void app_mask(OCTET* img, OCTET* mask, int n)
{
  for(int i = 0; i < n; i++)
    {
      if(mask[i] != 255)
	img[i] = 0;
    }
}


void next_img(OCTET* img, char nom[250], int id, char suf[250], int n)
{
  char imnom[250];
  sprintf(imnom, "%s%d%s", nom, id, suf);
  
  lire_image_pgm(imnom, img, n);
}


#include "fusion_window.cpp"
#include "self_crit.cpp"

void get_max(OCTET* img, OCTET* mask, int* map, OCTET* maxs, int n, int nlab)
{
  int i, val;
  OCTET k;
  
  for(i = 0; i < nlab; i++)
    {
      maxs[2 * i] = 0; 
      maxs[2 * i + 1] = 255;
    }
      
  for(i = 0; i < n; i++)
    {
      if(mask[i] && map[i])
	{
	  val = map[i] - 1;
	  k = img[i];
	  
	  if(k > maxs[2 * val])
	    maxs[2 * val] = k;
	  if(k < maxs[2 * val + 1])
	    maxs[2 * val + 1] = k;
	}
    }
}

void geo_val(OCTET* img, OCTET* mask, int* map, OCTET* maxs, int n, int nlab)
{
  int i, val;
  float min, max, k;

  for(i = 0; i < n; i++)
    {
      if(mask[i] && map[i])
	{
	  val = map[i] - 1;
	  
	  k = img[i];
	  max = maxs[2 * val];
	  min = maxs[2 * val + 1];
	  
	  img[i] = (255.0/(max - min))*(k - min);
	}
    }
      
}

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

void min_i(int* img, int n, int &m)//m = max
{
  int i;
  
  for(i = 0; i < n; i++)
    if(img[i] < m && img[i] > 0)
      m = img[i];
}

void max_i(float* img, int n, float &m)
{
  int i;
  m = 0;
    
  for(i = 0; i < n; i++)
    if(img[i] > m)
      m = img[i];
}

void min_i(float* img, int n, float &m)
{
  int i;
  
  for(i = 0; i < n; i++)
    if(img[i] < m && img[i] > 0)
      m = img[i];
}



void write_label(int* map, OCTET* img, OCTET* mask, int ind, int h, int w, int nlab)
{
  int i, max, n = h * w, nbm = 5;
  OCTET* out, *coul, *tmp;
  char name[250];
  
  allocation_tableau(out, OCTET, n);
  allocation_tableau(tmp, OCTET, n);
  allocation_tableau(coul, OCTET, 3*n);

  max_i(map, n, max);

  for(i = 0; i < n; i++)
    {
      out[i] = (OCTET)((float)map[i]/(float)max * 255.0);
      out[i] = 255 - out[i];
      if(map[i] == 0)
	out[i] = 0;
    }
  cout << " dilat " << endl;

  dilat(out, tmp, mask, h, w, nbm);
  erosion(tmp, out, mask, h, w, nbm);
    
  //******** geo label

  OCTET maxs[2 * nlab];
  get_max(img, mask, map, maxs, n, nlab);
  geo_val(img, mask, map, maxs, n, nlab);

  dilat(img, out, mask, h, w, nbm);
  erosion(out, tmp, mask, h, w, nbm);

  sprintf(name, "region_%d.ppm", ind);
  h_map(tmp, mask, coul, h * w);
  ecrire_image_ppm(name, coul, h, w);
    
  for(i = 0; i< h*w;i++)
    img[i] = tmp[i];
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
      
      if(mask[i] && img[i] != 0)
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


      
void red_zone(OCTET* img, OCTET* mask, int* map, vector<int>& sizes, OCTET tmin, int h, int w)
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

  for(i = 0; i < n; i++)
    out[i] = img[i];
  
  thresh(out, 254, n);
      

  for(mi = 0; mi < h; mi++)
    for(mj = 0; mj < w; mj++)
      {
	if(out[mi * w + mj] > 0 && map[mi * w + mj] == 0)
	  {
	    done.clear();
	    done.push_back(mi);
	    done.push_back(mj);
	  
	    size = 1;
	  
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
	    cout << " red_adv = " << adv/(float)true_n * 100.0 << endl;
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

  while(mi >= 0 && img[mi * w + mj] > 40)
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

		      
// void region(OCTET* img, OCTET* mask, int size_max, int size_min, int h, int w, OCTET s)
// {
//   int i, j, val, rlab, *map, *tmap, *result, done = 0, nTaille = h * w;
  
//   vector<int> sizes;
  
//   allocation_tableau(map, int, nTaille);
//   allocation_tableau(result, int, nTaille);
  
//   //label binary image
//   red_zone(img, mask, map, sizes, s, h, w);
  
//   //cout << "region " << endl;
//   /* for(i = 0; i < nTaille; i++)
//      {
//      //for each label in map
//      val = map[i];
      
//      if(val > 0 && (sizes[val - 1] < size_min || sizes[val - 1] > size_max))
//      sizes[val - 1] = 0;
//      else
//      sizes[val - 1] = val;
//      }
//   */
//   // for(i = 0; i < nTaille; i++)
//   //   if(map[i])
//   //     map[i] = sizes[map[i] - 1];
  
//   write_label(map, img, mask, h, w, sizes.size());

//   sort(sizes.begin(), sizes.end());
//   //cout << "sizes : " << sizes[sizes.size() - 1] << " " << sizes[sizes.size() - 2] << " " << sizes[sizes.size() - 3] << endl;
//   for(i = 0; i < sizes.size(); i++)
//     cout << i << " " << sizes[i] << endl;
// }

void get_window_all(char* name, int ind, char* suf, int nbimg, OCTET* img, OCTET* mask, int size_max, int size_min, int wh, int ww, int h, int w, OCTET s, char* outname)
{
  int i, j, *red_map, *map, *center, *tmp;
  vector<int> sizes;
  vector<int> red_size;
  vector<int> red_tab;
  vector<int> regions;
  OCTET* out;
  char true_name[250];
  
  allocation_tableau(red_map, int, h*w);
  allocation_tableau(map, int, h*w);
  allocation_tableau(tmp, int, h*w);
  allocation_tableau(out, OCTET, 3*h*w);

  for(j = 0; j < nbimg; j++)
    {
      app_mask(img, mask, h*w);
      label(img, mask, map, sizes, s, h, w);
      write_label(map, img, mask, ind+j-1, h, w, sizes.size());

      red_zone(img, mask, red_map, red_size, s, h, w);
      i = red_size.size();
      //allocation_tableau(center, int, 4 * i); // mini maxi minj maxj

      cout << "begin crit" << endl;
      crit_size(mask, map, red_map, sizes, red_size, name, ind + j, h, w);
      cout << "end size crit" << endl;
      crit_dens(mask, map, red_map, sizes, red_size, name, ind + j, h, w);
      
	next_img(img, name, ind + j, suf, h*w);
    }
}  


int main(int argc, char* argv[])
{
  char nomImg[250], suf[250], nomMask[250], name[250], outname[250];
  int nH, nW, nTaille, min, max, indice, nbimg;
  
  OCTET *ImgIn, *mask, *out, s;
  if (argc != 10) 
    {
      printf("Usage: ImageDif.pgm/ppm Mask taille_min taille_max?\n"); 
      exit (1) ;
    }
   
  sscanf (argv[1],"%s",nomImg);
  sscanf (argv[2],"%d",&indice);
  sscanf (argv[3],"%s",suf) ;
  sscanf (argv[4],"%s",nomMask);
  sscanf (argv[5],"%s",outname);
  sscanf (argv[6],"%d",&min);
  sscanf (argv[7],"%d",&max);
  sscanf (argv[8],"%d",&s);
  sscanf (argv[9],"%d",&nbimg);

  sprintf(name, "%s%d%s", nomImg, indice, suf);
  
  lire_nb_lignes_colonnes_image_pgm(name, &nH, &nW);
  nTaille = nH * nW;

  allocation_tableau(mask, OCTET, nTaille);
  allocation_tableau(ImgIn, OCTET, 3 * nTaille);

  lire_image_pgm(name, ImgIn, nTaille);
  lire_image_pgm(nomMask, mask, nH * nW);

  get_window_all(nomImg, indice, suf, nbimg, ImgIn, mask, max, min, 80, 120, nH, nW, s, outname);
  
  //h_map(ImgIn, mask, out, nTaille);

  //ecrire_image_ppm("carte_chal.ppm", out, nH, nW);

  return 1;
}
