


float4 mulMatVec(float16 m, float4 v){
	return (float4)(
		dot(m.s0123, v),
		dot(m.s4567, v),
		dot(m.s89ab, v),
		dot(m.scdef, v)
	);
}

//TODO check wether m1 and m2 should swap places
float16 mulMat(float16 m1, float16 m2){
	return (float16)(
		dot(m1.s0123, m2.s048c), dot(m1.s0123, m2.s159d), dot(m1.s0123, m2.s26ae), dot(m1.s0123, m2.s37bf),
		dot(m1.s4567, m2.s048c), dot(m1.s4567, m2.s159d), dot(m1.s4567, m2.s26ae), dot(m1.s4567, m2.s37bf),
		dot(m1.s89ab, m2.s048c), dot(m1.s89ab, m2.s159d), dot(m1.s89ab, m2.s26ae), dot(m1.s89ab, m2.s37bf),
		dot(m1.scdef, m2.s048c), dot(m1.scdef, m2.s159d), dot(m1.scdef, m2.s26ae), dot(m1.scdef, m2.s37bf)
	);
}