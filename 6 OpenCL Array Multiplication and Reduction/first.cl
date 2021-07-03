kernel
void
ArrayMult( global const float *dA, global const float *dB, global float *dC, global float *dD )
{
	int gid = get_global_id( 0 );

	// Multiply two arrays together using OpenCL:
	dD[gid] = dA[gid] * dB[gid];

	// Multiply two arrays together and add a third using OpenCL:
	// dD[gid] = ( dA[gid] * dB[gid] ) + dC[gid];
}
