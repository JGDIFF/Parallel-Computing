/*  Usage:
mpirun -np <p> ./staticParaMB -w <width> -h <height> -o <output file> [-m <magnify>] 
*/

#include <stdio.h>
#include <iostream>
#include <jpeglib.h>
#include <jerror.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "mpi.h"
#define CHANNELS 3  //number of bytes per pixel (RGB)
#define MAXITER 100 //maximum number of iterations to check for convergence
#define RED 1       

/* prototypes for functions in this file */
static void writeJPGImage(const char *, unsigned char *, int, int);
static void mandelbrot(unsigned char * image, int width, int height, float magnify, int numP, int myId);
static void putPixel(unsigned char * image, int width, int height,
              int x, int y, int count);
static void getColor(int count, char & red, char & green, char & blue);

static std::string getFileExt(const std::string & s); 
static void parseArgs(int argc, char * argv[], std::string & outputfile,
                      int & width, int & height, float & magnify);
static void checkArgs(std::string outputfile, int width, int height, float magnify);
static void printUsage();


int main(int argc, char * argv[])
{
   std::string filename;
   int height = 0, width = 0;
   float magnify = 1.0;
   unsigned char * imageAll = NULL;   //used by P0 to contain all pixels of the image
   unsigned char * imagePart = NULL;  //used by each process to hold a subset of the image rows

   MPI::Init();
   int myId = MPI::COMM_WORLD.Get_rank();
   int numP = MPI::COMM_WORLD.Get_size();

   //get the filename, width, height, and magnify and make sure they are valid
   parseArgs(argc, argv, filename, width, height, magnify); 
   checkArgs(filename, width, height, magnify); 

   if (myId == 0)
   {
       std::cout << "Generating an image of size " << width << " by " << height
                 << " from the Mandelbrot set\n";
   }

   if (myId == 0)
   {
	imageAll = (unsigned char*) malloc(sizeof(unsigned char) * height * width * CHANNELS);
   }
   imagePart = (unsigned char*) malloc(sizeof(unsigned char) * (height/numP) * width * CHANNELS);
   // TO DO: Add code here to create the arrays you need. 
   // 1) Process 0 will need to dynamically allocate space to hold the entire image. The image consists 
   // of height rows. Each row is width pixels. Each pixel is CHANNELS bytes. Initialize imageAll  
   // to point to the dynamically allocated data.
   // 2) All processes need to dynamically allocate space to hold height/numP rows of the image. 
   // Each row has width pixels. Each pixel is CHANNELS bytes. Initialize imagePart variable to point
   // to the dynamically allocated data.

   double start = MPI::Wtime();

   // TO DO: Call mandelbrot and collect the results on process 0. 
   // 1) Each process calls mandelbrot.
   // 2) Each process calls Gather to gather the results on process 0
   mandelbrot(imagePart, width, height, magnify, numP, myId);
   MPI::COMM_WORLD.Gather(imagePart, (height/numP * width * CHANNELS), MPI::UNSIGNED_CHAR, imageAll,  (height/numP * width * CHANNELS), MPI::UNSIGNED_CHAR, 0);

   double stop = MPI::Wtime();
  
   if (myId == 0)
   {
      std::cout << "Static Parallel Mandelbrot time: " << stop-start << " seconds\n";
      std::cout << "Output file: " << filename << "\n";
   }

   /* write image to file */
   if (myId == 0 && imageAll != NULL) 
      writeJPGImage(filename.c_str(), imageAll, width, height);
   MPI::Finalize();
   free(imagePart);
   free(imageAll);
}

/*
 * mandelbrot
 * Takes as input the width and height of the image to be generated and
 * generates an image based upon the Mandelbrot set. 
 * The Mandelbrot set is the set of complex numbers c for which the function 
 * fc(z) = z*z + c does not diverge when iterated from z = 0, i.e, for which 
 * the sequence fc(0), fc(fc(0)) etc, remains bounded in absolute value.
 * 
 * Inputs: 
 *    width, height - of image to be generated
 *    magnify - used to generate an image that is a zoom in or zoom
 *              of the set
 *    image - array of size width * height * CHANNELS to hold generated image
*/
void mandelbrot(unsigned char * image, int width, int height, float magnify, int numP, int myId)
{  
   double x,xx,y,cx,cy; 
   int iteration,hx,hy, color;
   int newhy = 0;

   /* TO DO: This is the sequential mandelbrot.  You need to modify it
      so that each process calculates 1/numP of the rows of pixels
      There are two places below that need to be se changed.  You probably
      also want to define some local variables.
    */
   
   //for each pixel in the image
   for (hy = (((height/numP)*myId)+1); hy <= ((height/numP)*(myId+1)); hy++)   //TO DO: This needs to change.
   {  
      for (hx = 1; hx <= width; hx++)
      {  
         cx = (((float)hx)/((float)width)-0.5)/magnify*3.0-0.7;
         cy = (((float)hy)/((float)height)-0.5)/magnify*3.0;
         x = 0.0; y = 0.0;
         color = 0; //assume cx, cy is in the set 
         for (iteration = 1; iteration < MAXITER; iteration++)
         {  
            xx = x*x-y*y+cx;
            y = 2.0*x*y+cy;
            x = xx; 
            if (x*x + y*y > 100.0)  //complex number is not in set
            {  
               color = RED;
               break;
            }
         }
         //set the appropriate pixel in the destination image
         putPixel(image, width, height/numP, hx - 1, newhy, color);  //TO DO: This needs to change.
      }
      newhy++;
   }
}


/*
 * getColor
 * Set the red, green, and blue bytes of a pixel based upon the
 * value of color.
 * Input:
 *    color - 0 or RED
 *    red, green, blue - references to the bytes of the pixel
*/
void getColor(int color, char & red, char & green, char & blue)
{
   // Feel free to change these colors
   if (color == RED) 
   {
      red = 180;
      green = blue = 0;
   } else
   {
      red = 0;
      green = blue = 255;
   }
}

/* putPixel
 * Sets the red, green, and blue bytes for the pixel at index (x, y)
 * Input:
 *     image - array to hold the image of size width * height * CHANNELS
 *     width - width of image in pixels
 *     height - height of image in pixels
 *     x, y - coordinates of pixels
 *     color - used to choose the color of the pixel
*/
void putPixel(unsigned char * image, int width, int height,
              int x, int y, int color) 
{
   //error checking
   if (x >= width || y >= height)
   {
      printf("x, y coordinates outside of size of image.\n");
      printf("\timage size: %d by %d\n", width, height);
      printf("\tcoordinates: (%d, %d)", x, y);
      exit(0);
   }
   char red, green, blue;
   //get the red, green, blue byte values for the pixel
   getColor(color, red, green, blue); 
   //flatten the x and y coordinates to get the index
   image[y * width * CHANNELS + x * CHANNELS] = red;
   image[y * width * CHANNELS + x * CHANNELS + 1] = green;
   image[y * width * CHANNELS + x * CHANNELS + 2] = blue;
}

/*
 * parseArgs
 * Parses the command line arguments in order to define the parameters
 * for the Mandelbrot program.
 * Input:
 *    argc - count of command line arguments
 *    argv - array of command line arguments
 *    filename - reference to a string to be set to the name of the output file
 *    width - reference to width parameter
 *    height - reference to height parameter
 *    magnify - reference to magnify parameter
*/
void parseArgs(int argc, char * argv[], std::string & filename,
               int & width, int & height, float & magnify)
{
   int i;
   bool bad = false;
   int myId = MPI::COMM_WORLD.Get_rank();

   if (argc < 7) bad = true;
   for (i = 1; i < argc - 1 && !bad; i+=2)
   {
      // -h height
      if (std::string(argv[i]) == "-h")
         height = atoi(argv[i+1]);
      // -w width
      else if (std::string(argv[i]) == "-w")
         width = atoi(argv[i+1]);
      //-o filename
      else if (std::string(argv[i]) == "-o")
         filename = argv[i+1];
      //-m magnitude
      else if (std::string(argv[i]) == "-m")
         magnify = atof(argv[i+1]);
      else 
         bad = true;
   }

   if (bad && myId == 0) printUsage();
   MPI::COMM_WORLD.Barrier();
   if (bad) MPI::COMM_WORLD.Abort(1);
}

/*
 * checkArgs
 * Checks the parameters for the Mandelbrot program.
 * Input:
 *    filename - needs to end with .jpg extension
 *    width - width in pixels of the output image (> 0)
 *    height - height in pixels of the output image (> 0)
 *    magnify - constant used in Mandelbrot set calculation (> 0)
*/
void checkArgs(std::string filename, int width, int height, float magnify)
{
   int numP = MPI::COMM_WORLD.Get_size();
   int myId = MPI::COMM_WORLD.Get_rank();

   //All processes check for bad parameters for the mandel brot
   bool badHeight = (height <= 0);
   bool notMultipleHeight = ((height % numP) != 0);
   bool badWidth = (width <= 0);
   bool badMagnify = (magnify <= 0);
   std::string extension = getFileExt(filename); 
   bool badFile = (filename.length() < 5 || extension != "jpg");
   bool bad = (notMultipleHeight || badHeight || badWidth || badMagnify || badFile);

   //only one process prints the error message
   if (badHeight && myId == 0) 
   {
      std::cout << "Bad value for image height: " << height << "\n";
      std::cout << "Cannot be less than or equal to 0\n\n";
   }
   if (notMultipleHeight && myId == 0)
   {
      std::cout << "Bad value for image height: " << height << "\n";
      std::cout << "Height must be a multiple of number of processes: "
                << numP << "\n";
   }
   if (badWidth && myId == 0)
   {
      std::cout << "Bad value for image width: " << width << "\n";
      std::cout << "Cannot be less than or equal to 0\n\n";
   }
   if (badMagnify && myId == 0) 
   {
      std::cout << "Bad value for magnify: " << magnify << "\n";
      std::cout << "Cannot be less than or equal to 0\n\n";
   }
   if (badFile && myId == 0)
   {
      std::cout << "Bad output file name: " << filename << "\n";
      std::cout << "Must end with .jpg extension\n\n";
   }

   //only one process prints usage info
   if (bad && myId == 0) printUsage();

   MPI::COMM_WORLD.Barrier();
   //if bad parameters call Abort
   if (bad) MPI::COMM_WORLD.Abort(1);
}

/*
 * getFileExt
 * Takes as input a string and returns the characters after the final dot
 * (i.e., the extension).
 * Input:
 *    s - string
*/
std::string getFileExt(const std::string & s) 
{
   size_t i = s.rfind('.', s.length());
   if (i != std::string::npos) {
      return(s.substr(i+1, s.length() - i));
   }
   return("");
}

/*
 * printUsage
 * Prints usage information and exits.
*/
void printUsage()
{
    std::cout << "usage: mpirun -np <p> ./staticParaMB -w <width> -h <height> -o <output file> [-m <magnify>]\n";
    std::cout << "\tThis program creates a jpeg file containing an image\n";
    std::cout << "\tgenerated from the Mandelbrot set.\n";
    std::cout << "\t<p> processes are created to run the program\n";
    std::cout << "\t<width> is the width of the generated image\n";
    std::cout << "\t<height> is the height of the generated image\n";
    std::cout << "\t<output file> is the name of the file the image will be stored in\n";
    std::cout << "\t<magnify> is a floating point value that increases/decreases\n";
    std::cout << "\t\tthe number of values in the Mandelbrot set. Default: 1.0\n\n";
    std::cout << "example: mpirun -np 4 ./staticParaMB -w 4000 -h 5000 -o mandel.jpg -m 1.0\n\n"; 
}

/*
 * writeJPGImage
 * Takes as input an array of pixels of size width by height and create
 * an jpg file that contains the bytes. 
 * Input:
 *    filename - name of the output file
 *    image - array of pixels (three bytes per pixel)
 *    width - width of image
 *    height - height of image
*/
void writeJPGImage(const char * filename, unsigned char * Pout,
                   int width, int height)
{
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr jerr;
   JSAMPROW rowPointer[1];

   //set up error handling
   cinfo.err = jpeg_std_error(&jerr);
   //initialize the compression object
   jpeg_create_compress(&cinfo);

   //open the output file
   FILE * fp;
   if ((fp = fopen(filename, "wb")) == NULL)
   {
     fprintf(stderr, "Can't open %s\n", filename);
     exit(1);
   }
   //initalize state for output to outfile
   jpeg_stdio_dest(&cinfo, fp);

   cinfo.image_width = width;    /* image width and height, in pixels */
   cinfo.image_height = height;
   cinfo.input_components = CHANNELS;   /* # of color components per pixel */
   cinfo.in_color_space = JCS_RGB;
   jpeg_set_defaults(&cinfo);
   jpeg_set_quality(&cinfo, 75, TRUE);

   //TRUE means it will write a complete interchange-JPEG file
   jpeg_start_compress(&cinfo, TRUE);

   while (cinfo.next_scanline < cinfo.image_height)
   {
      rowPointer[0] = &Pout[cinfo.next_scanline * width * CHANNELS];
      (void) jpeg_write_scanlines(&cinfo, rowPointer, 1);
   }
   jpeg_finish_compress(&cinfo);
   fclose(fp);
   jpeg_destroy_compress(&cinfo);
}

