#! /usr/bin/python2.7

#from __future__ import print_function
from dolfin import *
from mshr import *
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.tri as tri


# from sympy import *
# from sympy.utilities.codegen import codegen
# # from __future__ import division
# x, y, z, g, Eps = symbols('x y z g Eps')
# Phi=2*log( (1-g*exp(-sqrt(x**2+y**2+z**2)/sqrt(Eps))) / (1+g*exp(-sqrt(x**2+y**2+z**2)/sqrt(Eps))) )
# dphix = diff(Phi,x)
# dphiy = diff(Phi,y)
# dphiz = diff(Phi,z)
# [(c_name, c_code), (h_name, c_header)] = codegen(("f", dphiz), "C", "test", header=True, empty=False)
# #print diff(Phi,x)
# print c_code

# Fo = "./meshOut_2/"
Fo = "./output/"

# Set backend to PETSC
# parameters['linear_algebra_backend'] = 'Eigen'
# parameters["linear_algebra_backend"] = "PETSc"
parameters["allow_extrapolation"] = False


# Load mesh and define function space
mesh = Mesh("mesh1.xml.gz");

P1 = FiniteElement("Lagrange", mesh.ufl_cell(), 1)
V = FunctionSpace(mesh, P1*P1*P1)
V2 = FunctionSpace(mesh, "RT", 1)
pnp = Function(V,"./output/pnp_solution.xml")
vel = Function(V2,"./output/velocity_solution.xml")

xc=0.0
yc=0.0
zc=0.0
rc=0.4

N = 100
dx = (1.0-rc)/float(N)
print dx
x = np.arange(N)*dx
p0=1.0
x0=0.0
Eps=0.01
g = np.exp(x0)*( np.exp(p0/2.0) - 1.0 )/( np.exp(p0/2.0) + 1.0 )
K = np.sqrt(2.0/Eps)
print "K = ",K
pb=2*np.log( (1-g*np.exp(-x*K)) / (1+g*np.exp(-x*K)) )


vphi_t = np.zeros(3);
vphi1 = np.zeros(N);
vphi2 = np.zeros(N);
vphi3 = np.zeros(N);
vphi4 = np.zeros(N);
x_coord1=np.zeros(3);
x_coord2=np.zeros(3);
x_coord3=np.zeros(3);
x_coord4=np.zeros(3);
x_coord1[0]=xc; x_coord1[1]=yc; x_coord1[2]=zc+rc;
x_coord2[0]=xc; x_coord2[1]=yc-rc; x_coord2[2]=zc;
x_coord3[0]=xc; x_coord3[1]=yc; x_coord3[2]=zc-rc;
x_coord4[0]=xc; x_coord4[1]=yc+rc; x_coord4[2]=zc;
for j in range(N):
    pnp.eval(vphi_t,x_coord1);
    # print x_coord1
    vphi1[j]=vphi_t[2]
    pnp.eval(vphi_t,x_coord2);
    vphi2[j]=vphi_t[2]
    pnp.eval(vphi_t,x_coord3);
    vphi3[j]=vphi_t[2]
    pnp.eval(vphi_t,x_coord4);
    vphi4[j]=vphi_t[2]
    x_coord1[2]+=dx;
    x_coord2[1]-=dx;
    x_coord3[2]-=dx;
    x_coord4[1]+=dx;



plt.figure()
plt.plot(x, vphi1)
plt.plot(x, vphi2,'*')
plt.plot(x, vphi3)
plt.plot(x, vphi4,'--')
plt.plot(x, pb,'--')
plt.legend(('PNP','PNP','PNP','PNP','PB Python'),loc=4)
plt.savefig("potential_cut.eps")
plt.close()