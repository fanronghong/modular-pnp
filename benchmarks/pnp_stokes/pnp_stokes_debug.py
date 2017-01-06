#!/usr/bin/env python
# -*- coding: utf-8 -*-
from ufl import *
set_level(DEBUG)
# Copyright (C) 2014 CCMA@PSU Maximilian Metti, Xiaozhe Hu
#
#   Define bilinear and linear forms for 
#   the linearized Poisson-Nernst-Planck equations for a MOSFET
#
# Compile this form with FFC: ffc -O -l dolfin mosfet.ufl.

##  Finite Element Space
CG = FiniteElement("Lagrange", tetrahedron, 1)
Vit  = FiniteElement("RT", tetrahedron, 1)
Pres  = FiniteElement("Discontinuous Lagrange", tetrahedron, 0)
V  = MixedElement([CG,CG,CG,Vit,Pres])        # Solution Space
#V  = MixedElement([CG,CG,CG])*Vit*Pres       # Solution Space
#V = CG * CG * CG * Vit * Pres

(Cat, An, Phi, u, p) = TrialFunctions(V)
(cat, an, phi, v, q) = TestFunctions(V)


##  Previous Iterates
CatCat = Coefficient(CG)
AnAn = Coefficient(CG)
EsEs = Coefficient(CG)
uu  = Coefficient(Vit)
pp  = Coefficient(Pres)


##  Coefficients
eps = Coefficient(CG)
Dp = Coefficient(CG)
qp = Constant(tetrahedron)
Dn = Coefficient(CG)
qn = Constant(tetrahedron)

## Analytic solution
cation = Coefficient(CG)
anion = Coefficient(CG)
potential = Coefficient(CG)

##  Coefficients
mu  = Constant(tetrahedron)

##  DG-terms
alpha1 = Constant(tetrahedron)
alpha2 = Constant(tetrahedron)
h     = 2.0*Circumradius(tetrahedron)
h_avg = ( h('+')+h('-') )/2.0
n_vec = FacetNormal(tetrahedron)

a   = ( Dp*exp(CatCat) * ( inner(grad(Cat),grad(cat)) + inner(grad(CatCat)+qp*grad(EsEs),grad(cat)) * Cat ) )*dx \
    + ( qp*Dp*exp(CatCat) * inner(grad(Phi),grad(cat)) )*dx \
    + ( Dn*exp(AnAn) * ( inner(grad(An),grad(an)) + inner(grad(AnAn)+qn*grad(EsEs), grad(an)) * An ) )*dx \
    + ( qn*Dn*exp(AnAn) * inner(grad(Phi),grad(an)) )*dx \
    + ( eps * inner(grad(Phi),grad(phi)) )*dx \
    + ( -(qp*exp(CatCat)*Cat + qn*exp(AnAn)*An)*phi )*dx \
    - ( exp(CatCat)*Cat*(inner(uu,grad(cat))) )*dx \
    - ( exp(AnAn)   *An*(inner(uu,grad(an)))  )*dx \
    - ( exp(CatCat)*(inner(u,grad(cat))) )*dx \
    - ( exp(AnAn)*(inner(u,grad(an)))  )*dx \
    + ( 2.0*mu* inner( sym(grad(u)), sym(grad(v)) ) )*dx    -    ( p*div(v) )*dx   +   ( div(u)*q )*dx \
    + ( 2.0*mu*(alpha1)* inner( avg(sym(grad(u))), sym(outer(v('+'),n_vec('+')) + outer(v('-'),n_vec('-'))) ) )*dS \
    + ( 2.0*mu*(alpha1)* inner( sym(outer(u('+'),n_vec('+')) + outer(u('-'),n_vec('-'))), avg(sym(grad(v))) ) )*dS \
    + ( 2.0*mu*(alpha2/h_avg)* inner( jump(u),jump(v) ) )*dS \
    + ( 2.0*mu*(alpha2/h)*(inner(u,n_vec)*inner(v,n_vec) ) )*ds \
    + ( (qp*Cat*exp(CatCat)+qn*An*exp(AnAn))*inner(grad(EsEs),v) )*dx \
    + ( (qp*exp(CatCat)+qn*exp(CatCat))*inner(grad(Phi),v) )*dx
    

L   = - ( Dp*exp(CatCat)* (inner(grad(CatCat),grad(cat)) + qp*inner(grad(EsEs),grad(cat))) )*dx \
    - ( Dn*exp(AnAn  )* (inner(grad(AnAn  ),grad(an )) + qn*inner(grad(EsEs), grad(an))) )*dx \
    - ( eps * inner(grad(EsEs),grad(phi)) )*dx \
    - ( -(qp*exp(CatCat) + qn*exp(AnAn))*phi )*dx \
    + ( exp(CatCat)*(inner(uu,grad(cat))) )*dx \
    + ( exp(AnAn)*(inner(uu,grad(an)))  )*dx \
    - ( 2.0*mu* inner( sym(grad(uu)), sym(grad(v)) ) )*dx   +   ( pp*div(v) )*dx   -   ( div(uu)*q )*dx \
    - ( 2.0*mu*(alpha1)* inner( avg(sym(grad(uu))),  sym(outer(v('+'),n_vec('+')) + outer(v('-'),n_vec('-'))) ) )*dS \
    - ( 2.0*mu*(alpha1)* inner( sym(outer(uu('+'),n_vec('+')) + outer(uu('-'),n_vec('-'))), avg(sym(grad(v))) ) )*dS \
    - ( 2.0*mu*(alpha2/h_avg)* inner( jump(uu),jump(v) ) )*dS \
    - ( 2.0*mu*(alpha2/h)*(inner(uu,n_vec)*inner(v,n_vec) ) )*ds \
    - ( (qp*exp(CatCat)+qn*exp(AnAn))*inner(grad(EsEs),v) )*dx \
    + eps*inner( div(inner(grad(EsEs),grad(EsEs))), v )*dx
