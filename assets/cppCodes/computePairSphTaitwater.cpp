void PairSPHTaitwater::compute(int eflag, int vflag) {
  // Declaracion de variables y punteros
  int i, j, ii, jj, inum, jnum, itype, jtype;
  double xtmp, ytmp, ztmp, delx, dely, delz, fpair;

  int *ilist, *jlist, *numneigh, **firstneigh;
  double vxtmp, vytmp, vztmp, imass, jmass, fi, fj, fvisc, h, ih, ihsq;
  double rsq, tmp, wfd, delVdotDelR, mu, deltaE;
  // final de primer instancia de declaracion de variables

  if (eflag || vflag)
    ev_setup(eflag, vflag);
  else
    evflag = vflag_fdotr = 0;

  // Segunda instancia de declaracion e inicializacion de variables y punteros
  double **v = atom->vest;   // paso del valor del puntero que apunta a los punteros 
         // apuntando al primer elemento del vector velocidad de particulas
  double **x = atom->x;      // paso del valor del puntero que apunta a los punteros 
         // apuntando al primer elemento del vector posicion de particulas
  double **f = atom->f;      // paso del valor del puntero que apunta a los punteros 
         // apuntando al primer elemento del vector fuerza sobre particulas
  double *rho = atom->rho;   // paso del valor del puntero que apunta
         // al vector densidad de las particulas
  double *mass = atom->mass; // paso del valor del puntero que apunta
         // al vector masa de las particulas
  double *de = atom->de;     // paso del valor del puntero que apunta
         // al vector de cambio de energia interna de las particulas
  double *drho = atom->drho;
  int *type = atom->type;
  int nlocal = atom->nlocal;
  int newton_pair = force->newton_pair;

  // check consistency of pair coefficients

  if (first) {
    for (i = 1; i <= atom->ntypes; i++) {
      for (j = 1; i <= atom->ntypes; i++) {
        if (cutsq[i][j] > 1.e-32) {
          if (!setflag[i][i] || !setflag[j][j]) {
            if (comm->me == 0) {
              printf(
                  "SPH particle types %d and %d interact with cutoff=%g, but not all of their single particle properties are set.\n",
                  i, j, sqrt(cutsq[i][j]));
            }
          }
        }
      }
    }
    first = 0;
  }

  inum = list->inum;
  ilist = list->ilist;
  numneigh = list->numneigh;
  firstneigh = list->firstneigh;

  // loop over neighbors of my atoms

  for (ii = 0; ii < inum; ii++) {
    i = ilist[ii];
    xtmp = x[i][0];
    ytmp = x[i][1];
    ztmp = x[i][2];
    vxtmp = v[i][0];
    vytmp = v[i][1];
    vztmp = v[i][2];
    itype = type[i];
    jlist = firstneigh[i];
    jnum = numneigh[i];

    imass = mass[itype];

    // compute pressure of atom i with Tait EOS
    tmp = rho[i] / rho0[itype];
    fi = tmp * tmp * tmp;
    fi = B[itype] * (fi * fi * tmp - 1.0) / (rho[i] * rho[i]);

    for (jj = 0; jj < jnum; jj++) {
      j = jlist[jj];
      j &= NEIGHMASK;

      delx = xtmp - x[j][0];
      dely = ytmp - x[j][1];
      delz = ztmp - x[j][2];
      rsq = delx * delx + dely * dely + delz * delz;
      jtype = type[j];
      jmass = mass[jtype];

      if (rsq < cutsq[itype][jtype]) {

        h = cut[itype][jtype];
        ih = 1.0 / h;
        ihsq = ih * ih;

        wfd = h - sqrt(rsq);
        if (domain->dimension == 3) {
          // Lucy Kernel, 3d
          // Note that wfd, the derivative of the weight function with respect to r,
          // is lacking a factor of r.
          // The missing factor of r is recovered by
          // (1) using delV . delX instead of delV . (delX/r) and
          // (2) using f[i][0] += delx * fpair instead of f[i][0] += (delx/r) * fpair
          wfd = -25.066903536973515383e0 * wfd * wfd * ihsq * ihsq * ihsq * ih;
        } else {
          // Lucy Kernel, 2d
          wfd = -19.098593171027440292e0 * wfd * wfd * ihsq * ihsq * ihsq;
        }

        // compute pressure  of atom j with Tait EOS
        tmp = rho[j] / rho0[jtype];
        fj = tmp * tmp * tmp;
        fj = B[jtype] * (fj * fj * tmp - 1.0) / (rho[j] * rho[j]);

        // dot product of velocity delta and distance vector
        delVdotDelR = delx * (vxtmp - v[j][0]) + dely * (vytmp - v[j][1])
            + delz * (vztmp - v[j][2]);

        // artificial viscosity (Monaghan 1992)
        if (delVdotDelR < 0.) {
          mu = h * delVdotDelR / (rsq + 0.01 * h * h);
          fvisc = -viscosity[itype][jtype] * (soundspeed[itype]
              + soundspeed[jtype]) * mu / (rho[i] + rho[j]);
        } else {
          fvisc = 0.;
        }

        // total pair force & thermal energy increment
        fpair = -imass * jmass * (fi + fj + fvisc) * wfd;
        deltaE = -0.5 * fpair * delVdotDelR;

        f[i][0] += delx * fpair;
        f[i][1] += dely * fpair;
        f[i][2] += delz * fpair;

        // and change in density
        drho[i] += jmass * delVdotDelR * wfd;

        // change in thermal energy
        de[i] += deltaE;

        if (newton_pair || j < nlocal) {
          f[j][0] -= delx * fpair;
          f[j][1] -= dely * fpair;
          f[j][2] -= delz * fpair;
          de[j] += deltaE;
          drho[j] += imass * delVdotDelR * wfd;
        }

        if (evflag)
          ev_tally(i, j, nlocal, newton_pair, 0.0, 0.0, fpair, delx, dely, delz);
      }
    }
  }

  if (vflag_fdotr) virial_fdotr_compute();
}
