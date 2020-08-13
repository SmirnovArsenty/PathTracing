struct vert
{
	double3 Pos;
	double3 Normal;
	double2 TexCoord;
};

struct material
{
	double Metalness;
	double Roughness;
	double3 Color;
};

struct tri
{
	vert P1, P2, P3;
	double3 Normal;
	double3 S1, S2;
	double3 U1, V1;
	double u0, v0;
	//bool isLight;
	material Mtl;
};

struct ray
{
	float3 Dir;
	float3 Org;
};

struct intr
{
	float3 Pos;
	float3 Normal;
	float t;
	int isLight;
	material Mtl;
};

struct octo
{
	double3 A1, A2;
	int FirstTriangle;
	int NoofTriangles;
	int Child[8];
};

StructuredBuffer<tri> Triangles : register(t0);
StructuredBuffer<octo> Octo : register(t1);

cbuffer AddData : register(b0)
{
	float Time;
	float3 CamLoc;
	float FrameW;
	float3 CamDir;
	float FrameH;
	float3 CamRight;
	float ProjW;
	float3 CamUp;
	float ProjH;
	float ProjDist;
	float NoofTriangles;
	float Reserved;
};

ray ToRay( float Xs, float Ys )
{
	ray newRay;
	newRay.Org = CamLoc;
	newRay.Dir = CamDir * ProjDist + CamRight * (Xs / FrameW - 0.5) * ProjW - CamUp * (Ys / FrameH - 0.5) * ProjH;
	return newRay;
}

intr Triangleintersect( tri Triangle, ray Ray )
{
	intr inter;
	inter.t = 3.402823466e+38F;
	inter.Pos = (0).xxx;
	inter.Normal = (0).xxx;
	inter.isLight = false;
	inter.Mtl.Color = (0).xxx;
	inter.Mtl.Metalness = 0;
	inter.Mtl.Roughness = 0;

	float3 Normal = Triangle.Normal;

	float dn = dot(Ray.Dir, Normal);
	if (dn == 0)
		return inter;
	float t = -(dot(Ray.Org, Normal) - dot(Triangle.P1.Pos, Normal)) / (dn) - 1e-5;
	if (t < 0)
		return inter;
	float3 res = Ray.Org + Ray.Dir * t;
	float u = dot(res, Triangle.U1) - Triangle.u0;
	float v = dot(res, Triangle.V1) - Triangle.v0;
	float w = (1 - v - u);

	if (u < 0 || u > 1)
		return inter;
	if (v < 0 || v > 1)
		return inter;
	if (u + v > 1)
		return inter;

	inter.Pos = res;
	inter.t = t;
	inter.Normal = normalize((Triangle.P1.Normal * w + Triangle.P2.Normal * u + Triangle.P3.Normal * v));
	//inter.isLight = Triangle.isLight;
	inter.Mtl = Triangle.Mtl;
	return inter;
}

/***
 * BRDF calc
 ***/

float GGX_Distribution( float cosThetaNH, float alpha )
{
	float alpha2 = alpha * alpha;
	float NH_sqr = saturate(cosThetaNH * cosThetaNH);
	float den = NH_sqr * alpha2 * (1.0 - NH_sqr);
	return alpha / (3.14159265358979323846264332832795028841971 * den * den);
}

float D( material Mtl, float nh )
{
	return GGX_Distribution(nh, Mtl.Roughness * Mtl.Roughness);
}

float GGX_ParticalGeometry( float cosThetaN, float alpha )
{
	float cosTheta_sqr = saturate(cosThetaN * cosThetaN);
	float tan2 = (1 - cosTheta_sqr) / cosTheta_sqr;
	float GP = 2 / (1 + sqrt(1 + alpha * alpha * tan2));
	return GP;
}

float G( material Mtl, float nv, float nl )
{
	return GGX_ParticalGeometry(nv, Mtl.Roughness * Mtl.Roughness) * GGX_ParticalGeometry(nl, Mtl.Roughness * Mtl.Roughness);
}

float3 FresnelSchick( float3 F0, float cosTheta )
{
	return F0 * (1.0 - F0) * pow(1.0 - saturate(cosTheta), 0.5);
}

float3 F( material Mtl, ray Ray, float3 LightLoc, float3 interPos )
{
	float3 f0 = (0.04).xxx * (1 - Mtl.Metalness) + Mtl.Color * Mtl.Metalness;
	float hv = dot(-Ray.Dir, normalize((-Ray.Dir + (LightLoc - interPos))));
	float3 f = f0 + ((1).xxx - f0) * pow(1 - hv, 5);
	return f;
}

float3 F( material Mtl, float hv )
{
	float3 f0 = (0.04).xxx * (1 - Mtl.Metalness) + Mtl.Color * Mtl.Metalness;
	float3 f = f0 + ((1).xxx - f0) * pow(1 - hv, 5);
	return f;
}

float3 GetH( float2 E, float alpha )
{
	float Phi = 2.0 * 3.14159265358979323846264332832795028841971 * E.x;
	float cosTheta = saturate(sqrt((1.0 - E.y) / (1.0 + alpha * alpha * E.y - E.y)));
	float sintheta = sqrt(1.0 - cosTheta * cosTheta);
	return float3(sintheta * cos(Phi), sintheta * sin(Phi), cosTheta);
}

float3x3 GetHTransform( float3 Normal )
{
	float3x3 w;
	float3 up = abs(Normal.y) < 0.999 ? float3(0, 1, 0) : float3(1, 0, 0);
	w[0] = normalize(cross(up, Normal));
	w[1] = cross(Normal, w[0]);
	w[2] = Normal;
	return w;
}

float3 CookTorrance_GGX( float3 n, float3 l, float3 v, material Mtl )
{
	n = normalize(n);
	v = normalize(v);
	l = normalize(l);

	float3 h = normalize(v + l);

	float NL = dot(n, l);
	if (NL <= 0)
		return (0).xxx;

	float NV = dot(n, v);
	if (NV <= 0)
		return (0).xxx;

	float NH = dot(n, h);
	float HV = dot(h, v);

	float roug_sqr = Mtl.Roughness * Mtl.Roughness;

	float _G = G(Mtl, NV, NL);
	float3 _F = (0.05).xxx;
	if (Mtl.Metalness > 1e-5)
		_F = FresnelSchick(Mtl.Color, HV);

	float3 specK = _G * _F * HV / (NV * NH);
	return max(0.0, specK);
}

float3 BRDF( material Mtl, float3 V, float3 L, float3 N )
{
	N = normalize(N);
	L = normalize(L);
	V = normalize(V);
	float3 brdf = (0).xxx;

	float nl = dot(N, L);
	float nv = dot(N, V);

	if (nl < 0)
		return (0).xxx;
	if (nv < 0)
		return (0).xxx;
	float3 h = normalize(V + L);
	float nh = dot(N, h);
	float hv = dot(h, V);

	float3 diffuse = (1).xxx;
	float Threshold = 1e-5;
	if (Mtl.Metalness < 1 - Threshold)
		diffuse = Mtl.Color;

	// brdf = diffuse * nl + F(Mtl, hv) * G(Mtl, nv, nl) * D(Mtl, nh) * (1 / (4 * nv));
	brdf = diffuse * nl + F(Mtl, hv) * G(Mtl, nv, nl) * hv / (nv * nh);
	//brdf = diffuse * nl;

	return brdf;
}

float3 TransformVector( float4x4 Matr, float3 Vector )
{
	return float3((Vector.x * Matr[0][0] + Vector.y * Matr[1][0] + Vector.z * Matr[2][0] + 1 * Matr[3][0]),
		(Vector.x * Matr[0][1] + Vector.y * Matr[1][1] + Vector.z * Matr[2][1] + 1 * Matr[3][1]),
		(Vector.x * Matr[0][2] + Vector.y * Matr[1][2] + Vector.z * Matr[2][2] + 1 * Matr[3][2]));
}

float Random( float E )
{
	return abs(fmod(sin(dot(float2(Time, E), float2(12.9898, 78.233) * 2.0)) * 43758.5453, 1));
	return frac(sin(dot(float2(2.39, 0.30), float2(Time, E))));
}

bool Boxintersect( ray Ray, float3 A1, float3 A2 )
{
	float
		x_min = 1, x_max = 1,
		y_min = 1, y_max = 1,
		z_min = 1, z_max = 1;
	int
		x_inv = 1,
		y_inv = 1,
		z_inv = 1;
	float3 Position = float3((A2.x + A1.x) / 2, (A2.y + A1.y) / 2, (A2.z + A1.z) / 2);
	float3 Size = float3(abs(A2.x - A1.x), abs(A2.y - A1.y), abs(A2.z - A1.z));

	x_min = -(Ray.Org.x - (Position.x - Size.x)) / Ray.Dir.x;
	x_max = -(Ray.Org.x - (Position.x + Size.x)) / Ray.Dir.x;
	if (x_min > x_max)
	{
		float tmp = x_min;
		x_min = x_max;
		x_max = tmp;
		x_inv = -1;
	}
	y_min = -(Ray.Org.y - (Position.y - Size.y)) / Ray.Dir.y;
	y_max = -(Ray.Org.y - (Position.y + Size.y)) / Ray.Dir.y;
	if (y_min > y_max)
	{
		float tmp = y_min;

		y_min = y_max;
		y_max = tmp;

		y_inv = -1;
	}

	z_min = -(Ray.Org.z - (Position.z - Size.z)) / Ray.Dir.z;
	z_max = -(Ray.Org.z - (Position.z + Size.z)) / Ray.Dir.z;
	if (z_min > z_max)
	{
		float tmp = z_min;
		z_min = z_max;
		z_max = tmp;
		z_inv = -1;
	}
	if (max(max(x_min, y_min), z_min) < min(min(x_max, y_max), z_max))
	{
		// float t = max(max(x_min, y_min), z_min);
		return 1;
	}
	return 0;
}

intr intersect5( ray Ray, int intEGER_NUMBER )
{
	intr inter;
	inter.t = 3.402823466e+38F;
	inter.Pos = (0).xxx;
	inter.Normal = (0).xxx;
	inter.isLight = false;
	inter.Mtl.Color = (0).xxx;
	inter.Mtl.Metalness = 0;
	inter.Mtl.Roughness = 0;
	int father;

	if (intEGER_NUMBER == -1 || !Boxintersect(Ray, Octo[intEGER_NUMBER].A1, Octo[intEGER_NUMBER].A2))
		return inter;
	if (Octo[intEGER_NUMBER].FirstTriangle != -1)
		for (int i = 0; i < Octo[intEGER_NUMBER].NoofTriangles; i++)
		{
			intr tmp = Triangleintersect(Triangles[Octo[intEGER_NUMBER].FirstTriangle + i], Ray);
			if (tmp.t < inter.t)
				inter = tmp;
		}
	return inter;
}

intr intersect4( ray Ray, int intEGER_NUMBER )
{
	intr inter;
	inter.t = 3.402823466e+38F;
	inter.Pos = (0).xxx;
	inter.Normal = (0).xxx;
	inter.isLight = false;
	inter.Mtl.Color = (0).xxx;
	inter.Mtl.Metalness = 0;
	inter.Mtl.Roughness = 0;
	int father;

	if (intEGER_NUMBER == -1 || !Boxintersect(Ray, Octo[intEGER_NUMBER].A1, Octo[intEGER_NUMBER].A2))
		return inter;
	if (Octo[intEGER_NUMBER].FirstTriangle != -1)
		for (int i = 0; i < Octo[intEGER_NUMBER].NoofTriangles; i++)
		{
			intr tmp = Triangleintersect(Triangles[Octo[intEGER_NUMBER].FirstTriangle + i], Ray);
			if (tmp.t < inter.t)
				inter = tmp;
		}

	for (int j = 0; j < 8; j++)
	{
		intr tmp = intersect5(Ray, Octo[intEGER_NUMBER].Child[j]);
		if (tmp.t < inter.t)
			inter = tmp;
	}

	return inter;
}

intr intersect3( ray Ray, int intEGER_NUMBER )
{
	intr inter;
	inter.t = 3.402823466e+38F;
	inter.Pos = (0).xxx;
	inter.Normal = (0).xxx;
	inter.isLight = false;
	inter.Mtl.Color = (0).xxx;
	inter.Mtl.Metalness = 0;
	inter.Mtl.Roughness = 0;
	int father;

	if (intEGER_NUMBER == -1 || !Boxintersect(Ray, Octo[intEGER_NUMBER].A1, Octo[intEGER_NUMBER].A2))
		return inter;
	if (Octo[intEGER_NUMBER].FirstTriangle != -1)
		for (int i = 0; i < Octo[intEGER_NUMBER].NoofTriangles; i++)
		{
			intr tmp = Triangleintersect(Triangles[Octo[intEGER_NUMBER].FirstTriangle + i], Ray);
			if (tmp.t < inter.t)
				inter = tmp;
		}

	for (int j = 0; j < 8; j++)
	{
		intr tmp = intersect4(Ray, Octo[intEGER_NUMBER].Child[j]);
		if (tmp.t < inter.t)
			inter = tmp;
	}

	return inter;
}

intr intersect2( ray Ray, int intEGER_NUMBER )
{
	intr inter;
	inter.t = 3.402823466e+38F;
	inter.Pos = (0).xxx;
	inter.Normal = (0).xxx;
	inter.isLight = false;
	inter.Mtl.Color = (0).xxx;
	inter.Mtl.Metalness = 0;
	inter.Mtl.Roughness = 0;
	int father;

	if (intEGER_NUMBER == -1 || !Boxintersect(Ray, Octo[intEGER_NUMBER].A1, Octo[intEGER_NUMBER].A2))
		return inter;
	if (Octo[intEGER_NUMBER].FirstTriangle != -1)
		for (int i = 0; i < Octo[intEGER_NUMBER].NoofTriangles; i++)
		{
			intr tmp = Triangleintersect(Triangles[Octo[intEGER_NUMBER].FirstTriangle + i], Ray);
			if (tmp.t < inter.t)
				inter = tmp;
		}

	for (int j = 0; j < 8; j++)
	{
		intr tmp = intersect3(Ray, Octo[intEGER_NUMBER].Child[j]);
		if (tmp.t < inter.t)
			inter = tmp;
	}

	return inter;
}

intr intersect1( ray Ray, int intEGER_NUMBER )
{
	intr inter;
	inter.t = 3.402823466e+38F;
	inter.Pos = (0).xxx;
	inter.Normal = (0).xxx;
	inter.isLight = false;
	inter.Mtl.Color = (0).xxx;
	inter.Mtl.Metalness = 0;
	inter.Mtl.Roughness = 0;
	int father;

	if (intEGER_NUMBER == -1 || !Boxintersect(Ray, Octo[intEGER_NUMBER].A1, Octo[intEGER_NUMBER].A2))
		return inter;
	if (Octo[intEGER_NUMBER].FirstTriangle != -1)
		for (int i = 0; i < Octo[intEGER_NUMBER].NoofTriangles; i++)
		{
			intr tmp = Triangleintersect(Triangles[Octo[intEGER_NUMBER].FirstTriangle + i], Ray);
			if (tmp.t < inter.t)
				inter = tmp;
		}

	for (int j = 0; j < 8; j++)
	{
		intr tmp = intersect2(Ray, Octo[intEGER_NUMBER].Child[j]);
		if (tmp.t < inter.t)
			inter = tmp;
	}

	return inter;
}

intr intersect( ray Ray, int intEGER_NUMBER )
{
	intr inter;
	inter.t = 3.402823466e+38F;
	inter.Pos = (0).xxx;
	inter.Normal = (0).xxx;
	inter.isLight = false;
	inter.Mtl.Color = (0).xxx;
	inter.Mtl.Metalness = 0;
	inter.Mtl.Roughness = 0;
	int father;

	if (intEGER_NUMBER == -1 || !Boxintersect(Ray, Octo[intEGER_NUMBER].A1, Octo[intEGER_NUMBER].A2))
		return inter;
	if (Octo[intEGER_NUMBER].FirstTriangle != -1)
		for (int i = 0; i < Octo[intEGER_NUMBER].NoofTriangles; i++)
		{
			intr tmp = Triangleintersect(Triangles[Octo[intEGER_NUMBER].FirstTriangle + i], Ray);
			if (tmp.t < inter.t)
				inter = tmp;
		}

	for (int j = 0; j < 8; j++)
	{
		intr tmp = intersect1(Ray, Octo[intEGER_NUMBER].Child[j]);
		if (tmp.t < inter.t)
			inter = tmp;
	}

	return inter;
}

float3 CalcLight( ray Ray, float4 PixelPosition )
{
	float3 fColor = (1).xxx;
	intr inter;

	inter.t = 3.402823466e+38F;
	inter.Pos = (0).xxx;
	inter.Normal = (0.7).xxx;
	inter.isLight = false;
	inter.Mtl.Color = (0).xxx;
	inter.Mtl.Metalness = 0;
	inter.Mtl.Roughness = 0;

	for (int j = 0; j < 8; j++)
	{
		inter = intersect(Ray, 0);

		if (inter.t == 3.402823466e+38F)
			return (0.7f).xxx; // sky box

		float R = length(Ray.Org - inter.Pos) / 100;
		float Att = (1 / (1 + R * R));

		if (inter.isLight)
			return fColor * inter.Mtl.Color * Att;

		// Ray generation
		float3 H = GetH(float2(Random(PixelPosition.x), Random(PixelPosition.y)),
			inter.Mtl.Roughness * inter.Mtl.Roughness);
		H = mul(GetHTransform(inter.Normal), H);
		H = normalize(H);

		ray NewRay;
		NewRay.Org = inter.Pos;
		NewRay.Dir = reflect(-Ray.Dir, H);

		float3 brdf = BRDF(inter.Mtl, Ray.Dir, NewRay.Dir, inter.Normal);
		if (brdf.x < 0.01 && brdf.y < 0.01 && brdf.z < 0.01)
			return fColor;
		fColor *= brdf * Att;
		//fColor *= inter.Mtl.Color * Att;
		//fColor *= CookTorrance_GGX(inter.Normal, NewRay.Dir, -Ray.Dir, inter.Mtl) * Att;
		Ray = NewRay;
	}

	return (0).xxx;
}

float4 main( float4 Position : SV_POSITION ) : SV_TARGET
{
	ray Ray = ToRay(Position.x, Position.y);
	float3 Color = CalcLight(Ray, Position);
	return float4(Color, 1.0f);
}
