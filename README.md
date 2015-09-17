Setting up the CBP-4 infrastructure and simulating the given predictors
========================================================================
*	The download file and setting-up instruction for CBP-4 Kit is available at site : http://www.jilp.org/cbp2014/framework.html
*	There are predictor.cc and predictor.h files in all three directories - `gshare`, `perceptron`, and `piecewise_linear`.
*	For simulating the given predictors, you'll need to replace the predictor.cc and predictor.h file in `cbp2014/sim/` directory by those from the directory of the required predictor.
*	In `cbp2014/sim/` direcory execute : `rm *.o` and `rm predictor` followed by `make`. This will create the executable of the required predictor.
*	After this, for running the predictors and generating results, the instructions given on the CBP-4 site should be followed.

Changing the budgets for the predictors
=========================================
*	The storage budgets for the both the predictors can be changed in respective .h files by changing the `#define Budget 4` line.
*	The budgets for the perceptron predictor can be set to 4 or 8 or 32. 
*	The budgets for the piece-wise linear predictor can be set to 4 or 8 or 32 or 256. 
*	All the above budgets represent the budget in KB.
*	The required calculations as per the budget for different parameters have been shown in comments alongside the initializations of the parameters.
*	The included gshare directory is the one provided by default with the cbp2014 kit.