/*
 *       $Id$
 *
 *       This source code is part of
 *
 *        G   R   O   M   A   C   S
 *
 * GROningen MAchine for Chemical Simulations
 *
 *            VERSION 2.0
 * 
 * Copyright (c) 1991-1997
 * BIOSON Research Institute, Dept. of Biophysical Chemistry
 * University of Groningen, The Netherlands
 * 
 * Please refer to:
 * GROMACS: A message-passing parallel molecular dynamics implementation
 * H.J.C. Berendsen, D. van der Spoel and R. van Drunen
 * Comp. Phys. Comm. 91, 43-56 (1995)
 *
 * Also check out our WWW page:
 * http://rugmd0.chem.rug.nl/~gmx
 * or e-mail to:
 * gromacs@chem.rug.nl
 *
 * And Hey:
 * GRowing Old MAkes el Chrono Sweat
 */

#ifndef _ghat_h
#define _ghat_h

static char *SRCID_ghat_h = "$Id$";

#include <stdio.h>
#include "typedefs.h"
	
extern void symmetrize_ghat(int nx,int ny,int nz,real ***ghat);
/* Symmetrize the Ghat function. It is assumed that the 
 * first octant of the Ghat function is either read or generated
 * (all k-vectors from 0..nx/2 0..ny/2 0..nz/2).
 * Since Gk depends on the absolute value of k only, 
 * symmetry operations may shorten the time to generate it.
 */
 
extern void mk_ghat(FILE *fp,int nx,int ny,int nz,real ***ghat,
		    rvec box,real r1,real rc,bool bSym);
/* Generate a Ghat function from scratch. The ghat grid should
 * be allocated using the mk_rgrid function. When bSym, only
 * the first octant of the function is generated by direct calculation
 * and the above mentioned function is called for computing the rest.
 */

extern real ***rd_ghat(FILE *log,char *fn,ivec igrid,rvec gridspacing,
		       rvec beta,int *porder,real *rshort,real *rlong);
/* Read a Ghat function from a file as generated by the program
 * mk_ghat. The grid size (number of grid points) is returned in
 * igrid, the space between grid points in gridspacing,
 * beta is a constant that determines the contribution of first
 * and second neighbours in the grid to the force
 * (See Luty et al. JCP 103 (1995) 3014)
 * porder determines whether 8 (when porder = 1) or 27 (when
 * porder = 2) neighbouring grid points are used for spreading
 * the charge.
 * rshort and rlong are the lengths used for generating the Ghat
 * function.
 */
		       
#endif
