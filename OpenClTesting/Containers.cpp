#include "stdafx.h"
#include "Containers.hpp"
#include <tuple>

bool operator<(float2 a, float2 b) {
	return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

bool operator<(const float2& a, const float2& b) {
	return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

bool operator<(float3 a, float3 b) {
	return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
}

bool operator<(const float3& a, const float3& b) {
	return std::tie(a.x, a.y, a.z) < std::tie(b.x, b.y, b.z);
}

bool operator<(float4 a, float4 b) {
	return std::tie(a.x, a.y, a.z, a.w) < std::tie(b.x, b.y, b.z, b.w);
}

bool operator<(const float4& a, const float4& b) {
	return std::tie(a.x, a.y, a.z, a.w) < std::tie(b.x, b.y, b.z, b.w);
}


bool operator<(Vertex thisV, Vertex other) {

	return
		std::tie(
			thisV.position.x, thisV.position.y, thisV.position.z,
			thisV.normal.x, thisV.normal.y, thisV.normal.z,
			thisV.color.r, thisV.color.g, thisV.color.b, thisV.color.a
		) < 
		std::tie(
			other.position.x, other.position.y, other.position.z,
			other.normal.x, other.normal.y, other.normal.z,
			other.color.r, other.color.g, other.color.b, other.color.a
		);
}

/*bool Vertex::operator<(const Vertex other) {
	return 
		std::tie(this->position, this->normal, this->color) <
		std::tie(other.position, other.normal, other.color);
}*/