#ifndef HYPSYS_SHALLOWWATER
#define HYPSYS_SHALLOWWATER

#include "hypsys.hpp"

class ShallowWater : public HyperbolicSystem
{
public:
   explicit ShallowWater(FiniteElementSpace *fes_, BlockVector &u_block,
								 Configuration &config_);
   ~ShallowWater() { };

   virtual void EvaluateFlux(const Vector &u, DenseMatrix &FluxEval) const;
   virtual void ComputeErrors(Array<double> &errors, double DomainSize,
                              const GridFunction &u) const;
   virtual void WriteErrors(const Array<double> &errors) const;
};

#endif