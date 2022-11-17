#ifndef WALRUS_COMPUTE_ENGINE_GENERICS_HPP
#define WALRUS_COMPUTE_ENGINE_GENERICS_HPP

namespace walrus::sync::generics {

  template<class T>
  struct RenderSync {
    T present{};
    T render{};
  };


} // walrus

#endif //WALRUS_COMPUTE_ENGINE_GENERICS_HPP
