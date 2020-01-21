/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "GuyTom",                /* Team name */

    "Guy Shoham",            /* First member full name */
    "GuyShoham28@gmail.com", /* First member email address */

    "Tom Sendrovich",        /* Second member full name (leave blank if none) */
    "tsanderovich@gmail.com" /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";

void naive_rotate(int dim, pixel* src, pixel* dst) {
  int i, j;

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[RIDX(dim - 1 - j, i, dim)] = src[RIDX(i, j, dim)];
}

void my_rotate(int dim, pixel* src, pixel* dst) {
  int cacheSize = 32;
  int i, j, row, col, inCol, inRow;

  for (i = 0; i < dim; i = i + cacheSize) {
    for (j = 0; j < dim; j = j + cacheSize) {
      inCol = j + cacheSize;
      ///when we switch the two loops, we get better spatial locality, more cache hits.
      ///we go over a box of 32X32 (cacheSize) pixels at a time
      for (col = j; col < inCol; col++) {
        inRow = i + cacheSize;
        for (row = i; row < inRow; row++) {
          dst[RIDX(dim - 1 - col, row, dim)] = src[RIDX(row, col, dim)];
        }
      }
    }
  }
}

/*
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char my_rotate_descr[] = "rotate: Current working version";

void rotate(int dim, pixel* src, pixel* dst) {
  my_rotate(dim, src, dst);
}

/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() {
  add_rotate_function(&naive_rotate, naive_rotate_descr);
  add_rotate_function(&rotate, my_rotate_descr);
  /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
  int red;
  int green;
  int blue;
  int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }

static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum* sum) {
  sum->red = sum->green = sum->blue = 0;
  sum->num = 0;
  return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum* sum, pixel p) {
  sum->red += (int) p.red;
  sum->green += (int) p.green;
  sum->blue += (int) p.blue;
  sum->num++;
  return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel* current_pixel, pixel_sum sum) {
  current_pixel->red = (unsigned short) (sum.red / sum.num);
  current_pixel->green = (unsigned short) (sum.green / sum.num);
  current_pixel->blue = (unsigned short) (sum.blue / sum.num);
  return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel* src) {
  int ii, jj;
  pixel_sum sum;
  pixel current_pixel;

  initialize_pixel_sum(&sum);
  for (ii = max(i - 1, 0); ii <= min(i + 1, dim - 1); ii++)
    for (jj = max(j - 1, 0); jj <= min(j + 1, dim - 1); jj++)
      accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

  assign_sum_to_pixel(&current_pixel, sum);
  return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";

void naive_smooth(int dim, pixel* src, pixel* dst) {
  int i, j;

  for (i = 0; i < dim; i++)
    for (j = 0; j < dim; j++)
      dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

void my_smooth(int dim, pixel* src, pixel* dst) {
  int i, j, k;

  ///handling 3 cases:

  /// corners (4 successors)
  /// sides (6 successors)
  /// rest (9 successors)

  pixel tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9;
  int index;

  ///corners:

  ///top left
  tmp1 = src[0];
  tmp2 = src[1];
  tmp3 = src[dim];
  tmp4 = src[dim + 1];
  index = 0;

  dst[index].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red) / 4;
  dst[index].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue) / 4;
  dst[index].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green) / 4;

  ///bottom left
  i = dim * 2 - 1;

  tmp1 = src[dim - 2];
  tmp2 = src[dim - 1];
  tmp3 = src[i - 1];
  tmp4 = src[i];
  index = dim - 1;

  dst[index].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red) / 4;
  dst[index].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue) / 4;
  dst[index].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green) / 4;

  ///top right
  j = dim * (dim - 1);
  i = dim * (dim - 2);

  tmp1 = src[j];
  tmp2 = src[j + 1];
  tmp3 = src[i];
  tmp4 = src[i + 1];
  index = j;

  dst[index].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red) / 4;
  dst[index].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue) / 4;
  dst[index].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green) / 4;

  ///bottom right
  j = dim * dim - 1;
  i = dim * (dim - 1) - 1;

  tmp1 = src[j - 1];
  tmp2 = src[j];
  tmp3 = src[i - 1];
  tmp4 = src[i];
  index = j;

  dst[index].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red) / 4;
  dst[index].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue) / 4;
  dst[index].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green) / 4;

  ///sides:
  i = dim - 1;
  for (j = 1; j < i; j++) {
    tmp1 = src[j];
    tmp2 = src[j - 1];
    tmp3 = src[j + 1];
    tmp4 = src[j + dim];
    tmp5 = src[j + 1 + dim];
    tmp6 = src[j - 1 + dim];
    dst[j].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red + tmp5.red + tmp6.red) / 6;
    dst[j].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green + tmp5.green + tmp6.green) / 6;
    dst[j].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue + tmp5.blue + tmp6.blue) / 6;
  }

  i = dim * dim - 1;
  for (j = i - dim + 2; j < i; j++) {
    tmp1 = src[j];
    tmp2 = src[j - 1];
    tmp3 = src[j + 1];
    tmp4 = src[j - dim];
    tmp5 = src[j + 1 + dim];
    tmp6 = src[j - 1 - dim];
    dst[j].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red + tmp5.red + tmp6.red) / 6;
    dst[j].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green + tmp5.green + tmp6.green) / 6;
    dst[j].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue + tmp5.blue + tmp6.blue) / 6;
  }

  for (j = dim + dim - 1; j < dim * dim - 1; j += dim) {
    tmp1 = src[j];
    tmp2 = src[j - 1];
    tmp3 = src[j - dim];
    tmp4 = src[j + dim];
    tmp5 = src[j - dim - 1];
    tmp6 = src[j + dim - 1];
    dst[j].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red + tmp5.red + tmp6.red) / 6;
    dst[j].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green + tmp5.green + tmp6.green) / 6;
    dst[j].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue + tmp5.blue + tmp6.blue) / 6;
  }

  i = i - (dim - 1);
  for (j = dim; j < i; j += dim) {
    tmp1 = src[j];
    tmp2 = src[j - dim];
    tmp3 = src[j + 1];
    tmp4 = src[j + dim];
    tmp5 = src[j + dim + 1];
    tmp6 = src[j - dim + 1];
    dst[j].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red + tmp5.red + tmp6.red) / 6;
    dst[j].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green + tmp5.green + tmp6.green) / 6;
    dst[j].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue + tmp5.blue + tmp6.blue) / 6;
  }

  ///rest:
  k = dim;
  for (i = 1; i < dim - 1; i++) {
    for (j = 1; j < dim - 1; j++) {
      k++;
      tmp1 = src[k - 1];
      tmp2 = src[k];
      tmp3 = src[k + 1];
      tmp4 = src[k - dim - 1];
      tmp5 = src[k - dim];
      tmp6 = src[k - dim + 1];
      tmp7 = src[k + dim - 1];
      tmp8 = src[k + dim];
      tmp9 = src[k + dim + 1];

      dst[k].red = (tmp1.red + tmp2.red + tmp3.red + tmp4.red
          + tmp5.red + tmp6.red + tmp7.red + tmp8.red + tmp9.red) / 9;

      dst[k].green = (tmp1.green + tmp2.green + tmp3.green + tmp4.green
          + tmp5.green + tmp6.green + tmp7.green + tmp8.green + tmp9.green) / 9;

      dst[k].blue = (tmp1.blue + tmp2.blue + tmp3.blue + tmp4.blue
          + tmp5.blue + tmp6.blue + tmp7.blue + tmp8.blue + tmp9.blue) / 9;

    }
    k += 2;
  }
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version";

void smooth(int dim, pixel* src, pixel* dst) {
  my_smooth(dim, src, dst);
}

/*********************************************************************
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_smooth_functions() {
  add_smooth_function(&naive_smooth, naive_smooth_descr);
  add_smooth_function(&smooth, smooth_descr);
  /* ... Register additional test functions here */
}

