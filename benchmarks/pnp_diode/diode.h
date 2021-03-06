#ifndef __DIODE_H
#define __DIODE_H

#include <dolfin.h>

/**
 * Define coefficients for PNP diode problem
 * along with dimensional analysis
 */

/**
 * dimensional analysis
 */
const double elementary_charge = 1.60217662e-19; // C
const double boltzmann = 1.38064852e-23; // J / K
const double temperature = 3e+2; // K
const double thermodynamic_beta = elementary_charge / (temperature * boltzmann); // V
const double vacuum_permittivity = 8.854187817e-12; // C / V

const double reference_length = 1e-5; // m
const double reference_density = 1.5e+22; // mM = 1 / m^3
const double reference_diffusivity = 2.87e+1; // cm^2 / s
const double reference_relative_permittivity = 1.17e+1; // dimensionless

const double permittivity_factor = reference_relative_permittivity * vacuum_permittivity /
  (elementary_charge * thermodynamic_beta * reference_density * reference_length * reference_length);

double scale_density (double density) { return density / reference_density; };
double scale_potential (double phi) { return (phi) * thermodynamic_beta; };
double scale_diffusivity (double diff) { return diff / reference_diffusivity; };
double scale_rel_permittivity (double rel_perm) { return rel_perm / reference_relative_permittivity; };


/**
 * define coefficients
 */
const double majority_carrier = 1.5e+22; // mM = 1 / m^3
const double minority_carrier = 6.6e+9; // mM = 1 / m^3
std::vector<double> valencies = { 0.0, 1.0, -1.0 }; // potential "valency" is at valencies[0] and should be zero
std::vector<double> reactions (double x) { return { 0.0, 0.0, 0.0 }; };
std::vector<double> diffusivities (double x) {
  double cation_diffusivity = x < 0.0 ? scale_diffusivity(1.07e+1) : scale_diffusivity(1.09e+1); // cm^2 / s
  double anion_diffusivity = x < 0.0 ? scale_diffusivity(2.69e+1) : scale_diffusivity(2.87e+1); // cm^2 / s

  if (fabs(x) < 0.05) {
    cation_diffusivity = scale_diffusivity(1.07e+1 + 10.0 * (x + 0.05) * (1.09e+1 - 1.07e+1));
    anion_diffusivity = scale_diffusivity(2.69e+1 + 10.0 * (x + 0.05) * (2.87e+1 - 2.69e+1));
  }

  return { 0.0, cation_diffusivity, anion_diffusivity };
};
double relative_permittivity (double x) { return scale_rel_permittivity(1.17e+1); }; // dimensionless
double fixed (double x) {
  if (fabs(x) < 0.05) {
    return scale_density(majority_carrier + 10.0 * (x + 0.05) * (-2.0 * majority_carrier));
  }

  return x < 0.0 ? scale_density(majority_carrier) : -scale_density(majority_carrier); // mM = 1 / m^3
};

/**
 * boundary conditions
 */
std::vector<double> left_contact (double x, double voltage_drop) {
  return {
    0.5 * scale_potential(voltage_drop), // V
    std::log(scale_density(minority_carrier)), // log(mM)
    std::log(scale_density(majority_carrier)) // log(mM)
  };
};
std::vector<double> right_contact (double x, double voltage_drop) {
  return {
    -0.5 * scale_potential(voltage_drop), // V
    std::log(scale_density(majority_carrier)), // log(mM)
    std::log(scale_density(minority_carrier)) // log(mM)
  };
};

/**
 * define expressions from coefficients
 */
class Initial_Guess : public dolfin::Expression {
  public:
    Initial_Guess (double voltage_drop) : dolfin::Expression(3), volt(voltage_drop) {}
    void eval(dolfin::Array<double>& values, const dolfin::Array<double>& x) const {
      std::vector<double> left(left_contact(-1.0, volt));
      std::vector<double> right(right_contact(+1.0, volt));
      values[0] = 0.5 * (left[0] * (1.0 - x[0]) + right[0] * (x[0] + 1.0));
      values[1] = 0.5 * (left[1] * (1.0 - x[0]) + right[1] * (x[0] + 1.0));
      values[2] = 0.5 * (left[2] * (1.0 - x[0]) + right[2] * (x[0] + 1.0));

      values[1] *= 0.5;
      values[2] *= 0.5;
      if (fabs(x[0]) < 0.05) {
        values[1] += 0.5 * (left[1] + 10.0 * (x[0] + 0.05) * (right[1] - left[1]));
        values[2] += 0.5 * (left[2] + 10.0 * (x[0] + 0.05) * (right[2] - left[2]));
      } else {
        values[1] += 0.5 * (x[0] < 0.0 ? left[1] : right[1]);
        values[2] += 0.5 * (x[0] < 0.0 ? left[2] : right[2]);
      }

      // if (fabs(x[0]) < 0.05) {
      //   values[1] = left[1] + 10.0 * (x[0] + 0.05) * (right[1] - left[1]);
      //   values[2] = left[2] + 10.0 * (x[0] + 0.05) * (right[2] - left[2]);
      // } else {
      //   values[1] = x[0] < 0.0 ? left[1] : right[1];
      //   values[2] = x[0] < 0.0 ? left[2] : right[2];
      // }
    }
  private:
    double volt;
};

class Permittivity_Expression : public dolfin::Expression {
  public:
    void eval(dolfin::Array<double>& values, const dolfin::Array<double>& x) const {
      values[0] = relative_permittivity(x[0]);
    }
};

class Poisson_Scale_Expression : public dolfin::Expression {
  public:
    void eval(dolfin::Array<double>& values, const dolfin::Array<double>& x) const {
      values[0] = 1.0 / permittivity_factor;
    }
};

class Fixed_Charged_Expression : public dolfin::Expression {
  public:
    void eval(dolfin::Array<double>& values, const dolfin::Array<double>& x) const {
      values[0] = fixed(x[0]);
    }
};

class Diffusivity_Expression : public dolfin::Expression {
  public:
    Diffusivity_Expression() : dolfin::Expression(3) {}
    void eval(dolfin::Array<double>& values, const dolfin::Array<double>& x) const {
      std::vector<double> diff(diffusivities(x[0]));
      values[0] = diff[0];
      values[1] = diff[1];
      values[2] = diff[2];
    }
};

class Reaction_Expression : public dolfin::Expression {
  public:
    Reaction_Expression() : dolfin::Expression(3) {}
    void eval(dolfin::Array<double>& values, const dolfin::Array<double>& x) const {
      std::vector<double> reac(reactions(x[0]));
      values[0] = reac[0];
      values[1] = reac[1];
      values[2] = reac[2];
    }
};

class Valency_Expression : public dolfin::Expression {
  public:
    Valency_Expression() : dolfin::Expression(3) {}
    void eval(dolfin::Array<double>& values, const dolfin::Array<double>& x) const {
      values[0] = valencies[0]; // potential valency is not used and should be zero
      values[1] = valencies[1];
      values[2] = valencies[2];
    }
};

#endif
