// Copyright (c) 2010-2020, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#include "tmop.hpp"
#include "linearform.hpp"
#include "pgridfunc.hpp"
#include "tmop_tools.hpp"
#include "../general/forall.hpp"
#include "../linalg/kernels.hpp"

namespace mfem
{

void TMOP_Integrator::AssemblePA(const FiniteElementSpace &fes)
{
   MFEM_VERIFY(IntRule,"");
   MFEM_ASSERT(fes.GetOrdering() == Ordering::byNODES,
               "PA Only supports Ordering::byNODES!");

   PA.fes = &fes;
   Mesh *mesh = fes.GetMesh();
   const int dim = PA.dim = mesh->Dimension();
   const int nq = PA.nq = IntRule->GetNPoints();
   const int ne = PA.ne = fes.GetMesh()->GetNE();
   const IntegrationRule &ir = *IntRule;
   PA.maps = &fes.GetFE(0)->GetDofToQuad(ir, DofToQuad::TENSOR);
   PA.geom = mesh->GetGeometricFactors(ir, GeometricFactors::JACOBIANS);

   // Energy & One vectors
   PA.E.UseDevice(true);
   PA.E.SetSize(ne*nq, Device::GetDeviceMemoryType());

   PA.O.UseDevice(true);
   PA.O.SetSize(ne*nq, Device::GetDeviceMemoryType());

   // P (AddMultPA) & A (AddMultGradPA) vectors
   PA.P.UseDevice(true);
   PA.P.SetSize(dim*dim * nq*ne, Device::GetDeviceMemoryType());

   PA.setup = false;
   PA.A.UseDevice(true);
   PA.A.SetSize(dim*dim * dim*dim * nq*ne, Device::GetDeviceMemoryType());

   // X gradient vector
   const ElementDofOrdering ordering = ElementDofOrdering::LEXICOGRAPHIC;
   PA.elem_restrict_lex = fes.GetElementRestriction(ordering);
   if (PA.elem_restrict_lex)
   {
      PA.X.SetSize(PA.elem_restrict_lex->Height(), Device::GetMemoryType());
      PA.X.UseDevice(true);
   }
}

void TMOP_Integrator::AddMultPA(const Vector &x, Vector &y) const
{
   if (PA.dim == 2) { return AddMultPA_2D(x,y); }
   if (PA.dim == 3) { return AddMultPA_3D(x,y); }
   MFEM_ABORT("Not yet implemented!");
}

void TMOP_Integrator::AssembleGradPA(const DenseMatrix &Jtr,
                                     const Vector &x) const
{
   if (PA.dim == 2) { return AssembleGradPA_2D(Jtr,x); }
   if (PA.dim == 3) { return AssembleGradPA_3D(Jtr,x); }
   MFEM_ABORT("Not yet implemented!");
}

void TMOP_Integrator::AddMultGradPA(const Vector &x, const Vector &r,
                                    Vector &c) const
{
   if (PA.dim == 2) { return AddMultGradPA_2D(x,r,c); }
   if (PA.dim == 3) { return AddMultGradPA_3D(x,r,c); }
   MFEM_ABORT("Not yet implemented!");
}

double TMOP_Integrator::GetGridFunctionEnergyPA(const FiniteElementSpace &fes,
                                                const Vector &x) const
{
   if (PA.dim == 2) { return GetGridFunctionEnergyPA_2D(fes,x); }
   if (PA.dim == 3) { return GetGridFunctionEnergyPA_3D(fes,x); }
   MFEM_ABORT("Not yet implemented!");
   return 0.0;
}

} // namespace mfem
