/** @file
 * @copyright University of Warsaw
 * @section LICENSE
 * GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
 */

#pragma once

#include <libmpdata++/concurr/detail/concurr_common.hpp>

// TODO: make it work with clang as well!
#if !defined(_REENTRANT)
#  error _REENTRANT not defined, please use something like -pthread flag for gcc
#endif
#include <boost/thread.hpp>

#include <cstdlib> // std::getenv()

namespace libmpdataxx
{
  namespace concurr
  {
    namespace detail
    {
      std::map<boost::thread::id, int> boost_thread_id;
    };

    template <
      class solver_t,
      bcond::bcond_e bcxl,
      bcond::bcond_e bcxr,
      bcond::bcond_e bcyl = bcond::null,
      bcond::bcond_e bcyr = bcond::null,
      bcond::bcond_e bczl = bcond::null,
      bcond::bcond_e bczr = bcond::null
    >
    class boost_thread : public detail::concurr_common<solver_t, bcxl, bcxr, bcyl, bcyr, bczl, bczr>
    {
      using parent_t = detail::concurr_common<solver_t, bcxl, bcxr, bcyl, bcyr, bczl, bczr>;
 
      class mem_t : public parent_t::mem_t 
      {
        boost::barrier b;

        public:

        int rank()
        {
          return detail::boost_thread_id[boost::this_thread::get_id()];
        }

	static int size() 
	{
	  const char *env_var("OMP_NUM_THREADS");
	  return (std::getenv(env_var) != NULL)
	    ? std::atoi(std::getenv(env_var)) // TODO: check if conversion OK?
	    : boost::thread::hardware_concurrency();
	}

        // ctor
        mem_t(const std::array<int, solver_t::n_dims> &grid_size) :
          b(size()),
          parent_t::mem_t(grid_size, size()) 
        {}; 

	void barrier()
	{
// TODO: if (size() != 1) ???
	  b.wait();
	}
      };

      public:

      void solve(int nt)
      {
        boost::thread_group threads;
        for (int i = 0; i < this->algos.size(); ++i) 
        {  
          std::unique_ptr<boost::thread> thp;
          thp.reset(new boost::thread(
            &solver_t::solve, boost::ref(this->algos[i]), nt
          ));
          detail::boost_thread_id[thp->get_id()] = i;
          threads.add_thread(thp.release());
        }
        threads.join_all();
      }

      // ctor
      boost_thread(const typename solver_t::rt_params_t &p) : 
        parent_t(p, new mem_t(p.grid_size), mem_t::size())
      {}

    };
  }; // namespace concurr
}; // namespace libmpdataxx
