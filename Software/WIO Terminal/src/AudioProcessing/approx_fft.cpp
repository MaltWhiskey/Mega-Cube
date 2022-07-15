// see here: https://create.arduino.cc/projecthub/abhilashpatel121/approxfft-fastest-fft-function-for-arduino-fd4917

int fast_cosine(int Amp, int th);
int fast_sine(int Amp, int th);
int fastRSS(int a, int b);

//---------------------------------lookup data------------------------------------//
unsigned char isin_data[128] =
    {0, 1, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15, 17, 18, 19, 20,
     22, 23, 24, 26, 27, 28, 29, 31, 32, 33, 35, 36, 37, 39, 40, 41, 42,
     44, 45, 46, 48, 49, 50, 52, 53, 54, 56, 57, 59, 60, 61, 63, 64, 65,
     67, 68, 70, 71, 72, 74, 75, 77, 78, 80, 81, 82, 84, 85, 87, 88, 90,
     91, 93, 94, 96, 97, 99, 100, 102, 104, 105, 107, 108, 110, 112, 113, 115, 117,
     118, 120, 122, 124, 125, 127, 129, 131, 133, 134, 136, 138, 140, 142, 144, 146, 148,
     150, 152, 155, 157, 159, 161, 164, 166, 169, 171, 174, 176, 179, 182, 185, 188, 191,
     195, 198, 202, 206, 210, 215, 221, 227, 236};
unsigned int Pow2[14] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
unsigned char RSSdata[20] = {7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2};
//---------------------------------------------------------------------------------//

//-----------------------------FFT Function----------------------------------------------//
/*
Code to perform High speed and Accurate FFT on arduino,
setup:

1. in[]     : Data array, 
2. N        : Number of sample (recommended sample size 2,4,8,16,32,64,128,256,512...)
3. Frequency: sampling frequency required as input (Hz)

It will by default return frequency with max aplitude,
if you need complex output or magnitudes uncomment required sections

If sample size is not in power of 2 it will be clipped to lower side of number. 
i.e, for 150 number of samples, code will consider first 128 sample, remaining sample  will be omitted.
For Arduino nano, FFT of more than 256 sample not possible due to mamory limitation 
Code by ABHILASH
Contact: abhilashpatel121@gmail.com
Documentation & details: https://www.instructables.com/member/abhilash_patel/instructables/
*/

float Approx_FFT(int in[], int out_r[], int out_im[], unsigned int N, float Frequency)
{
  int a, c1, f, o = 0, x, data_max, data_min = 0;
  long data_avg, data_mag, temp11;
  unsigned char scale, check = 0;

  data_max = 0;
  data_avg = 0;
  data_min = 0;

  for (int i = 0; i < 12; i++) //calculating the levels
  {
    if (Pow2[i] <= N)
    {
      o = i;
    }
  }
  a = Pow2[o];

  for (int i = 0; i < a; i++) //getting min max and average for scalling
  {
    out_r[i] = 0;
    out_im[i] = 0;
    data_avg = data_avg + in[i];
    if (in[i] > data_max)
    {
      data_max = in[i];
    }
    if (in[i] < data_min)
    {
      data_min = in[i];
    }
  }

  data_avg = data_avg >> o;
  scale = 0;
  data_mag = data_max - data_min;
  temp11 = data_mag;

  for (int i = 0; i < 128; i++) //scalling data  from +512 to -512

    if (data_mag > 1024)
    {
      while (temp11 > 1024)
      {
        temp11 = temp11 >> 1;
        scale = scale + 1;
      }
    }

  if (data_mag < 1024)
  {
    while (temp11 < 1024)
    {
      temp11 = temp11 << 1;
      scale = scale + 1;
    }
  }

  if (data_mag > 1024)
  {
    for (int i = 0; i < a; i++)
    {
      in[i] = in[i] - data_avg;
      in[i] = in[i] >> scale;
    }
    scale = 128 - scale;
  }

  if (data_mag < 1024)
  {
    scale = scale - 1;
    for (int i = 0; i < a; i++)
    {
      in[i] = in[i] - data_avg;
      in[i] = in[i] << scale;
    }

    scale = 128 + scale;
  }

  x = 0;
  for (int b = 0; b < o; b++) // bit reversal order stored in im_out array
  {
    c1 = Pow2[b];
    f = Pow2[o] / (c1 + c1);
    for (int j = 0; j < c1; j++)
    {
      x = x + 1;
      out_im[x] = out_im[j] + f;
    }
  }

  for (int i = 0; i < a; i++) // update input array as per bit reverse order
  {
    out_r[i] = in[out_im[i]];
    out_im[i] = 0;
  }

  int i10, i11, n1, tr, ti;
  float e;
  int c, temp4;
  for (int i = 0; i < o; i++) //fft
  {
    i10 = Pow2[i];               // overall values of sine/cosine
    i11 = Pow2[o] / Pow2[i + 1]; // loop with similar sine cosine
    e = 1024 / Pow2[i + 1];      //1024 is equivalent to 360 deg
    e = 0 - e;
    n1 = 0;

    for (int j = 0; j < i10; j++)
    {
      c = e * j; //c is angle as where 1024 unit is 360 deg
      while (c < 0)
      {
        c = c + 1024;
      }
      while (c > 1024)
      {
        c = c - 1024;
      }

      n1 = j;

      for (int k = 0; k < i11; k++)
      {
        temp4 = i10 + n1;
        if (c == 0)
        {
          tr = out_r[temp4];
          ti = out_im[temp4];
        }
        else if (c == 256)
        {
          tr = -out_im[temp4];
          ti = out_r[temp4];
        }
        else if (c == 512)
        {
          tr = -out_r[temp4];
          ti = -out_im[temp4];
        }
        else if (c == 768)
        {
          tr = out_im[temp4];
          ti = -out_r[temp4];
        }
        else if (c == 1024)
        {
          tr = out_r[temp4];
          ti = out_im[temp4];
        }
        else
        {
          tr = fast_cosine(out_r[temp4], c) - fast_sine(out_im[temp4], c); //the fast sine/cosine function gives direct (approx) output for A*sinx
          ti = fast_sine(out_r[temp4], c) + fast_cosine(out_im[temp4], c);
        }

        out_r[n1 + i10] = out_r[n1] - tr;
        out_r[n1] = out_r[n1] + tr;
        if (out_r[n1] > 15000 || out_r[n1] < -15000)
        {
          check = 1;
        } //check for int size, it can handle only +31000 to -31000,

        out_im[n1 + i10] = out_im[n1] - ti;
        out_im[n1] = out_im[n1] + ti;
        if (out_im[n1] > 15000 || out_im[n1] < -15000)
        {
          check = 1;
        }

        n1 = n1 + i10 + i10;
      }
    }

    if (check == 1)
    { // scalling the matrics if value higher than 15000 to prevent varible from overflowing
      for (int i = 0; i < a; i++)
      {
        out_r[i] = out_r[i] >> 1;
        out_im[i] = out_im[i] >> 1;
      }
      check = 0;
      scale = scale - 1; // tracking overall scalling of input data
    }
  }

  if (scale > 128)
  {
    scale = scale - 128;
    for (int i = 0; i < a; i++)
    {
      out_r[i] = out_r[i] >> scale;
      out_im[i] = out_im[i] >> scale;
    }
    scale = 0;
  } // revers all scalling we done till here,
  else
  {
    scale = 128 - scale;
  } // in case of nnumber getting higher than 32000, we will represent in as multiple of 2^scale

  /*
for(int i=0;i<a;i++)
{
Serial.print(out_r[i]);Serial.print("\t");                    // un comment to print RAW o/p    
Serial.print(out_im[i]); 
Serial.print("i");Serial.print("\t"); 
Serial.print("*2^");Serial.println(scale); 
}
*/

  //---> here onward out_r contains amplitude and our_in conntains frequency (Hz)

  int fout, fm, fstp;
  float fstep;
  fstep = Frequency / N;
  fstp = fstep;
  fout = 0;
  fm = 0;

  for (unsigned int i = 1; i < Pow2[o - 1]; i++) // getting amplitude from compex number
  {
    out_r[i] = fastRSS(out_r[i], out_im[i]);
    // Approx RSS function used to calculated magnitude quickly

    out_im[i] = out_im[i - 1] + fstp;
    if (fout < out_r[i])
    {
      fm = i;
      fout = out_r[i];
    }
  }

  float fa, fb, fc;
  fa = out_r[fm - 1];
  fb = out_r[fm];
  fc = out_r[fm + 1];
  fstep = (fa * (fm - 1) + fb * fm + fc * (fm + 1)) / (fa + fb + fc);

  return (fstep * Frequency / N);
  // return 0;
}

//---------------------------------fast sine/cosine---------------------------------------//

int fast_sine(int Amp, int th)
{
  int temp3, m1, m2;
  unsigned char temp1, temp2, test, quad, accuracy;
  accuracy = 5; // set it value from 1 to 7, where 7 being most accurate but slowest
                // accuracy value of 5 recommended for typical applicaiton
  while (th > 1024)
  {
    th = th - 1024;
  } // here 1024 = 2*pi or 360 deg
  while (th < 0)
  {
    th = th + 1024;
  }
  quad = th >> 8;

  if (quad == 1)
  {
    th = 512 - th;
  }
  else if (quad == 2)
  {
    th = th - 512;
  }
  else if (quad == 3)
  {
    th = 1024 - th;
  }

  temp1 = 0;
  temp2 = 128; //2 multiple
  m1 = 0;
  m2 = Amp;

  temp3 = (m1 + m2) >> 1;
  Amp = temp3;
  for (int i = 0; i < accuracy; i++)
  {
    test = (temp1 + temp2) >> 1;
    temp3 = temp3 >> 1;
    if (th > isin_data[test])
    {
      temp1 = test;
      Amp = Amp + temp3;
      m1 = Amp;
    }
    else if (th < isin_data[test])
    {
      temp2 = test;
      Amp = Amp - temp3;
      m2 = Amp;
    }
  }

  if (quad == 2)
  {
    Amp = 0 - Amp;
  }
  else if (quad == 3)
  {
    Amp = 0 - Amp;
  }
  return (Amp);
}

int fast_cosine(int Amp, int th)
{
  th = 256 - th; //cos th = sin (90-th) formula
  return (fast_sine(Amp, th));
}

//--------------------------------------------------------------------------------//

//--------------------------------Fast RSS----------------------------------------//
int fastRSS(int a, int b)
{
  if (a == 0 && b == 0)
  {
    return (0);
  }
  int min, max, temp1, temp2;
  unsigned char clevel;
  if (a < 0)
  {
    a = -a;
  }
  if (b < 0)
  {
    b = -b;
  }
  clevel = 0;
  if (a > b)
  {
    max = a;
    min = b;
  }
  else
  {
    max = b;
    min = a;
  }

  if (max > (min + min + min))
  {
    return max;
  }
  else
  {
    temp1 = min >> 3;
    if (temp1 == 0)
    {
      temp1 = 1;
    }
    temp2 = min;
    while (temp2 < max)
    {
      temp2 = temp2 + temp1;
      clevel = clevel + 1;
    }
    temp2 = RSSdata[clevel];
    temp1 = temp1 >> 1;
    for (int i = 0; i < temp2; i++)
    {
      max = max + temp1;
    }
    return (max);
  }
}
//--------------------------------------------------------------------------------//