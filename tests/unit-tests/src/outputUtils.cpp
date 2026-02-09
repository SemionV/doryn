#pragma once

#include "outputUtils.h"

namespace dory::test_utilities
{
    void printStack()
    {
        namespace bw = backward;

        bw::StackTrace st;
        st.load_here(64);  // capture up to 64 frames

        bw::TraceResolver resolver;
        resolver.load_stacktrace(st);

        for (int i = static_cast<int>(st.size()) - 1; i >= 0; --i)
        {
            const bw::ResolvedTrace trace = resolver.resolve(st[i]);
            std::cout << "#" << i << " ";

            if (!trace.source.filename.empty())
            {
                std::cout << trace.source.filename << ":"
                          << trace.source.line << " - ";
            }

            std::cout << trace.object_function << std::endl;
        }
    }
}