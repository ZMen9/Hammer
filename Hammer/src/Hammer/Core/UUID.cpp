#include "hmpch.h"
#include "UUID.h"
#include <random>


namespace hammer {

static std::random_device kRandomDevice;
static std::mt19937_64 kEngine(kRandomDevice());
static std::uniform_int_distribution<uint64_t> kUniformDistribution;

UUID::UUID() : uuid_(kUniformDistribution(kEngine)) {}

UUID::UUID(uint64_t uuid) : uuid_(uuid){}

}  // namespace hammer