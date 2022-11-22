#ifndef WALRUS_COMPUTE_ENGINE_GENERICS_HPP
#define WALRUS_COMPUTE_ENGINE_GENERICS_HPP

namespace walrus::sync::generics {

  template<class T>
  struct RenderSync {
    T* pPresent = nullptr;
    T* pRender = nullptr;
  };


} // walrus

#endif //WALRUS_COMPUTE_ENGINE_GENERICS_HPP
