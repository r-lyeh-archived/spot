#ifndef FLIF_FACTORY_HPP
#define FLIF_FACTORY_HPP

#include "transform.hpp"
#include <string>

template <typename IO>
Transform<IO> *create_transform(std::string desc);

#endif