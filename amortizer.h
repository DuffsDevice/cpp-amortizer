#ifndef _AMORTIZER_H_
#define _AMORTIZER_H_

// Copyright (c) 2018 Jakob Riedle (DuffsDevice)
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE AUTHOR 'AS IS' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <utility> // for std::forward
#include <iterator> // for std::iterator
#include <type_traits> // for std::remove_reference

namespace detail
{
	struct identity{
		template<typename T>
		auto operator()( T&& x ) const -> T { return std::forward<T>(x); }
	};
}

template<
	typename IterFn = std::iterator<std::input_iterator_tag,double>,
	typename ViewFn = detail::identity
>
class amortizer_impl
{
	template<typename,typename>
	friend class amortizer_impl;
	
private:
	
	IterFn	t_fn_begin;
	IterFn	t_fn_end;
	ViewFn	t_view_fn;
	
private:
	
	//! Internal Ctor
	amortizer_impl(
		IterFn fn_begin , IterFn fn_end , ViewFn view_fn
	) :
		t_fn_begin( std::move(fn_begin) )
		, t_fn_end( std::move(fn_end) )
		, t_view_fn( std::move(view_fn) )
	{}
	
public:
	
	//! Ctors and Assignment Operators
	amortizer_impl() = default;
	amortizer_impl( const amortizer_impl& ) = default;
	amortizer_impl( amortizer_impl&& ) = default;
	amortizer_impl& operator=( const amortizer_impl& ) = default;
	amortizer_impl& operator=( amortizer_impl&& ) = default;
	
	//! Setters
	template<typename NewIterFn>
	auto fn_values( NewIterFn&& fn_begin , NewIterFn&& fn_end ) &&
		-> amortizer_impl<typename std::remove_reference<NewIterFn>::type,ViewFn>
	{
		return { std::forward<NewIterFn>(fn_begin) , std::forward<NewIterFn>(fn_end) , std::move(t_view_fn) };
	}
	template<typename T>
	auto fn_values( T&& container ) && -> decltype(std::move(*this).fn_values(std::begin(container),std::end(container))) {
		return std::move(*this).fn_values( std::begin( container ) , std::end( container ) );
	}
	template<typename NewViewFn>
	auto fn_view( NewViewFn&& val ) &&
		-> amortizer_impl<IterFn,typename std::remove_reference<NewViewFn>::type>
	{
		return { std::move(t_fn_begin) , std::move(t_fn_end) , std::forward<NewViewFn>(val) };
	}
	
	/**
	 * Tries to apply amortization to a dataset of times,
	 * which are assumed to be in chronological order.
	 */
	template<typename OutputIterator>
	OutputIterator apply( OutputIterator dest ) const
	{
		// std::vector<double> itm_times; // Vector to store intermediate amortization values
		// size_t	num_prev_times = 0;
		// IterFn	iter_fn = t_fn_begin;
		// for( ; iter_fn != t_fn_end ; num_prev_times++ )
		// {
			// // Cache current time
			// double cur_time = ;
			
			// // Sum up the times of all runs before and including the current time
			// double sigma_time = cur_time;
			// for( int i = 0 ; i < num_prev_times ; i++ )
				// sigma_time += itm_times[i];
			
			// // Determine average time per run
			// double avg_time = sigma_time / ( num_prev_times + 1 );
			
			// // If average time is higher, don't try to amortize this run
			// if( avg_time >= time ){
				// itm_times.push_back(time);
				// *dest++ = avg_time; // Output current amortization level
				// continue;
			// }
			
			// // Set all previous intermediate values to the average time
			// for( int i = 0 ; i < num_prev_times ; i++ )
				// itm_times[i] = avg_time;
			
			// itm_times.push_back(avg_time); // Push the amortized value into the array of values
			// *dest++ = time; // Output amortized time value
		// }
		
		int		num_summands = 0;
		double	sigma = 0;
		IterFn	iter_fn = t_fn_begin;
		while( iter_fn != t_fn_end )
			*dest++ = ( sigma += (double) t_view_fn( *iter_fn++ ) ) / ++num_summands;
		
		return dest;
	}
};

using amortizer = amortizer_impl<>;

#endif