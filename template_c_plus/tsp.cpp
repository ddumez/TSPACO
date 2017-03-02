
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <assert.h>

#define LINE_BUF_LEN     100
#define M_PI 3.14159265358979323846264
#define TRACE( x )

using namespace std;

struct point {
  double x;
  double y;
};

class TSP {

   FILE *tsp_file;
   char          name[LINE_BUF_LEN];      	   /* instance name */
   char          edge_weight_type[LINE_BUF_LEN];  /* selfexplanatory */
   long int      optimum;                /* optimal tour length if known, otherwise a bound */
   long int      n;                      /* number of cities */
   long int      n_near;                 /* number of nearest neighbors */
   struct point  *nodeptr;               /* array of structs containing coordinates of nodes */
   long int      **distance;	          /* distance matrix: distance[i][j] gives distance between city i und j */
   /*long int      **nn_list;              /* nearest neighbor list; contains for each node i a
                                           sorted list of n_near nearest neighbors */

    /* Distance functions*/
   long int     round_distance (long int i, long int j);
   long int     ceil_distance (long int i, long int j);
   long int     geo_distance (long int i, long int j);
   long int     att_distance (long int i, long int j);
   long int **  compute_distances(void);
   long int     compute_distance(long int i, long int j);

   static double dtrunc (double x);

   public:
     TSP (const char *tsp_file_name);
     ~TSP();
     void printDistance(void) ;
     long int getSize();
     long int getDistance(long int i, long int j);
      

};

/* Constructor*/
TSP::TSP (const char *tsp_file_name){
    
    char         buf[LINE_BUF_LEN];
    long int     i, j;

    // Open file
    tsp_file = fopen(tsp_file_name, "r");
    if ( tsp_file == NULL ) {
	fprintf(stderr,"No instance file specified, abort\n");
	exit(1);
    }
    assert(tsp_file != NULL);
    printf("\nReading tsp-file %s ... \n", tsp_file_name);

    // Read lines
    fscanf(tsp_file,"%s", buf);
    while ( strcmp("NODE_COORD_SECTION", buf) != 0 ) {
	if ( strcmp("NAME", buf) == 0 ) {
	    fscanf(tsp_file, "%s", buf);
	    TRACE ( printf("%s ", buf));
	    fscanf(tsp_file, "%s", buf);
	    strcpy(name, buf);
	    TRACE ( printf("%s \n", name));
	    buf[0]=0;
	}
	else if ( strcmp("NAME:", buf) == 0 ) {
	    fscanf(tsp_file, "%s", buf);
	    strcpy(name, buf);
	    TRACE ( printf("%s \n", name); );
	    buf[0]=0;
	}
	else if ( strcmp("COMMENT", buf) == 0 ){
	    fgets(buf, LINE_BUF_LEN, tsp_file);
	    TRACE ( printf("%s", buf); );
	    buf[0]=0;
	}
	else if ( strcmp("COMMENT:", buf) == 0 ){
	    fgets(buf, LINE_BUF_LEN, tsp_file);
	    TRACE ( printf("%s", buf); );
	    buf[0]=0;
	}
	else if ( strcmp("TYPE", buf) == 0 ) {
	    fscanf(tsp_file, "%s", buf);
	    TRACE ( printf("%s ", buf); );
	    fscanf(tsp_file, "%s", buf);
	    TRACE ( printf("%s\n", buf); );
	    if( strcmp("TSP", buf) != 0 ) {
		fprintf(stderr,"\n Not a TSP instance in TSPLIB format !!\n");
		exit(1);
	    }
	    buf[0]=0;
	}
	else if ( strcmp("TYPE:", buf) == 0 ) {
	    fscanf(tsp_file, "%s", buf);
	    TRACE ( printf("%s\n", buf); );
	    if( strcmp("TSP", buf) != 0 ) {
		fprintf(stderr,"\n Not a TSP instance in TSPLIB format !!\n");
		exit(1);
	    }
	    buf[0]=0;
	}
	else if( strcmp("DIMENSION", buf) == 0 ){
	    fscanf(tsp_file, "%s", buf);
	    TRACE ( printf("%s ", buf); );
	    fscanf(tsp_file, "%ld", &n);
	    TRACE ( printf("%ld\n", n); );
	    assert ( n > 2 && n < 6000);
	    buf[0]=0;
	}
	else if ( strcmp("DIMENSION:", buf) == 0 ) {
	    fscanf(tsp_file, "%ld", &n);
	    TRACE ( printf("%ld\n", n); );
	    assert ( n > 2 && n < 6000);
	    buf[0]=0;
	}
	else if( strcmp("DISPLAY_DATA_TYPE", buf) == 0 ){
	    fgets(buf, LINE_BUF_LEN, tsp_file);
	    TRACE ( printf("%s", buf); );
	    buf[0]=0;
	}
	else if ( strcmp("DISPLAY_DATA_TYPE:", buf) == 0 ) {
	    fgets(buf, LINE_BUF_LEN, tsp_file);
	    TRACE ( printf("%s", buf); );
	    buf[0]=0;
	}
	else if( strcmp("EDGE_WEIGHT_TYPE", buf) == 0 ){
	    buf[0]=0;
	    fscanf(tsp_file, "%s", buf);
	    TRACE ( printf("%s ", buf); );
	    buf[0]=0;
	    fscanf(tsp_file, "%s", buf);
	    TRACE ( printf("%s\n", buf); );
	    if ( strcmp("EUC_2D", buf) != 0 && strcmp("CEIL_2D", buf) != 0 && strcmp("GEO", buf) != 0 && strcmp("ATT", buf) != 0) {
		fprintf(stderr,"EDGE_WEIGHT_TYPE %s not implemented\n",buf);
            }
	    strcpy(edge_weight_type, buf);
	    buf[0]=0;
	}
	else if( strcmp("EDGE_WEIGHT_TYPE:", buf) == 0 ){
	    /* set pointer to appropriate distance function; has to be one of 
	       EUC_2D, CEIL_2D, GEO, or ATT. Everything else fails */
	    buf[0]=0;
	    fscanf(tsp_file, "%s", buf);
	    TRACE ( printf("%s\n", buf); );
		printf("%s\n", buf);
	    printf("%s\n", buf);
	    if ( strcmp("EUC_2D", buf) != 0 && strcmp("CEIL_2D", buf) != 0 && strcmp("GEO", buf) != 0 && strcmp("ATT", buf) != 0) {
		fprintf(stderr,"EDGE_WEIGHT_TYPE %s not implemented\n",buf);
		exit(1);
	    }
	    strcpy(edge_weight_type, buf);
	    buf[0]=0;
	}
	buf[0]=0;
	fscanf(tsp_file,"%s", buf);
    }


    if( strcmp("NODE_COORD_SECTION", buf) == 0 ){
	TRACE ( printf("found section contaning the node coordinates\n"); )
    } else{
	fprintf(stderr,"\n\nSome error ocurred finding start of coordinates from tsp file !!\n");
	exit(1);
    }

    if( (nodeptr = (point *) malloc(sizeof(struct point) * n)) == NULL )
	exit(EXIT_FAILURE);
    else {
	for ( i = 0 ; i < n ; i++ ) {
	    fscanf(tsp_file,"%ld %lf %lf", &j, &nodeptr[i].x, &nodeptr[i].y );
	}
    }
    printf(" number of cities is %ld\n",n);

    // Compute distances
    distance = compute_distances();

    printf("... done\n\n"); 

};

/* Destructor*/
TSP::~TSP (){
    printf("TSP destructor.");

    free( distance );
    distance =NULL;
    free( nodeptr );
    nodeptr=NULL;
    //free( nn_list );
};


long int TSP::round_distance (long int i, long int j) 
/*    
      FUNCTION: compute Euclidean distances between two nodes rounded to next 
                integer for TSPLIB instances
      INPUT:    two node indices
      OUTPUT:   distance between the two nodes
      COMMENTS: for the definition of how to compute this distance see TSPLIB
*/
{
    double xd = nodeptr[i].x - nodeptr[j].x;
    double yd = nodeptr[i].y - nodeptr[j].y;
    double r  = sqrt(xd*xd + yd*yd) + 0.5;

    return (long int) r;
}

long int TSP::ceil_distance (long int i, long int j) 
/*    
      FUNCTION: compute ceiling distance between two nodes rounded to next 
                integer for TSPLIB instances
      INPUT:    two node indices
      OUTPUT:   distance between the two nodes
      COMMENTS: for the definition of how to compute this distance see TSPLIB
*/
{
    double xd = nodeptr[i].x - nodeptr[j].x;
    double yd = nodeptr[i].y - nodeptr[j].y;
    double r  = sqrt(xd*xd + yd*yd) + 0.000000001;

    return (long int)r;
}

long int TSP::geo_distance (long int i, long int j) 
/*    
      FUNCTION: compute geometric distance between two nodes rounded to next 
                integer for TSPLIB instances
      INPUT:    two node indices
      OUTPUT:   distance between the two nodes
      COMMENTS: adapted from concorde code
                for the definition of how to compute this distance see TSPLIB
*/
{
    double deg, min;
    double lati, latj, longi, longj;
    double q1, q2, q3;
    long int dd;
    double x1 = nodeptr[i].x, x2 = nodeptr[j].x, 
	y1 = nodeptr[i].y, y2 = nodeptr[j].y;

    deg = dtrunc (x1);
    min = x1 - deg;
    lati = M_PI * (deg + 5.0 * min / 3.0) / 180.0;
    deg = dtrunc (x2);
    min = x2 - deg;
    latj = M_PI * (deg + 5.0 * min / 3.0) / 180.0;

    deg = dtrunc (y1);
    min = y1 - deg;
    longi = M_PI * (deg + 5.0 * min / 3.0) / 180.0;
    deg = dtrunc (y2);
    min = y2 - deg;
    longj = M_PI * (deg + 5.0 * min / 3.0) / 180.0;

    q1 = cos (longi - longj);
    q2 = cos (lati - latj);
    q3 = cos (lati + latj);
    dd = (int) (6378.388 * acos (0.5 * ((1.0 + q1) * q2 - (1.0 - q1) * q3)) + 1.0);
    return dd;

}

long int TSP::att_distance (long int i, long int j) 
/*    
      FUNCTION: compute ATT distance between two nodes rounded to next 
                integer for TSPLIB instances
      INPUT:    two node indices
      OUTPUT:   distance between the two nodes
      COMMENTS: for the definition of how to compute this distance see TSPLIB
*/
{
    double xd = nodeptr[i].x - nodeptr[j].x;
    double yd = nodeptr[i].y - nodeptr[j].y;
    double rij = sqrt ((xd * xd + yd * yd) / 10.0);
    double tij = dtrunc (rij);
    long int dij;

    if (tij < rij)
        dij = (int) tij + 1;
    else
        dij = (int) tij;
    return dij;
}


long int ** TSP::compute_distances(void)
/*    
      FUNCTION: computes the matrix of all intercity distances
      INPUT:    none
      OUTPUT:   pointer to distance matrix, has to be freed when program stops
*/
{
    long int     i, j;
    long int     **matrix;

    if((matrix = (long int **) malloc(sizeof(long int) * n * n +
			sizeof(long int *) * n	 )) == NULL){
	fprintf(stderr,"Out of memory, exit.");
	exit(1);
    }
    for ( i = 0 ; i < n ; i++ ) {
	matrix[i] = (long int *)(matrix + n) + i*n;
	for ( j = 0  ; j < n ; j++ ) {
	    matrix[i][j] = compute_distance(i, j);
	}
    }
    return matrix;
}

long int TSP::compute_distance(long int i, long int j){
    if ( strcmp("EUC_2D", edge_weight_type) == 0 ) {
         return(round_distance(i,j));
    }
    else if ( strcmp("CEIL_2D", edge_weight_type) == 0 ) {
         return(ceil_distance(i,j));
    }
    else if ( strcmp("GEO", edge_weight_type) == 0 ) {
	 return(geo_distance(i,j));
    }
    else if ( strcmp("ATT", edge_weight_type) == 0 ) {
	 return(att_distance(i,j));
    }
    return(-1);

}

void TSP::printDistance(void) {
  long int i,j;

  printf("Distance Matrix:\n");
  for ( i = 0 ; i < n ; i++) {
    printf("From %ld:  ",i);
    for ( j = 0 ; j < n - 1 ; j++ ) {
      printf(" %ld", distance[i][j]);
    }
    printf(" %ld\n", distance[i][n-1]);
    printf("\n");
  }
  printf("\n");
}

double TSP::dtrunc (double x){
    int k;
    k = (int) x;
    x = (double) k;
    return x;
}

/* Return the number of cities*/
long int TSP::getSize(){
   return(n);
}

/* Get distance between cities i and j*/
long int TSP::getDistance(long int i, long int j){
   if(i>=n || i<0 || j>=n || j<0 ){
      printf("ERROR accessing distance.\n");
      exit(1);
   }     
   return(distance[i][j]); 
}

