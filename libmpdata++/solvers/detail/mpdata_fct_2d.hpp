/** 
 * @file
 * @copyright University of Warsaw
 * @section LICENSE
 * GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 *
 * @brief FCT option for MPDATA as formulated in @copybrief Smolarkiewicz_and_Grabowski_1990
 */

#pragma once

#include <libmpdata++/solvers/detail/mpdata_fct_common.hpp>
#include <libmpdata++/formulae/mpdata/formulae_mpdata_fct_2d.hpp>

namespace libmpdataxx
{
  namespace solvers
  {
    namespace detail
    {
      template <typename ct_params_t, int minhalo> 
      class mpdata_fct<
	ct_params_t, 
	minhalo,
	typename std::enable_if<ct_params_t::n_dims == 2>::type
      > : public detail::mpdata_fct_common<ct_params_t, minhalo> 
      {
	using parent_t = detail::mpdata_fct_common<ct_params_t, minhalo>; 
	using parent_t::parent_t; // inheriting ctors

	void fct_init(int e)
	{
	  const auto i = this->i^1, j = this->j^1; // not optimal - with multiple threads some indices are repeated among threads
	  const auto psi = this->mem->psi[e][this->n[e]]; 

	  this->psi_min(i,j) = min(min(min(min(
			 psi(i,j+1),
	    psi(i-1,j)), psi(i,j  )), psi(i+1,j)),
			 psi(i,j-1)
	  );
	  this->psi_max(i,j) = max(max(max(max(
			 psi(i,j+1),
	    psi(i-1,j)), psi(i,j  )), psi(i+1,j)), 
			 psi(i,j-1)
	  ); 
	}

	void fct_adjust_antidiff(int e, int iter)
	{
	  const auto psi = this->mem->psi[e][this->n[e]];
	  const auto &GC_corr = parent_t::GC_corr(iter);
          const auto &G = *this->mem->G;
	  const auto &im = this->im; // calculating once for i-1/2 and i+1/2
	  const auto &jm = this->jm; // calculating once for i-1/2 and i+1/2

	  // fill halos of GC_corr -> mpdata works with halo=1, we need halo=2
          this->xchng_vctr_alng(GC_corr);

	  // calculating the monotonic corrective velocity
	  this->GC_mono[0]( im+h, this->j ) = formulae::mpdata::GC_mono<ct_params_t::opts, 0>(psi, this->psi_min, this->psi_max, GC_corr, G, im, this->j);
	  this->GC_mono[1]( this->i, jm+h ) = formulae::mpdata::GC_mono<ct_params_t::opts, 1>(psi, this->psi_min, this->psi_max, GC_corr, G, jm, this->i);

	  // in the last iteration waiting as advop for the next equation will overwrite psi_min/psi_max
	  if (iter == this->n_iters - 1 && parent_t::n_eqns > 1) this->mem->barrier();  // TODO: move to common
	}
      };
    }; // namespace detail
  }; // namespace solvers
}; // namespace libmpdataxx
