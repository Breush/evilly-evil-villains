#pragma once

#include <dlfcn.h>
#include <execinfo.h>
#include <typeinfo>
#include <string>
#include <memory>
#include <cxxabi.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <string>
#include <vector>
#include <sstream>

namespace tools
{
    //! Call-stack entry datastructure.
    struct Entry
    {
        int line = 0;
        std::string file;
        std::string function;
    };
    
    //! Stack-trace base class, for retrieving the current call-stack.
    class CallStack
    {
    public:
    
        //! Recompute the current callstack.
        void refresh(const size_t numDiscard = 0)
        {
            m_entryStack.clear();
        
            using namespace abi;

            // retrieve call-stack
            void* trace[64];
            int stackDepth = backtrace(trace, sizeof(trace) / sizeof(void*));
            char** messages = backtrace_symbols(trace, stackDepth);

            // We discard this function for sure.
            for (int i = numDiscard + 1; i < stackDepth; i++) {
                // Find first occurence of '(' or ' ' in message[i]
                int p = 0;
                while (messages[i][p] != '(' && messages[i][p] != ' ' && messages[i][p] != 0)
                    ++p;
            
                // We'll get filename and line number through addr2line function
                char syscom[256];
                sprintf(syscom, "addr2line %p -e %.*s", trace[i], p, messages[i]);
        
                char fileBuffer[1024];
                FILE *syscomFile = popen(syscom, "r");
                fgets(fileBuffer, sizeof(fileBuffer), syscomFile);
                pclose(syscomFile);
                
                // Get function name
                Dl_info dlinfo;
                dladdr(trace[i], &dlinfo);

                int status;
                const char* symname = dlinfo.dli_sname;
                char* demangled = abi::__cxa_demangle(symname, nullptr, 0, &status);
                if (status == 0 && demangled)
                    symname = demangled;
                    
                // Create entry
                Entry entry;
                entry.file = fileBuffer;
                entry.file = entry.file.substr(entry.file.rfind("/") + 1);
                entry.file = entry.file.substr(0, entry.file.find(":"));
                if (entry.file[0] == '?' && dlinfo.dli_fname)
                    entry.file = dlinfo.dli_fname;
                entry.line = atoi(strstr(fileBuffer, ":") + 1);
                entry.function = (symname)? symname : "??";
                m_entryStack.emplace_back(entry);
                
                // Freeing
                if (demangled)
                    free(demangled);
                    
                // Stop when we hit main
                if (entry.function == "main")
                    break;
            }
        }

        //! Serializes the entire call-stack into a text string.
        std::string toString () const
        {
            std::ostringstream os;
            for (auto& entry : m_entryStack)
                os << entry.file << ":" << entry.line << " [" << entry.function << "]" << std::endl;
            return os.str();
        }

    private:

        //! Call stack.
        std::vector<Entry> m_entryStack;
    };
}

