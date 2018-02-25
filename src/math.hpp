#ifndef MATH_HPP
#define MATH_HPP

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

namespace glmath = glm;

using glmath::vec3;
using glmath::mat4;

using glmath::clamp;
using glmath::cross;
using glmath::length;
using glmath::radians;

using glmath::lookAt;
using glmath::perspective;

#endif
