#ifndef __VECTOR_PNP_H
#define __VECTOR_PNP_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <dolfin.h>
#include <ufc.h>
#include "domain.h"
#include "dirichlet.h"
extern "C" {
  #include "fasp.h"
  #include "fasp_functs.h"
}

class PDE {
  public:

    /// Create a PNP problem equipped with necessary
    /// methods for defining, updating, and solving
    /// the specific PNP problem.
    ///
    /// *Arguments*
    ///  mesh (dolfin::Mesh)
    ///    The mesh
    ///  domain (_domain_param_)
    ///    Parameters for domain dimensions and BCs
    ///  itsolver (_itsolver_param_)
    ///    Parameters for iterative linear solver
    ///  amg (_AMG_param_)
    ///    Parameters for AMG linear solver
    PDE (
      const std::shared_ptr<const dolfin::Mesh> mesh,
      const std::shared_ptr<dolfin::FunctionSpace> function_space,
      const std::shared_ptr<dolfin::Form> bilinear_form,
      const std::shared_ptr<dolfin::Form> linear_form,
      const std::map<std::string, std::vector<double>> coefficients,
      const std::map<std::string, std::vector<double>> sources
    );

    /// Destructor
    virtual ~PDE ();

    /// Update the mesh
    void update_mesh (
      const std::shared_ptr<const dolfin::Mesh> mesh
    );

    /// Return mesh from the Poisson object
    dolfin::Mesh get_mesh ();

    /// Print coefficient names to console
    void print_coefficients ();

    /// Remove DoFs for Dirichlet boundary condition
    void remove_Dirichlet_dof (
      std::vector<std::size_t> coordinate
    );

    /// Set Dirichlet Boundary condition
    void set_DirichletBC (
      std::vector<std::size_t> component,
      std::vector<std::vector<double>> boundary
    );

    /// Return the DirichletBC SubDomain
    std::vector<std::shared_ptr<dolfin::SubDomain>> get_Dirichlet_SubDomain ();

    /// return the dimension of the solution
    std::size_t get_solution_dimension ();

    /// Set the solution to a constant value
    void set_solution (
      double value
    );

    /// Set the solution to a vector of constant values
    void set_solution (
      std::vector<double> value
    );

    /// Set the solution to interpolate an expression
    void set_solution (
      std::vector<Linear_Function::Linear_Function> expression
    );

    /// Copy function to solution
    void set_solution (
      const dolfin::Function& new_solution
    );

    /// Get the current solution
    dolfin::Function get_solution ();





    /// Compute and store the dof-map for the solution space
    void get_dofs ();

    /// Solve the problem using dolfin
    dolfin::Function dolfin_solve ();

    /// Setup the linear system in dolfin
    void setup_linear_algebra ();

    /// Update solution given an update function
    void update_solution (
      dolfin::Function solution,
      const dolfin::Function& update
    );

    /// Compute the residual given the current solution
    double compute_residual (
      std::string norm_type
    );


    /// Define analytic functions from read-in files
    ///
    /// *Arguments*
    ///  coeff (_std::vector<double>_)
    ///    Parameters describing the PDE
    ///    May contain script defining coefficients
    void set_coefficients (
      std::map<std::string, std::vector<double>> coefficients,
      std::map<std::string, std::vector<double>> sources
    );

    void set_coefficients (
      std::map<std::string, dolfin::Function> coefficients,
      std::map<std::string, dolfin::Function> sources
    );

    void EigenMatrix_to_dCSRmat (
      std::shared_ptr<const dolfin::EigenMatrix> eigen_matrix,
      dCSRmat* dCSR_matrix
    );

    void EigenVector_to_dvector (
      std::shared_ptr<const dolfin::EigenVector> eigen_vector,
      dvector* vector
    );

    /// Linear algebra
    std::shared_ptr<dolfin::EigenMatrix> _eigen_matrix;
    std::shared_ptr<dolfin::EigenVector> _eigen_vector;
    dolfin::Function _convert_EigenVector_to_Function (
      const dolfin::EigenVector &eigen_vector
    );

    std::shared_ptr<dolfin::FunctionSpace> _function_space;

    /// Forms
    std::shared_ptr<dolfin::Form> _linear_form;

    std::shared_ptr<dolfin::Form> _bilinear_form;

    std::map<std::size_t, std::vector<dolfin::la_index>> _dof_map;

    /// Dirichlet boundary conditions
    std::vector<std::shared_ptr<dolfin::DirichletBC>> _dirichletBC;
    std::vector<std::shared_ptr<dolfin::SubDomain>> _dirichlet_SubDomain;

  private:
    /// Mesh
    std::shared_ptr<dolfin::Mesh> _mesh;
    std::vector<double> _mesh_max, _mesh_min;
    std::size_t _mesh_dim;
    double _mesh_epsilon;

    /// Current solution
    std::shared_ptr<dolfin::Function> _solution_function;

    /// Coefficients
    std::map<std::string, std::shared_ptr<const dolfin::Constant>> _bilinear_coefficient;
    std::map<std::string, std::shared_ptr<const dolfin::Constant>> _linear_coefficient;
};

#endif