void read_map(char* nom, int* map, int n, int &size, int &r_size)
{
  FILE *f_image;
  int  nb_colonnes, nb_lignes, max_grey_val;
  taille_image=3*taille_image;

  fscanf(f_image, "%d %d",
	 &size, &r_size); /*lecture entete*/
	
   
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
      count << i << " " << win.size()/2 << endl;
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
    out[i] = gt[i];

  i = 0;
  mi = -1;
  size = 0;

  while(i < h)
    {
      j = 0;
      while( j < w)
	{
	  if(out[i * w + j] > 0)
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
			if(out[voisin] > 0)
			  {
			    done.push_back(i);
			    done.push_back(j);
			    
			    out[i * w + j] = 0;
			  }
		      }
		}
	      //cout << " adv = " << adv/(float)true_n * 100.0 << endl;
	    }
	}
    }
}

void check_gt(OCTET* gt, vector<int> win, int size, ofstream roc, int wh, int ww, int h, int w, int ind)
{
  int i, k, l, kmin, kmax, lmin, lmax, fn, fp, vp, tmp;
  fp = 0;
  fn = size;
  vp = 0;

  for(i = 0; i < win.size(); i+=2)
    {
      kmin = max(0, win[ind] - wh);
      kmax = min(h, win[ind] + wh);
      lmin = max(0, win[ind + 1] - ww);
      lmax = min(w, win[ind + 1] + ww);
      
      tmp = 0;

      for(k = kmin; k < kmax; k++)
    	for(l = lmin; l < lmax; l++)
	  if(gt[k*w + l] > 0)
	    {
	      vp++;
	      fn--;
	      tmp = 1;
	      break;
	    }
      
      if(!tmp)
	fp++;
    }
  
  roc << ind << " " << vp << " " << fp << " " << fn << endl;
}
 
void chk_dat(OCTET* gt, vector<int> win, ofstream count, ofstream roc, int wh, int ww, int h, int w, int ind, int gt)
{
  count_window(win, count, i);

  if(gt)
    {
      int size = count_gt(gt, h, w);
      check_gt(gt, win, size, roc, wh, ww, h, w, ind);
    }
}
