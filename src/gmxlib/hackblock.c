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
 * Gnomes, ROck Monsters And Chili Sauce
 */
static char *SRCID_hackblock_c = "$Id$";

#include <string.h>
#include "hackblock.h"
#include "smalloc.h"
#include "vec.h"
#include "string2.h"

/* these MUST correspond to the enum in hackblock.h */
char *btsNames[ebtsNR] = { "bonds", "angles", "dihedrals", "impropers" };
int btsNiatoms[ebtsNR] = { 2,       3,        4,           4           };

static void free_t_bonded(t_rbonded *rb)
{
  int i;
  
  for (i=0; i<MAXATOMLIST; i++)
    sfree(rb->a[i]);
  sfree(rb->s);
}

static void free_t_bondeds(t_rbondeds *rbs)
{
  int i;
  
  for(i=0; i<rbs->nb; i++)
    free_t_bonded(&rbs->b[i]);
  sfree(rbs->b);
  rbs->b=NULL;
  rbs->nb=0;
}

void free_t_restp(int nrtp, t_restp **rtp)
{
  int i,j;
  
  for(i=0; i<nrtp; i++) {
    sfree((*rtp)[i].resname);
    sfree((*rtp)[i].atom);
    for(j=0; j<(*rtp)[i].natom; j++) {
      sfree(*(*rtp)[i].atomname[j]);
      sfree((*rtp)[i].atomname[j]);
    }
    sfree((*rtp)[i].atomname);
    sfree((*rtp)[i].cgnr);
    for(j=0; j<ebtsNR; j++)
      free_t_bondeds(&(*rtp)[i].rb[j]);
  }
  free(*rtp);
}

void free_t_hack(int nh, t_hack **h)
{
  int i, j;
  
  for(i=0; i<nh; i++) {
    sfree((*h)[i].oname);
    sfree((*h)[i].nname);
    sfree((*h)[i].atom);
    for(j=0; j<4; j++)
      sfree((*h)[i].a[j]);
  }
  sfree(*h);
  *h=NULL;
}

void free_t_hackblock(int nhb, t_hackblock **hb)
{
  int i, j;
  
  for(i=0; i<nhb; i++) {
    sfree((*hb)[i].name);
    free_t_hack((*hb)[i].nhack, &(*hb)[i].hack);
    for(j=0; j<ebtsNR; j++)
      free_t_bondeds(&(*hb)[i].rb[j]);
  }
  sfree(*hb);
}

void clear_t_hackblock(t_hackblock *hb)
{
  int i;
  
  hb->nhack  = 0;
  hb->maxhack= 0;
  hb->hack   = NULL;
  for(i=0; i<ebtsNR; i++) {
    hb->rb[i].nb=0;
    hb->rb[i].b=NULL;
  }
}

void clear_t_hack(t_hack *hack)
{
  int i;
  
  hack->nr    = 0;
  hack->oname = NULL;
  hack->nname = NULL;
  hack->atom  = NULL;
  hack->cgnr  = NOTSET;
  hack->tp    = 0;
  for(i=0; i<4; i++)
    hack->a[i]  = NULL;
  for(i=0; i<DIM; i++)
    hack->newx[i] = NOTSET;
}

#define safe_strdup(str) (str) ? strdup(str) : NULL

static void copy_t_rbonded(t_rbonded *s, t_rbonded *d)
{
  int i;
  
  for(i=0; i<MAXATOMLIST; i++)
    d->a[i] = safe_strdup(s->a[i]);
  d->s = safe_strdup(s->s);
}

void merge_t_bondeds(t_rbondeds s[], t_rbondeds d[])
{
  int i, j;
  
  for(i=0; i < ebtsNR; i++)
    if ( s[i].nb > 0 ) {
      /* make space */
      srenew(d[i].b, d[i].nb + s[i].nb);
      for(j=0; j < s[i].nb; j++)
	copy_t_rbonded(&s[i].b[j], &d[i].b[ d[i].nb + j ]);
      d[i].nb += s[i].nb;
    }
}

void copy_t_restp(t_restp *s, t_restp *d)
{
  int i;
  
  *d = *s;
  d->resname = safe_strdup(s->resname);
  snew(d->atom, s->natom);
  for(i=0; i < s->natom; i++)
    d->atom[i] = s->atom[i];
  snew(d->atomname, s->natom);
  for(i=0; i < s->natom; i++) {
    snew(d->atomname[i],1);
    *d->atomname[i] = safe_strdup(*s->atomname[i]);
  }
  snew(d->cgnr, s->natom);
  for(i=0; i < s->natom; i++)
    d->cgnr[i] = s->cgnr[i];
  for(i=0; i < ebtsNR; i++) {
    d->rb[i].nb = 0;
    d->rb[i].b = NULL;
  }
  merge_t_bondeds(s->rb, d->rb);
}

void copy_t_hack(t_hack *s, t_hack *d)
{
  int i;
  
  *d = *s;
  d->oname = safe_strdup(s->oname);
  d->nname = safe_strdup(s->nname);
  if (s->atom) {
    snew(d->atom, 1);
    *(d->atom) = *(s->atom);
  } else
    d->atom = NULL;
  for(i=0; i<4; i++)
    d->a[i] = safe_strdup(s->a[i]);
  copy_rvec(s->newx, d->newx);
}

void merge_hacks_lo(int ns, t_hack *s, int *nd, t_hack **d)
{
  int i;
  
  if (ns) {
    srenew(*d, *nd + ns);
    for(i=0; i < ns; i++)
      copy_t_hack(&s[i], &(*d)[*nd + i]);
    (*nd) += ns;
  }
}

void merge_hacks(t_hackblock *s, t_hackblock *d)
{
  merge_hacks_lo(s->nhack, s->hack, &d->nhack, &d->hack);
}

void merge_t_hackblock(t_hackblock *s, t_hackblock *d)
{
  merge_hacks(s, d);
  merge_t_bondeds(s->rb, d->rb);
}

void copy_t_hackblock(t_hackblock *s, t_hackblock *d)
{
  int i;
  
  *d       = *s;
  d->name  = safe_strdup(s->name);
  d->nhack = 0;
  d->hack  = NULL;
  for(i=0; i<ebtsNR; i++) {
    d->rb[i].nb=0;
    d->rb[i].b=NULL;
  }
  merge_t_hackblock(s, d);
}

#undef safe_strdup

void dump_hb(FILE *out, int nres, t_hackblock hb[])
{
  int i,j,k,l;
  
#define SS(s) (s) ? (s) : "-"
#define SA(s) (s) ? "+" : ""
  fprintf(out,"t_hackblock\n");
  for(i=0; i<nres; i++) {
    fprintf(out, "%3d %4s %2d %2d\n",
	    i, SS(hb[i].name), hb[i].nhack, hb[i].maxhack);
    if (hb[i].nhack)
      for(j=0; j<hb[i].nhack; j++) {
	fprintf(out, "%d: %d %4s %4s %1s %2d %d %4s %4s %4s %4s\n", 
		j, hb[i].hack[j].nr, 
		SS(hb[i].hack[j].oname), SS(hb[i].hack[j].nname),
		SA(hb[i].hack[j].atom), hb[i].hack[j].tp, hb[i].hack[j].cgnr,
		SS(hb[i].hack[j].AI), SS(hb[i].hack[j].AJ),
		SS(hb[i].hack[j].AK), SS(hb[i].hack[j].AL) );
      }
    for(j=0; j<ebtsNR; j++)
      if (hb[i].rb[j].nb) {
	fprintf(out, " %c %d:", btsNames[j][0], hb[i].rb[j].nb);
	for(k=0; k<hb[i].rb[j].nb; k++) {
	  fprintf(out, " [");
	  for(l=0; l<btsNiatoms[j]; l++)
	    fprintf(out, " %s",hb[i].rb[j].b[k].a[l]);
	  fprintf(out, " %s]",SS(hb[i].rb[j].b[k].s));
	}
	fprintf(out,"\n");
      }
    fprintf(out,"\n");
  }
#undef SS
#undef SA
}

