
/*
instantiate necessary template for external project to link Noise3D lib
*/

#include "Noise3D.h"

//explicit template instantiation
#include "GeometryEntity.cpp"
template Noise3D::GeometryEntity<N_DefaultVertex, uint32_t>;
template Noise3D::GeometryEntity<N_SimpleVertex, uint32_t>;
