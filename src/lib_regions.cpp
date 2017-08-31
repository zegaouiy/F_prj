void write_map(char* nom, int* map, int n, vector<int> &sizes)
{
  FILE *f_image;
  int i, tab[sizes.size()];

  for(i = 0; i < sizes.size(); i++)
    tab[i] = sizes[i];
  
  if( (f_image = fopen(nom, "wb")) == NULL)
    {
      printf("\nPas d'acces en ecriture sur l'image %s \n", nom);
      exit(EXIT_FAILURE);
    }

  fprintf(f_image,"%d", sizes.size()) ;

  i = sizes.size();
  if( (fwrite((int*)tab, sizeof(int), i, f_image))
      != (int)(i))
	    {
	       printf("\nErreur d'ecriture de l'image %s \n", nom);
	       exit(EXIT_FAILURE);
	    }

  if( (fwrite((int*)map, sizeof(int), n, f_image))
	     != (int)(n))
	    {
	       printf("\nErreur d'ecriture de l'image %s \n", nom);
	       exit(EXIT_FAILURE);
	    }
	 fclose(f_image);
}

void next_img(OCTET* img, char nom[250], int id, char suf[250], int n)
{
  char imnom[250];
  sprintf(imnom, "%s%d%s", nom, id, suf);
  
  lire_image_pgm(imnom, img, n);
}

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
      
    }
}

void get_window_all(char* name, int ind, char* suf, int nbimg, OCTET* img, OCTET* mask, int size_max, int size_min, int wh, int ww, int h, int w, OCTET s, char* outname)
{
  int i, j, *red_map, *map, *center, *tmp, n = h*w;
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
      cout << "begin label" << endl;
      label(img, mask, map, sizes, s, h, w);
      sprintf(true_name, "carte_regions_%d.raw", ind + j);
      write_map(true_name, map, n, sizes);
      cout << "end label" << endl;
      write_label(map, img, mask, ind+j-1, h, w, sizes.size());

      red_zone(img, mask, red_map, red_size, s, h, w);
      sprintf(true_name, "red_carte_regions_%d.raw", ind + j);
      write_map(true_name, red_map, n, red_size);

      cout << "begin crit" << endl;
      crit_size(mask, map, red_map, sizes, red_size, name, ind + j, h, w);
      cout << "end size crit" << endl;
      crit_dens(mask, map, red_map, sizes, red_size, name, ind + j, h, w);
      cout << "end dens crit" << endl;

      next_img(img, name, ind + j, suf, h*w);
    }
}  

//*************************************************************

double abs(double a)
{
  if (a < 0)
    a *= -1;
  return a;
}

int abs(int a)
{
  if (a < 0)
    a *= -1;
  return a;
}

void next_img(OCTET* img, char nom[250], int id, char suf[250], int n)
{
  char imnom[250];
  sprintf(imnom, "%s%d%s", nom, id, suf);

  lire_image_ppm(imnom, img, n);
}


void histo(int* h, OCTET* img, OCTET* mask, int n)
{
  int i;
  
  for(i = 0; i < 256; i++)
    h[i] = 0;

  for(i = 0; i < n; i++)
    {
      if(mask[i] == 255)
	h[img[i]]++;
    }
}

void diff(OCTET* img, OCTET* ref, int n)
{
  int i, dif;

  for(i = 0; i < n; i++)
    {
      dif = (int)ref[i] - (int)img[i];
      img[i] = (OCTET)abs(dif);
    }
}

void repart(OCTET* img, OCTET* mask, int* hist, double* ddp, int n)
{
  int i;
  double sum = 0;

  for(i = 0; i < 256; i++)
    {
      ddp[i] = (double)hist[i]/(double)n;
      
    }
  for(i = 0; i < 256; i++)
    {
      sum += ddp[i];
      ddp[i] = sum;
      
    }  
}
      
void egalise(OCTET* img, OCTET* mask, int* hist, double* ddp, int n)
{
  int i;
  
  for(i = 0; i < n; i++)
    {
      img[i] = (OCTET)(ddp[img[i]] * 255.0);
      
    }
}

void spec(OCTET* img, OCTET* ref, OCTET* mask, int n)
{
  int i, k, h[256], href[256], true_n = n, deb, fin;
  double ddp[256], d_ref[256];
  OCTET val;
  
  for(i = 0; i < n; i++)
    {
      if(!mask[i])
	true_n--;
    }
  
  histo(h, img, mask, n);
  repart(img, mask, h, ddp, true_n);
  histo(href, ref, mask, n);
  repart(ref, mask, href, d_ref, true_n);
 
  egalise(img, mask, h, ddp, n);
  
  for(i = 0; i < n; i++)
    {
      if(mask[i])
	{
	  deb = 0;
	  fin = 255;
	  while(fin  - deb > 1)
	    {
	      val = (fin + deb)/2 + (fin + deb)%2;
	      if((OCTET)(255.0 * d_ref[val] < img[i]))
		deb = val;
	      else
		fin = val;
	      
	    }
	  img[i] = val;
	}
    }
}

void spec_rgb(OCTET* img, OCTET* ref, OCTET* mask, int n)
{
  int i;
  OCTET *r, *b, *g, *r_ref, *g_ref, *b_ref;

  allocation_tableau(r, OCTET, n);
  allocation_tableau(g, OCTET, n);
  allocation_tableau(b, OCTET, n);
  
  planR(r, img, n);
  planV(g, img, n);
  planB(b, img, n);

  allocation_tableau(r_ref, OCTET, n);
  allocation_tableau(g_ref, OCTET, n);
  allocation_tableau(b_ref, OCTET, n);
  
  planR(r_ref, ref, n);
  planV(g_ref, ref, n);
  planB(b_ref, ref, n);

  spec(r, r_ref, mask, n);
  spec(g, g_ref, mask, n);
  spec(b, b_ref, mask, n);

  for(i = 0; i < n; i++)
    {
      img[3*i] = r[i];
      img[3*i + 1] = g[i];
      img[3*i + 2] = b[i];
    }
  free(r);
  free(g);
  free(b);  
  free(r_ref);
  free(g_ref);
  free(b_ref);
}
  

void bs(OCTET* img, OCTET* ref, OCTET* mask, int n, OCTET s)
{
  int* dif;
  
  // allocation_tableau(dif, int, 3 * n);
  
  spec_rgb(img, ref, mask, n);

}

void do_spec(OCTET* img, OCTET* ref, OCTET* mask, char* nom, int id, char* suf, int h, int w, int nbimg)//////////////:SEUIL
{
  int i, n = h * w;
  char out[250];

  for(i = 1; i < nbimg; i++)
    {
      bs(img, ref, mask, n, 1);
      cout << "done spec n°" << i << endl;
      sprintf(out, "%s%d%s", "spec_", id+i, suf);
      ecrire_image_ppm(out, img, h, w);
      next_img(img, nom, id + i, suf, n);
    }
}

void diff(OCTET* img, OCTET* ref, int n)
{
  int i, dif;

  for(i = 0; i < n; i++)
    {
      dif = (int)ref[i] - (int)img[i];
      img[i] = (OCTET)abs(dif);
    }
}

void to_nb(OCTET* img, OCTET* out, int n)
{
  int i;
  float r, g, b;
  
  for(i = 0; i < n; i++)
    {
      r = img[3 * i];
      g = img[3 * i + 1];
      b = img[3 * i + 2];

      out[i] = 0.3 * r + 0.6 * g + 0.1 * b;
    }
}

void median(OCTET* img, OCTET* out, OCTET* mask, int win, int h, int w)
{
  int i, j, n, m;
  vector<OCTET> vals(win * win);

  for(i = 0; i < h; i++)
    for(j = 0; j < w; j++)
      {
	if(mask[i * w + j])
	  {
	    vals.clear();
	    
	    for(n = (i - win < 0?0:i - win); n < (i + win < h?i + win:h - 1); n++)
	      for(m = (j - win < 0?0:j - win); m < (j + win < w?j + win:w - 1); m++)
		vals.push_back(img[n * w + m]);
	    
	    sort(vals.begin(), vals.end());

	    out[i * w + j] = vals[win*win/2];
	  }
      }
}

void order_dif(OCTET* img, OCTET* mask, OCTET* ref, OCTET* res, int win, int nH, int nW)
{
  OCTET *tmp;
  int i, j, n = nH * nW;
  
  app_mask(img, mask, n);
  app_mask(ref, mask, n);

  allocation_tableau(tmp, OCTET, n);
  
  median(img, res, mask, win, nH, nW,);
  diff(res, ref, n);
  erosion(res, tmp, mask, nH, nW, 2);
  dilat(tmp, res, mask, nH, nW, 2);

  //h_map(res, mask, coul, nH * nW);
}

void do_diff(OCTET* img, OCTET* mask, OCTET* ref, int win, char nom[250], int id, char suf[250], int h, int w, int nbimg)
{
  int i = 1, n = h * w;
  char file[250];
  OCTET *res, *imtemp;

  allocation_tableau(res, OCTET, n);
  allocation_tableau(imtemp, OCTET, 3 * h * w);

  order_dif(img, mask, ref, res, win, h, w);
  sprintf(file, "dif_%d.pgm", id);
  ecrire_image_pgm(file, res, h, w);

  while(i < nbimg)
    {
      next_img(imtemp, nom, id + i, suf, n);
      to_nb(imtemp, img, n);
      order_dif(img, mask, ref, res, win, h, w);
      sprintf(file, "dif_%d.pgm", id + i);
      ecrire_image_pgm(file, res, h, w);
      i++;
    }
}

void update(int* mean, OCTET* img, int n)
{
  int i;
  
  for(i = 0; i < 3 * n; i++)
    mean[i] += img[i];
}

void mean_ref(OCTET* img, char nom[250], int id, char suf[250], int h, int w, int nbimg, int day)
{
  int i, *mean, n = h*w;
  OCTET* out;
  char out_name[250];

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

  sprintf(out_name, "img_reference_day%d.ppm", day);
  ecrire_image_ppm("reference_mean.ppm", out, h, w);
}
