void PairSPHTaitwater::coeff(int narg, char **arg) {
  if (narg != 6)
    error->all(FLERR,
        "Incorrect args for pair_style sph/taitwater coefficients");
  if (!allocated)
    allocate();

  int ilo, ihi, jlo, jhi;
  force->bounds(arg[0], atom->ntypes, ilo, ihi);
  force->bounds(arg[1], atom->ntypes, jlo, jhi);

  double rho0_one = force->numeric(arg[2]);
  double soundspeed_one = force->numeric(arg[3]);
  double viscosity_one = force->numeric(arg[4]);
  double cut_one = force->numeric(arg[5]);
  double B_one = soundspeed_one * soundspeed_one * rho0_one / 7.0;

  int count = 0;
  for (int i = ilo; i <= ihi; i++) {
    rho0[i] = rho0_one;
    soundspeed[i] = soundspeed_one;
    B[i] = B_one;
    for (int j = MAX(jlo,i); j <= jhi; j++) {
      viscosity[i][j] = viscosity_one;
      //printf("setting cut[%d][%d] = %f\n", i, j, cut_one);
      cut[i][j] = cut_one;

      setflag[i][j] = 1;

      //cut[j][i] = cut[i][j];
      //viscosity[j][i] = viscosity[i][j];
      //setflag[j][i] = 1;
      count++;
    }
  }

  if (count == 0)
    error->all(FLERR,"Incorrect args for pair coefficients");
}
