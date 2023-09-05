#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_statistics.h>


#define NI 10000 // numero de individuos
#define TR 1.0 // tamanho da rede
#define R 0.01 // raio de interacao
#define Rm 0.4 // raio de movimento
#define NG 500 // numero de geragoes = 800 anos t=1,2anos
#define NF 250// numero de arquivos gerados

#define LS 0

#define sigma 0.00447213595499957939
#define eta 1.0

#define r (double)NG/NF
#define q pow(NG, 1.0/(NF-1))

struct INDV{
	int ezst; // 1 = individuo vivo | 0 = vazio ou individuo morto
	double x; // posicao x do individuo
	double y; // posicao y do individuo
	int sexo;
	//int teste;
};

void ic(const gsl_rng *w, struct INDV *in){
	int c;
	
	// Criando  individuos em posicoes aleatorias
	for(c = 0; c < 10; c++){
		in[c].ezst = 1;
		in[c].x=  gsl_rng_uniform(w);
		in[c].y=  gsl_rng_uniform(w);
		if(gsl_rng_uniform(w) < 0.5){
			in[c].sexo = 1;
		}else{
			in[c].sexo = 2;
		}
		//in[c].teste = c;

	}
}

void op(int t, struct INDV *in){
	int c;
	char name[100];
	FILE *file;
	
	sprintf(name, "dat/p-%d.dat", t);
	file = fopen(name, "w");
	
	/*if(t == 0){
		fprintf(file, "x    y     teste\n");
	}*/

	for(c = 0; c < NI; c++){
		if(in[c].ezst != 0)
			fprintf(file, "%e %e %d\n", in[c].x, in[c].y, in[c].sexo);
	}
	fclose(file);
}

int main(int argc, char **argv){
	int n, j, t, l, f;
	double dy, dx, dxm, dym, angulo, md;
	struct INDV *in;

	#if LS == 1
		double a0= 1.0;
	#else
		double a0= r;
	#endif

	in  = (struct INDV *) calloc(NI,    sizeof(struct INDV));

	// Definido a seed 
	gsl_rng_default_seed= (argc == 2) ? atoi(argv[1]) : time(NULL);
	
	// Criando um ponteiro para guardar um número aleatório
	gsl_rng *w= gsl_rng_alloc(gsl_rng_taus);

    ic(w, in);
	op(0, in);

	l = 0;

	for(t = 1; t <= NG; t++){
		
		for(n = 0; n < NI; n++){
			md = 20.0;
			for(j = 0; j <NI; j++){
				if(in[j].ezst == 1 && j != n){
					dx = in[j].x - in[n].x;
					if(fabs(dx) > TR * 0.5 && dx > 0)
						dx -= TR;
					else if(fabs(dx) > 0.5 * TR && dx < 0)
						dx += TR;
					//printf("dx = %e", dx);
					dy = in[j].y - in[n].y;
					if(fabs(dy) > TR * 0.5 && dy > 0)
						dy -= TR;
					else if(fabs(dy) > TR * 0.5 && dy < 0)
						dy += TR;
					//printf(" dy = %e", dy);
					if(sqrt(dx*dx+dy*dy) < md && in[n].sexo != in[j].sexo){
						md = sqrt(dx*dx+dy*dy);
						//printf(" md = %e", md);
						dxm = dx;
						dym = dy;
					}
					//printf(" angulo = %e", angulo);
					//printf("\n====================\n");
				}
			}
			if(dxm > 0){
				angulo = atan2(dym, dxm);
			}else if(dxm < 0){
				if(dym != 0){
					angulo = atan2(dym, dxm);
				}else{
					angulo = M_PI;
				}
			}else{
				if(dym > 0){
					angulo = M_PI/2;
				}else if(dym < 0){
					angulo = -M_PI/2;
				}else{
					angulo = 0;
				}
			}
			if(dym == 0 && dxm ==0){
				in[n].x += (gsl_rng_uniform(w) * 0.01) + (gsl_rng_uniform(w) * (-0.01));
				if(in[n].x>TR){
					in[n].x -= TR;
				}
				if(in[n].x<0){
					in[n].x += TR;
				}
				in[n].y += (gsl_rng_uniform(w) * 0.01) + (gsl_rng_uniform(w) * (-0.01));
				if(in[n].y>TR){
					in[n].y -= TR;
				}
				if(in[n].y<0){
					in[n].y += TR;
				}		
			}else{
				in[n].x += cos(angulo)*(gsl_rng_uniform(w) * 0.01);
				if(in[n].x>TR){
					in[n].x -= TR;
				}
				if(in[n].x<0){
					in[n].x += TR;
				}
				in[n].y += sin(angulo)*(gsl_rng_uniform(w) * 0.01);
				if(in[n].y>TR){
					in[n].y -= TR;
				}
				if(in[n].y<0){
					in[n].y += TR;
				}
			}
			
			for(j = 0; j < NI; j++){
				if(in[j].ezst == 1 && in[j].sexo != in[n].sexo){
					dx = in[j].x - in[n].x;
					if(dx>TR){dx-=TR;}
					if(dx<0){dx+=TR;}
					dy = in[j].y - in[n].y;
					if(dy>TR){dy-=TR;}
					if(dy<0){dy+=TR;}
					if(sqrt(dx*dx+dy*dy) < R){
						for(f = 0; f < NI; f++){
							if(in[f].ezst == 0){
								in[f].ezst = 1;
								if(in[n].x > in[j].x){
									in[f].x = in[j].x + (0.5*dx);
								}else{
									in[f].x = in[n].x + (0.5*dx);
								}
								if(in[n].y > in[j].y){
									in[f].y = in[j].y + (0.5*dy);
								}else{
									in[f].y = in[n].y + (0.5*dy);
								}
								if(gsl_rng_uniform(w) < 0.5)
									in[f].sexo = 1;
								else
									in[f].sexo = 2;
								f += NI;
							}
						}
						j += NI;
					}
				}
			}
		}

		if(t >= round(a0)){
			#if LS == 1
				a0*= q;
			#else 
				a0+= r;
			#endif
				op(++l, in);
		}
	}
}
