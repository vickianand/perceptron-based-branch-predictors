/*
  An implementation of the algorithm given by Daniel A. Jimenez
  in his paper "Piecewise Linear Branch Prediction"

  written by : Vicki Anand
  as a part of course project for CS422A course at IITK
  April 2015
*/


#ifndef _PREDICTOR_H_
#define _PREDICTOR_H_

#include "utils.h"
#include "tracer.h"
#include <assert.h>

#define BUDGET 4 // in KiloBytes

// Tuned history lengths for predictors and values of N, M 
// As given in paper (based on empirical results)
// H, N, M : Taken from Table-2 of the paper
// given formula for calculating THETA : floor(2.14*(H+1) + 20.58)
#if BUDGET == 4
  #define H 19
  #define N 1
  #define M 215
  #define THETA 63          
#elif BUDGET == 8
  #define H 19
  #define N 2
  #define M 176
  #define THETA 63
#elif BUDGET == 32
  #define H 26
  #define N 8
  #define M 118
  #define THETA 76
#elif BUDGET == 256
  #define H 51
  #define N 8
  #define M 603
  #define THETA 131
#else
  #define H 0
  #define N 0
  #define M 0
  #define THETA 0
#endif

bool ghr[H+1];     // 0th bit always set to 1 and never changed because w0 is going to be the bias.
                           // x0, x1, x2, ..., xn

int GA[H+1];  // An array of addresses mod M. As branches are executed, there addresses are shifted into the first position of this array.
              // we won't use the zeroth bit
// GHR and GA gives the path history for current branch to be predicted.

int W[N][M][H+1];   // w0, w1, w2, ..., wn
                                    // W keeps track of correlations for every branch in the program.
                                    // think of W as a set of matrices, one for each branch
                                    // Addition and substraction on elements of W saturates at +127 and -128

int temp;       // This integer is the value of linear function computed to predict the branch.

int debug_n_predictions = 0;      // used for debugging purpose.

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

class PREDICTOR
{

  public:

  PREDICTOR(void)
  {
    ghr[0] = 1;
    for(int i=1; i<=H; i++)
      ghr[i] = 0;              // here 0 is for -1

    for(int i=0; i<=H; i++)
      GA[i] = 0;

    for(int i=0; i<N; i++)
      for(int j=0; j<M; j++)
        for(int k=0; k<=H; k++)
          W[i][j][k] = 0;

    printf("\nBUDGET = %d, \nLEN_GHR = %d, N = %d, \nTHETA = %d\n\n", BUDGET, H, N, THETA);

  }

  int sat_incr(int i)   // Addition and substraction on elements of W saturates at +127 and -128
  {
    if (i < 127)
      return i+1;
    else
    {
      assert(i == 127);
      return i;
    }
  }

  int sat_decr(int i)   // Addition and substraction on elements of W saturates at +127 and -128
  {
    if (i > -128)
      return i-1;
    else
    {
      assert(i == -128);
      return i;
    }
  }

  bool GetPrediction(UINT32 PC)
  {
    PC = PC%N;
    temp = W[PC][0][0];
    for(int i=1; i<=H; i++)
    {
      // if (debug_n_predictions) {printf("temp -> %d\n", temp);}
      if(ghr[i] == 0)
      {
        temp -= W[PC][GA[i]][i];
      }
      else if (ghr[i] == 1)
      {
        temp += W[PC][GA[i]][i];
      }
      else assert(false);
    }

    if(temp >= 0)
    {
      if (debug_n_predictions){printf("true -> %d\n", temp); debug_n_predictions -= 1;}
      return true;
    }
    else
    {
      if (debug_n_predictions){printf("false -> %d\n", temp); debug_n_predictions -= 1;}
      return false;
    }
  }  

  void UpdatePredictor(UINT32 PC, bool resolveDir, bool predDir, UINT32 branchTarget)
  {
    temp = W[PC%N][0][0];
    for(int i=1; i<=H; i++)
    {
      // if (debug_n_predictions) {printf("temp -> %d\n", temp);}
      if(ghr[i] == 0)
      {
        temp -= W[PC%N][GA[i]][i];
      }
      else if (ghr[i] == 1)
      {
        temp += W[PC%N][GA[i]][i];
      }
      else assert(false);
    }

    assert(predDir == (temp >= 0));

    if(temp < 0)
      { temp = -temp; }   // temp = absolute(temp)

    assert(temp >= 0);      // absolute(temp) must be positive.

    if((temp < THETA) || (resolveDir != predDir))
    {
      if(resolveDir)
        W[PC%N][0][0] += 1;
      else
        W[PC%N][0][0] -= 1;

      for(int i=1; i <= H; i++)
      {
        if(resolveDir == ghr[i])
          {
            // W[PC%N][GA[i]][i] += 1;
            W[PC%N][GA[i]][i] = sat_incr(W[PC%N][GA[i]][i]);
          }
        else
          {
            // W[PC%N][GA[i]][i] -= 1;
            W[PC%N][GA[i]][i] = sat_decr(W[PC%N][GA[i]][i]);
          }
      }
    }

    for(int i=2; i<=H; i++)
      {
        ghr[i-1] = ghr[i];
        GA[i-1] = GA[i];
      }
    ghr[H] = resolveDir;
    GA[H] = PC%M;
  }

  void TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget)
  {

  }


};



/***********************************************************/
#endif

