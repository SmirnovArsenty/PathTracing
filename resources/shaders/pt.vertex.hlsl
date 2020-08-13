float4 main( unsigned int id : SV_VertexID ) : SV_POSITION
{
	return float4(4 * ((id & 2) >> 1) - 1.0, 4 * (id & 1) - 1.0, 0, 1);
}
