#include "MeshApp.hpp"
#include "Camera.hpp"
#include "Model.hpp"

Mesh3D::Mesh3D(const std::string& ObjPath) {
    mModel = std::make_unique<Model>(ObjPath);
} //destructor functie niet nodig vanwege smart pointer.
