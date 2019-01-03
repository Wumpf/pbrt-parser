// ======================================================================== //
// Copyright 2019 Ingo Wald                                                 //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "FilePos.h"
#include "ospcommon/AffineSpace.h"
#include <stack>
namespace rib {
  using namespace ospcommon;

  typedef enum { FLOAT, INT, STRING } ParamType;

  template<typename T> struct getParamType;
  template<> struct getParamType<float> { static const ParamType value = FLOAT; };
  template<> struct getParamType<int> { static const ParamType value = INT; };
  template<> struct getParamType<std::string> { static const ParamType value = STRING; };
    
  struct Param {
    typedef std::shared_ptr<Param> SP;
    Param(ParamType type, const std::string &name)
      : type(type),
        name(name)
    {}
    
    const ParamType type;
    const std::string name;
  };
  
  template<typename T>
  struct ParamT : public Param {
    ParamT(const std::string &name, const std::vector<T> &values)
      : Param(getParamType<T>::value,name),
        values(values)
    {}
        
    std::vector<T> values;
  };

  struct Params {
    typedef std::shared_ptr<Params> SP;
    
    Param::SP operator[](const std::string &name)
    {
      if (values.find(name) == values.end())
        return Param::SP();
      else
        return values[name];
    }
    void add(Param::SP p) {
      values[p->name] = p;
    }
    
    std::map<std::string,Param::SP> values;
  };
  
  struct RIBParser {
    RIBParser()
    {
      xfmStack.push(affine3f(one));
    }

    affine3f currentXfm() { return xfmStack.top(); }
    
    void applyXfm(const affine3f &xfm)
    { xfmStack.top() = xfmStack.top() * xfm; }
    void pushXfm() { xfmStack.push(xfmStack.top()); }
    void popXfm() { xfmStack.pop(); }

    void makeSubdivMesh(const std::string &type,
                        const std::vector<int> &faceVertexCount,
                        const std::vector<int> &vertexIndices,
                        const std::vector<int> &ignore0,
                        const std::vector<int> &ignore1,
                        const std::vector<float> &ignore2,
                        Params &params)
    {
      int nextIdx = 0;
      Param::SP _P = params["P"];
      if (!_P) return;
      ParamT<float> &P = (ParamT<float>&)*_P;
      
      const vec3f *vertex = (const vec3f*)P.values.data();
      for (auto nextFaceVertices : faceVertexCount) {
        std::vector<int> faceIndices;
        for (int i=0;i<nextFaceVertices;i++)
          faceIndices.push_back(vertexIndices[nextIdx++]);
      }
    }

    
    std::stack<affine3f> xfmStack;
  };

  
}
