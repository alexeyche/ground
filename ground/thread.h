#pragma once

#include <ground/base/base.h>

namespace NGround {

	struct TIndexSlice {
	    TIndexSlice(ui32 from, ui32 to) : From(from), To(to), Size(To-From) {}

	    ui32 From;
	    ui32 To;
	    ui32 Size;
	};


	TVector<TIndexSlice> DispatchOnThreads(ui32 elements_size, ui32 jobs);


} // namespace NGround