#!/bin/bash

#USAGE: ./test-ants.sh instance folder

#inputs
instance=$1
folder=$2

rm -rf ${folder}/data-ants.txt

echo "trial:m5:m10:m20:m50:m100" >> ${folder}/data-ants.txt

#Set initial seed for different trials
seed=1234
for i in {1..20}; do
  AUX="${i}" #number of trial
  for m in {5,10,20,50,100}; do
    #Execute the algorithm with the seed and the parameter and save the output file in a folder.
    ./../template_c_plus/aco --instance ${instance} --rho 0.5 --tours 500 --ants ${m} --seed ${seed}  >  ${folder}/output-m${m}-${seed}.txt

    #Read the output file and extract the best tour found
    SOL=$(cat ${folder}/output-m${m}-${seed}.txt | grep -o -E 'Best [-+0-9.e]+' | cut -d ' ' -f2)
    
    #Save for printing
    AUX="${AUX}:${SOL}"
  done
  #Print matrix of best tour found
  echo "$AUX" >>  ${folder}/data-ants.txt
  #Next seed
  seed=$(( seed + 1 ))
done

#Remove any unuseful file.....
rm -rf stat* cmp* best* ./${folder}/output*
