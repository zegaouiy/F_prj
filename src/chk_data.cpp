void read_map(int* map, char* nom, int n)
{
  FILE *f_image;
  int  nb_colonnes, nb_lignes, max_grey_val;
  taille_image=3*taille_image;
   
  if( (f_image = fopen(nom, "rb")) == NULL)
    {
      printf("\nPas d'acces en ecriture sur l'image %s \n", nom);
      exit(EXIT_FAILURE);
    }
   
  if( (fread((int*)map, sizeof(int), n, f_image))
      != (int)(n))
    {
      printf("\nErreur d'ecriture de l'image %s \n", nom);
      exit(EXIT_FAILURE);
    }
  fclose(f_image);
}

void count_window(vector<int> win, ofstream count, int i)
{
  if (count.is_open())
    {
      count << i << " " << win.size() << endl;
    }
}

void count_gt(OCTET* gt, int h, int w)
{
  int  true_n, i, j, size, voisin, mi, mj, p_i, p_j, lab = 1, n = h * w;
  vector<int> done;
  OCTET* out, t;
  float adv = 0;

  allocation_tableau(out, OCTET, n);

  for(i = 0; i < n; i++)
    out[i] = 0;

  i = 0;
  mi = -1;
  size = 0;

  while(i < h)
    {
      j = 0;
      while( j < w)
	{
	  if(gt[i * w + j] > 0)
	    {
	      done.clear();
	      done.push_back(i);
	      done.push_back(j);
	      size++;

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
	  //cout << " adv = " << adv/(float)true_n * 100.0 << endl;
	}
    }
