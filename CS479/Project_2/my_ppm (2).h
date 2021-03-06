#ifndef __MY_PPM_H__
#define __MY_PPM_H__
/**
    @file my_ppm.h

    @author Terence Henriod

    @brief Provides basic functionality for reading and writing .ppm
           image files.    

    @version Original Code 1.00 (2/28/2014) - T. Henriod

    UNOFFICIALLY:
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

/*==============================================================================
=======     HEADER FILES     ===================================================
==============================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


/*==============================================================================
=======     CONSTANTS / MACROS     =============================================
==============================================================================*/
#define false 0
#define true 1
#define IMAGE_NAME_LENGTH 60


/*==============================================================================
=======     USER DEFINED TYPES     =============================================
==============================================================================*/

/**
@struct ImageInfo

A structure containing information relevant to the image format.

@var name         The image name
@var width        The width of the image
@var height       The height of the image
@var num_shades   The number of shades to be represented in the image.
                  Generally, 256 will be enough for my purposes, hence the
                  byte sized type.
@var data         A 2-d byte array pointer for the image data
*/
typedef struct
{
  char name[IMAGE_NAME_LENGTH];
  unsigned int width;
  unsigned int height;
  unsigned short num_shades;
  RgbTriple** data; 
} PpmImageData;


/**
@struct RgbTriple

A structure containing values for the Red Green and Blue components of
a .ppm image pixel.

@var red     The red value of the pixel.
@var green   The green value of the pixel.
@var blue    The blue value of the pixel.
*/
typedef struct
{
  unsigned int red;
  unsigned int green;
  unsigned int blue; 
} RgbTriple;


/*==============================================================================
=======     FUNCTION PROTOTYPES     ============================================
==============================================================================*/

/**
createPPMimage

Takes a given matrix of byte data and exports that data to a .pgm image file.

@param file_name      The name the the created file will have, as a C-string
@param image_width    The width of the image/number of data columns
@param image_height   The height of the image/the number rows for the
                      image/data
@param max_shades     The shade range for the data
@param image_data     A pointer to the image data matrix

@return success   The success of the operation. Returns false if the image
                  cannot be created, true if it can.

@pre
-# Valid dimensions that match the data must be given
-# All data must be given in bytes
-# The number of shades should be a normal count (it will be decremented
   to enable storage as a byte)

@post
-# A binary .pgm file will be created

@detail @bAlgorithm
-# A new image file with the given name is created
-# A header for a .pgm file is written
-# The data from the given matrix to the image file is written
-# The file is then closed

@code
@endcode
*/
int createPpmFile( PpmImageData* image );


/**
writePPMheader

Writes the header for the ppm file

@param image_file     A file pointer to the newly created file
@param image_width    The width of the image/number of data columns
@param image_height   The height of the image/the number rows for the
                      image/data
@param max_shades     The shade range for the data


@pre
-# Valid dimensions that match the data must be given
-# All data must be given in bytes
-# The number of shades should be a normal count (it will be decremented
   to enable storage as a byte)

@post
-# A binary .pgm file header will be written

@detail @bAlgorithm
-# The "magic number" for a binary grayscale format is written
-# A comment is written for the file
-# The dimensions of the image are written
-# Finally, the number of possible shades are written

@code
@endcode
*/
void writePpmHeader( FILE* image_file, PpmImageData* image );


int readPpmFile( char* file_name );

int readPpmHeader( FILE* image_file, PpmImageData* image );

/*==============================================================================
=======     FUNCTION IMPLEMENTATIONS     =======================================
==============================================================================*/

int readPpmFile( PpmImageData* image, char* file_name )
{
  // variables
  int read_success = true;
  FILE* image_file;

  // open the file
  image_file = fopen( file_name );

  // case: the file was opened successfully
  if( image_file != NULL )
  {
    // read in the header
    read_sucess = readPpmHeader( image_file, image );

    // read in the image data
    readPpmData( image_file, image );
  }

  // image data returned by reference, return the success of the operation
  return read_success;
}


int readPpmHeader( FILE* image_file, PpmImageData* image )
{
  // variables
  int read_success = true;  // TODO: error checking
  char dumb_string[80];

  // read the magic number
  fscanf( image_file, "%s", dumb_string );

  // read the comment
  fscanf( image_file, "%s", dumb_string );

  // read the image dimensions
  fscanf( image_file, "%d %d", &(image->width), &(image->height) );

  // read the color scale
  fscanf( image_file, "%d", &(image->num_shades) );

  // allocate the image matrix
  allocatePpmImage( image, image->width, image->height );

  // read the image data
  readPpmData( image_file, image );

  // return the success of the operation
  return read_success;
}


int readPpmData( FILE* image_file, PpmImageData* image )
{
  // variables
  int read_success = true;  // TODO: error checking

  // read all of the rows
  for( i = 0; i < image->height; i++ )
  {
    // read the color bytes for each row
    for( j = 0; j < image->width; j++ )
    {
      // read the color bytes
      fscanf( image_file, "%c%c%c", &(image->data[i][j].red),
                                    &(image->data[i][j].green),
                                    &(image->data[i][j].blue) );
    }
  }

  // return the success of the operation
  return read_success;
}


int createPGMimage( PGMImageData* image )
{
  // variables
  int success = false;
  FILE* the_file = NULL;
  int pixel_x = 0;
  int pixel_y = 0;
  unsigned char pixel = 0;

  // assert necessities for variables
  assert( image->num_shades <= 255 ); // byte max
  assert( image->num_shades > 0 );

  // create a new file
  the_file = fopen( image->name, "wb" );

  // case: the file opening was successful
  if( the_file != NULL )
  {
    // update the success status
    success = true;

    // write the file header
    writePGMheader( the_file, image );

    // write the image data
    // iterate through rows
    for( pixel_x = 0; pixel_x < image->height; pixel_x++ )
    {
      // iterate across the row
      for( pixel_y = 0; pixel_y < image->width; pixel_y++ )
      {
        // print the pixel value
        pixel = (unsigned char) image->data[pixel_y][pixel_x];
        fprintf( the_file, "%c", (unsigned char) pixel );
      }

      // NOTE: ASCII mode requires a '\n' to end the row,
      //       binary mode will create a black line if you do this
    }

  // close the file
  fclose( the_file );
  }

  // return the success state of the operation
  return success;
}

void writePpmHeader( FILE* image_file, PpmImageData* image )
{
  // variables
    // none

  // print the pgm (Portable Gray Map) format
  // use P2 for ASCII mode (gives easy human readability, requires spaces)
  // use P5 for binary mode (gives better storage and processing)
  fprintf( image_file, "P6\n" );

  // write the file comment
  //fprintf( image_file, "# Makin' my PGM file\n" );    Let's not

  // specify image dimensions
  fprintf( image_file, "%d %d\n", image->width, image->height );

  // print the max shade value (granularity) number
  fprintf( image_file, "%d\n", image->num_shades );

  // no return - void
}


int allocatePpmImage( PpmImageData* image, int width, int height )
{
  // variables
  int allocation_success = true;  // TODO: error checking
  int i = 0;

  // set the new deimensions
  image->width = width;
  image->height = height;

  // allocate then row pointers
  image->data = (RgbTriple**) malloc( height * sizeof( RgbTriple* ) );

  // allocate all the rows
  for( i = 0; i < height; i++ )
  {
    // allocate the memory
    image->data[i] = (RgbTriple*) malloc( width * sizeof( RgbTriple ) );
  }

  // return the success of the operation
  return allocation_success;
}


void deconstructPpmImage( PpmImage* image )
{
  // variables
  int i = 0;

  // deallocate each row
  for( i = 0; i < image->height; i++ )
  {
    // deallocate it
    free( image->data[i] );
  }

  // deallocate the row pointers
  free( image->data );
  image->data = NULL;

  // zero the dimensions
  image->width = 0;
  image->height = 0;

  // return the image by reference
}

#endif

