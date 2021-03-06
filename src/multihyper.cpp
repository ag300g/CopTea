#include "CopTea.h"
#include <Rmath.h>

SEXP RMultiHyper(SEXP _N, SEXP _K, SEXP _M)
{
  int N = INTEGER_POINTER(AS_INTEGER(_N))[0];
  int K = INTEGER_POINTER(AS_INTEGER(_K))[0];

  PROTECT(_M = AS_INTEGER(_M));
  int *M = INTEGER_POINTER(_M);
  int nM = length(_M);

  SEXP _X;
  PROTECT(_X = NEW_INTEGER(nM * N));
  setDim2(_X, nM, N);
  int *X = INTEGER_POINTER(_X);

  int sumM = 0;
  for (int i = 0; i < nM; i++)
      sumM += M[i];

  GetRNGstate();
  for (int n = 0; n < N; n++)
  {
    int restM = sumM;
    int restK = K;
    for (int i = 1; i < nM; i++)
    {
      restM -= M[i];
      X[i] = rhyper(M[i], restM, restK);
      restK -= X[i];
    }
    X[0] = restK;
    X += nM;
  }
  PutRNGstate();
  
  UNPROTECT(2);
  return (_X);
}

double pmultihyper(double X, int K, int nM, int tM, int *M, double *W)
{
  double P;
  if (K > tM) P = 0;
  else if (X <= 0) P = 1;
  else if (K * W[0] < X) P = 0;
  else if (nM == 1) P = 1;
  else
  {
    int Z1 = ceil(X / W[0]) - 1;
    int Z2 = (W[0] > W[1]) ? ceil((X - K * W[1]) / (W[0] - W[1])) : 0;
    if (Z2 < 0) Z2 = 0;
    int tM0 = tM - M[0];
    P = phyper(Z1, M[0], tM0, K, 0, 0);
    for (int i = Z1; i >= Z2; i--)
    {
      P += dhyper(i, M[0], tM0, K, 0) * pmultihyper(X-i*W[0], K-i, nM-1, tM0, M+1, W+1);
    }
  }
  return(P);
}

SEXP PMultiHyper(SEXP _X, SEXP _K, SEXP _M, SEXP _W)
{
  double X = NUMERIC_POINTER(AS_NUMERIC(_X))[0];
  int K = INTEGER_POINTER(AS_INTEGER(_K))[0];

  PROTECT(_M = AS_INTEGER(duplicate(_M)));
  int *M = INTEGER_POINTER(_M);
  int nM = length(_M);
  
  PROTECT(_W = AS_NUMERIC(duplicate(_W)));
  double *W = NUMERIC_POINTER(_W);

	revsort(W, M, nM);

  int tM = 0;
  for (int i = 0; i < nM; i++)
  {
    tM += M[i];
  }

  SEXP _P;
  PROTECT(_P = NEW_NUMERIC(1));
  double *P = NUMERIC_POINTER(_P);
 
  *P = pmultihyper(X, K, nM, tM, M, W);

  UNPROTECT(3);
  return(_P);
}

double pmultinom(double X, int K, int nM, int tM, int *M, double *W)
{
  double P;
  if (K > tM) P = 0;
  else if (X <= 0) P = 1;
  else if (K * W[0] < X) P = 0;
  else if (nM == 1) P = 1;
  else
  {
    int Z1 = ceil(X / W[0]) - 1;
    int Z2 = (W[0] > W[1]) ? ceil((X - K * W[1]) / (W[0] - W[1])) : 0;
    if (Z2 < 0) Z2 = 0;
    double prob = (double) M[0] / tM;
    P = pbinom(Z1, K, prob, 0, 0);
    for (int i = Z1; i >= Z2; i--)
    {
      P += dbinom(i, K, prob, 0) * pmultinom(X-i*W[0], K-i, nM-1, tM-M[0], M+1, W+1);
    }
  }
  return(P);
}

SEXP PMultiNom(SEXP _X, SEXP _K, SEXP _M, SEXP _W)
{
  double X = NUMERIC_POINTER(AS_NUMERIC(_X))[0];
  int K = INTEGER_POINTER(AS_INTEGER(_K))[0];

  PROTECT(_M = AS_INTEGER(duplicate(_M)));
  int *M = INTEGER_POINTER(_M);
  int nM = length(_M);
  
  PROTECT(_W = AS_NUMERIC(duplicate(_W)));
  double *W = NUMERIC_POINTER(_W);

  revsort(W, M, nM);

  int tM = 0;
  for (int i = 0; i < nM; i++)
  {
    tM += M[i];
  }

  SEXP _P;
  PROTECT(_P = NEW_NUMERIC(1));
  double *P = NUMERIC_POINTER(_P);
 
  *P = pmultinom(X, K, nM, tM, M, W);

  UNPROTECT(3);
  return(_P);
}
