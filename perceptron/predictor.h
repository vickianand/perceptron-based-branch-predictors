/*
  An implementation of the algorithm given in the paper -
    "Dynamic Branch Prediciton with Perceptrons"
      by Daniel A. Jimenez  and  Calvin Lin
  
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

// As given in paper
// formula for calculating THETA : floor(1.93*h + 14)
// History lengths taken from Table-1 of the paper
#if BUDGET == 4
  #define LEN_GHR 28
  #define THETA 68          // => 8 bits needed for weights
  #define TABLE_SIZE 141    // (4*8*1024)/(28+1)*8
#elif BUDGET == 8
  #define LEN_GHR 34
  #define THETA 79          //  => 8 bits needed for weights
  #define TABLE_SIZE 234    // (8*8*1024)/(34+1)*8
#elif BUDGET == 32
  #define LEN_GHR 59
  #define THETA 127         //  => 8 bits needed for weights
  #define TABLE_SIZE 546    //  (32*8*1024)/(59+1)*8
#else
  #define LEN_GHR 0
  #define THETA 0
  #define TABLE_SIZE 0
#endif

int debug_n_predictions = 0;
int temp;
bool ghr[LEN_GHR + 1];                  // 0th bit always set to 1 and never changed because w0 is going to be the bias.
                                        // x0, x1, x2, ..., xn
int weights[TABLE_SIZE][LEN_GHR + 1];   // w0, w1, w2, ..., wn

/////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////

class PREDICTOR
{

  public:

  PREDICTOR(void)
  {
    ghr[0] = 1;
    for(int i=1; i<=LEN_GHR; i++)
      ghr[i] = 0;              // here 0 is for -1

    for(int i=0; i < TABLE_SIZE; i++)
      for(int j=0; j <= LEN_GHR; j++)
        weights[i][j] = 0;

    printf("\nBUDGET = %d, \nLEN_GHR = %d, TABLE_SIZE = %d, \nTHETA = %d\n\n", BUDGET, LEN_GHR, TABLE_SIZE, THETA);

  }

  // We have 8 bits keeping for all the integers
  // So we keep our weights between -128 t0 127
  int sat_incr(int w)
  {
    if (w < 127)
      return w+1;
    else
    {
      assert(w == 127);
      return w;
    }
  }

  int sat_decr(int w)
  {
    if (w > -128)
      return w-1;
    else
    {
      assert(w == -128);
      return w;
    }
  }

  bool GetPrediction(UINT32 PC)
  {
    PC = PC%TABLE_SIZE;
    temp = 0;
    for(int i=0; i<=LEN_GHR; i++)
    {
      // if (debug_n_predictions) {printf("temp -> %d\n", temp);}
      if(ghr[i] == 0)
      {
        temp -= weights[PC][i];
      }
      else if (ghr[i] == 1)
      {
        // printf("befor assert, i = %d, ghr[i] = %d\n", i, ghr[i]);
        temp += weights[PC][i];
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
    PC = PC%TABLE_SIZE;
    temp = 0;
    for(int i=0; i<=LEN_GHR; i++)
    {
      // if (debug_n_predictions) {printf("temp -> %d\n", temp);}
      if(ghr[i] == 0)
      {
        assert(ghr[i] == 0);
        temp -= weights[PC][i];
      }
      else if(ghr[i] == 1)
      {
        assert(ghr[i] == 1);
        temp += weights[PC][i];
      } 
      else assert(false);
    }

    assert(predDir == (temp >= 0));

    if(temp < 0)
      { temp = -temp; }   // temp = absolute(temp)

    assert(temp >= 0);      // absolute(temp) must be positive.

    if((temp <= THETA) || (resolveDir != predDir))
    {
      for(int i=0; i <= LEN_GHR; i++)
      {
        if(resolveDir == ghr[i])
          {
            // weights[PC][i] += 1;
            weights[PC][i] = sat_incr(weights[PC][i]);
          }
        else
          {
            // weights[PC][i] -= 1;
            weights[PC][i] = sat_decr(weights[PC][i]);
          }
      }
    }

    for(int i=2; i<=LEN_GHR; i++)
      {
        ghr[i-1] = ghr[i];
      }
    ghr[LEN_GHR] = resolveDir;
  }

  void TrackOtherInst(UINT32 PC, OpType opType, UINT32 branchTarget)
  {

  }


};



/***********************************************************/
#endif

