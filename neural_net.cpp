/*
*Author :Tharindra Galahena
*Project:snake game playing AI (neural netwark + Q learning)
*Blog   :www.inf0warri0r.blogspot.com
*Date   :02/09/2012
*License:
* 
*     Copyright 2012 tharindra Galahena
*
* This program is free software: you can redistribute it and/or modifood_y it under the terms of 
* the GNU General Public License as published by the Free Software Foundation, either 
* version 3 of the License, or (at your option) any later version. This program is distributed
* in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied 
* warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General 
* Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program. 
* If not, see http://www.gnu.org/licenses/.
*
*/

#include <cstdlib>
#include <math.h>
#include <iostream>
#include <cstdlib>

typedef struct node{
	int num_inputs;
	float *weights;
	float *inputs;
	float *errors;
	float output;
} node;

typedef struct layer{
	int num_nodes;
	node *chr;
} layer;

using namespace std;

class neural{
	private:
		int num_inputs;
		int num_outputs;
		int num_layers;
		int num_weights;
		int num_hid_nodes;
		float leaning_rate;
		layer *layers;
		float *weights;
		
	public:
		neural();
		neural(int in, int out, int num, int hn, float lrate);		
		void init();	
		int get_num_weights(); 				
		float *get_weights();  				
		void put_weights(float *weights); 	
		float* feed(float *inputs);	
		float get_weighted_error(int l, int in);
		void learn(float *dout);	
		float convert(float input);	
		~neural();	
};

neural :: neural(){
}
neural :: neural(int in, int out, int num, int hn, float lrate){
	num_inputs = in;
	num_outputs = out;
	num_layers = num;
	num_hid_nodes = hn;
	num_weights = 0;
	leaning_rate = lrate;
	layers = (layer *)malloc(sizeof(layer) * num);
	layers[0].num_nodes = in;
	layers[0].chr = (node *)malloc(sizeof(node) * in);
	for(int i = 0; i < in; i++){
		(layers[0].chr[i]).num_inputs = 1;
		num_weights += 1;
		(layers[0].chr[i]).weights = (float *)malloc(sizeof(float) * (1));
		(layers[0].chr[i]).inputs = (float *)malloc(sizeof(float) * (1));
		(layers[0].chr[i]).errors = (float *)malloc(sizeof(float) * (1));
		for(int e = 0; e < 1; e++) (layers[0].chr[i]).errors[e] = 0.0;
	}
	for(int i = 1; i < num - 1; i++){
		layers[i].chr = (node *)malloc(sizeof(node) * hn);
		layers[i].num_nodes = hn;
		int nd = layers[i - 1].num_nodes;
		for(int j = 0; j < hn; j++){
			(layers[i].chr[j]).num_inputs = nd + 1;
			num_weights += nd + 1;
			(layers[i].chr[j]).weights = (float *)malloc(sizeof(float) * (nd + 1));
			(layers[i].chr[j]).inputs = (float *)malloc(sizeof(float) * (nd + 1));
			(layers[i].chr[j]).errors = (float *)malloc(sizeof(float) * (nd + 1));
			for(int e = 0; e < nd + 1; e++) (layers[i].chr[j]).errors[e] = 0.0;
		}
	}
	int nd = layers[num - 2].num_nodes;
	layers[num - 1].num_nodes = out;
	layers[num - 1].chr = (node *)malloc(sizeof(node) * out);
	for(int i = 0; i < out; i++){
		(layers[num - 1].chr[i]).num_inputs = nd + 1;
		num_weights += nd + 1;
		(layers[num - 1].chr[i]).weights = (float *)malloc(sizeof(float) * (nd + 1));
		(layers[num - 1].chr[i]).inputs = (float *)malloc(sizeof(float) * (nd + 1));
		(layers[num - 1].chr[i]).errors = (float *)malloc(sizeof(float) * (nd + 1));
		for(int e = 0; e < nd + 1; e++) (layers[num - 1].chr[i]).errors[e] = 0.0;
	}
	weights = (float *)malloc(sizeof(float) * num_weights);
}

neural :: ~neural(){
	for(int i = 0; i < num_layers; i++){
		for(int j = 0; j < layers[i].num_nodes; j++){
			delete[] (layers[i].chr[j]).weights;
			delete[] (layers[i].chr[j]).inputs;
			delete[] (layers[i].chr[j]).errors;
		}
		delete[] layers[i].chr;
	}
	delete[] layers;
}
void neural :: init(){
	float weights[num_weights];
	for(int i = 0; i < num_weights; i++){
		 weights[i] = (float)rand() / (float)RAND_MAX - 0.5;
	}
	put_weights(weights);
	for(int i = num_layers - 2; i >= 0; i--){
		for(int j = 0; j < layers[i].num_nodes ; j++){
			for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){	
				(layers[i].chr[j]).weights[k] = 1;
			}
		}
	}
}
int neural :: get_num_weights(){
	return num_weights;
}

float* neural :: get_weights(){
	int n = 0;
	for(int i = 0; i < num_layers; i++){
		for(int j = 0; j < layers[i].num_nodes; j++){
			for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){
				weights[n] =  (layers[i].chr[j]).weights[k];
				n++;
			}
		}
	}
	return weights;
}
void neural :: put_weights(float *weights){
	int n = 0;
	for(int i = 0; i < num_layers; i++){
		for(int j = 0; j < layers[i].num_nodes; j++){

			for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){
				(layers[i].chr[j]).weights[k] = weights[n];
				n++;
			}
		}
	}
}
float* neural :: feed(float *inputs){
	int n = 0;
	float *outputs;
	for(int i = 0; i < num_layers; i++){
		outputs = (float *)malloc(sizeof(float) * layers[i].num_nodes + 1);
		for(int j = 0; j < layers[i].num_nodes; j++){
			float sum = 0.0;
			if(i == 0){
				(layers[i].chr[j]).inputs[0] = inputs[j];
				sum = (layers[i].chr[j]).weights[0] * inputs[j];
			}else{
				
				for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){
					(layers[i].chr[j]).inputs[k] = inputs[k];
					sum += (layers[i].chr[j]).weights[k] * inputs[k];
				}
			}
			outputs[j] = sum;
			(layers[i].chr[j]).output = outputs[j];
		}
		outputs[layers[i].num_nodes] = -1.0;
		inputs = outputs;
	}
	return outputs;
}
float neural :: get_weighted_error(int l, int in){
	float sum = 0.0;
	for(int j = 0; j < layers[l].num_nodes; j++){
		float error  = (layers[l].chr[j]).errors[in];
		float weight = (layers[l].chr[j]).weights[in];
		sum += error * weight; 
	}
	return sum;
}
void neural :: learn(float *dout){
	int tmp = num_layers - 1;
	for(int j = 0; j < layers[tmp].num_nodes; j++){
		
		for(int k = 0; k < (layers[tmp].chr[j]).num_inputs; k++){
			(layers[tmp].chr[j]).errors[k] = dout[0];
			(layers[tmp].chr[j]).weights[k] += 
					leaning_rate * (layers[tmp].chr[j]).inputs[k] *
					(layers[tmp].chr[j]).errors[k];
		}
	}
	
	for(int i = num_layers - 2; i >= 0; i--){
		for(int j = 0; j < layers[i].num_nodes ; j++){
			
			float sum = get_weighted_error(i + 1, j); 
			for(int k = 0; k < (layers[i].chr[j]).num_inputs; k++){
				
				(layers[i].chr[j]).errors[k] = sum;		
				(layers[i].chr[j]).weights[k] +=
					leaning_rate * (layers[i].chr[j]).inputs[k] * (layers[i].chr[j]).errors[k];
			}
		}
	}
	
}
