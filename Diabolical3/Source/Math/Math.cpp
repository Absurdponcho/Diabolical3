#include "Math.h"

const SVector2f SVector2f::ZeroVector = { 0, 0 };
const SVector3f SVector3f::ZeroVector = { 0, 0, 0 };
const SVector4f SVector4f::ZeroVector = { 0, 0, 0, 0 };
const SVector2i SVector2i::ZeroVector = { 0, 0 };
const SVector3i SVector3i::ZeroVector = { 0, 0, 0 };

const SVector2f SVector2f::OneVector = { 1, 1 };
const SVector3f SVector3f::OneVector = { 1, 1, 1 };
const SVector4f SVector4f::OneVector = { 1, 1, 1, 1};
const SVector2i SVector2i::OneVector = { 1, 1 };
const SVector3i SVector3i::OneVector = { 1, 1, 1 };

const STransformf STransformf::Identity = {{ 0, 0, 0 }, { 1, 1, 1 }};
const SMatrix44f SMatrix44f::Identity = {{ 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 0, 0, 1 }};
const SMatrix33f SMatrix33f::Identity = {{ 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 }};

const SQuaternionf SQuaternionf::Identity = SQuaternionf(glm::vec3(0.f, 0.f, 0.f));
