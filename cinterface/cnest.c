#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define M_PI 3.14

extern void __nested_MOD_nestrun(
	int *mmodal, int *ceff, int *nlive, double *tol, double *efr, int *ndims,
	int *nPar, int *nClsPar, int *maxModes, int *updInt, double *Ztol, 
	char *root, int *seed, int *pWrap, int *fb, int *resume, 
	void (*Loglike)(double *Cube, int *n_dim, int *n_par, double *lnew), 
	int *context);

struct problem {
	void (*Prior)(double *Cube, int n_dim, int n_par);
	double (*LogLike)(double *Cube, int n_dim, int n_par);
} p;

void Prior_eggbox(double *Cube, int ndim, int npars)
{
	for(int i = 0; i < ndim; i++)
	{
		Cube[i] *= 10.0 * M_PI;
	}
}

double LogLike_eggbox(double *Cube, int ndim, int npars)
{
	double chi = 1.0;
	for(int i = 0; i < ndim; i++)
		chi *= cos(Cube[i]/2.0);
	return powf(chi + 2.0, 5.0);
}


void _LogLike(double *Cube, int *ndim, int *npars, double *lnew)
{
	//printf("ndim: %d, npars: %d\n", *ndim, *npars);
	//printf("values before: %f, %f\n", Cube[0], Cube[1]);
	p.Prior(Cube, *ndim, *npars);
	//printf("values after : %f, %f\n", Cube[0], Cube[1]);
	*lnew = p.LogLike(Cube, *ndim, *npars);
	//printf("loglikelihood: %f\n", *lnew);
}

void run(
	int mmodal, int ceff, int nlive, double tol, double efr, int ndims,
	int nPar, int nClsPar, int maxModes, int updInt, double Ztol, 
	char *rootstr, int seed, int * pWrap, int fb, int resume, int context,
	void (*Prior)(double *Cube, int n_dim, int n_par),
	double (*LogLike)(double *Cube, int n_dim, int n_par)
)
{
	char root[100];
	p.Prior = Prior;
	//p.Prior = Prior_eggbox;
	p.LogLike = LogLike;
	//p.LogLike = LogLike_eggbox;

	/* filling root with spaces, because fortran likes that */
	strcpy(root, rootstr);
	memset(root + strlen(root), ' ', 100 - strlen(root));

	/* running MultiNest */
	__nested_MOD_nestrun(&mmodal, &ceff, &nlive, &tol, &efr, &ndims, 
	&nPar, &nClsPar, &maxModes, &updInt, &Ztol,
	root, &seed, pWrap, &fb, &resume, _LogLike, &context);
}

