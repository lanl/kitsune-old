/**
  ***************************************************************************
  * Copyright (c) 2017, Los Alamos National Security, LLC.
  * All rights reserved.
  *
  *  Copyright 2010. Los Alamos National Security, LLC. This software was
  *  produced under U.S. Government contract DE-AC52-06NA25396 for Los
  *  Alamos National Laboratory (LANL), which is operated by Los Alamos
  *  National Security, LLC for the U.S. Department of Energy. The
  *  U.S. Government has rights to use, reproduce, and distribute this
  *  software.  NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY,
  *  LLC MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY
  *  FOR THE USE OF THIS SOFTWARE.  If software is modified to produce
  *  derivative works, such modified software should be clearly marked,
  *  so as not to confuse it with the version available from LANL.
  *
  *  Additionally, redistribution and use in source and binary forms,
  *  with or without modification, are permitted provided that the
  *  following conditions are met:
  *
  *    * Redistributions of source code must retain the above copyright
  *      notice, this list of conditions and the following disclaimer.
  *
  *    * Redistributions in binary form must reproduce the above
  *      copyright notice, this list of conditions and the following
  *      disclaimer in the documentation and/or other materials provided
  *      with the distribution.
  *
  *    * Neither the name of Los Alamos National Security, LLC, Los
  *      Alamos National Laboratory, LANL, the U.S. Government, nor the
  *      names of its contributors may be used to endorse or promote
  *      products derived from this software without specific prior
  *      written permission.
  *
  *  THIS SOFTWARE IS PROVIDED BY LOS ALAMOS NATIONAL SECURITY, LLC AND
  *  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  *  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  *  DISCLAIMED. IN NO EVENT SHALL LOS ALAMOS NATIONAL SECURITY, LLC OR
  *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
  *  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  *  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  *  SUCH DAMAGE.
  *
  ***************************************************************************/

#ifndef FleCSIYAML
#define FleCSIYAML

#include "llvm/Support/YAMLTraits.h"

#include "clang/Sema/Kitsune/FleCSIMisc.h"



// -----------------------------------------------------------------------------
// SequenceTraits<vector<T>>
// -----------------------------------------------------------------------------

namespace llvm {
namespace yaml {

template<class T, class ALLOCATOR>
class SequenceTraits<std::vector<T,ALLOCATOR>> {
public:

   // size
   static std::size_t size(
      IO &, std::vector<T,ALLOCATOR> &vec
   ) {
      kitsune_debug("size()");
      return vec.size();
   }

   // element
   static T &element(
      IO &, std::vector<T,ALLOCATOR> &vec,
      const std::size_t index
   ) {
      kitsune_debug("element()");
      return vec[index];
   }

};

}
}



// -----------------------------------------------------------------------------
// YAML constructs related to varargs
// -----------------------------------------------------------------------------

namespace flecsi {

// FleCSIVarArgType
struct FleCSIVarArgType {
   std::string type;
   FleCSIVarArgType() : type("") { }
   FleCSIVarArgType(const std::string &_type)
    : type(_type)
   { }
};

// FleCSIVarArgTypeValue
struct FleCSIVarArgTypeValue {
   std::string type;
   std::string value;
   FleCSIVarArgTypeValue() : type(""), value("") { }
   FleCSIVarArgTypeValue(const std::string &_type, const std::string &_value)
    : type (_type),
      value(_value)
   { }
};

}



namespace llvm {
namespace yaml {

// MappingTraits<FleCSIVarArgType>
template<>
struct MappingTraits<flecsi::FleCSIVarArgType> {
   static void mapping(IO &io, flecsi::FleCSIVarArgType &c)
   {
      io.mapRequired("type", c.type);
   }
};

// MappingTraits<FleCSIVarArgTypeValue>
template<>
struct MappingTraits<flecsi::FleCSIVarArgTypeValue> {
   static void mapping(IO &io, flecsi::FleCSIVarArgTypeValue &c)
   {
      io.mapRequired("type",  c.type);
      io.mapRequired("value", c.value);
   }
};

}
}



// -----------------------------------------------------------------------------
// Helper functions dealing with the above YAML constructs
// -----------------------------------------------------------------------------

namespace flecsi {

void getVarArgs(
   const clang::CallExpr *const,
   std::vector<FleCSIVarArgTypeValue> &,
   const unsigned start = 0
);

void getVarArgsFleCSIFunctionHandle(
   const clang::TypeAliasDecl *const,
   std::vector<FleCSIVarArgType> &
);

}

#endif
