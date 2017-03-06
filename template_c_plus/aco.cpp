
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <vector>

#include "ant.cpp"


using namespace std;

char * instance_file=NULL;
TSP* tsp;

/*Probabilistic rule related variables*/
double  ** pheromone;   /* pheromone matrix */
double  ** heuristic;   /* heuristic information matrix */
double  ** probability;    /* combined value of pheromone X heuristic information */
double initial_pheromone=1.0;

long int max_iterations;   //Max iterations
long int iterations=0;           
long int max_tours;        //Maz tours
long int tours=0;
double alpha;
double beta;
double rho;
long int n_ants;   
long int seed = -1;

vector<Ant> colony;
Ant * best_ant;
long int best_tour_length=LONG_MAX;     /* length of the shortest tour found */

/*Default parameters: set them!*/
void setDefaultParameters(){
   alpha=1;
   beta=1;
   rho=0.2;
   n_ants=10;
   max_iterations=-1;
   max_tours=10000;
   instance_file=NULL;
   seed=seed = (long int) time(NULL);
}

/*Print default parameters*/
void printParameters(){
   cout << "\nACO parameters:\n"
             << "  ants: "  << n_ants << "\n" 
             << "  alpha: " << alpha << "\n"
             << "  beta: "  << beta << "\n"
             << "  rho: "   << rho << "\n"
             << "  tours: " << max_tours << "\n"
             << "  iterations: "   << max_iterations << "\n"
             << "  seed: "   << seed << "\n"
             << "  initial pheromone: "   << initial_pheromone << "\n"
             << endl;
}

void printHelp(){
   cout << "\nACO Usage:\n"
        << "   ./aco --ants <int> --alpha <float> --beta <float> --rho <float> --tours <int> --iterations <int> --seed <int> --instance <path>\n\n"
        << "Example: ./aco --tours 2000 --seed 123 --instance eil151.tsp\n\n"
        << "\nACO flags:\n"
             << "   --ants: Number of ants to build every iteration. Default=10.\n" 
             << "   --alpha: Alpha parameter (float). Default=1.\n"
             << "   --beta: Beta parameter (float). Default=1.\n"
             << "   --rho: Rho parameter (float). Defaut=0.2.\n"
             << "   --tours: Maximum number of tours to build (integer). Default=10000.\n"
             << "   --iterations: Maximum number of iterations to perform (interger). Default:0 (disabled).\n"
             << "   --seed: Number for the random seed generator.\n"
             << "   --instance: Path to the instance file\n"
        << "\nACO other parameters:\n"  
             << "   initial pheromone: "   << initial_pheromone << "\n"
        << endl;

}


/* Read arguments from command line */ 
bool readArguments(int argc, char *argv[] ){
     
     setDefaultParameters();
    
    for(int i=1; i< argc ; i++){
       if(strcmp(argv[i], "--ants") == 0){
           n_ants = atol(argv[i+1]);
           i++;
       } else if(strcmp(argv[i], "--alpha") == 0){
           alpha = atof(argv[i+1]);
           i++;
       } else if(strcmp(argv[i], "--beta") == 0){
           beta = atof(argv[i+1]);
           i++;
       } else if(strcmp(argv[i], "--rho") == 0) {
           rho = atof(argv[i+1]);
           i++;
       } else if(strcmp(argv[i], "--iterations") == 0) {
           max_iterations = atol(argv[i+1]);
           i++;
       } else if(strcmp(argv[i], "--tours") == 0) {
           max_tours = atol(argv[i+1]);
           i++;
       } else if(strcmp(argv[i], "--seed") == 0) {
           seed = atol(argv[i+1]);
           i++;
       }else if(strcmp(argv[i], "--instance") == 0) {
           instance_file = argv[i+1];
           i++;
       }else if(strcmp(argv[i], "--help") == 0) {
           printHelp();
           return(false);
       }else{
           cout << "Parameter " << argv[i] << "no recognized.\n";
           return(false);
       }
    }
    if(instance_file==NULL){
       cout << "No instance file provided.\n";
       return(false);
    }
    printParameters();
    return(true);
}

/*Create colony structure*/
void createColony (){
    cout << "Creating colony.\n\n";
    for (int i = 0 ; i < n_ants ; i++ ) {
       colony.push_back(Ant(tsp, probability, &seed));
    }
}

/*Initialize pheromone with an initial value*/
void initializePheromone( double initial_value ) {
  long int size = tsp->getSize();

  pheromone = new double * [size];
  for (int i = 0 ; i < size ; i++ ) {
    pheromone[i] = new double [size];
    for (int j = 0  ; j < size ; j++ ) {
      pheromone[i][j] = initial_value;  /* symmetric TSP instances; hence phermone[i][j] = pheromone[j][i] */ 
    }
  }
}

void printPheromone () {
  long int i, j;
  long int size = tsp->getSize();

  printf("\n");
  for ( i = 0 ; i < size ; i++ ) {
    for ( j = 0 ; j < size ; j++ ) {
      printf(" %lf ", pheromone[i][j]);
    }
    printf("\n");
  }
}  

// Initialize the heuristic information matrix 
void initializeHeuristic () {
  long int size = tsp->getSize();
  heuristic = new double * [size];

  for (int i = 0 ; i < size ; i++ ) {
    heuristic[i] = new double [size];
    heuristic[i][i] = 0;
    for (int j = 0 ; j < size ; j++ ) {
    	if (j != i) { heuristic[i][j] = 1.0 / tsp->getDistance(i,j);}
    }
  }
} 

// Calculate probability in base of heuristic
// information and pheromone
void calculateProbability () {
	long int size = tsp->getSize();
	double sum;

	for (int i = 0 ; i<size ; ++i) {
		probability[i][i] = 0;
		for (int j = i+1; j<size; ++j) {
			probability[i][j] = pow(pheromone[i][j], alpha) * pow(heuristic[i][j], beta);
			probability[j][i] = probability[i][j];
		}
	}
}

/*Pheromone evaporation*/
void evaporatePheromone(){
  long int size = tsp->getSize();

  for (int i = 0 ; i < size ; i++ ) {
    for (int j = i  ; j < size ; j++ ) {
      pheromone[i][j] = pheromone[i][j] * (1-rho);  /* symmetric TSP instances; hence phermone[i][j] = pheromone[j][i] */ 
    	pheromone[j][i] = pheromone[i][j];
    }
  }
}

/*Update pheromone*/
void depositPheromone(){
	for(int a = 0; a<n_ants; ++a) {
		colony[a].depositPheromone(pheromone);
	}
}

/*Check termination condition based on iterations or tours.
 One of the criteria must be active (=!0).*/
bool terminationCondition(){
  if (max_iterations < 0) {
    return (tours > max_tours);
  } else {
    return (tours > max_tours) || (iterations > max_iterations);
  }
}

/*Free memory used*/
void freeMemory(){
  for(int i=0; i < tsp->getSize(); i++){
    delete[] pheromone[i];
    delete[] heuristic[i];
    delete[] probability[i];
  }
  delete[] pheromone;
  delete[] heuristic;
  delete[] probability;
  delete tsp;

}

/*This function calls methods that MUST be implemented by you*/
int main(int argc, char *argv[] ){

   if(!readArguments(argc, argv)){
     exit(1);
   }

  tsp= new TSP (instance_file);
  probability = new double * [tsp->getSize()];
  for (int i = 0 ; i < tsp->getSize() ; i++ ) {
    probability[i] = new double [tsp->getSize()];
  }

   initializePheromone(initial_pheromone);
   initializeHeuristic();
   calculateProbability();
   createColony();

   //Iterations loop
   while(!terminationCondition()){
      //build solutions
      for(int i=0; i< n_ants; i++){
         //Construct solution
         colony[i].Search();//to implement

         /*Check for new local optimum*/
				 colony[i].ComputeTourLength();
         if(best_tour_length > colony[i].getTourLength()){
            best_tour_length = colony[i].getTourLength();
            best_ant = new Ant(colony.at(i));
         }
         tours++;
      }

      evaporatePheromone();
      depositPheromone();
      calculateProbability();
      iterations++;
   }
   best_ant->print();
   freeMemory();   // Free memory.
   cout << "\nEnd ACO execution.\n" << endl;
}
