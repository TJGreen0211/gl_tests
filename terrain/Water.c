#include "Water.h"
#include <stdio.h>
#include <math.h>

complex complexMult(complex c0, complex c1) {
	complex c;
	c.real = c0.real * c1.real - c0.im * c1.im;
	c.im = c0.real * c1.im + c0.im * c1.real;
	return c;
}

complex complexAdd(complex c0, complex c1) {
	complex c;
	c.real = c0.real + c1.real;
	c.im = c0.im + c1.im;
	return c;
}

complex complexConj(complex c) {
	complex cConj;
	cConj.real = c.real;
	cConj.im = -c.im;
	return cConj;
}

vec4 gaussRand() {
	float noise00 = (float)rand()/(float)(RAND_MAX);
	float noise01 = (float)rand()/(float)(RAND_MAX);
	float noise02 = (float)rand()/(float)(RAND_MAX);
	float noise03 = (float)rand()/(float)(RAND_MAX);

	float u0 = 2.0*M_PI*noise00;
	float v0 = sqrt(-2.0 * logf(noise01));
	float u1 = 2.0*M_PI*noise02;
	float v1 = sqrt(-2.0 * logf(noise03));
	vec4 grnd = {v0*cos(u0), v0 *sin(u0), v1*cos(u1), v1 * sin(u1)};
	return grnd;
}

static complex *dx;
static complex *dy;
static complex *dz;

static complex *tildeh0k;
static complex *conjTildeh0MK;

/*
void twiddleIndices(int dim, vec4 *pixels) {
	int waveTexSize = dim;
	float fWaveTexSize = (float)dim;
	int log2n = log(waveTexSize)/log(2);

	int index = 0;
	for(float N = 0.0; N < log2n; N += 1.0) {
		for(float M = 0.0; M < waveTexSize; M +=1.0) {
			float k = fmod(M * (fWaveTexSize/pow(2.0,N+1.0)), fWaveTexSize);
			//printf("k: %f\n", k);
			complex twiddle = {cos(2.0*M_PI*k/fWaveTexSize), sin(2.0*M_PI*k/fWaveTexSize)};

			int butterflySpan = (int)pow(2, N);
 			int butterflyWing;

			if(fmod(M, pow(2,N+1)) < pow(2, N))
				butterflyWing = 1;
			else butterflyWing = 0;
			//printf("%d:%d butterflyWing: %f, %f\n", (int)N, (int)M, fmod(M, pow(2,N+1)), pow(2, N));
			//printf("%d:%d k: %f\n", (int)N, (int)M, k);

			//printf("N: %d\n", butterflyWing);
			//printf("real: %f, im: %f\n", twiddle.real, twiddle.im);
			if(N == 0.0) {
				if(butterflyWing == 1) {
					pixels[index].x = fabsf(twiddle.real);
					pixels[index].y = fabsf(twiddle.im);
					pixels[index].z = M;
					pixels[index].w = M+1;
					index++;
				}
				else {
					pixels[index].x = fabsf(twiddle.real);
					pixels[index].y = fabsf(twiddle.im);
					pixels[index].z = M-1;
					pixels[index].w = M;
					index++;
				}

			}
			else {
				if(butterflyWing == 1) {
					pixels[index].x = fabsf(twiddle.real);
					pixels[index].y = fabsf(twiddle.im);
					pixels[index].z = M;
					pixels[index].w = M+butterflySpan;
					index++;
				}
				else {
					pixels[index].x = fabsf(twiddle.real);
					pixels[index].y = fabsf(twiddle.im);
					pixels[index].z = M-butterflySpan;
					pixels[index].w = M;
					index++;
				}
			}
			//printf("%d, %d\n", (int)pixels[index-1].z, (int)pixels[index-1].w);
		}
	}
}

void horizontalButterfly(int dim, vec4 *twiddleIndices, vec4 *pingpong0, vec4 *pingpong1) {
	int log2n = log(dim)/log(2);
	int pingpong = 0;
	int index = 0;
	for(int l = 0; l < log2n; l++) {
		for(int i = 0; i < dim; i++) {
			vec4 data = twiddleIndices[index++];
			//printf("d0 z:%d, w:%d\n", (int)data.z, (int)data.w);
		}
		pingpong++;
		pingpong = pingpong%2;
	}

	pingpong = 0;
	for(int i = 0; i < log2n; i++) {
		for(int N = 0; N < dim; N++){
			for(int M = 0; M < dim; M++){
				if(pingpong == 0) {
					vec4 data = twiddleIndices[(i+1)*(N+1)-1];

					complex p = {pingpong0[((int)data.z+1)*(M+1)-1].x, pingpong0[((int)data.z+1)*(M+1)-1].y};
					complex q = {pingpong0[((int)data.w+1)*(M+1)-1].x, pingpong0[((int)data.w+1)*(M+1)-1].y};
					complex w = {data.x, data.y};

					complex H = complexAdd(p, complexMult(w, q));

					pingpong1[(N+1)*(M+1)-1].x = H.real;
					pingpong1[(N+1)*(M+1)-1].y = H.im;
					pingpong1[(N+1)*(M+1)-1].z = 0.0;
					pingpong1[(N+1)*(M+1)-1].w = 1.0;
					//printf("d0 x:%d, y:%d, z:%d, w:%d\n", (int)data.x, (int)data.y, (int)data.z, (int)data.w);
					//printf("1 pingpong0 x: %d, y: %d, global: %d\n", ((int)data.z+1), (M+1), ((int)data.z+1)*(M+1)-1);
					//printf("1 pingpong1 x: %d, y: %d, global: %d\n", (N+1), (M+1), (N+1)*(M+1)-1);
					//printf("data: %d pingpong: %d, texture1: %f, %f\n", (i+1)*(N+1)-1, pingpong, pingpong1[(N+1)*(M+1)-1].x, pingpong1[(N+1)*(M+1)-1].y);
				}
				else if(pingpong == 1) {
					vec4 data = twiddleIndices[(i+1)*(N+1)-1];

					complex p = {pingpong1[((int)data.z+1)*(M+1)-1].x, pingpong1[((int)data.z+1)*(M+1)-1].y};
					complex q = {pingpong1[((int)data.w+1)*(M+1)-1].x, pingpong1[((int)data.w+1)*(M+1)-1].y};
					complex w = {data.x, data.y};

					complex H = complexAdd(p, complexMult(w, q));

					pingpong0[(N+1)*(M+1)-1].x = H.real;
					pingpong0[(N+1)*(M+1)-1].y = H.im;
					pingpong0[(N+1)*(M+1)-1].z = 0.0;
					pingpong0[(N+1)*(M+1)-1].w = 1.0;
					//printf("d1 x:%d, y:%d, z:%d, w:%d\n", (int)data.x, (int)data.y, (int)data.z, (int)data.w);
					//printf("0 pingpong1 x: %d, y: %d, global: %d\n", ((int)data.z+1), (M+1), ((int)data.z+1)*(M+1)-1);
					//printf("0 pingpong0 x: %d, y: %d, global: %d\n", (N+1), (M+1), (N+1)*(M+1)-1);
					//printf("data: %d pingpong: %d, texture0: %f, %f\n", (i+1)*(N+1)-1, pingpong, pingpong0[(N+1)*(M+1)-1].x, pingpong0[(N+1)*(M+1)-1].y);
				}
			}
		}
		pingpong++;
		pingpong = pingpong%2;
	}

}*/

float phillips(vec2 k) {
	float A = 1.0;
	float g = 9.81;
	vec2 waveDir = {30.0, 0.0};

	float k2 = k.x * k.x + k.y * k.y;
	if(k2 < 0.0001) k2 = 0.0001;
	float wV = lengthvec2(waveDir);
	float L = (wV*wV)/g;
	float dampingVal = 0.0001;
	float omegaK = dotvec2(normalizevec2(waveDir),normalizevec2(k));
	//vec2 test = normalizevec2(k);
	//printf("omegaK: %f, wV: %f, L: %f\n", omegaK, wV, L);

	//	         exp(-1/(kL)^2)
	//P(k) = A * --------------- * |K . W|^2 * exp(-k^2*l^2)
	//				 k^4
	return A * (exp(-1.0/ (k2 * L * L))/(k2*k2)) * (omegaK*omegaK*omegaK*omegaK) * exp(-k2*L*L*dampingVal*dampingVal);
}

void initGlobalVars(int dim) {
	dx = malloc(dim*dim*sizeof(complex));
	dy = malloc(dim*dim*sizeof(complex));
	dz = malloc(dim*dim*sizeof(complex));
	tildeh0k = malloc(dim*dim*sizeof(complex));
	conjTildeh0MK = malloc(dim*dim*sizeof(complex));
}

void initH0(int dim) {
	int waveTexSize = dim;
	float L = 2000.0;
	int offset = 0;
	for(float N = 0.0; N < waveTexSize; N +=1.0) {
		for(float M = 0.0; M < waveTexSize; M +=1.0) {
			vec2 k = {2.0*M_PI*N/L, 2.0*M_PI*M/L};

			vec4 grnd = gaussRand();
			float h0 = 1/sqrt(2.0) * sqrt(phillips(k));
			vec2 kNegative = {-k.x, -k.y};
			float h0Conj = 1/sqrt(2.0) * sqrt(phillips(kNegative));
			tildeh0k[offset].real = grnd.x*h0; tildeh0k[offset].im = grnd.y*h0;
			conjTildeh0MK[offset].real = grnd.z*h0Conj; conjTildeh0MK[offset].im = grnd.w*h0Conj;
			conjTildeh0MK[offset] = complexConj(conjTildeh0MK[offset]);
			offset++;
		}
	}
}

void calcH0(int dim, complex *dx, complex *dy, complex *dz, complex *tildeh0kTemp, complex *conjTildeh0MKTemp) {
	int waveTexSize = dim;

	float L = 2000.0;
	int offset = 0;
	for(float N = 0.0; N < waveTexSize; N +=1.0) {
		for(float M = 0.0; M < waveTexSize; M +=1.0) {
			vec2 k = {2.0*M_PI*N/L, 2.0*M_PI*M/L};

			float magnitude = lengthvec2(k);
			if(magnitude < 0.0001) magnitude = 0.0001;
			float w = sqrt(9.81* magnitude);

			float cosinus = cos(w+glfwGetTime());
			float sinus = sin(w+glfwGetTime());

			//float cosinus = cos(w);
			//float sinus = sin(w);

			complex expIWT = {cosinus, sinus};
			complex invExpIWT = {cosinus, -sinus};

			dy[offset] = complexAdd(complexMult(tildeh0kTemp[offset], expIWT), complexMult(conjTildeh0MKTemp[offset], invExpIWT));
			//printf("%d r: %f i: %f\n", offset, tildeh0k[offset].real, tildeh0k[offset].im);
			dx[offset].real = 0.0; dx[offset].im = -k.x/magnitude;//dx[offset] = {0.0, -k.y/magnitude};
			dx[offset] = complexMult(dx[offset], dy[offset]);
			dz[offset].real = 0.0; dz[offset].im = -k.y/magnitude;//= {0.0, -k.y/magnitude};
			dz[offset] = complexMult(dz[offset], dy[offset]);

			//printf("[%f][%f] P(k): %f, P(-k): %f\n", N, M, phillips(k), phillips(kNegative));
			//printf("gauss.x: %f, gauss.y %f\n", grnd.x, grnd.y);
			//printf("dx: %f %f, dy: %f %f, dz: %f %f\n", dx.real, dx.im, dy.real, dy.im, dz.real, dz.im);
			offset++;
		}
	}
}

void complexBitReverse(complex *c, int dim) {
	for(unsigned int i = 0, j = 0; i < dim; i++) {
		if(i < j) {
			complex tmp = {c[i].real, c[i].im};
			c[i].real = c[j].real;
			c[i].im = c[j].im;
			c[j].real = tmp.real;
			c[j].im = tmp.im;
		}

		unsigned bit = ~i & (i+1);
		unsigned rev = (dim / 2) / bit;
		j ^= (dim-1) & ~(rev-1);
	}
}

void fft(int dim, complex *c) {
	complexBitReverse(c, dim);
	int log2n = log(dim)/log(2);

	float c1 = -1.0;
	float c2 = 0.0;
	long l2 = 1;
	long i1;
	for(int l = 0; l < log2n; l++) {
		long l1 = l2;
		l2 <<= 1;
		float u1 = 1.0;
		float u2 = 0.0;
		for(int j = 0; j < l1; j++) {
			for(int i = j; i < dim; i+=l2) {
				i1 = i + l1;
				complex temp = {u1 * c[i1].real - u2 * c[i1].im, u1 * c[i1].im + u2 * c[i1].real};
				c[i1].real = c[i].real - temp.real;
            	c[i1].im = c[i].im - temp.im;
            	c[i].real += temp.real;
            	c[i].im += temp.im;
			}
			float z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}
}

void fft2d(int dim, complex *c) {
	complex *row = malloc(dim*2*sizeof(float));
	int index = 0;
	int cpIndex = 0;
	for(int j = 0; j < dim; j++) {
		for(int i = 0; i < dim; i++) {
			row[i].real = c[index].real;
			row[i].im = c[index].im;
			//printf("in: %d, real: %f, im: %f\n", index, row[i].real, row[i].im);
			//printf("0in: %d, %f+%fj\n", i, row[i].real, row[i].im);
			index++;
		}
		fft(dim, row);
		for(int i = 0; i < dim; i++) {
			c[cpIndex].real = row[i].real;
			c[cpIndex].im = row[i].im;
			//printf("out: %d, real: %f, im: %f\n", cpIndex, c[cpIndex].real, c[cpIndex].im);
			//printf("0out: %d, %f+%fj\n", cpIndex, c[cpIndex].real, c[cpIndex].im);
			cpIndex++;
		}
	}

	index = 0;
	for(int i = 0; i < dim; i++) {
		for(int j = 0; j < dim; j++) {
			row[j].real = c[dim*j+i].real;
			row[j].im = c[dim*j+i].im;
			//printf("in: %d, real: %f, im: %f\n", dim*j+index, row[j].real, row[j].im);
			//printf("1in: %d, %f+%fj\n", j, row[j].real, row[j].im);
		}

		fft(dim, row);
		for(int j = 0; j < dim; j++) {
			c[dim*j+i].real = row[j].real;
			c[dim*j+i].im = row[j].im;
			//printf("in: %d, real: %f, im: %f\n", dim*j+i, row[j].real, row[j].im);
			//printf("1out: %d, %f+%fj\n", j, c[(i+1)*(j+1)-1].real, c[(i+1)*(j+1)-1].im);
		}
	}
	free(row);
}

void initializeWaves(int dim) {
	initGlobalVars(dim);
	initH0(dim);
}

void generateWaves(int dimension, GLuint *dxTexture, GLuint *dyTexture, GLuint *dzTexture) {

	float *displacementdx = malloc(dimension*dimension*4*sizeof(float));
	float *displacementdy = malloc(dimension*dimension*4*sizeof(float));
	float *displacementdz = malloc(dimension*dimension*4*sizeof(float));

	calcH0(dimension, dx, dy, dz, tildeh0k, conjTildeh0MK);
	fft2d(dimension, dy);
	fft2d(dimension, dx);
	fft2d(dimension, dz);
	int index = 0;
	for(int i = 0; i < dimension*dimension; i++) {
		float dxh = fabsf(dx[i].real/(dimension*dimension));
		float dyh = fabsf(dy[i].real/(dimension*dimension));
		float dzh = fabsf(dz[i].real/(dimension*dimension));
		//float h = dy[i].real/(dimension*dimension);

		displacementdx[index] = dxh;
		displacementdx[index+1] = dxh;
		displacementdx[index+2] = dxh;
		displacementdx[index+3] = 1.0;

		displacementdy[index] = dyh;
		displacementdy[index+1] = dyh;
		displacementdy[index+2] = dyh;
		displacementdy[index+3] = 1.0;

		displacementdz[index] = dzh;
		displacementdz[index+1] = dzh;
		displacementdz[index+2] = dzh;
		displacementdz[index+3] = 1.0;

		index+=4;
	}
	glBindTexture(GL_TEXTURE_2D, *dxTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimension, dimension, 0, GL_RGBA, GL_FLOAT, displacementdx);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, *dyTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimension, dimension, 0, GL_RGBA, GL_FLOAT, displacementdy);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, *dzTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimension, dimension, 0, GL_RGBA, GL_FLOAT, displacementdz);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(displacementdx);
	free(displacementdy);
	free(displacementdz);
}
